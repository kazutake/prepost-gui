#include "../misc/cgnslinkfollower.h"
#include "../pre/grid/structured2dgrid.h"
#include "../project/projectcgnsfile.h"
#include "../project/projectdata.h"
#include "../project/projectmainfile.h"
#include "../solverdef/solverdefinitiongridattributeinteger.h"
#include "../solverdef/solverdefinitiongridattributereal.h"
#include "../solverdef/solverdefinitiongridtype.h"
#include "postcalculatedresult.h"
#include "postsolutioninfo.h"
#include "postzonedatacontainer.h"
#include "private/postzonedatacontainer_particleloader.h"
#include "private/postzonedatacontainer_polydataloader.h"

#include <cs/coordinatesystem.h>
#include <guibase/vtkdatasetattributestool.h>
#include <misc/filesystemfunction.h>
#include <misc/stringtool.h>

#include <QDomNode>
#include <QFile>
#include <QLineF>
#include <QPointF>
#include <QRegExp>
#include <QTextStream>
#include <QXmlStreamWriter>

#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkDataSetAttributes.h>
#include <vtkDoubleArray.h>
#include <vtkExtractGrid.h>
#include <vtkFloatArray.h>
#include <vtkGeometryFilter.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>
#include <vtkQuad.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkStructuredGrid.h>
#include <vtkTriangle.h>
#include <vtkUnstructuredGrid.h>

#include <cgnslib.h>
#include <iriclib.h>
#include <ogr_spatialref.h>
#include <shapefil.h>
#include <vector>

#define ELEMNODENAME "Element"

// namespace for local funcitions
namespace {

void setPointIds(vtkIdList* idlist, cgsize_t* points, int num)
{
	for (int i = 0; i < num; ++i) {
		int id = *(points + i) - 1;
		idlist->SetId(i, id);
	}
}

void insertTriangleCells(int fn, int baseId, int zoneId, int secId, vtkUnstructuredGrid* grid)
{
	cgsize_t numCells;
	cg_ElementDataSize(fn, baseId, zoneId, secId, &numCells);
	numCells = numCells / 3;
	std::vector<cgsize_t> elements(3 * numCells, 0);
	cg_elements_read(fn, baseId, zoneId, secId, elements.data(), NULL);
	vtkSmartPointer<vtkTriangle> tri = vtkSmartPointer<vtkTriangle>::New();
	for (int i = 0; i < numCells; ++i) {
		setPointIds(tri->GetPointIds(), &(elements[i * 3]), 3);
		grid->InsertNextCell(tri->GetCellType(), tri->GetPointIds());
	}
}

void insertQuadCells(int fn, int baseId, int zoneId, int secId, vtkUnstructuredGrid* grid)
{
	cgsize_t numCells;
	cg_ElementDataSize(fn, baseId, zoneId, secId, &numCells);
	numCells = numCells / 4;
	std::vector<cgsize_t> elements(4 * numCells, 0);
	cg_elements_read(fn, baseId, zoneId, secId, elements.data(), NULL);
	vtkSmartPointer<vtkQuad> quad = vtkSmartPointer<vtkQuad>::New();
	for (int i = 0; i < numCells; ++i) {
		setPointIds(quad->GetPointIds(), &(elements[i * 4]), 4);
		grid->InsertNextCell(quad->GetCellType(), quad->GetPointIds());
	}
}

vtkDoubleArray* buildDoubleArray(const std::string& name, vtkIdType size)
{
	vtkDoubleArray* ret = vtkDoubleArray::New();
	ret->SetName(name.c_str());
	ret->Allocate(size);
	for (vtkIdType i = 0; i < size; ++i) {
		ret->InsertNextValue(0);
	}
	return ret;
}

} // namespace

const QString PostZoneDataContainer::labelName {"_LABEL"};
const QString PostZoneDataContainer::IBC {"IBC"};
const double PostZoneDataContainer::IBCLimit {0.99};

PostZoneDataContainer::PostZoneDataContainer(const std::string& baseName, const std::string& zoneName, SolverDefinitionGridType* gridtype, ProjectDataItem* parent) :
	PostDataContainer {parent},
	m_gridType {gridtype},
	m_particleData {nullptr},
	m_baseName (baseName),
	m_zoneName (zoneName),
	m_baseId {0},
	m_zoneId {0},
	m_cellDim {0},
	m_loadedOnce {false}
{}
PostZoneDataContainer::~PostZoneDataContainer()
{
	for (auto r : m_calculatedResults) {
		delete r;
	}
}

SolverDefinitionGridType* PostZoneDataContainer::gridType() const
{
	return m_gridType;
}

vtkPointSet* PostZoneDataContainer::data() const
{
	return m_data;
}

vtkPointSet* PostZoneDataContainer::labelData() const
{
	return m_labelData;
}

vtkPolyData* PostZoneDataContainer::particleData() const
{
	return m_particleData;
}

const std::map<std::string, vtkSmartPointer<vtkPolyData> >& PostZoneDataContainer::polyDataMap() const
{
	return m_polyDataMap;
}

vtkPolyData* PostZoneDataContainer::polyData(const std::string& name) const
{
	auto it = m_polyDataMap.find(name);
	if (it == m_polyDataMap.end()) {return nullptr;}

	return it->second.Get();
}

const std::vector<int>& PostZoneDataContainer::polyDataCellIds(const std::string& name) const
{
	auto it = m_polyDataCellIdsMap.find(name);
	if (it == m_polyDataCellIdsMap.end()) {
		// returns the first.
		it = m_polyDataCellIdsMap.begin();
	}
	return it->second;
}

int PostZoneDataContainer::baseId() const
{
	return m_baseId;
}

int PostZoneDataContainer::zoneId() const
{
	return m_zoneId;
}

const std::string& PostZoneDataContainer::zoneName() const
{
	return m_zoneName;
}

QString PostZoneDataContainer::caption() const
{
	return zoneName().c_str();
}

bool PostZoneDataContainer::handleCurrentStepUpdate(const int fn)
{
	return handleCurrentStepUpdate(fn, false);
}

bool PostZoneDataContainer::handleCurrentStepUpdate(const int fn, bool disableCalculatedResult)
{
	loadFromCgnsFile(fn, disableCalculatedResult);
	return m_loadOK;
}

