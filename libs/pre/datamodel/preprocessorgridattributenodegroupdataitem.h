#ifndef PREPROCESSORGRIDATTRIBUTENODEGROUPDATAITEM_H
#define PREPROCESSORGRIDATTRIBUTENODEGROUPDATAITEM_H

#include <guicore/misc/targeted/targeteditemi.h>
#include <guicore/pre/base/preprocessordataitem.h>
#include <misc/opacitycontainer.h>
#include <QMap>
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>
#include <vtkContourFilter.h>

#include <string>

class QAction;
class NamedGraphicWindowDataItem;
class PreProcessorGridAttributeNodeDataItem;

class PreProcessorGridAttributeNodeGroupDataItem : public PreProcessorDataItem , public TargetedItemI
{
	Q_OBJECT

public:
	PreProcessorGridAttributeNodeGroupDataItem(PreProcessorDataItem* parent);
	~PreProcessorGridAttributeNodeGroupDataItem();

	std::string target() const override;
	void setTarget(const std::string& target) override;

	void updateActorSettings();
	void informDataChange(const std::string& name);
	void setupActors();
	void updateZDepthRangeItemCount() override;
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void assignActorZValues(const ZDepthRange& range) override;
	void informGridUpdate();
	const QList<PreProcessorGridAttributeNodeDataItem*> conditions() const;
	PreProcessorGridAttributeNodeDataItem* nodeDataItem(const std::string& name) {return m_nameMap.value(name, 0);}
	void handleStandardItemChange() override;
	void setOpacity(int o) {m_opacity = o;}
	int opacity() {return m_opacity;}
	void informSelectedVerticesChanged(const QVector<vtkIdType>& vertices);
	QAction* showAttributeBrowserAction() const {return m_showAttributeBrowserAction;}
	void addCustomMenuItems(QMenu* menu) override;
	void initAttributeBrowser();
	void clearAttributeBrowser();
	void fixAttributeBrowser(const QPoint& p, VTKGraphicsView* v);
	void updateAttributeBrowser(const QPoint& p, VTKGraphicsView* v);
	bool addToolBarButtons(QToolBar* toolbar) override;

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);
	void showAttributeBrowser();

protected:
	PreProcessorGridAttributeNodeDataItem* activeChildItem();
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void updateAttributeBrowser(vtkIdType vid, double x, double y, VTKGraphicsView* v);

private:
	vtkIdType findVertex(const QPoint& p, VTKGraphicsView* v);

	std::string m_target;
	vtkSmartPointer<vtkPolyData> m_contourPolyData;
	vtkSmartPointer<vtkActor> m_contourActor;
	vtkSmartPointer<vtkDataSetMapper> m_contourMapper;
	vtkSmartPointer<vtkActor> m_isolineActor;
	vtkSmartPointer<vtkPolyDataMapper> m_isolineMapper;
	vtkSmartPointer<vtkContourFilter> m_isolineFilter;
	vtkSmartPointer<vtkActor> m_fringeActor;
	vtkSmartPointer<vtkDataSetMapper> m_fringeMapper;

	QAction* m_showAttributeBrowserAction;
	OpacityContainer m_opacity;
	bool m_attributeBrowserFixed;
	QMap<std::string, PreProcessorGridAttributeNodeDataItem*> m_nameMap;
};

#endif // PREPROCESSORGRIDATTRIBUTENODEGROUPDATAITEM_H
