#include "../preprocessorgraphicsview.h"
#include "preprocessorbackgroundimagedataitem.h"
#include "preprocessorbackgroundimagesdataitem.h"
#include "preprocessorrootdataitem.h"

#include <guibase/objectbrowserview.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>

#include <QMenu>
#include <QMessageBox>

#include <vtkRenderWindow.h>
#include <vtkRenderer.h>

PreProcessorBackgroundImagesDataItem::PreProcessorBackgroundImagesDataItem(GraphicsWindowDataItem* parent)
	: PreProcessorDataItem(tr("Background Images"), QIcon(":/libs/guibase/images/iconFolder.png"), parent)
{
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);
	m_standardItem->setStatusTip(tr("Background Images"));
	m_standardItem->setData(QVariant(tr("BACKGROUNDIMAGES")), Qt::UserRole + 10);

	m_standardItemCopy = m_standardItem->clone();
	m_isDeletable = false;

	m_addAction = new QAction(tr("&Add Image..."), this);
	m_deleteAllAction = new QAction(tr("&Delete All"), this);

	connect(m_addAction, SIGNAL(triggered()), projectData()->mainfile(), SLOT(addBackgroundImage()));
	connect(m_deleteAllAction, SIGNAL(triggered()), this, SLOT(deleteAll()));
	connect(projectData()->mainfile(), SIGNAL(backgroundImageAdded()), this, SLOT(addChildItem()));
	connect(this, SIGNAL(selectBackgroundImage(QModelIndex)), dataModel(), SLOT(handleObjectBrowserSelection(QModelIndex)));
	connect(projectData()->mainfile(), SIGNAL(backgroundImageDeleted(int)), this, SLOT(deleteChildItem(int)));
	connect(projectData()->mainfile(), SIGNAL(backgroundImageMovedUp(int)), this, SLOT(moveUpChildItem(int)));
	connect(projectData()->mainfile(), SIGNAL(backgroundImageMovedDown(int)), this, SLOT(moveDownChildItem(int)));
	connect(projectData()->mainfile(), SIGNAL(backgroundActorVisibilityChanged(int, bool)), this, SLOT(updateChildCheckState(int, bool)));

	projectData()->mainfile()->addRenderer(renderer());
}

void PreProcessorBackgroundImagesDataItem::addChildItem()
{
	BackgroundImageInfo* image = projectData()->mainfile()->backgroundImages().first();
	PreProcessorBackgroundImageDataItem* iItem = new PreProcessorBackgroundImageDataItem(image, this);
	// make the standard item top.
	QList<QStandardItem*> takenItems = m_standardItem->takeRow(iItem->standardItem()->row());
	m_standardItem->insertRows(0, takenItems);
	m_childItems.push_front(iItem);

	updateItemMap();
	updateZDepthRange();

	dataModel()->objectBrowserView()->select(iItem->standardItem()->index());
	dataModel()->graphicsView()->ResetCameraClippingRange();
	emit selectBackgroundImage(iItem->standardItem()->index());
}

void PreProcessorBackgroundImagesDataItem::deleteChildItem(int i)
{
	auto it = m_childItems.begin();
	delete *(it + i);
	dynamic_cast<PreProcessorRootDataItem*>(parent())->updateItemMap();
	for (it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		PreProcessorBackgroundImageDataItem* bgItem = dynamic_cast<PreProcessorBackgroundImageDataItem*>(*it);
		QModelIndex idx = bgItem->standardItem()->index();
		if (idx != dataModel()->objectBrowserView()->currentIndex()) { continue; }
		bgItem->updateMoveUpDownActions(dataModel()->objectBrowserView());
	}
	renderGraphicsView();
}

void PreProcessorBackgroundImagesDataItem::moveUpChildItem(int i)
{
	// reorder the standard item.
	QList<QStandardItem*> items = m_standardItem->takeRow(i);
	m_standardItem->insertRows(i - 1, items);

	// reorder the m_childItems.
	auto it = m_childItems.begin() + i;
	auto it2 = it - 1;
	GraphicsWindowDataItem* item = *it;
	int idx = m_childItems.indexOf(*it);
	int idx2 = m_childItems.indexOf(*it2);
	m_childItems.swap(idx, idx2);

	// update the ZDepthRange.
	updateZDepthRange();
	renderGraphicsView();

	dataModel()->objectBrowserView()->select(item->standardItem()->index());
}

void PreProcessorBackgroundImagesDataItem::moveDownChildItem(int i)
{
	// reorder the standard item.
	QList<QStandardItem*> items = m_standardItem->takeRow(i);
	m_standardItem->insertRows(i + 1, items);

	// reorder the m_childList.
	auto it = m_childItems.begin() + i;
	auto it2 = it + 1;
	GraphicsWindowDataItem* item = *it;
	int idx = m_childItems.indexOf(*it);
	int idx2 = m_childItems.indexOf(*it2);
	m_childItems.swap(idx, idx2);

	// update the ZDepthRange.
	updateZDepthRange();
	renderGraphicsView();

	dataModel()->objectBrowserView()->select(item->standardItem()->index());
}

void PreProcessorBackgroundImagesDataItem::visiblePropBounds(double bounds[6])
{
	renderer()->ComputeVisiblePropBounds(bounds);
}

void PreProcessorBackgroundImagesDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(m_addAction);
	menu->addSeparator();
	menu->addAction(m_deleteAllAction);
}

void PreProcessorBackgroundImagesDataItem::updateChildCheckState(int idx, bool vis)
{
	dataModel()->itemModel()->blockSignals(true);
	if (idx < 0) {
		if (vis) {
			m_standardItem->setCheckState(Qt::Checked);
		} else {
			m_standardItem->setCheckState(Qt::Unchecked);
		}
	} else {
		auto it = m_childItems.begin() + idx;
		PreProcessorBackgroundImageDataItem* item = dynamic_cast<PreProcessorBackgroundImageDataItem*>(*it);
		if (vis) {
			item->standardItem()->setCheckState(Qt::Checked);
		} else {
			item->standardItem()->setCheckState(Qt::Unchecked);
		}
	}
	dataModel()->itemModel()->blockSignals(false);
	updateVisibility();
}

void PreProcessorBackgroundImagesDataItem::handleStandardItemChange()
{
	GraphicsWindowDataItem::handleStandardItemChange();
	projectData()->mainfile()->updateActorVisibility(- 1, m_standardItem->checkState() == Qt::Checked);
}

void PreProcessorBackgroundImagesDataItem::deleteAll()
{
	int ret = QMessageBox::warning(mainWindow(), tr("Warning"), tr("Are you sure you want to delete all background images?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (ret == QMessageBox::No) {return;}

	while (m_childItems.count() > 0) {
		GraphicsWindowDataItem* item = *(m_childItems.begin());
		projectData()->mainfile()->deleteImage(item->standardItem()->index());
	}
}