bool PostZoneDataContainer::setBaseId(const int fn)
{
	// if m_baseID is already set, we do not have to do it again.
	if (m_baseId != 0) {return true;}

	int ier;
	int numBases;
	ier = cg_nbases(fn, &numBases);
	if (ier != 0) {return false;}
	for (int B = 1; B <= numBases; ++B) {
		char basename[32];
		int phys_dim;
		ier = cg_base_read(fn, B, basename, &m_cellDim, &phys_dim);
		if (ier != 0) {return false;}
		if (m_baseName == basename) {
			m_baseId = B;
			return true;
		}
	}
	return false;
}

bool PostZoneDataContainer::setZoneId(const int fn)
{
	// if m_zoneID is already set, we do not have to do it again.
	if (m_zoneId != 0) {return true;}

	int ier;
	int numZones;
	ier = cg_nzones(fn, m_baseId, &numZones);
	if (ier != 0) {return false;}
	for (int Z = 1; Z <= numZones; ++Z) {
		char zonename[32];
		ier = cg_zone_read(fn, m_baseId, Z, zonename, m_sizes);
		if (ier != 0) {return false;}
		if (m_zoneName == zonename) {
			m_zoneId = Z;
			return true;
		}
	}
	return false;
}

bool PostZoneDataContainer::loadZoneSize(const int fn)
{
	int ier;
	char zonename[32];
	ier = cg_zone_read(fn, m_baseId, m_zoneId, zonename, m_sizes);
	return (ier == 0);
}

bool PostZoneDataContainer::loadStructuredGrid(const int fn, const int currentStep)
{
	if (m_data != nullptr) {
		m_data->Initialize();
		m_labelData->Initialize();
	} else {
		m_data = vtkSmartPointer<vtkStructuredGrid>::New();
		m_labelData = vtkSmartPointer<vtkUnstructuredGrid>::New();
	}
	vtkPointSet* p1 = m_data;
	vtkStructuredGrid* grid = dynamic_cast<vtkStructuredGrid*>(p1);
	int NVertexI = 0, NVertexJ = 0, NVertexK = 0;
	if (m_cellDim == 1) {
		NVertexI = m_sizes[0];
		NVertexJ = 1;
		NVertexK = 1;
	} else if (m_cellDim == 2) {
		NVertexI = m_sizes[0];
		NVertexJ = m_sizes[1];
		NVertexK = 1;
	} else if (m_cellDim == 3) {
		NVertexI = m_sizes[0];
		NVertexJ = m_sizes[1];
		NVertexK = m_sizes[2];
	}
	grid->SetDimensions(NVertexI, NVertexJ, NVertexK);
	// Find zone iterative data.
	char zoneItername[32];
	int ier;

	/// only for test!

	ier = cg_ziter_read(fn, m_baseId, m_zoneId, zoneItername);
	bool iterativeCoordinates = false;
	if (ier == 0) {
		// zone iterative data exists.
		// try to read the array information of GridCoordinagesPointers.
		ier = cg_goto(fn, m_baseId, "Zone_t", m_zoneId, zoneItername, 0, "end");
		if (ier != 0) {return false;}
		int narrays;
		ier = cg_narrays(&narrays);
		if (ier != 0) {return false;}
		for (int i = 1; i <= narrays; ++i) {
			char arrayname[ProjectCgnsFile::BUFFERLEN];
			DataType_t dataType;
			int dimension;
			cgsize_t dimVector[3];
			cg_array_info(i, arrayname, &dataType, &dimension, dimVector);
			if (QString(arrayname) == "GridCoordinatesPointers") {
				// GridCoordinatesPointers found.
				std::vector<char> pointers(dimVector[0] * dimVector[1]);
				cg_array_read(i, pointers.data());
				char currentCoordinates[32];
				memcpy(currentCoordinates, &(pointers[32 * currentStep]), 32);
				// currentCoordinates is not null terminated.
				currentCoordinates[31] = '\0';
				QString curCoord = QString(currentCoordinates).trimmed();
				// now, goto the specified coordinates node.
				ier = cg_goto(fn, m_baseId, "Zone_t", m_zoneId, iRIC::toStr(curCoord).c_str(), 0, "end");
				iterativeCoordinates = true;
			}
		}
	}
	if (! iterativeCoordinates) {
		// The grid shape used as input grid is also used as result grid.
		ier = cg_goto(fn, m_baseId, "Zone_t", m_zoneId, "GridCoordinates", 0, "end");
	}
	if (ier != 0) {
		// grid data does not exists.
		return false;
	}
	// get array info in order to know what data type is used.
	// assume that x, y and z are the same type.
	char aName[ProjectCgnsFile::BUFFERLEN];
	DataType_t dType;
	int d;
	cgsize_t dVector[3];
	cg_array_info(1, aName, &dType, &d, dVector);

	size_t numPoints = NVertexI * NVertexJ * NVertexK;
	std::vector<double> dataX(numPoints, 0);
	std::vector<double> dataY(numPoints, 0);
	std::vector<double> dataZ(numPoints, 0);

	ier = cg_array_read_as(1, RealDouble, dataX.data());
	if (ier != 0) {return false;}

	if (m_cellDim >= 2) {
		ier = cg_array_read_as(2, RealDouble, dataY.data());
		if (ier != 0) {return false;}
	}
	if (m_cellDim == 3) {
		ier = cg_array_read_as(3, RealDouble, dataZ.data());
		if (ier != 0) {return false;}
	}

	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	auto offset = this->offset();
	for (int k = 0; k < NVertexK; ++k) {
		for (int j = 0; j < NVertexJ; ++j) {
			for (int i = 0; i < NVertexI; ++i) {
				int idx = i + NVertexI * (j + NVertexJ * k);
				points->InsertNextPoint(dataX[idx] - offset.x(), dataY[idx] - offset.y(), dataZ[idx]);
			}
		}
	}
	grid->SetPoints(points);
	grid->Modified();

	return true;
}

