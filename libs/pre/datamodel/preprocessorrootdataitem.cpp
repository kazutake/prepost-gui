#include "../misc/preprocessorgridattributemappingmode.h"
#include "../preobjectbrowserview.h"
#include "../preprocessordatamodel.h"
#include "../preprocessorgraphicsview.h"
#include "../preprocessorwindow.h"
#include "preprocessorbackgroundimagesdataitem.h"
#include "preprocessorbcdataitem.h"
#include "preprocessorbcsettingdataitem.h"
#include "preprocessorbcsettinggroupdataitem.h"
#include "preprocessorgridandgridcreatingconditiondataitem.h"
#include "preprocessorgridattributemappingsettingtopeditdialog.h"
#include "preprocessorgridcreatingconditiondataitem.h"
#include "preprocessorgriddataitem.h"
#include "preprocessorgridtypedataitem.h"
#include "preprocessorinputconditiondataitem.h"
#include "preprocessormeasureddatatopdataitem.h"
#include "preprocessorrawdatadataitem.h"
#include "preprocessorrawdatagroupdataitem.h"
#include "preprocessorrawdatatopdataitem.h"
#include "preprocessorrootdataitem.h"

#include <guicore/axis2d/axis2ddataitem.h>
#include <guicore/datamodel/attributebrowsertargetdataitem.h>
#include <guicore/distancemeasure/distancemeasuregroupdataitem.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/base/preprocessorgridcreatingconditiondataiteminterface.h>
#include <guicore/pre/base/preprocessorgriddataiteminterface.h>
#include <guicore/pre/base/preprocessorrawdatatopdataiteminterface.h>
#include <guicore/project/projectdata.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/xmlsupport.h>
#include <rawdata/pointmap/rawdatapointmapmappingmode.h>
#include <rawdata/pointmap/rawdatapointmaptemplatemappingsetting.h>

#include <QAction>
#include <QSettings>
#include <QStandardItemModel>
#include <QUndoCommand>
#include <QXmlStreamWriter>

#include <vtkRenderWindow.h>

PreProcessorRootDataItem::PreProcessorRootDataItem(PreProcessorWindow* window, ProjectDataItem* parent)
	: GraphicsWindowRootDataItem(window, parent)
{
	QSettings settings;
	int mm = settings.value("general/gridmappingmode", 0).value<int>();
	PreProcessorGridAttributeMappingMode::mode = static_cast<PreProcessorGridAttributeMappingMode::Mode>(mm);

	SolverDefinition* def = projectData()->solverDefinition();
	const QList<SolverDefinitionGridType*>& types = def->gridTypes();

	// build grid type data items.
	int i = 0;
	QList<PreProcessorGridTypeDataItem*> gtitems;
	for (auto it = types.begin(); it != types.end(); ++it) {
		SolverDefinitionGridType* gt = *it;
		if (gt->defaultGridType() == SolverDefinitionGridType::gtUnknownGrid) {
			// dummy grid type.
			continue;
		}
		PreProcessorGridTypeDataItem* item = new PreProcessorGridTypeDataItem(*it, this);
		m_gridTypeDataItems.append(item);
		gtitems.append(item);
		++i;
	}
	// Measured values data item node.
	m_measuredDataTopDataItem = new PreProcessorMeasuredDataTopDataItem(this);
	m_childItems.append(m_measuredDataTopDataItem);

	for (int j = 0; j < gtitems.count(); ++j) {
		m_childItems.append(gtitems.at(j));
	}
	// Background images data item node.
	m_backgroundImagesDataItem = new PreProcessorBackgroundImagesDataItem(this);
	m_childItems.append(m_backgroundImagesDataItem);

	m_editGridAttributeMappingSettingAction = new QAction(tr("&Setting..."), this);
	connect(m_editGridAttributeMappingSettingAction, SIGNAL(triggered()), this, SLOT(editGridAttributeMappingSetting()));

	// Input condition
	m_inputConditionDataItem = new PreProcessorInputConditionDataItem(this);
	m_childItems.append(m_inputConditionDataItem);

	m_axesDataItem = new Axis2dDataItem(this);
	m_childItems.append(m_axesDataItem);

	m_distanceMeasureGroupDataItem = new DistanceMeasureGroupDataItem(this);
	m_childItems.push_front(m_distanceMeasureGroupDataItem);

	m_attributeBrowserTargetDataItem = new AttributeBrowserTargetDataItem(this);
	m_childItems.push_front(m_attributeBrowserTargetDataItem);

	updateZDepthRangeItemCount();
	// update item map initially.
	updateItemMap();
}

PreProcessorRootDataItem::~PreProcessorRootDataItem()
{
	delete m_measuredDataTopDataItem;
	delete m_backgroundImagesDataItem;
	delete m_axesDataItem;
	delete m_distanceMeasureGroupDataItem;
	delete m_attributeBrowserTargetDataItem;
	for (auto gt : m_gridTypeDataItems) {
		delete gt;
	}
}

