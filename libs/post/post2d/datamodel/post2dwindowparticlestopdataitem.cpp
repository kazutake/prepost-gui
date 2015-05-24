#include "post2dwindowparticlestopdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/xmlsupport.h>
#include <postbase/postparticlebasicpropertydialog.h>

#include <QSettings>

#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>

Post2dWindowParticlesTopDataItem::Post2dWindowParticlesTopDataItem(Post2dWindowDataItem* p)
	: Post2dWindowDataItem(tr("Particles"), QIcon(":/libs/guibase/images/iconPaper.png"), p)
{
	QSettings setting;

	m_isDeletable = false;
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();
	m_color = setting.value("graphics/particlecolor", QColor(Qt::black)).value<QColor>();
	m_size = setting.value("graphics/particlesize", 3).toInt();

	setupActors();
	updateActorSettings();
}

Post2dWindowParticlesTopDataItem::~Post2dWindowParticlesTopDataItem()
{
	renderer()->RemoveActor(m_actor);
}

void Post2dWindowParticlesTopDataItem::updateActorSettings()
{
	m_actor->VisibilityOff();
	m_actorCollection->RemoveAllItems();

	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == 0 || cont->particleData() == 0) {return;}

	m_actor->GetProperty()->SetPointSize(m_size);
	m_actor->GetProperty()->SetColor(m_color.redF(), m_color.greenF(), m_color.blueF());
	m_mapper->SetInputData(cont->particleData());

	m_actorCollection->AddItem(m_actor);
	updateVisibilityWithoutRendering();
}

void Post2dWindowParticlesTopDataItem::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(1);
}

void Post2dWindowParticlesTopDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_actor->SetPosition(0, 0, range.min());
}

void Post2dWindowParticlesTopDataItem::update()
{
	updateActorSettings();
}

void Post2dWindowParticlesTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_color = iRIC::getColorAttribute(node, "color");
	m_size = iRIC::getIntAttribute(node, "size", 3);
}

void Post2dWindowParticlesTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	iRIC::setColorAttribute(writer, "color", m_color);
	iRIC::setIntAttribute(writer, "size", m_size);
}

void Post2dWindowParticlesTopDataItem::setupActors()
{
	m_actor = vtkSmartPointer<vtkActor>::New();
	renderer()->AddActor(m_actor);
	m_actor->GetProperty()->LightingOff();

	m_mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_actor->SetMapper(m_mapper);
}

QDialog* Post2dWindowParticlesTopDataItem::propertyDialog(QWidget* parent)
{
	PostParticleBasicPropertyDialog* dialog = new PostParticleBasicPropertyDialog(parent);
	dialog->setColor(m_color);
	dialog->setSize(m_size);

	return dialog;
}

class Post2dWindowParticlesTopSetProperty : public QUndoCommand
{
public:
	Post2dWindowParticlesTopSetProperty(const QColor& color, int size, Post2dWindowParticlesTopDataItem* item) {
		m_newColor = color;
		m_newSize = size;

		m_oldColor = item->m_color;
		m_oldSize = item->m_size;

		m_item = item;
	}

	void undo() {
		m_item->setIsCommandExecuting(true);
		m_item->m_color = m_oldColor;
		m_item->m_size = m_oldSize;
		m_item->updateActorSettings();
		m_item->renderGraphicsView();
		m_item->setIsCommandExecuting(false);
	}
	void redo() {
		m_item->setIsCommandExecuting(true);
		m_item->m_color = m_newColor;
		m_item->m_size = m_newSize;
		m_item->updateActorSettings();
		m_item->renderGraphicsView();
		m_item->setIsCommandExecuting(false);
	}

private:
	QColor m_oldColor;
	int m_oldSize;

	QColor m_newColor;
	int m_newSize;

	Post2dWindowParticlesTopDataItem* m_item;
};

void Post2dWindowParticlesTopDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	PostParticleBasicPropertyDialog* dialog = dynamic_cast<PostParticleBasicPropertyDialog*>(propDialog);
	iRICUndoStack::instance().push(new Post2dWindowParticlesTopSetProperty(dialog->color(), dialog->size(), this));
}
