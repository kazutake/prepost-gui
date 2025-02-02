#include "../../../guibase/objectbrowserview.h"
#include "post2dwindowcontoursettingdialog.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindowcellscalargrouptopdataitem.h"
#include "post2dwindowcellscalargroupdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guicore/datamodel/vtkgraphicsview.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinition.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>

#include <QDomNode>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QStandardItem>
#include <QXmlStreamWriter>

#include <vtkCellData.h>
#include <vtkPointData.h>

#include <set>
#include <map>

Post2dWindowCellScalarGroupTopDataItem::Post2dWindowCellScalarGroupTopDataItem(Post2dWindowDataItem* p) :
	Post2dWindowDataItem {tr("Scalar (cell center)"), QIcon(":/libs/guibase/images/iconFolder.png"), p}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	Post2dWindowGridTypeDataItem* gtItem = dynamic_cast<Post2dWindowGridTypeDataItem*>(parent()->parent());
	for (const auto& val : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(cont->data()->GetCellData())) {
		if (PostZoneDataContainer::hasInputDataPrefix(val)) {continue;}

		m_colorbarTitleMap.insert(val, val.c_str());
		auto item = new Post2dWindowCellScalarGroupDataItem(this, NotChecked, NotReorderable, NotDeletable);
		m_scalarmap[val] = item;
		m_childItems.push_back(item);
		item->setTarget(val);
		item->m_lookupTableContainer = gtItem->nodeLookupTable(val);
	}
}

Post2dWindowCellScalarGroupTopDataItem::~Post2dWindowCellScalarGroupTopDataItem()
{
}

void Post2dWindowCellScalarGroupTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	Q_ASSERT(node.toElement().nodeName() == "ScalarCellCenter");
	if (node.toElement().nodeName() == "ScalarCellCenter") {
		// load contours from main file
		QDomNodeList children = node.childNodes();
		std::set<Post2dWindowCellScalarGroupDataItem*> missing_quadrant;
		for (int i = 0; i < children.count(); ++i) {
			QDomElement childElem = children.at(i).toElement();
			if (childElem.nodeName() == "ScalarGroup") {
				std::string solution = iRIC::toStr(children.at(i).toElement().attribute("solution", ""));
				if (solution.size()) {
					auto it = m_scalarmap.find(solution);
					if (it != m_scalarmap.end()) {
						(*it).second->loadFromProjectMainFile(children.at(i));
						// store checked items that have no quadrant set
						if ((*it).second->m_standardItem->checkState() != Qt::Unchecked) {
							if ((*it).second->m_setting.scalarBarSetting.quadrant == ScalarBarSetting::Quadrant::None) {
								missing_quadrant.insert((*it).second);
							}
						}
					}
				}
			}
		}

		Q_ASSERT(missing_quadrant.size() <= 4);
		std::set<ScalarBarSetting::Quadrant> quads = ScalarBarSetting::getQuadrantSet();
		while (missing_quadrant.size() && quads.size()) {
			// find closest item to each quadrant
			auto quad = quads.begin();
			std::multimap<double, Post2dWindowCellScalarGroupDataItem*> closest;
			for (auto item : missing_quadrant) {
				closest.insert({item->m_setting.scalarBarSetting.distanceFromDefault(*quad), item});
			}
			closest.begin()->second->m_setting.scalarBarSetting.quadrant = *quad;
			missing_quadrant.erase(closest.begin()->second);
			quads.erase(quad);
		}

		updateItemMap();
		updateVisibilityWithoutRendering();
	}
}

void Post2dWindowCellScalarGroupTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	// scalar bar titles
	writer.writeStartElement("ScalarBarTitles");
	QMapIterator<std::string, QString> i(m_colorbarTitleMap);
	while (i.hasNext()) {
		i.next();
		if (i.key().size() > 0) {
			writer.writeStartElement("ScalarBarTitle");
			writer.writeAttribute("value", i.key().c_str());
			writer.writeAttribute("title", i.value());
			writer.writeEndElement();
		}
	}
	writer.writeEndElement();

	// contours
	for (auto item : m_childItems) {
		writer.writeStartElement("ScalarGroup");
		item->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void Post2dWindowCellScalarGroupTopDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount((unsigned int)m_childItems.size());
}

void Post2dWindowCellScalarGroupTopDataItem::assignActorZValues(const ZDepthRange& range)
{
	for (auto item : m_childItems) {
		Post2dWindowCellScalarGroupDataItem* typedi = dynamic_cast<Post2dWindowCellScalarGroupDataItem*>(item);
		typedi->assignActorZValues(range);
	}
}