void PreProcessorRootDataItem::setupStandardModel(QStandardItemModel* model)
{
	model->clear();
	// add gridtypes.
	SolverDefinition* def = projectData()->solverDefinition();
	if (def->gridTypes().count() == 1) {
		// If there is no gridtypeDataitem, do nothing.
		if (m_gridTypeDataItems.begin() != m_gridTypeDataItems.end()) {
			// Current solver support only one grid type, and it does not allow multiple grids to input.
			// So, construct a simplified object tree, in the object browser.
			PreProcessorGridTypeDataItem* item = *(m_gridTypeDataItems.begin());
			// take Raw Data item. and put them to the model.
			PreProcessorRawDataTopDataItemInterface* rtitem = item->rawdataTop();
			item->standardItem()->takeChild(rtitem->standardItem()->row());
			model->appendRow(rtitem->standardItem());
			// GridCreatingCondition, Grid are added as top item.
			PreProcessorGridAndGridCreatingConditionDataItem* citem = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItem*>(*(item->conditions().begin()));
			PreProcessorGridCreatingConditionDataItemInterface* gccitem = citem->creatingConditionDataItem();
			citem->standardItem()->takeChild(gccitem->standardItem()->row());
			model->appendRow(gccitem->standardItem());
			// Boundary condition setting node here if needed.
			PreProcessorGridDataItem* gitem = dynamic_cast<PreProcessorGridDataItem*>(citem->gridDataItem());
			if (gitem->bcGroupDataItem() != nullptr) {
				PreProcessorBCSettingGroupDataItem* bcsgitem = citem->bcSettingGroupDataItem();
				citem->standardItem()->takeChild(bcsgitem->standardItem()->row());
				model->appendRow(bcsgitem->standardItem());
			}
			// Grid.
			citem->standardItem()->takeChild(gitem->standardItem()->row());
			model->appendRow(gitem->standardItem());
		}
	} else {
		// current solver support more than two grid types, or the only
		// grid type allows multiple grids.
		for (auto it = m_gridTypeDataItems.begin(); it != m_gridTypeDataItems.end(); ++it) {
			model->appendRow((*it)->standardItem());
		}
	}
	// add measured values item row.
	model->appendRow(m_measuredDataTopDataItem->standardItem());
	// add background item row.
	model->appendRow(m_backgroundImagesDataItem->standardItem());
	// add axes item row.
	model->appendRow(m_axesDataItem->standardItem());
	// add distance measure
	model->appendRow(m_distanceMeasureGroupDataItem->standardItem());

	model->appendRow(m_attributeBrowserTargetDataItem->standardItem());
}

void PreProcessorRootDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();
	PreProcessorGridAttributeMappingMode::mode = static_cast<PreProcessorGridAttributeMappingMode::Mode>(elem.attribute("mappingMode", "0").toInt());
	RawDataPointmapMappingMode::mode = static_cast<RawDataPointmapMappingMode::Mode>(elem.attribute("geoMappingMode", "0").toInt());
	RawDataPointmapTemplateMappingSetting& setting = RawDataPointmapTemplateMappingSetting::setting;
	setting.tempAutoMode = static_cast<bool>(elem.attribute("tempAutoRegion", "1").toInt());
	double tmpdbl = elem.attribute("tempStreamWiseLength", "0").toDouble();
	if (tmpdbl != 0) {setting.tempStreamWiseLength = tmpdbl;}
	tmpdbl = elem.attribute("tempCrossStreamLength", "0").toDouble();
	if (tmpdbl != 0) {setting.tempCrossStreamLength = tmpdbl;}
	setting.tempNumExpansion = elem.attribute("numExpansion", "3").toInt();
	setting.tempWeightExponent = elem.attribute("weightExponent", "1").toDouble();

	QDomNode bgNode = iRIC::getChildNode(node, "BackgroundImages");
	if (! bgNode.isNull()) {m_backgroundImagesDataItem->loadFromProjectMainFile(bgNode);}

	for (auto it = m_gridTypeDataItems.begin(); it != m_gridTypeDataItems.end(); ++it) {
		QDomNode c = node.firstChild();
		while (! c.isNull()) {
			if (c.nodeName() == "GridType" && c.toElement().attribute("name") == (*it)->name()) {
				(*it)->loadFromProjectMainFile(c);
			}
			c = c.nextSibling();
		}
	}
	SolverDefinition* def = projectData()->solverDefinition();
	const SolverDefinitionGridType* firstType = *(def->gridTypes().begin());
	if (def->gridTypes().count() == 1 && !(firstType->multiple())) {
		// Current solver support only one grid type, and it does not allow multiple grids to input.
		// The only, and hidden gridtype node should be checked always.
		PreProcessorGridTypeDataItem* gtItem = *(m_gridTypeDataItems.begin());
		gtItem->standardItem()->setCheckState(Qt::Checked);
	}
	QDomNode mdNode = iRIC::getChildNode(node, "MeasuredDatas");
	if (! mdNode.isNull()) {m_measuredDataTopDataItem->loadFromProjectMainFile(mdNode);}
	QDomNode icNode = iRIC::getChildNode(node, "InputCondition");
	if (! icNode.isNull()) {m_inputConditionDataItem->loadFromProjectMainFile(icNode);}
	QDomNode axesNode = iRIC::getChildNode(node, "Axes");
	if (! axesNode.isNull()) {m_axesDataItem->loadFromProjectMainFile(axesNode);}
	QDomNode dmNode = iRIC::getChildNode(node, "DistanceMeasures");
	if (! dmNode.isNull()) {m_distanceMeasureGroupDataItem->loadFromProjectMainFile(dmNode);}
	updateItemMap();
	updateZDepthRange();