bool PostZoneDataContainer::loadUnstructuredGrid(const int fn, const int currentStep)
{
	if (m_data != 0) {
		vtkUnstructuredGrid::SafeDownCast(m_data)->Reset();
	} else {
		m_data = vtkSmartPointer<vtkUnstructuredGrid>::New();
		m_labelData = m_data;
	}
	vtkPointSet* p1 = m_data;
	vtkUnstructuredGrid* grid = dynamic_cast<vtkUnstructuredGrid*>(p1);
	int NVertex = m_sizes[0];
	// Find zone iterative data.
	char zoneItername[32];
	int ier;
	ier = cg_ziter_read(fn, m_baseId, m_zoneId, zoneItername);
	bool iterativeCoordinates = false;
	if (ier == 0) {
		// zone iterative data exists.
		// try to read the array information of GridCoordinagesPointers.
		ier = cg_goto(fn, m_baseId, "Zone_t", m_zoneId, zoneItername, 0, "end");
		if (ier != 0) {return false;}
		int narrays;
		ier = cg_narrays(&narrays);
		if (ier != 0) {return false;}
		for (int i = 1; i <= narrays; ++i) {
			char arrayname[ProjectCgnsFile::BUFFERLEN];
			DataType_t dataType;
			int dimension;
			cgsize_t dimVector[3];
			cg_array_info(i, arrayname, &dataType, &dimension, dimVector);
			if (QString(arrayname) == "GridCoordinatesPointers") {
				// GridCoordinatesPointers found.
				std::vector<char> pointers(dimVector[0] * dimVector[1]);
				cg_array_read(i, pointers.data());
				char currentCoordinates[32];
				memcpy(currentCoordinates, &(pointers[32 * currentStep]), 32);
				// currentCoordinates is not null terminated.
				currentCoordinates[31] = '\0';
				QString curCoord = QString(currentCoordinates).trimmed();
				// now, goto the specified coordinates node.
				ier = cg_goto(fn, m_baseId, "Zone_t", m_zoneId, iRIC::toStr(curCoord).c_str(), 0, "end");
				iterativeCoordinates = true;
			}
		}
	}
	if (! iterativeCoordinates) {
		// The grid shape used as input grid is also used as result grid.
		ier = cg_goto(fn, m_baseId, "Zone_t", m_zoneId, "GridCoordinates", 0, "end");
	}
	if (ier != 0) {
		// grid data does not exists.
		return false;
	}
	// get array info in order to know what data type is used.
	// assume that x, y and z are the same type.
	char aName[ProjectCgnsFile::BUFFERLEN];
	DataType_t dType;
	int d;
	cgsize_t dVector[3];
	cg_array_info(1, aName, &dType, &d, dVector);

	size_t numPoints = NVertex;
	std::vector<double> dataX(numPoints, 0);
	std::vector<double> dataY(numPoints, 0);
	std::vector<double> dataZ(numPoints, 0);

	ier = cg_array_read_as(1, RealDouble, dataX.data());
	if (ier != 0) {return false;}

	if (m_cellDim >= 2) {
		ier = cg_array_read_as(2, RealDouble, dataY.data());
		if (ier != 0) {return false;}
	}
	if (m_cellDim == 3) {
		ier = cg_array_read_as(3, RealDouble, dataZ.data());
		if (ier != 0) {return false;}
	}

	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	auto offset = this->offset(); 
	for (int i = 0; i < NVertex; ++i) {
		points->InsertNextPoint(dataX[i] - offset.x(), dataY[i] - offset.y(), dataZ[i]);
	}
	grid->SetPoints(points);

	grid->Modified();

	// Grid coordinates are loaded.
	// load grid node connectivity data.
	// Unstructured grid that consists of triangles is supported.

	int numSections;
	cg_nsections(fn, m_baseId, m_zoneId, &numSections);
	for (int S = 1; S <= numSections; ++S) {
		ElementType_t eType;
		cgsize_t startIndex, endIndex;
		int nBndry, parent_flag;
		char buffer[32];
		cg_section_read(fn, m_baseId, m_zoneId, S, buffer, &eType, &startIndex, &endIndex, &nBndry, &parent_flag);
		if (eType == TRI_3) {
			// Triangle
			insertTriangleCells(fn, m_baseId, m_zoneId, S, grid);
		} else if (eType == QUAD_4) {
			// Quadrangle
			insertQuadCells(fn, m_baseId, m_zoneId, S, grid);
		}
	}
	return true;
}

bool PostZoneDataContainer::findSolutionId(const int fn, const int currentStep, int* solId, const char* arrName)
{
	int ier;
	char zoneItername[32];
	ier = cg_ziter_read(fn, m_baseId, m_zoneId, zoneItername);
	if (ier == 0) {
		ier = cg_goto(fn, m_baseId, "Zone_t", m_zoneId, zoneItername, 0, "end");
		if (ier != 0) {return false;}
		int narrays;
		ier = cg_narrays(&narrays);
		if (ier != 0) {return false;}
		for (int i = 1; i <= narrays; ++i) {
			char arrayname[32];
			DataType_t dataType;
			int dimension;
			cgsize_t dimVector[3];
			cg_array_info(i, arrayname, &dataType, &dimension, dimVector);
			if (QString(arrayname) == arrName) {
				// arrName found.
				char* pointers;
				// dimension = 2, dimVector = [32, NumberOfSteps].
				pointers = new char[dimVector[0] * dimVector[1]];
				cg_array_read(i, pointers);
				char curSol[32];
				memcpy(curSol, &(pointers[32 * currentStep]), 32);
				// the currentSolution is not null terminated.
				curSol[31] = '\0';
				QString currentSolution = QString(curSol).trimmed();
				delete pointers;
				// now, find the solution data and set solId.
				int nsols;
				cg_nsols(fn, m_baseId, m_zoneId, &nsols);
				for (int j = 1; j <= nsols; ++j) {
					char solname[32];
					GridLocation_t location;
					ier = cg_sol_info(fn, m_baseId, m_zoneId, j, solname, &location);
					if (ier != 0) {return false;}
					if (currentSolution == solname) {
						*solId = j;
						return true;
					}
				}
				break;
			}
		}
	} else {
		*solId = currentStep + 1;
		return true;
	}
	return false;
}

bool PostZoneDataContainer::getSolutionId(const int fn, const int currentStep, int* solId)
{
	return findSolutionId(fn, currentStep, solId, "FlowSolutionPointers");
}

bool PostZoneDataContainer::getCellSolutionId(const int fn, const int currentStep, int* solId)
{
	return findSolutionId(fn, currentStep, solId, "FlowCellSolutionPointers");
}

template<class T, class DA>
static void loadScalarDataT(const QString& name, vtkDataSetAttributes* atts, int index, int datalen)
{
	std::vector<T> data(datalen, 0);
	cg_array_read(index, data.data());
	vtkSmartPointer<DA> tmpArray = vtkSmartPointer<DA>::New();
	tmpArray->SetName(iRIC::toStr(name).c_str());
	tmpArray->Allocate(datalen);
	if (PostZoneDataContainer::IBC == name) {
		// for IBC values, special handling is done: 0 is inactive the others are all active.
		for (int i = 0; i < datalen; ++i) {
			int val = static_cast<int>(data[i]);
			if (val != 0) {val = 1;}
			tmpArray->InsertNextValue(val);
		}
	} else {
		for (int i = 0; i < datalen; ++i) {
			tmpArray->InsertNextValue(data[i]);
		}
	}
	atts->AddArray(tmpArray);
}

