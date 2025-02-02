#include "ui_preprocessorlegendboxeditdialog.h"

#include "preprocessorlegendboxeditdialog.h"

#include <guibase/vtktextpropertysettingdialog.h>

PreProcessorLegendBoxEditDialog::PreProcessorLegendBoxEditDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::PreProcessorLegendBoxEditDialog)
{
	ui->setupUi(this);
	connect(ui->fontSettingButton, SIGNAL(clicked()), this, SLOT(editEntryTextSetting()));
}

PreProcessorLegendBoxEditDialog::~PreProcessorLegendBoxEditDialog()
{
	delete ui;
}

void PreProcessorLegendBoxEditDialog::setWidth(double width)
{
	ui->widthSpinBox->setValue(width);
}

void PreProcessorLegendBoxEditDialog::setHeight(double height)
{
	ui->heightSpinBox->setValue(height);
}

void PreProcessorLegendBoxEditDialog::setPositionX(double x)
{
	ui->positionXSpinBox->setValue(x);
}

void PreProcessorLegendBoxEditDialog::setPositionY(double y)
{
	ui->positionYSpinBox->setValue(y);
}

double PreProcessorLegendBoxEditDialog::width() const
{
	return ui->widthSpinBox->value();
}

double PreProcessorLegendBoxEditDialog::height() const
{
	return ui->heightSpinBox->value();
}

double PreProcessorLegendBoxEditDialog::positionX() const
{
	return ui->positionXSpinBox->value();
}

double PreProcessorLegendBoxEditDialog::positionY() const
{
	return ui->positionYSpinBox->value();
}

const vtkTextPropertySettingContainer& PreProcessorLegendBoxEditDialog::entryTextSetting() const
{
	return m_entryTextSetting;
}

void PreProcessorLegendBoxEditDialog::setEntryTextSetting(const vtkTextPropertySettingContainer& cont)
{
	m_entryTextSetting = cont;
}

void PreProcessorLegendBoxEditDialog::editEntryTextSetting()
{
	vtkTextPropertySettingDialog dialog(this);
	dialog.setSetting(m_entryTextSetting);
	dialog.disableSize();
	dialog.disableColor();
	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}
	m_entryTextSetting = dialog.setting();
}