//	dataModel()->graphicsView()->ResetCameraClippingRange();
//	updateVisibility();
}
void PreProcessorRootDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	QString tmpstr;
	tmpstr.setNum(PreProcessorGridAttributeMappingMode::mode);
	writer.writeAttribute("mappingMode", tmpstr);
	tmpstr.setNum(RawDataPointmapMappingMode::mode);
	writer.writeAttribute("geoMappingMode", tmpstr);

	RawDataPointmapTemplateMappingSetting& setting = RawDataPointmapTemplateMappingSetting::setting;
	tmpstr.setNum(setting.tempAutoMode);
	writer.writeAttribute("tempAutoRegion", tmpstr);
	tmpstr.setNum(setting.tempStreamWiseLength);
	writer.writeAttribute("tempStreamWiseLength", tmpstr);
	tmpstr.setNum(setting.tempCrossStreamLength);
	writer.writeAttribute("tempCrossStreamLength", tmpstr);
	tmpstr.setNum(setting.tempNumExpansion);
	writer.writeAttribute("numExpansion", tmpstr);
	tmpstr.setNum(setting.tempWeightExponent);
	writer.writeAttribute("weightExponent", tmpstr);

	writer.writeStartElement("BackgroundImages");
	m_backgroundImagesDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();

	for (auto it = m_gridTypeDataItems.begin(); it != m_gridTypeDataItems.end(); ++it) {
		writer.writeStartElement("GridType");
		(*it)->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	writer.writeStartElement("MeasuredDatas");
	m_measuredDataTopDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();

	writer.writeStartElement("InputCondition");
	m_inputConditionDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();

	writer.writeStartElement("Axes");
	m_axesDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();
	writer.writeStartElement("DistanceMeasures");
	m_distanceMeasureGroupDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();
}

PreProcessorGridTypeDataItem* PreProcessorRootDataItem::gridTypeDataItem(const QString& name)
{
	for (auto it = m_gridTypeDataItems.begin(); it != m_gridTypeDataItems.end(); ++it) {
		if ((*it)->name() == name) {return *it;}
	}
	return nullptr;
}

class PreProcessorRootDataItemSetMappingSettingCommand : public QUndoCommand
{
public:
	PreProcessorRootDataItemSetMappingSettingCommand(PreProcessorGridAttributeMappingMode::Mode mm, RawDataPointmapMappingMode::Mode gmm, bool a, double streamWise, double crossStream, int numExp, double exp, PreProcessorRootDataItem* i)
		: QUndoCommand(PreProcessorRootDataItem::tr("Change Attribute Mapping Setting")) {
		m_newMappingMode = mm;
		m_newGeodataMappingMode = gmm;
		m_newTempAutoMode = a;
		m_newTempStreamWiseLength = streamWise;
		m_newTempCrossStreamLength = crossStream;
		m_newTempNumExpansion = numExp;
		m_newTempWeightExponent = exp;

		m_oldMappingMode = PreProcessorGridAttributeMappingMode::mode;
		m_oldGeodataMappingMode = RawDataPointmapMappingMode::mode;
		RawDataPointmapTemplateMappingSetting& s = RawDataPointmapTemplateMappingSetting::setting;
		m_oldTempAutoMode = s.tempAutoMode;
		m_oldTempStreamWiseLength = s.tempStreamWiseLength;
		m_oldTempCrossStreamLength = s.tempCrossStreamLength;
		m_oldTempNumExpansion = s.tempNumExpansion;
		m_oldTempWeightExponent = s.tempWeightExponent;

		m_item = i;
	}