bool PostZoneDataContainer::loadScalarData(vtkDataSetAttributes* atts, int firstAtt)
{
	int narrays;
	cg_narrays(&narrays);
	for (int i = firstAtt; i <= narrays; ++i) {
		DataType_t datatype;
		int dimension;
		cgsize_t dimVector[3];
		char arrayname[33];
		cg_array_info(i, arrayname, &datatype, &dimension, dimVector);
		QString name(arrayname);

		// skip vector values.
		QRegExp rx;
		// For example, "VelocityX", "VelocityY", "VelocityZ"
		rx = QRegExp("(.*)(X|Y|Z)$");
		if (rx.indexIn(name) != -1) {continue;}
		int datalen = 1;
		for (int j = 0; j < dimension; ++j) {
			datalen *= dimVector[j];
		}
		if (datatype == Integer) {
			loadScalarDataT<int, vtkIntArray>(arrayname, atts, i, datalen);
		} else if (datatype == RealSingle) {
			loadScalarDataT<float, vtkFloatArray>(arrayname, atts, i, datalen);
		} else if (datatype == RealDouble) {
			loadScalarDataT<double, vtkDoubleArray>(arrayname, atts, i, datalen);
		}
	}
	atts->Modified();
	return true;
}

static int findArray(const QString& name, DataType_t dt, int dim, int narrays)
{
	for (int i = 1; i <= narrays; ++i) {
		char arrayname[30];
		DataType_t datatype;
		int dimension;
		cgsize_t dimVector[3];
		cg_array_info(i, arrayname, &datatype, &dimension, dimVector);
		if (name == arrayname && dt == datatype && dim == dimension) {
			return i;
		}
	}
	return 0;
}

template<class T, class DA>
static void loadVectorDataT(const QString& name, vtkDataSetAttributes* atts, int iX, int iY, int iZ, int datalen)
{
	std::vector<T> dataX(datalen, 0);
	std::vector<T> dataY(datalen, 0);
	std::vector<T> dataZ(datalen, 0);
	std::vector<double> dataMag(datalen, 0);
	// read x.
	cg_array_read(iX, dataX.data());
	// read y.
	if (iY != 0) {
		cg_array_read(iY, dataY.data());
	}
	// read z
	if (iZ != 0){
		cg_array_read(iZ, dataZ.data());
	}
	for (int i = 0; i < datalen; ++i) {
		double sum = 0;
		sum += dataX[i] * dataX[i];
		sum += dataY[i] * dataY[i];
		sum += dataZ[i] * dataZ[i];
		dataMag[i] = std::sqrt(sum);
	}
	vtkSmartPointer<DA> tmpArray = vtkSmartPointer<DA>::New();
	tmpArray->SetName(iRIC::toStr(name).c_str());
	tmpArray->SetNumberOfComponents(3);
	tmpArray->Allocate(datalen);
	for (int i = 0; i < datalen; ++i) {
		tmpArray->InsertNextTuple3(dataX[i], dataY[i], dataZ[i]);
	}
	atts->AddArray(tmpArray);

	vtkSmartPointer<vtkDoubleArray> magArray = vtkSmartPointer<vtkDoubleArray>::New();
	QString magName = name;
	magName.append(" (magnitude)");
	magArray->SetName(iRIC::toStr(magName).c_str());
	magArray->Allocate(datalen);
	for (int i = 0; i < datalen; ++i) {
		magArray->InsertNextValue(dataMag[i]);
	}
	atts->AddArray(magArray);
}

bool PostZoneDataContainer::loadVectorData(vtkDataSetAttributes* atts, int firstAtt)
{
	int narrays;
	cg_narrays(&narrays);

	// try to find vector attributes.
	for (int i = firstAtt; i <= narrays; ++i) {
		char arrayname[30];
		DataType_t datatype;
		int dimension;
		cgsize_t dimVector[3];
		cg_array_info(i, arrayname, &datatype, &dimension, dimVector);
		QString name(arrayname);

		QRegExp rx;
		// find solution with name "VelocityX", for example.
		rx = QRegExp("(.*)X$");
		if (rx.indexIn(name) != -1) {
			int indexX, indexY, indexZ;
			// vector data found.
			QString vectorName = rx.cap(1);

			indexX = i;
			indexY = 0;
			indexZ = 0;
			// try to find Y component. i.e. "VelocityY"
			QString yname = vectorName;
			yname.append("Y");
			indexY = findArray(yname, datatype, dimension, narrays);
			if (indexY != 0) {
				// try to find Z component. i.e. "VelocityZ"
				QString zname = vectorName;
				zname.append("Z");
				indexZ = findArray(zname, datatype, dimension, narrays);
			}
			// indexX, indexY, indexZ are set correctly.
			unsigned int datalen = 1;
			for (int j = 0; j < dimension; ++j) {
				datalen *= dimVector[j];
			}
			if (datatype == Integer) {
				loadVectorDataT<int, vtkIntArray> (vectorName, atts, indexX, indexY, indexZ, datalen);
			} else if (datatype == RealSingle) {
				loadVectorDataT<float, vtkFloatArray> (vectorName, atts, indexX, indexY, indexZ, datalen);
			} else if (datatype == RealDouble) {
				loadVectorDataT<double, vtkDoubleArray> (vectorName, atts, indexX, indexY, indexZ, datalen);
			}
		}
	}
	return true;
}

bool PostZoneDataContainer::loadGridScalarData(const int fn, const int solid)
{
	int ier;
	char solname[32];
	GridLocation_t location;
	ier = cg_sol_info(fn, m_baseId, m_zoneId, solid, solname, &location);
	if (ier != 0) {return false;}
	ier = cg_goto(fn, m_baseId, "Zone_t", m_zoneId, "FlowSolution_t", solid, "end");
	if (ier != 0) {return false;}

	CgnsLinkFollower linkFollower;

	vtkDataSetAttributes* data;
	if (location == Vertex) {
		// vertex.
		data = m_data->GetPointData();
	} else {
		// cell center.
		data = m_data->GetCellData();
	}
	return loadScalarData(data);
}

