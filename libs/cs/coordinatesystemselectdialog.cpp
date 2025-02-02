#include "ui_coordinatesystemselectdialog.h"

#include "coordinatesystem.h"
#include "coordinatesystembuilder.h"
#include "coordinatesystemselectdialog.h"

CoordinateSystemSelectDialog::CoordinateSystemSelectDialog(QWidget* parent) :
	QDialog(parent),
	m_currentSystem {nullptr},
	ui(new Ui::CoordinateSystemSelectDialog)
{
	ui->setupUi(this);

	connect(ui->searchEdit, SIGNAL(textEdited(QString)), this, SLOT(updateList()));
	connect(ui->listWidget, SIGNAL(currentRowChanged(int)), this, SLOT(updateCurrent(int)));
}

CoordinateSystemSelectDialog::~CoordinateSystemSelectDialog()
{
	delete ui;
}

void CoordinateSystemSelectDialog::setBuilder(CoordinateSystemBuilder* builder)
{
	m_builder = builder;
}

CoordinateSystem* CoordinateSystemSelectDialog::coordinateSystem() const
{
	return m_currentSystem;
}

void CoordinateSystemSelectDialog::setCoordinateSystem(CoordinateSystem* cs)
{
	m_currentSystem = cs;
	updateList();
}

void CoordinateSystemSelectDialog::updateList()
{
	QList<CoordinateSystem*> list = m_builder->systems();
	QString searchStr = ui->searchEdit->text().toLower();

	ui->listWidget->clear();
	m_listSystems.clear();

	ui->listWidget->addItem(tr("(Not Specified)"));
	m_listSystems.append(0);

	for (int i = 0; i < list.count(); ++i) {
		CoordinateSystem* cs = list.at(i);
		if (cs->searchTarget().contains(searchStr)) {
			ui->listWidget->addItem(cs->caption());
			m_listSystems.append(cs);
		}
	}
	int index = m_listSystems.indexOf(m_currentSystem);
	if (index == -1) {index = 0;}
	ui->listWidget->setCurrentRow(index);
}

void CoordinateSystemSelectDialog::updateCurrent(int index)
{
	if (index == -1) {return;}
	m_currentSystem = m_listSystems.at(index);
}
