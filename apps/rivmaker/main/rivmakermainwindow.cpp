#include "data/project/project.h"
#include "rivmakermainwindow.h"
#include "../dialogs/mousehelpdialog.h"

#include "ui_rivmakermainwindow.h"

#include <QCloseEvent>
#include <QMdiSubWindow>
#include <QMessageBox>

namespace {

} // namespace

RivmakerMainWindow::Impl::Impl(RivmakerMainWindow* w) :
	m_preProcessorWindow {w},
	m_verticalCrossSectionWindow {w},
	m_project {nullptr}
{
}

RivmakerMainWindow::Impl::~Impl()
{
	delete m_project;
}

// public interfaces

RivmakerMainWindow::RivmakerMainWindow(QWidget *parent) :
	QMainWindow(parent),
	impl {new Impl {this}},
	ui(new Ui::RivmakerMainWindow)
{
	ui->setupUi(this);
	setupConnections();

	auto pw = ui->centralwidget->addSubWindow(&(impl->m_preProcessorWindow));
	pw->setWindowIcon(impl->m_preProcessorWindow.windowIcon());
	pw->showMaximized();
	auto vw = ui->centralwidget->addSubWindow(&(impl->m_verticalCrossSectionWindow));
	vw->setWindowIcon(impl->m_verticalCrossSectionWindow.windowIcon());
	vw->hide();

	newProject();
}

RivmakerMainWindow::~RivmakerMainWindow()
{
	delete ui;
	delete impl;
}

void RivmakerMainWindow::newProject()
{
	deleteProject();
	impl->m_project = new Project();
	impl->m_preProcessorWindow.setProject(impl->m_project);
}

void RivmakerMainWindow::focusPreProcessorWindow()
{
	auto pw = impl->m_preProcessorWindow.parentWidget();
	pw->show();
	pw->setFocus();
}

void RivmakerMainWindow::focusVerticalCrossSectionWindow()
{
	if (!impl->m_project->checkIfReadyToOpenVerticalCrossSectionWindow(this)) {return;}

	auto pw = impl->m_verticalCrossSectionWindow.parentWidget();
	pw->show();
	pw->setFocus();
}

void RivmakerMainWindow::openCrossSectionWindow()
{

}

void RivmakerMainWindow::viewToggleToolBar(bool visible)
{
	ui->toolBar->setVisible(visible);
}

void RivmakerMainWindow::viewToggleStatusBar(bool visible)
{
	ui->statusbar->setVisible(visible);
}

void RivmakerMainWindow::helpMouseHint()
{
	static MouseHelpDialog* dialog = nullptr;
	if (dialog == nullptr) {
		dialog = new MouseHelpDialog(this);
	}
	dialog->show();
}

void RivmakerMainWindow::closeEvent(QCloseEvent *e)
{
	int ret = QMessageBox::warning(this, tr("Warning"), tr("Are you sure you want to exit Rivmaker?"), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
	if (ret == QMessageBox::Cancel) {
		e->ignore();
		return;
	}
	e->accept();
}

void RivmakerMainWindow::deleteProject()
{
	delete impl->m_project;
	impl->m_project = nullptr;
}