void Post2dWindowCellScalarGroupTopDataItem::update()
{
	// forward to children
	for (auto item : m_childItems) {
		Post2dWindowCellScalarGroupDataItem* typedi = dynamic_cast<Post2dWindowCellScalarGroupDataItem*>(item);
		typedi->update();
	}
}

QDialog* Post2dWindowCellScalarGroupTopDataItem::addDialog(QWidget* p)
{
	Post2dWindowContourSettingDialog* dialog = new Post2dWindowContourSettingDialog(p);
	Post2dWindowGridTypeDataItem* gtItem = dynamic_cast<Post2dWindowGridTypeDataItem*>(parent()->parent());
	dialog->setGridTypeDataItem(gtItem);
	Post2dWindowZoneDataItem* zItem = dynamic_cast<Post2dWindowZoneDataItem*>(parent());
	if (zItem->dataContainer() == nullptr || zItem->dataContainer()->data() == nullptr) {
		delete dialog;
		return nullptr;
	}
	dialog->setZoneData(zItem->dataContainer(), CellCenter);
	if (! zItem->dataContainer()->IBCCellExists()) {
		dialog->disableActive();
	}

	Post2dWindowContourSetting setting;
	setting.target = zItem->dataContainer()->data()->GetCellData()->GetArrayName(0);

	if (! nextScalarBarSetting(setting.scalarBarSetting)) {
		return nullptr;
	}

	dialog->setSetting(setting);
	dialog->setColorBarTitleMap(m_colorbarTitleMap);

	return dialog;
}

bool Post2dWindowCellScalarGroupTopDataItem::nextScalarBarSetting(ScalarBarSetting& scalarBarSetting)
{
	std::set<ScalarBarSetting::Quadrant> quads = ScalarBarSetting::getQuadrantSet();

	for (auto item : m_childItems) {
		Post2dWindowCellScalarGroupDataItem* typedi = dynamic_cast<Post2dWindowCellScalarGroupDataItem*>(item);
		// note use m_standardItemCopy which hasn't been changed yet
		if (typedi->m_standardItemCopy->checkState() == Qt::Checked) {
			auto it = quads.find(typedi->m_setting.scalarBarSetting.quadrant);
			if (it != quads.end()) {
				quads.erase(it);
			}
		}
	}
	if (quads.empty()) {
		QMessageBox::warning(postProcessorWindow(), tr("Warning"), tr("A maximum of four contours may be defined."));
		return false;
	}
	scalarBarSetting.setDefaultPosition(*quads.begin());
	return true;
}

bool Post2dWindowCellScalarGroupTopDataItem::hasTransparentPart()
{
	if (standardItem()->checkState() == Qt::Unchecked) {return false;}
	return true;
}

void Post2dWindowCellScalarGroupTopDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->initNodeResultAttributeBrowser();
}

void Post2dWindowCellScalarGroupTopDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->clearNodeResultAttributeBrowser();
}

void Post2dWindowCellScalarGroupTopDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseMoveEvent(event);
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->updateNodeResultAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowCellScalarGroupTopDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMousePressEvent(event);
}

void Post2dWindowCellScalarGroupTopDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseReleaseEvent(event);
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->fixNodeResultAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowCellScalarGroupTopDataItem::addCustomMenuItems(QMenu* menu)
{
	//QAction* abAction = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->showNodeAttributeBrowserAction();
	//menu->addAction(abAction);
	//menu->addSeparator();
	menu->addAction(dataModel()->objectBrowserView()->addAction());
}

class Post2dWindowCellScalarGroupTopDataItem::CreateCommand : public QUndoCommand
{
public:
	CreateCommand(Post2dWindowCellScalarGroupTopDataItem *item, QDialog* propDialog)
		: QUndoCommand(QObject::tr("Create Cell Contour"))
		, m_topItem(item)
		, m_firstDialog(true)
		, m_propDialog(propDialog)
		, m_undoCommand(nullptr)
	{}
	~CreateCommand() {
		delete m_undoCommand;
		if (! m_firstDialog) {
			delete m_propDialog;
		}
	}
	void redo() {
		m_item = new Post2dWindowCellScalarGroupDataItem(m_topItem, Checked, NotReorderable, Deletable);
		m_topItem->m_childItems.push_back(m_item);
		delete m_undoCommand;
		m_undoCommand = new QUndoCommand();
		m_item->undoCommands(m_propDialog, m_undoCommand);
		m_undoCommand->redo();
		m_topItem->setZDepthRange(m_topItem->m_zDepthRange);
		m_topItem->updateItemMap();
	}
	void undo() {
		// don't delete original propDialog
		if (! m_firstDialog) {
			delete m_propDialog;
		}
		m_firstDialog = false;
		m_propDialog = m_item->propertyDialog(m_item->mainWindow());
		m_undoCommand->undo();
		delete m_item;
		m_item = nullptr;
		m_topItem->updateItemMap();
	}
private:
	Post2dWindowCellScalarGroupTopDataItem* m_topItem;
	Post2dWindowCellScalarGroupDataItem* m_item;
	QDialog* m_propDialog;
	QUndoCommand* m_undoCommand;
	bool m_firstDialog;
};