bool PostZoneDataContainer::loadGridVectorData(const int fn, const int solid)
{
	int ier;
	char solname[32];
	GridLocation_t location;
	ier = cg_sol_info(fn, m_baseId, m_zoneId, solid, solname, &location);
	if (ier != 0) {return false;}
	ier = cg_goto(fn, m_baseId, "Zone_t", m_zoneId, "FlowSolution_t", solid, "end");
	if (ier != 0) {return false;}

	CgnsLinkFollower linkFollower;

	vtkDataSetAttributes* data;
	if (location == Vertex) {
		// vertex.
		data = m_data->GetPointData();
	} else {
		// cell center.
		data = m_data->GetCellData();
	}
	return loadVectorData(data);
}

bool PostZoneDataContainer::loadCellFlagData(const int fn)
{
	if (m_cellDim != 2) {
		// cell flag data can be loaded only when m_cellDim == 2.
		return true;
	}

	const QList<SolverDefinitionGridAttribute*>& conds = m_gridType->gridAttributes();
	for (auto it = conds.begin(); it != conds.end(); ++it) {
		const SolverDefinitionGridAttribute* cond = *it;
		if (cond->position() != SolverDefinitionGridAttribute::CellCenter) {continue;}
		if (! cond->isOption()) {continue;}
		const SolverDefinitionGridAttributeInteger* icond = dynamic_cast<const SolverDefinitionGridAttributeInteger*>(cond);
		if (icond == 0) {continue;}

		// this is a cell flag to load.
		int ier;
		ier = cg_goto(fn, m_baseId, "Zone_t", m_zoneId, "GridConditions", 0, cond->name().c_str(), 0, "end");
		if (ier != 0) {
			// Corresponding node does not exists.
			// Maybe user is trying to load calculation results in project file created
			// using older solver.
			//
			// initializes the array with default value, and returns true.

			vtkSmartPointer<vtkIntArray> iarray = vtkSmartPointer<vtkIntArray>::New();
			int defaultVal = icond->defaultValue();
			for (vtkIdType i = 0; i < m_data->GetNumberOfCells(); ++i) {
				iarray->InsertNextValue(defaultVal);
			}
			iarray->SetName(addInputDataPrefix(cond->name()).c_str());

			m_data->GetCellData()->AddArray(iarray);

			continue;
		}
		// Find "Value" array.
		int narrays;
		cg_narrays(&narrays);
		for (int i = 1; i <= narrays; ++i) {
			char arrayName[ProjectCgnsFile::BUFFERLEN];
			DataType_t dt;
			int dataDimension;
			cgsize_t dimensionVector[3];
			cg_array_info(i, arrayName, &dt, &dataDimension, dimensionVector);
			if (QString(arrayName) == "Value") {
				// We've found the array!
				// load data.
				vtkSmartPointer<vtkIntArray> iarray = vtkSmartPointer<vtkIntArray>::New();
				unsigned int count = m_data->GetNumberOfCells();
				std::vector<int> data(count);
				ier = cg_array_read(i, data.data());
				for (int val : data) {
					iarray->InsertNextValue(val);
				}
				iarray->SetName(addInputDataPrefix(cond->name()).c_str());

				m_data->GetCellData()->AddArray(iarray);
			}
		}
	}
	return true;
}

bool PostZoneDataContainer::setupIndexData()
{
	vtkStructuredGrid* sGrid = vtkStructuredGrid::SafeDownCast(m_data);
	vtkSmartPointer<vtkStringArray> indexArray = vtkSmartPointer<vtkStringArray>::New();
	indexArray->SetName(iRIC::toStr(labelName).c_str());
	vtkSmartPointer<vtkDoubleArray> newElevArray;
	vtkSmartPointer<vtkDoubleArray> origElevArray;
	if (elevationName() != "") {
		origElevArray = vtkDoubleArray::SafeDownCast(m_data->GetPointData()->GetArray(iRIC::toStr(elevationName()).c_str()));
		newElevArray = vtkSmartPointer<vtkDoubleArray>::New();
		newElevArray->SetName(origElevArray->GetName());
	}

	if (sGrid != nullptr) {
		// structured grid.
		int dims[3];
		sGrid->GetDimensions(dims);
		QString label;
		if (dims[2] == 1) {
			label = QString("(%1,%2)");
		} else {
			label = QString("(%1,%2,%3)");
		}
		vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
		points->SetDataTypeToDouble();
		for (int i = 0; i < dims[0]; ++i) {
			double p[3];
			sGrid->GetPoint(nodeIndex(i, 0, 0), p);
			points->InsertNextPoint(p);
			indexArray->InsertNextValue(iRIC::toStr(label.arg(i + 1).arg(1).arg(1)).c_str());
			if (newElevArray != nullptr) {
				newElevArray->InsertNextValue(origElevArray->GetValue(nodeIndex(i, 0, 0)));
			}
		}
		for (int j = 1; j < dims[1]; ++j) {
			double p[3];
			sGrid->GetPoint(nodeIndex(0, j, 0), p);
			points->InsertNextPoint(p);
			indexArray->InsertNextValue(iRIC::toStr(label.arg(1).arg(j + 1).arg(1)).c_str());
			if (newElevArray != nullptr) {
				newElevArray->InsertNextValue(origElevArray->GetValue(nodeIndex(0, j, 0)));
			}
		}
		for (int k = 1; k < dims[2]; ++k) {
			double p[3];
			sGrid->GetPoint(nodeIndex(0, 0, k), p);
			points->InsertNextPoint(p);
			indexArray->InsertNextValue(iRIC::toStr(label.arg(1).arg(1).arg(k + 1)).c_str());
			if (newElevArray != nullptr) {
				newElevArray->InsertNextValue(origElevArray->GetValue(nodeIndex(0, 0, k)));
			}
		}
		m_labelData->SetPoints(points);
		m_labelData->GetPointData()->AddArray(indexArray);
		if (newElevArray != nullptr) {
			m_labelData->GetPointData()->AddArray(newElevArray);
		}
	} else {
		// unstructured grid.
		QString label("(%1)");
		for (int i = 0; i < m_data->GetNumberOfPoints(); ++i) {
			indexArray->InsertNextValue(iRIC::toStr(label.arg(i + 1)).c_str());
		}
		m_labelData->GetPointData()->AddArray(indexArray);
	}
	return true;
}

