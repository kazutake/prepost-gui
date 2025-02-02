#ifndef SCALARSTOCOLORSEDITWIDGET_H
#define SCALARSTOCOLORSEDITWIDGET_H

#include "../guicore_global.h"
#include <QWidget>

class ScalarsToColorsContainer;

class GUICOREDLL_EXPORT ScalarsToColorsEditWidget : public QWidget
{
public:
	ScalarsToColorsEditWidget(QWidget* parent = nullptr);
	virtual ~ScalarsToColorsEditWidget();

	ScalarsToColorsContainer* container() const;
	void setContainer(ScalarsToColorsContainer* c);

	virtual void save() = 0;

private:
	virtual void setupWidget() = 0;
	ScalarsToColorsContainer* m_container;
};

#endif // SCALARSTOCOLORSEDITWIDGET_H
