#include "mousehelpdialog.h"
#include "ui_mousehelpdialog.h"

#include <QCloseEvent>
#include <QFile>
#include <QLocale>
#include <QSettings>

MouseHelpDialog::MouseHelpDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::MouseHelpDialog)
{
	ui->setupUi(this);
	QSettings setting("iRIC Organization", "iRIC GUI 3");
	QString locale = setting.value("general/locale", QLocale::system().name()).value<QString>();
	QString fname = QString(":/images/%1/imgMouseHelp.png").arg(locale);
	if (! QFile::exists(fname)) {
		fname = ":/images/imgMouseHelp.png";
	}
	m_image = QPixmap(fname);
	ui->imageLabel->setPixmap(m_image);
}

MouseHelpDialog::~MouseHelpDialog()
{
	delete ui;
}

void MouseHelpDialog::closeEvent(QCloseEvent* e)
{
	hide();
	e->ignore();
}
