#include "inputconditionwidgetfunctionalgraphview.h"

#include <QPainter>

void InputConditionWidgetFunctionalGraphview::dataChanged(const QModelIndex& /*topLeft*/, const QModelIndex& /*bottomRight*/)
{
	viewport()->update();
}
void InputConditionWidgetFunctionalGraphview::paintEvent(QPaintEvent* /*e*/)
{
	QPainter painter(viewport());
	QBrush solidBrush(Qt::blue, Qt::SolidPattern);
	QRect vp = painter.viewport();
	QMatrix matrix = getMatrix(vp);
	painter.setRenderHint(QPainter::Antialiasing);
	QRectF region = getRegion();
	// Draw the region box
	drawRegion(painter, region, matrix);
	// Draw the data
	QAbstractItemModel* m = model();
	double oldx = 0, oldy = 0;
	QBrush oldbrush = painter.brush();
	painter.setBrush(solidBrush);
	// Draw lines
	for (int i = 0; i < m->rowCount(); ++i) {
		double x = m->data(m->index(i, 0)).toDouble();
		double y = m->data(m->index(i, 1)).toDouble();
		QPointF point = matrix.map(QPointF(x, y));
		if (i != 0) {
			QPointF oldpoint = matrix.map(QPointF(oldx, oldy));
			painter.drawLine(oldpoint, point);
		}
		oldx = x;
		oldy = y;
	}
	// Draw the circles
	for (int i = 0; i < m->rowCount(); ++i) {
		double x = m->data(m->index(i, 0)).toDouble();
		double y = m->data(m->index(i, 1)).toDouble();
		QPointF point = matrix.map(QPointF(x, y));
		QRectF r(point.x() - ellipseR, point.y() - ellipseR, ellipseR * 2, ellipseR * 2);
		painter.drawEllipse(r);
	}
}
void InputConditionWidgetFunctionalGraphview::drawRegion(QPainter& painter, const QRectF& region, const QMatrix& matrix)
{
	double xlength = region.right() - region.left();
	double ylength = region.bottom() - region.top();

	double left = region.left() - xlength * fLeftMargin;
	double right = region.right() + xlength * fRightMargin;
	double top = region.top() - ylength * fTopMargin;
	double bottom = region.bottom() + ylength * fBottomMargin;

	// Draw the region box
	QPointF from, to;
	from = matrix.map(QPointF(left, top));
	to = matrix.map(QPointF(right, top));
	painter.drawLine(from, to);

	from = matrix.map(QPointF(left, bottom));
	to = matrix.map(QPointF(right, bottom));
	painter.drawLine(from, to);

	from = matrix.map(QPointF(left, bottom));
	to = matrix.map(QPointF(left, top));
	painter.drawLine(from, to);

	from = matrix.map(QPointF(right, bottom));
	to = matrix.map(QPointF(right, top));
	painter.drawLine(from, to);
}
QRectF InputConditionWidgetFunctionalGraphview::getRegion()
{
	QRectF ret(0., 0., 0., 0.);
	QAbstractItemModel* m = model();
	for (int i = 0; i < m->rowCount(); ++i) {
		double x = m->data(m->index(i, 0)).toDouble();
		double y = m->data(m->index(i, 1)).toDouble();
		if (i == 0 || x < ret.left()) {ret.setLeft(x);}
		if (i == 0 || x > ret.right()) {ret.setRight(x);}
		if (i == 0 || y < ret.top()) {ret.setTop(y);}
		if (i == 0 || y > ret.bottom()) {ret.setBottom(y);}
	}
	return ret;
}

QMatrix InputConditionWidgetFunctionalGraphview::getMatrix(QRect& viewport)
{
	QRectF region = getRegion();
	QMatrix translate1, scale, translate2;
	double xlength = region.right() - region.left();
	double ylength = region.bottom() - region.top();
	if (xlength == 0) {xlength = 1;}
	if (ylength == 0) {ylength = 1;}

	translate1 = QMatrix(1, 0, 0, 1, - (region.left() - fLeftMargin * xlength), - (region.bottom() + fBottomMargin * ylength));

	double xscale =
		(viewport.right() - viewport.left() - iLeftMargin - iRightMargin) /
		(region.right() - region.left() + (fLeftMargin + fRightMargin) * xlength);
	double yscale = -
									(viewport.bottom() - viewport.top() - iTopMargin - iBottomMargin) /
									(region.bottom() - region.top() + (fTopMargin + fBottomMargin) * ylength);
	scale = QMatrix(xscale, 0, 0, yscale, 0, 0);

	translate2 = QMatrix(1, 0, 0, 1, viewport.left() + iLeftMargin, viewport.top() + iTopMargin);

	return translate1 * scale * translate2;
}