	~PreProcessorRootDataItemSetMappingSettingCommand() {}
	void redo() {
		PreProcessorGridAttributeMappingMode::mode = m_newMappingMode;
		RawDataPointmapMappingMode::mode = m_newGeodataMappingMode;
		RawDataPointmapTemplateMappingSetting& s = RawDataPointmapTemplateMappingSetting::setting;
		s.tempAutoMode = m_newTempAutoMode;
		s.tempStreamWiseLength = m_newTempStreamWiseLength;
		s.tempCrossStreamLength = m_newTempCrossStreamLength;
		s.tempNumExpansion = m_newTempNumExpansion;
		s.tempWeightExponent = m_newTempWeightExponent;
	}
	void undo() {
		PreProcessorGridAttributeMappingMode::mode = m_oldMappingMode;
		RawDataPointmapMappingMode::mode = m_oldGeodataMappingMode;
		RawDataPointmapTemplateMappingSetting& s = RawDataPointmapTemplateMappingSetting::setting;
		s.tempAutoMode = m_oldTempAutoMode;
		s.tempStreamWiseLength = m_oldTempStreamWiseLength;
		s.tempCrossStreamLength = m_oldTempCrossStreamLength;
		s.tempNumExpansion = m_oldTempNumExpansion;
		s.tempWeightExponent = m_oldTempWeightExponent;
	}
private:
	PreProcessorGridAttributeMappingMode::Mode m_newMappingMode;
	RawDataPointmapMappingMode::Mode m_newGeodataMappingMode;
	bool m_newTempAutoMode;
	double m_newTempStreamWiseLength;
	double m_newTempCrossStreamLength;
	int m_newTempNumExpansion;
	double m_newTempWeightExponent;

	PreProcessorGridAttributeMappingMode::Mode m_oldMappingMode;
	RawDataPointmapMappingMode::Mode m_oldGeodataMappingMode;
	bool m_oldTempAutoMode;
	double m_oldTempStreamWiseLength;
	double m_oldTempCrossStreamLength;
	int m_oldTempNumExpansion;
	double m_oldTempWeightExponent;

	PreProcessorRootDataItem* m_item;
};


void PreProcessorRootDataItem::editGridAttributeMappingSetting()
{
	PreProcessorGridAttributeMappingSettingTopEditDialog dialog(mainWindow());
	dialog.setMappingMode(PreProcessorGridAttributeMappingMode::mode);
	dialog.setPointmapMappingMode(RawDataPointmapMappingMode::mode);
	RawDataPointmapTemplateMappingSetting& s = RawDataPointmapTemplateMappingSetting::setting;
	dialog.setAutoMode(s.tempAutoMode);
	dialog.setStreamWiseLength(s.tempStreamWiseLength);
	dialog.setCrossStreamLength(s.tempCrossStreamLength);
	dialog.setNumExpansion(s.tempNumExpansion);
	dialog.setWeightExponent(s.tempWeightExponent);

	if (QDialog::Accepted != dialog.exec()) {return;}

	iRICUndoStack::instance().push(new PreProcessorRootDataItemSetMappingSettingCommand(dialog.mappingMode(), dialog.pointmapMappingMode(), dialog.autoMode(), dialog.streamWiseLength(), dialog.crossStreamLength(), dialog.numExpansion(), dialog.weightExponent(), this));
}

bool PreProcessorRootDataItem::gridEdited()
{
	for (auto it = m_gridTypeDataItems.begin(); it != m_gridTypeDataItems.end(); ++it) {
		bool edited = (*it)->gridEdited();
		if (edited) {return true;}
	}
	return false;
}

void PreProcessorRootDataItem::toggleGridEditFlag()
{
	for (auto it = m_gridTypeDataItems.begin(); it != m_gridTypeDataItems.end(); ++it) {
		(*it)->toggleGridEditFlag();
	}
}

void PreProcessorRootDataItem::deleteItem(QStandardItem* item)
{
	GraphicsWindowDataItem* dataItem = modelItemFromItem(item);
	PreProcessorRawdataDataItem* dItem = dynamic_cast<PreProcessorRawdataDataItem*>(dataItem);
	PreProcessorRawDataGroupDataItem* gItem = nullptr;
	PreProcessorBCSettingDataItem* bcsitem = dynamic_cast<PreProcessorBCSettingDataItem*>(dataItem);
	if (dItem != nullptr) {
		gItem = dynamic_cast<PreProcessorRawDataGroupDataItem*>(dItem->parent());
	}

	if (bcsitem != nullptr) {
		GraphicsWindowRootDataItem::deleteItem(bcsitem->bcDataItem()->standardItem());
	} else {
		GraphicsWindowRootDataItem::deleteItem(item);
	}

	if (gItem != nullptr) {
		gItem->informDataChange();
	}
}

void PreProcessorRootDataItem::saveToCgnsFile(const int fn)
{
	cg_iRIC_InitRead(fn);
	cg_iRIC_Clear_Complex();
	GraphicsWindowDataItem::saveToCgnsFile(fn);
}
