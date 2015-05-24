#ifndef RAWDATAPOINTMAP_H
#define RAWDATAPOINTMAP_H

#include <guicore/pre/rawdata/rawdata.h>
#include <misc/zdepthrange.h>
#include "rawdatapointmaprepresentationdialog.h"
#include <vtkUnstructuredGrid.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPolygon.h>
#include <vtkDoubleArray.h>
#include <vtkActor.h>
#include <vtkLODActor.h>
#include <vtkDataSetMapper.h>
#include <vtkPointLocator.h>
#include <vtkMaskPoints.h>

class QAction;
class QPolygonF;

class RawDataPointMapEditCommand;
class RawDataPMPolygonAddPointCommand;
class RawDataPMFinishDefiningCommand;
class RawDataPointMapEditPtsLessThanCommand;
class RawDataPointMapEditPtsGreaterThanCommand;
class RawDataPointMapEditPtsValueCommand;
class RawDataPMInterpLineAddPointCommand;
class RawDataPMAddPointsCommand;
class RawDataPMAddInterpPtsCommand;
class RawDataPointmapBreakLine;
class RawDataPointmapBreakLineAddCommand;
class RawDataPointmapBreakLineAddPointCommand;
class RawDataPointmapBreakLineFinishDefinitionCommand;
class RawDataPointmapBreakLineCancelDefinitionCommand;
class RawDataPointMapAddPointSetReferenceCommand;

/// This class handles the coordinates of points (X, Y), and attribute values
/// defined in at the position of the points.
/**
 * The attibute value type can be a real number, or an integer number.
 * We often use TIN to map attribute values defined at grid nodes,
 * se we use vtkUnstructuredGrid instance to hold the data.
 * vtkUnstructuredGrid can handle so many types of cells, but
 * in this class, all cells in the vtkGrid() are vtkTriangle instances.
 *
 * All Z values of vertices are 0.
 */
class RawDataPointmap : public RawData
{
	Q_OBJECT
public:
	enum MappingMode {
		mmTIN,
		mmTemplate
	};
	enum MouseEventMode {
		meNormal,
		meNormalWithShift,
		meSMBox,
		meSMBoxPrepare,
		meSMBoxNotPossible,
		meSMPolygon,
		meSMPolygonPrepare,
		meSMPolygonNotPossible,

		meAddPointSelectReferenceNotPossible,
		meAddPointSelectReference,
		meAddPoint,

		meSMAddPoint,        // depricated
		meSMAddCtrlPoint,    // depricated
		meSMAddPointPrepare, // depricated
		meSMInterpPoint,
		meSMInterpCtrlPoint,
		meSMInterpPointPrepare,
		meSMInterpPointNotPossible,
		meBreakLineAddNotPossible,
		meBreakLineAdd,
		meBreakLineRemoveNotPossible,
		meBreakLineRemove
	};
	/// Constructor
	RawDataPointmap(ProjectDataItem* d, RawDataCreator* creator, SolverDefinitionGridRelatedCondition* condition);
	virtual ~RawDataPointmap();
	vtkPolyData* vtkGrid() {return m_vtkGrid;}
	vtkPolyData* delaunayedPolyData() {return m_vtkDelaunayedPolyData;}
	const QPolygonF polygon();