void Post2dWindowCellScalarGroupTopDataItem::handleAddDialogAccepted(QDialog* propDialog)
{
	iRICUndoStack::instance().push(new CreateCommand(this, propDialog));
	iRICUndoStack::instance().clear();
	//
	// Note: Can't add to stack since m_item is deleted.  The following sequence would
	// cause iRIC to crash:
	// Create new contour -> modify contour -> undo -> undo -> redo -> redo
	// since the modify contour would attempt to use a deleted pointer.
	//
	// Need to be able to remove m_item from m_topItem and reuse m_item.
}

QList<QString> Post2dWindowCellScalarGroupTopDataItem::selectedScalars()
{
	QList<QString> ret;
	for (auto item : m_childItems) {
		Post2dWindowCellScalarGroupDataItem* typedi = dynamic_cast<Post2dWindowCellScalarGroupDataItem*>(item);
		if (typedi->standardItem()->checkState() == Qt::Checked) {
			ret.append(typedi->target().c_str());
		}
	}
	return ret;
}

QList<QString> Post2dWindowCellScalarGroupTopDataItem::availableScalars()
{
	QList<QString> ret;
	for (auto item : m_childItems) {
		Post2dWindowCellScalarGroupDataItem* typedi = dynamic_cast<Post2dWindowCellScalarGroupDataItem*>(item);
		ret.append(typedi->target().c_str());
	}
	return ret;
}

bool Post2dWindowCellScalarGroupTopDataItem::checkKmlExportCondition(const QString& target)
{
	for (auto item : m_childItems) {
		Post2dWindowCellScalarGroupDataItem* typedi = dynamic_cast<Post2dWindowCellScalarGroupDataItem*>(item);
		if (target == QString(typedi->target().c_str())) {
			return typedi->checkKmlExportCondition();
		}
	}
	return false;
}

bool Post2dWindowCellScalarGroupTopDataItem::exportKMLHeader(QXmlStreamWriter& writer, const QString& target)
{
	for (auto item : m_childItems) {
		Post2dWindowCellScalarGroupDataItem* typedi = dynamic_cast<Post2dWindowCellScalarGroupDataItem*>(item);
		if (target == QString(typedi->target().c_str())) {
			return typedi->exportKMLHeader(writer);
		}
	}
	return false;
}

bool Post2dWindowCellScalarGroupTopDataItem::exportKMLFooter(QXmlStreamWriter& writer, const QString& target)
{
	for (auto item : m_childItems) {
		Post2dWindowCellScalarGroupDataItem* typedi = dynamic_cast<Post2dWindowCellScalarGroupDataItem*>(item);
		if (target == QString(typedi->target().c_str())) {
			return typedi->exportKMLFooter(writer);
		}
	}
	return false;
}

bool Post2dWindowCellScalarGroupTopDataItem::exportKMLForTimestep(QXmlStreamWriter& writer, const QString& target, int index, double time)
{
	for (auto item : m_childItems) {
		Post2dWindowCellScalarGroupDataItem* typedi = dynamic_cast<Post2dWindowCellScalarGroupDataItem*>(item);
		if (target == QString(typedi->target().c_str())) {
			return typedi->exportKMLForTimestep(writer, index, time);
		}
	}
	return false;
}

bool Post2dWindowCellScalarGroupTopDataItem::exportContourFigureToShape(const QString& target, const QString& filename, double time)
{
	for (auto item : m_childItems) {
		Post2dWindowCellScalarGroupDataItem* typedi = dynamic_cast<Post2dWindowCellScalarGroupDataItem*>(item);
		//if (target == QString(typedi->target().c_str())) {
		//	return typedi->exportContourFigureToShape(filename, time);
		//}
	}
	return false;
}

bool Post2dWindowCellScalarGroupTopDataItem::checkShapeExportCondition(const QString& target)
{
	for (auto item : m_childItems) {
		Post2dWindowCellScalarGroupDataItem* typedi = dynamic_cast<Post2dWindowCellScalarGroupDataItem*>(item);
		if (target == QString(typedi->target().c_str())) {
			if (typedi->contour() == ContourSettingWidget::ContourFigure) {
				return true;
			}
		}
	}
	QMessageBox::warning(mainWindow(), tr("Error"), tr("To export shape file, switch \"Display setting\" to \"Contour Figure\", not \"Color Fringe\" or \"Isolines\"."));
	return false;
}
