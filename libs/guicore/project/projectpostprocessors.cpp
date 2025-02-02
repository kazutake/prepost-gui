#include "../base/iricmainwindowinterface.h"
#include "../post/postprocessorwindow.h"
#include "../post/postprocessorwindowfactoryinterface.h"
#include "../post/postprocessorwindowfactoryinterface.h"
#include "../post/postprocessorwindowprojectdataitem.h"
#include "projectdata.h"
#include "projectpostprocessors.h"
//#include "main/iricmainwindow.h"
//#include "post/postprocessorwindow.h"
//#include "post/postprocessorwindowfactory.h"
//#include "post/postprocessorwindowprojectdataitem.h"
//#include "projectdata.h"

#include <misc/errormessage.h>

#include <QDomNode>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QXmlStreamWriter>

ProjectPostProcessors::ProjectPostProcessors(ProjectDataItem* parent)
	: ProjectDataItem(parent)
{

}

ProjectPostProcessors::~ProjectPostProcessors()
{
	if (projectData()->mainWindow() == nullptr) {return;}
	// remove all post processors from the MDI area of main window.
	QMdiArea* area = dynamic_cast<QMdiArea*>(projectData()->mainWindow()->centralWidget());

	for (auto it = m_postProcessorWindows.begin(); it != m_postProcessorWindows.end(); ++it) {
		area->removeSubWindow((*it)->window()->parentWidget());
	}
}

void ProjectPostProcessors::doLoadFromProjectMainFile(const QDomNode& node, bool import)
{
	QDomNode child = node.firstChild();
	QWidget* parentWindow = projectData()->mainWindow();
	while (! child.isNull()) {
		PostProcessorWindowProjectDataItem* di = nullptr;
		if (import) {
			QDomElement elem = child.toElement();
			di = m_factory->factory(elem.attribute("type"), this, parentWindow);
		} else {
			di = m_factory->restore(child, this, parentWindow);
		}
		if (di != nullptr) {
			add(di);
			try {
				di->loadFromProjectMainFile(child);
			} catch (ErrorMessage m) {
				m_postProcessorWindows.removeOne(di);
				delete di;
			}
		}
		child = child.nextSibling();
	}
}

void ProjectPostProcessors::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (auto it = m_postProcessorWindows.begin(); it != m_postProcessorWindows.end(); ++it) {
		writer.writeStartElement("PostProcessor");
		(*it)->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

QMdiSubWindow* ProjectPostProcessors::add(PostProcessorWindowProjectDataItem* newitem)
{
	iRICMainWindowInterface* w = projectData()->mainWindow();
	if (w == nullptr) {return nullptr;}
	QMdiArea* area = dynamic_cast<QMdiArea*>(w->centralWidget());
	QMdiSubWindow* container = area->addSubWindow(newitem->window());
	container->setWindowIcon(newitem->window()->icon());

	m_postProcessorWindows.push_back(newitem);
	return container;
}

void ProjectPostProcessors::requestDelete(PostProcessorWindowProjectDataItem* item)
{
	for (auto it = m_postProcessorWindows.begin(); it != m_postProcessorWindows.end(); ++it) {
		if (*it == item) {
			m_postProcessorWindows.erase(it);
			delete item;
			return;
		}
	}
}

void ProjectPostProcessors::loadFromProjectMainFile(const QDomNode& node, bool import)
{
	doLoadFromProjectMainFile(node, import);
}

int ProjectPostProcessors::windowCount() const
{
	return m_postProcessorWindows.count();
}

void ProjectPostProcessors::setFactory(PostProcessorWindowFactoryInterface* factory)
{
	m_factory = factory;
}

void ProjectPostProcessors::applyOffset(double x_diff, double y_diff)
{
	for (auto w : m_postProcessorWindows) {
		w->window()->applyOffset(x_diff, y_diff);
	}
}