	/// Execute the delaunay division.
	bool doDelaunay(bool allowCancel = false);
	void setupActors();
	void setupActions();
	void setupMenu();
	bool addToolBarButtons(QToolBar* /*parent*/) override;
	void setPoints(vtkPoints* points, vtkDataArray* values);
	void setSTLData(vtkPolyData* data, vtkDataArray* values);
	void updateZDepthRangeItemCount(ZDepthRange& range);
	void assignActorZValues(const ZDepthRange& range) override;
	QDialog* propertyDialog(QWidget* parent);
	void handlePropertyDialogAccepted(QDialog* propDialog);
	void updateRepresentation();
	void updateActorSettings();
	void addCustomMenuItems(QMenu* menu) override;
	void keyPressEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v);
	void keyReleaseEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v);
	void mouseDoubleClickEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v);
	void mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v);
	void mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v);
	void mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v);
	vtkPolygon* getVtkPolygon() {return m_vtkPolygon;}
	vtkPolygon* getVtkInterpPolygon() {return m_vtkInterpPolygon;}
	vtkDoubleArray* getVtkInterpValue() {return m_vtkInterpValue;}
	vtkPolyData* selectedVerticesGrid() {return m_selectedVerticesGrid;}
	QVector<vtkIdType> selectedVertices();
	void definePolygon(bool doubleClick, bool xOr);
	void showPolygonSelectedPoints(bool xOr);
	void selectPointsInsideBox(MouseBoundingBox* box, bool xOr);
	void selectPointsNearPoint(const QVector2D& pos, bool xOr);
	void clearPointsSelection();
	void clearNewPoints();
	void resetSelectionPolygon();
	void resetSelectedInterp();
	void unwindSelectedInterp();
	void enablePointSelectedActions(bool val);
	void finishAddPoint();
	void finishInterpPoint();
	bool needRemeshing() {return m_needRemeshing;}

public slots:
	void remeshTINS(bool nodialog = false);

private:
	bool checkBreakLines();
	void updateBreakLinesOnDelete(QVector<vtkIdType>& deletedPoints);
	void updateBreakLinesOnInsert(QVector<vtkIdType>& deletedPoints);
	bool pointsUsedForBreakLines(const QVector<vtkIdType>& points);

	MappingMode m_mappingMode;
	QPoint m_dragStartPoint;
	QPoint m_currentPoint;
	QPoint m_mouseMovePoint;

	MouseEventMode m_mouseEventMode;
	void buildGridFromPolydata();
	void updateActionStatus();
	void updateMouseEventMode();
	bool isVertexSelectable(const QVector2D& pos);
	int m_selectedVertexId;
	int m_selectedVertexId2;
	double m_selectedZPos;
	QPixmap m_addPixmap;
	QCursor m_addCursor;
	QPixmap m_removePixmap;
	QCursor m_removeCursor;
	QPixmap m_interpPointAddPixmap;
	QCursor m_interpPointAddCursor;
	QPixmap m_interpPointCtrlAddPixmap;
	QCursor m_interpPointCtrlAddCursor;
	bool m_canceled;
	void updateShapeData();
	void updateInterpShapeData();

private slots:
	void showDisplaySetting();
	void selectionModePoint(bool on);
	void selectionModeBox(bool on);
	void selectionModePolygon(bool on);
	void interpolatePoints(bool on);
	void addPoints(bool on);

	void addBreakLine();
	void removeBreakLine();
	void removeAllBreakLines();

	void editPoints();
	void editPointsDelete();
	void editPointsExport();
	void editPointsLessThan();
	void editPointsGreaterThan();
	void cancel() {m_canceled = true;}
protected:
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
	void updateFilename();
	void loadExternalData(const QString& filename);
	void saveExternalData(const QString& filename);
	void updateMouseCursor(PreProcessorGraphicsViewInterface* v);
	void doApplyOffset(double x, double y);

