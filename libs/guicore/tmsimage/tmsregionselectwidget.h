#ifndef TMSREGIONSELECTWIDGET_H
#define TMSREGIONSELECTWIDGET_H

#include "../guicore_global.h"

#include <QWidget>

#include <string>

class CoordinateSystem;

class GUICOREDLL_EXPORT TmsRegionSelectWidget : public QWidget
{
	Q_OBJECT

public:
	TmsRegionSelectWidget(QWidget* parent);
	~TmsRegionSelectWidget();

	void paintEvent(QPaintEvent *);
	void resizeEvent(QResizeEvent *);

	void mousePressEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent *);
	void wheelEvent(QWheelEvent* event);

	void setMapSetting(const std::string& setting);
	void setCenter(double lon, double lat);
	void setZoomLevel(double zoomLevel);

	bool isSelected() const;
	double minLon() const;
	double maxLon() const;
	double minLat() const;
	double maxLat() const;

	void requestUpdate();

public slots:
	void zoomIn();
	void zoomOut();

private slots:
	void handleImageUpdate(int requestId);
	void handleTimer();

private:
	class Impl;
	Impl* impl;
};

#endif // TMSREGIONSELECTWIDGET_H