void PostZoneDataContainer::loadFromCgnsFile(const int fn)
{
	loadFromCgnsFile(fn, false);
}

void PostZoneDataContainer::loadFromCgnsFile(const int fn, bool disableCalculatedResult)
{
	m_loadOK = true;
	int currentStep = dynamic_cast<PostSolutionInfo*>(parent())->currentStep();

	bool ret;
	// set baseId.
	ret = setBaseId(fn);
	if (ret == false) {goto ERROR;}
	// set zoneId.
	ret = setZoneId(fn);
	if (ret == false) {goto ERROR;}
	// first, check whether structured or unstructured.
	ZoneType_t type;
	cg_zone_type(fn, m_baseId, m_zoneId, &type);
	ret = loadZoneSize(fn);
	if (ret == false) {goto ERROR;}
	if (type == Structured) {
		ret = loadStructuredGrid(fn, currentStep);
		if (ret == false) {goto ERROR;}
	} else {
		ret = loadUnstructuredGrid(fn, currentStep);
		if (ret == false) {goto ERROR;}
	}
	// load solution data.
	int solId;
	ret = getSolutionId(fn, currentStep, &solId);
	if (ret == false) {goto ERROR;}
	ret = loadGridScalarData(fn, solId);
	if (ret == false) {goto ERROR;}

	// load cell-centered data.
	int cellSolId;
	if (getCellSolutionId(fn, currentStep, &cellSolId)) {
		loadGridScalarData(fn, cellSolId);
	}

	ret = loadGridVectorData(fn, solId);
	if (ret == false) {goto ERROR;}
	ret = loadCellFlagData(fn);
	if (ret == false) {goto ERROR;}
	ret = setupIndexData();

	// load particles
	ret = ParticleLoader::load(fn, m_baseId, m_zoneId, currentStep, &m_particleData, this->offset());
	// load polydata
	ret = PolyDataLoader::load(fn, m_baseId, m_zoneId, currentStep, &m_polyDataMap, &m_polyDataCellIdsMap, this->offset());

	if (! disableCalculatedResult) {
		addCalculatedResultArrays();
	}

	m_data->Modified();

	m_loadedOnce = true;
	emit dataUpdated();
	return;
ERROR:
	m_loadOK = false;
	m_data = nullptr;
}

void PostZoneDataContainer::loadIfEmpty(const int fn)
{
	if (m_data != nullptr) {return;}
	loadFromCgnsFile(fn);
}

int PostZoneDataContainer::nodeIndex(int i, int j, int k) const
{
	int dim[3];
	vtkStructuredGrid* grid = vtkStructuredGrid::SafeDownCast(m_data);
	grid->GetDimensions(dim);
	return i + dim[0] * (j + dim[1] * k);
}

void PostZoneDataContainer::getNodeIJKIndex(int index, int* i, int* j, int* k) const
{
	int dim[3];
	vtkStructuredGrid* grid = vtkStructuredGrid::SafeDownCast(m_data);
	grid->GetDimensions(dim);

	*i = index % (dim[0]);
	*j = ((index - *i) / dim[0]) % dim[1];
	*k = index / (dim[0] * dim[1]);
}

int PostZoneDataContainer::cellIndex(int i, int j, int k) const
{
	int dim[3];
	vtkStructuredGrid* grid = vtkStructuredGrid::SafeDownCast(m_data);
	grid->GetDimensions(dim);
	return i + (dim[0] - 1) * (j + (dim[1] - 1) * k);
}

void PostZoneDataContainer::getCellIJKIndex(int index, int* i, int* j, int* k) const
{
	int dim[3];
	vtkStructuredGrid* grid = vtkStructuredGrid::SafeDownCast(m_data);
	grid->GetDimensions(dim);

	*i = index % (dim[0] - 1);
	*j = ((index - *i) / dim[0] - 1) % (dim[1] - 1);
	*k = index / ((dim[0] - 1) * (dim[1] - 1));
}

bool PostZoneDataContainer::scalarValueExists() const
{
	auto names = vtkDataSetAttributesTool::getArrayNamesWithOneComponent(m_data->GetPointData());
	for (const auto& name : names) {
		if (hasInputDataPrefix(name)) {continue;}
		return true;
	}
	return false;
}

bool PostZoneDataContainer::cellScalarValueExists() const
{
	auto names = vtkDataSetAttributesTool::getArrayNamesWithOneComponent(m_data->GetCellData());
	for (const auto& name : names) {
		if (hasInputDataPrefix(name)) {continue;}
		return true;
	}
	return false;
}

bool PostZoneDataContainer::vectorValueExists() const
{
	return vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(m_data->GetPointData()).size() > 0;
}

std::vector<PostCalculatedResult*>& PostZoneDataContainer::calculatedResults()
{
	return m_calculatedResults;
}

const std::vector<PostCalculatedResult*>& PostZoneDataContainer::calculatedResults() const
{
	return m_calculatedResults;
}

bool PostZoneDataContainer::IBCExists() const
{
	for (std::string name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(m_data->GetPointData())) {
		if (IBC == name.c_str()) {return true;}
	}

	return false;
}

bool PostZoneDataContainer::IBCCellExists() const
{
	for (std::string name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(m_data->GetCellData())) {
		if (IBC == name.c_str()) { return true; }
	}

	return false;
}

QString PostZoneDataContainer::elevationName() const
{
	for (std::string name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(m_data->GetPointData())) {
		QString tmpName = name.c_str();
		if (tmpName.toLower().left(9) == "elevation") {
			return tmpName;
		}
	}
	return "";
}

vtkPolyData* PostZoneDataContainer::filteredData(double xmin, double xmax, double ymin, double ymax, bool& masked) const
{
	if (vtkStructuredGrid::SafeDownCast(m_data) != nullptr) {
		// Structured Data
		return filteredDataStructured(xmin, xmax, ymin, ymax, masked);
	} else {
		// Unstructured Data
		return filteredDataUnstructured(xmin, xmax, ymin, ymax, masked);
	}
}

