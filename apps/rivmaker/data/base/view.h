#ifndef VIEW_H
#define VIEW_H

#include <QWidget>

class Model;

class View : public QWidget
{
	Q_OBJECT

private:
	const static int CLICK_LIMIT;
	const static int NEAR_LIMIT;

public:
	View(QWidget* parent);
	virtual ~View();

	void updateView();

	QPointF conv(const QPointF& point) const;
	QPointF rconv(const QPointF& point) const;

	QRectF drawnRegion() const;

	const QTransform& transform() const;

	Model* model() const;
	void setModel(Model* m);

	QPointF drawnPos(const QPoint& pos) const;

	static bool isClick(const QPoint& pressP, const QPoint& releaseP);
	static bool isNear(const QPointF& p1, const QPointF& p2);

public slots:
	void fit();

	void zoomIn();
	void zoomOut();

	void resetRotation();
	void rotate90();

signals:
	void positionChanged(const QPointF& pos);

private:
	void paintEvent(QPaintEvent* event) override;
	void keyPressEvent(QKeyEvent* event) override;
	void keyReleaseEvent(QKeyEvent* event) override;
	void mouseDoubleClickEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void wheelEvent(QWheelEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;

	virtual void paramsFit() = 0;
	virtual void paramsZoomIn() = 0;
	virtual void paramsZoomOut() = 0;

	virtual void paramsResetRotation();
	virtual void paramsRotate90();

	virtual void viewMouseMoveEvent(QMouseEvent* event) = 0;
	virtual void viewMousePressEvent(QMouseEvent* event) = 0;
	virtual void viewMouseReleaseEvent(QMouseEvent* event) = 0;
	virtual void viewWheelEvent(QWheelEvent* event) = 0;

	virtual bool inViewOperation() = 0;

	void emitPosition(QMouseEvent* event);
	virtual void updateTransform() = 0;

	void setupCursors();
	bool prepareDrawModel();

	Model* m_model {nullptr};

protected:
	QTransform m_transform {};

	QPoint m_previousPos {};
	QPointF m_drawOffset {};

	QPoint m_rightClickPos {};

	QCursor m_zoomCursor;
	QCursor m_moveCursor;
	QCursor m_rotateCursor;

private:
	QPixmap m_zoomPixmap;
	QPixmap m_movePixmap;
	QPixmap m_rotatePixmap;
};

#endif // VIEW_H
