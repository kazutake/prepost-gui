#ifndef PREPROCESSORLEGENDBOXEDITDIALOG_H
#define PREPROCESSORLEGENDBOXEDITDIALOG_H

#include <guibase/vtktextpropertysettingcontainer.h>
#include <QDialog>

namespace Ui
{
	class PreProcessorLegendBoxEditDialog;
}

class PreProcessorLegendBoxEditDialog : public QDialog
{
	Q_OBJECT

public:
	PreProcessorLegendBoxEditDialog(QWidget* parent = nullptr);
	~PreProcessorLegendBoxEditDialog();

	void setWidth(double width);
	void setHeight(double height);
	void setPositionX(double x);
	void setPositionY(double y);
	void setEntryTextSetting(const vtkTextPropertySettingContainer& cont);

	double width() const;
	double height() const;
	double positionX() const;
	double positionY() const;
	const vtkTextPropertySettingContainer& entryTextSetting() const;

private slots:
	void editEntryTextSetting();

private:
	vtkTextPropertySettingContainer m_entryTextSetting;

	Ui::PreProcessorLegendBoxEditDialog* ui;
};

#endif // PREPROCESSORLEGENDBOXEDITDIALOG_H