vtkPolyData* PostZoneDataContainer::filteredDataStructured(double xmin, double xmax, double ymin, double ymax, bool& masked) const
{
	masked = false;

	double xcenter = (xmin + xmax) * 0.5;
	double ycenter = (ymin + ymax) * 0.5;

	double xwidth = (xmax - xmin);
	double ywidth = (ymax - ymin);

	xmin -= xwidth * 0.2;
	xmax += xwidth * 0.2;
	ymin -= ywidth * 0.2;
	ymax += ywidth * 0.2;

	// 1. Find the grid vertex that is the nearest to the region center.
	vtkIdType vid = m_data->FindPoint(xcenter, ycenter, 0);
	double* cv = m_data->GetPoint(vid);
	if (*cv < xmin || *cv > xmax || *(cv + 1) < ymin || *(cv + 1) > ymax) {
		// 2. If the point is out of the region, the whole grid is out of the region.
		vtkSmartPointer<vtkPolyData> emptyPoly = vtkSmartPointer<vtkPolyData>::New();
		emptyPoly->SetPoints(m_data->GetPoints());
		emptyPoly->Register(0);
		return emptyPoly;
	}

	RectRegion region(xmin, xmax, ymin, ymax);

	int centerI, centerJ, centerK;

	getNodeIJKIndex(vid, &centerI, &centerJ, &centerK);
	int lineLimitIMin, lineLimitIMax, lineLimitJMin, lineLimitJMax;
	double tmpv[3];

	// test I = 0
	m_data->GetPoint(nodeIndex(0, centerJ, 0), tmpv);
	if (region.pointIsInside(tmpv[0], tmpv[1])) {
		lineLimitIMin = 0;
	} else {
		lineLimitIMin = lineLimitI(centerJ, centerI, 0, region);
	}
	// test I = imax
	m_data->GetPoint(nodeIndex(m_sizes[0] - 1, centerJ, 0), tmpv);
	if (region.pointIsInside(tmpv[0], tmpv[1])) {
		lineLimitIMax = m_sizes[0] - 1;
	} else {
		lineLimitIMax = lineLimitI(centerJ, centerI, m_sizes[0] - 1, region);
	}
	// test J = 0
	m_data->GetPoint(nodeIndex(centerI, 0, 0), tmpv);
	if (region.pointIsInside(tmpv[0], tmpv[1])) {
		lineLimitJMin = 0;
	} else {
		lineLimitJMin = lineLimitJ(centerI, centerJ, 0, region);
	}
	// test J = jmax
	m_data->GetPoint(nodeIndex(centerI, m_sizes[1] - 1, 0), tmpv);
	if (region.pointIsInside(tmpv[0], tmpv[1])) {
		lineLimitJMax = m_sizes[1] - 1;
	} else {
		lineLimitJMax = lineLimitJ(centerI, centerJ, m_sizes[1] - 1, region);
	}

	int lineLimitIMin2, lineLimitIMax2, lineLimitJMin2, lineLimitJMax2;

	// test I min direction
	if (lineLimitIMin == 0) {
		lineLimitIMin2 = 0;
	} else {
		if (lineAtIIntersect(0, region)) {
			lineLimitIMin2 = 0;
		} else {
			lineLimitIMin2 = lineLimitI2(lineLimitIMin, 0, region);
		}
	}
	// test I max direction
	if (lineLimitIMax == m_sizes[0] - 1) {
		lineLimitIMax2 = m_sizes[0] - 1;
	} else {
		if (lineAtIIntersect(m_sizes[0] - 1, region)) {
			lineLimitIMax2 = m_sizes[0] - 1;
		} else {
			lineLimitIMax2 = lineLimitI2(lineLimitIMax, m_sizes[0] - 1, region);
		}
	}

	// test J min direction
	if (lineLimitJMin == 0) {
		lineLimitJMin2 = 0;
	} else {
		if (lineAtJIntersect(0, region)) {
			lineLimitJMin2 = 0;
		} else {
			lineLimitJMin2 = lineLimitJ2(lineLimitJMin, 0, region);
		}
	}
	// test J max direction
	if (lineLimitJMax == m_sizes[1] - 1) {
		lineLimitJMax2 = m_sizes[1] - 1;
	} else {
		if (lineAtJIntersect(m_sizes[1] - 1, region)) {
			lineLimitJMax2 = m_sizes[1] - 1;
		} else {
			lineLimitJMax2 = lineLimitI2(lineLimitJMax, m_sizes[1] - 1, region);
		}
	}

	vtkSmartPointer<vtkExtractGrid> exGrid = vtkSmartPointer<vtkExtractGrid>::New();
	exGrid->SetVOI(lineLimitIMin2, lineLimitIMax2, lineLimitJMin2, lineLimitJMax2, 0, 0);
	exGrid->SetInputData(vtkStructuredGrid::SafeDownCast(m_data));
	exGrid->Update();
	vtkSmartPointer<vtkStructuredGrid> extractedGrid = exGrid->GetOutput();
	int exRate = 1;
	bool cullEnable;
	int cullCellLimit, cullIndexLimit;
	Grid::getCullSetting(&cullEnable, &cullCellLimit, &cullIndexLimit);
	while (cullEnable && extractedGrid->GetNumberOfCells() > cullCellLimit){
		exRate *= 2;
		exGrid->SetSampleRate(exRate, exRate, 1);
		exGrid->Update();
		extractedGrid = exGrid->GetOutput();
		masked = true;
	}

	vtkSmartPointer<vtkGeometryFilter> geo = vtkSmartPointer<vtkGeometryFilter>::New();
	geo->SetInputConnection(exGrid->GetOutputPort());

	geo->Update();
	vtkPolyData* data = geo->GetOutput();
	data->Register(0);
	return data;
}

vtkPolyData* PostZoneDataContainer::filteredDataUnstructured(double xmin, double xmax, double ymin, double ymax, bool& masked) const
{
	double xwidth = xmax - xmin;
	double ywidth = ymax - ymin;
	masked = false;

	vtkSmartPointer<vtkGeometryFilter> gfilter = vtkSmartPointer<vtkGeometryFilter>::New();
	gfilter->SetExtent(xmin - xwidth * 0.2, xmax + xwidth * 0.2, ymin - ywidth * 0.2, ymax + ywidth * 0.2, -1, 1);
	gfilter->ExtentClippingOn();
	gfilter->SetInputData(m_data);
	gfilter->Update();
	vtkPolyData* data = gfilter->GetOutput();
	data->Register(0);
	return data;
}

int PostZoneDataContainer::lineLimitI(int j, int iIn, int iOut, const RectRegion& region) const
{
	if (qAbs(iOut - iIn) == 1) {
		return iIn;
	}
	int i = (iIn + iOut) / 2;
	double tmpv[3];
	m_data->GetPoint(nodeIndex(i, j, 0), tmpv);
	if (region.pointIsInside(tmpv[0], tmpv[1])) {
		return lineLimitI(j, i, iOut, region);
	} else {
		return lineLimitI(j, iIn, i, region);
	}
}

