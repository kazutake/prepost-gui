#ifndef COORDINATEEDITWIDGET_H
#define COORDINATEEDITWIDGET_H

#include <QLineEdit>

class CoordinateEditWidget : public QLineEdit
{
	Q_OBJECT

public:
	CoordinateEditWidget(QWidget* parent = 0);
	void setEventCheck(bool check = true);
	void setValue(double newvalue);
	double value() const;

signals:
	void valueChanged(double value);

private slots:
	void handleTextChange();

private:
	void closeEvent(QCloseEvent* e);
	void focusOutEvent(QFocusEvent* e);
	bool updateValue();

	double m_doubleValue;
	bool m_eventCheck;
};

#endif // COORDINATEEDITWIDGET_H
