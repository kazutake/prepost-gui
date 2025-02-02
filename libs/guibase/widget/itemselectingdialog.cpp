#include "ui_itemselectingdialog.h"

#include "itemselectingdialog.h"

ItemSelectingDialog::ItemSelectingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::ItemSelectingDialog)
{
	ui->setupUi(this);
}

ItemSelectingDialog::~ItemSelectingDialog()
{
	delete ui;
}

void ItemSelectingDialog::setMessage(const QString& message)
{
	ui->messageLabel->setText(message);
}

void ItemSelectingDialog::setSelectedIndex(int index)
{
	ui->comboBox->setCurrentIndex(index);
}

int ItemSelectingDialog::selectedIndex()
{
	return ui->comboBox->currentIndex();
}

QComboBox* ItemSelectingDialog::comboBox() const
{
	return ui->comboBox;
}