int PostZoneDataContainer::lineLimitJ(int i, int jIn, int jOut, const RectRegion& region) const
{
	if (qAbs(jOut - jIn) == 1) {
		return jIn;
	}
	int j = (jIn + jOut) / 2;
	double tmpv[3];
	m_data->GetPoint(nodeIndex(i, j, 0), tmpv);
	if (region.pointIsInside(tmpv[0], tmpv[1])) {
		return lineLimitJ(i, j, jOut, region);
	} else {
		return lineLimitJ(i, jIn, j, region);
	}
}

int PostZoneDataContainer::lineLimitI2(int iIn, int iOut, const RectRegion& region) const
{
	if (qAbs(iOut - iIn) == 1) {
		return iIn;
	}
	int i = (iIn + iOut) / 2;
	if (lineAtIIntersect(i, region)) {
		return lineLimitI2(i, iOut, region);
	} else {
		return lineLimitI2(iIn, i, region);
	}
}

int PostZoneDataContainer::lineLimitJ2(int jIn, int jOut, const RectRegion& region) const
{
	if (qAbs(jOut - jIn) == 1) {
		return jIn;
	}
	int j = (jIn + jOut) / 2;
	if (lineAtJIntersect(j, region)) {
		return lineLimitJ2(j, jOut, region);
	} else {
		return lineLimitJ2(jIn, j, region);
	}
}

bool PostZoneDataContainer::lineAtIIntersect(int i, const RectRegion& region) const
{
	QPointF p1, p2;
	double tmpv[3];
	m_data->GetPoint(nodeIndex(i, 0, 0), tmpv);
	p1 = QPointF(tmpv[0], tmpv[1]);
	for (int j = 1; j < m_sizes[1]; ++j) {
		m_data->GetPoint(nodeIndex(i, j, 0), tmpv);
		p2 = QPointF(tmpv[0], tmpv[1]);
		QLineF line(p1, p2);
		if (region.intersect(line)) {return true;}
	}
	return false;
}

bool PostZoneDataContainer::lineAtJIntersect(int j, const RectRegion& region) const
{
	QPointF p1, p2;
	double tmpv[3];
	m_data->GetPoint(nodeIndex(0, j, 0), tmpv);
	p1 = QPointF(tmpv[0], tmpv[1]);
	for (int i = 1; i < m_sizes[0]; ++i) {
		m_data->GetPoint(nodeIndex(i, j, 0), tmpv);
		p2 = QPointF(tmpv[0], tmpv[1]);
		QLineF line(p1, p2);
		if (region.intersect(line)) {return true;}
	}
	return false;
}

void PostZoneDataContainer::applyOffset(double x_diff, double y_diff)
{
	doApplyOffset(x_diff, y_diff);
}

void PostZoneDataContainer::doApplyOffset(double x_diff, double y_diff)
{
	doApplyOffset(m_data, x_diff, y_diff);
	doApplyOffset(m_particleData, x_diff, y_diff);
	for (auto pair : m_polyDataMap) {
		doApplyOffset(pair.second, x_diff, y_diff);
	}
}

void PostZoneDataContainer::doApplyOffset(vtkPointSet* ps, double x_diff, double y_diff)
{
	vtkPoints* points = ps->GetPoints();
	vtkIdType numPoints = points->GetNumberOfPoints();
	double v[3];
	for (vtkIdType id = 0; id < numPoints; ++id) {
		points->GetPoint(id, v);
		v[0] -= x_diff;
		v[1] -= y_diff;
		points->SetPoint(id, v);
	}
	points->Modified();
}

void PostZoneDataContainer::addCalculatedResultArrays()
{
	for (auto result : m_calculatedResults) {
		if (result->dataType() == PostCalculatedResult::GridNode) {
			auto vals = buildDoubleArray(result->name(), m_data->GetNumberOfPoints());
			m_data->GetPointData()->AddArray(vals);
			vals->Delete();
		} else if (result->dataType() == PostCalculatedResult::GridCell) {
			auto vals = buildDoubleArray(result->name(), m_data->GetNumberOfCells());
			m_data->GetCellData()->AddArray(vals);
			vals->Delete();
		} else if (result->dataType() == PostCalculatedResult::Particle) {
			auto vals = buildDoubleArray(result->name(), m_data->GetNumberOfPoints());
			m_data->GetPointData()->AddArray(vals);
			vals->Delete();
		} else if (result->dataType() == PostCalculatedResult::PolyData) {
			// @todo implement this.
		}
		result->updateValues();
	}
}

void PostZoneDataContainer::doLoadFromProjectMainFile(const QDomNode& node)
{
	for (int i = 0; i < node.childNodes().size(); ++i) {
		auto childNode = node.childNodes().at(i);
		if (childNode.nodeName() == "SimpleOperationResult") {
			auto cr = new PostCalculatedResult(this);
			cr->loadFromProjectMainFile(childNode);
			m_calculatedResults.push_back(cr);
		}
	}
}

void PostZoneDataContainer::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("name", m_zoneName.c_str());

	for (auto result : m_calculatedResults) {
		writer.writeStartElement("SimpleOperationResult");
		result->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

std::vector<PostCalculatedResult*> PostZoneDataContainer::detachCalculatedResult()
{
	std::vector<PostCalculatedResult*> ret;

	for (auto r : m_calculatedResults) {
		r->setParent(nullptr);
		ret.push_back(r);
	}
	m_calculatedResults.clear();

	return ret;
}

void PostZoneDataContainer::attachCalculatedResult(std::vector<PostCalculatedResult*> results)
{
	for (auto r : results) {
		r->setParent(this);
		m_calculatedResults.push_back(r);
	}
}

std::string PostZoneDataContainer::inputDataPrefix()
{
	return "__input__";
}

std::string PostZoneDataContainer::addInputDataPrefix(const std::string& name)
{
	std::string newName = inputDataPrefix();
	newName.append(name.c_str());
	return newName;
}

std::string PostZoneDataContainer::removeInputDataPrefix(const std::string& name)
{
	auto prefix = inputDataPrefix();
	return name.substr(prefix.size());
}

bool PostZoneDataContainer::hasInputDataPrefix(const std::string& name)
{
	auto prefix = inputDataPrefix();
	auto subName = name.substr(0, prefix.size());
	return subName == prefix;
}