protected:
	/// The polygon data container.
	vtkSmartPointer<vtkPolygon> m_vtkPolygon;
	vtkSmartPointer<vtkPolygon> m_vtkInterpPolygon;
	vtkSmartPointer<vtkDoubleArray> m_vtkInterpValue;

	vtkSmartPointer<vtkPolyData> m_vtkGrid;
	vtkSmartPointer<vtkPolyData> m_vtkDelaunayedPolyData;

	QList<RawDataPointmapBreakLine*> m_breakLines;
	RawDataPointmapBreakLine* m_activeBreakLine;

	vtkSmartPointer<vtkPolyData> m_vtkSelectedPolyData;
	int m_opacityPercent;

	vtkSmartPointer<vtkPointLocator> m_vtkPointLocator;

	RawDataPointmapRepresentationDialog::Representation m_representation;
	bool m_hideBreakLines;
	int m_pointSize;

	vtkSmartPointer<vtkMaskPoints> m_maskPoints10k;
	vtkSmartPointer<vtkMaskPoints> m_maskPoints40k;

	vtkSmartPointer<vtkPolyDataMapper> m_maskPoints10kMapper;
	vtkSmartPointer<vtkPolyDataMapper> m_maskPoints40kMapper;

	vtkSmartPointer<vtkPolyDataMapper> m_vtkMapper;
	vtkSmartPointer<vtkLODActor> m_actor;

	vtkSmartPointer<vtkPolyDataMapper> m_pointsMapper;
	vtkSmartPointer<vtkLODActor> m_pointsActor;

	vtkSmartPointer<vtkUnstructuredGrid> m_polyEdgeGrid;
	vtkSmartPointer<vtkDataSetMapper> m_polyEdgeMapper;
	vtkSmartPointer<vtkActor> m_polyEdgeActor;

	vtkSmartPointer<vtkUnstructuredGrid> m_polyVertexGrid;
	vtkSmartPointer<vtkDataSetMapper> m_polyVertexMapper;
	vtkSmartPointer<vtkActor> m_polyVertexActor;

	double m_newPointValue;
	vtkSmartPointer<vtkPolyData> m_newPoints;
	vtkSmartPointer<vtkPolyDataMapper> m_newPointsMapper;
	vtkSmartPointer<vtkActor> m_newPointsActor;

	vtkSmartPointer<vtkPolyData> m_selectedVerticesGrid;
	vtkSmartPointer<vtkPolyDataMapper> m_selectedMapper;
	vtkSmartPointer<vtkActor> m_selectedActor;

	vtkSmartPointer<vtkUnstructuredGrid> m_InterpLineGrid;
	vtkSmartPointer<vtkDataSetMapper> m_InterpLineMapper;
	vtkSmartPointer<vtkActor> m_InterpLineActor;

	QAction* m_selectionModePoint;
	QAction* m_selectionModeBox;
	QAction* m_selectionModePolygon;
	QAction* m_addPointAction;
	QAction* m_interpolatePointAction;
	//QAction* m_editAction;
	QAction* m_mappingModeAction;
	QAction* m_displaySettingAction;

	QAction* m_editPointsAction;
	QAction* m_editPointsDeleteAction;
	QAction* m_editPointsExportAction;
	QAction* m_editPointsLessThanAction;
	QAction* m_editPointsGreaterThanAction;

	QAction* m_remeshAction;
	QAction* m_addBreakLineAction;
	QAction* m_removeBreakLineAction;
	QAction* m_removeAllBreakLinesAction;

	QMenu*   m_rightClickingMenu;

	bool lastInterpPointKnown;
	bool doubleclick;
	/// When Points are added/deleted after the last remeshing, this flag is true.
	bool m_needRemeshing;
	ZDepthRange m_zDepthRange;

public:
	friend class RawDataPointmapBreakLine;
	friend class RawDataPointMapEditCommand;
	friend class RawDataPMPolygonAddPointCommand;
	friend class RawDataPMFinishDefiningCommand;
	friend class RawDataPointMapEditPtsLessThanCommand;
	friend class RawDataPointMapEditPtsGreaterThanCommand;
	friend class RawDataPointMapEditPtsValueCommand;
	friend class RawDataPMInterpLineAddPointCommand;
	friend class RawDataPMAddPointsCommand;
	friend class RawDataPMAddInterpPtsCommand;
	friend class RawDataPointMapDeletePtsCommand;
	friend class RawDataPointMapDeletePointsCommand;
	friend class RawDataPointMapInsertNewPoints;
	friend class RawDataPointMapEditPoints;
	friend class RawDataPointMapEditSinglePoint;
	friend class RawDataPointmapBreakLineAddCommand;
	friend class RawDataPointmapBreakLineAddPointCommand;
	friend class RawDataPointmapBreakLineFinishDefinitionCommand;
	friend class RawDataPointmapBreakLineCancelDefinitionCommand;
	friend class RawDataPointMapAddPointSetReferenceCommand;
};

#endif // RAWDATAPOINTMAP_H
