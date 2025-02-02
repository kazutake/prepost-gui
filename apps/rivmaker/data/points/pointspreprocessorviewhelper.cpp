#include "points.h"
#include "pointspreprocessorviewhelper.h"
#include "../base/view.h"
#include "../../geom/geometrypoint.h"

#include <QColor>
#include <QPainter>
#include <QPointF>

namespace {

int labelOffset = 8;
int fontSize = 10;

} // namespace

const int PointsPreProcessorViewHelper::STD_SIZE = 6;

PointsPreProcessorViewHelper::PointsPreProcessorViewHelper(DataItemView* v) :
	DataItemViewHelperI {v}
{}

void PointsPreProcessorViewHelper::drawCircle(const QPointF& position, int size, const QColor& color, int transparency, QPainter* painter)
{
	painter->save();

	QColor c = color;
	c.setAlphaF(1.0 - transparency / 100.0);
	painter->setBrush(c);
	painter->setPen(Qt::NoPen);

	QRectF rect(position.x() - size * 0.5, position.y() - size * 0.5, size, size);
	painter->drawEllipse(rect);

	painter->restore();
}

void PointsPreProcessorViewHelper::drawRect(const QPointF& position, int size, const QColor& color, int transparency, QPainter* painter)
{
	painter->save();

	QColor c = color;
	c.setAlphaF(1.0 - transparency / 100.0);
	QRectF rect(position.x() - size * 0.5, position.y() - size * 0.5, size, size);
	painter->fillRect(rect, c);

	painter->restore();
}

void PointsPreProcessorViewHelper::drawDiamond(const QPointF& p, int size, const QColor& color, int transparency, QPainter* painter)
{
	painter->save();

	QColor c = color;
	c.setAlphaF(1.0 - transparency / 100.0);
	painter->setPen(c);
	painter->setBrush(QBrush(c));
	QPolygonF polygon;
	polygon.push_back(QPointF(p.x()             , p.y() - size * 0.5));
	polygon.push_back(QPointF(p.x() - size * 0.5, p.y()             ));
	polygon.push_back(QPointF(p.x()             , p.y() + size * 0.5));
	polygon.push_back(QPointF(p.x() + size * 0.5, p.y()             ));
	polygon.push_back(QPointF(p.x()             , p.y() - size * 0.5));
	painter->drawPolygon(polygon);

	painter->restore();
}

void PointsPreProcessorViewHelper::drawTriangle(const QPointF& p, int size, const QColor& color, int transparency, QPainter* painter)
{
	painter->save();

	QColor c = color;
	c.setAlphaF(1.0 - transparency / 100.0);
	painter->setPen(c);
	painter->setBrush(QBrush(c));

	QPolygonF polygon;
	polygon.push_back(QPointF(p.x()             , p.y() - size * 0.5));
	polygon.push_back(QPointF(p.x() - size * 0.5, p.y() + size * 0.5));
	polygon.push_back(QPointF(p.x() + size * 0.5, p.y() + size * 0.5));
	polygon.push_back(QPointF(p.x()             , p.y() - size * 0.5));
	painter->drawPolygon(polygon);

	painter->restore();
}

void PointsPreProcessorViewHelper::drawReverseTriangle(const QPointF& p, int size, const QColor& color, int transparency, QPainter* painter)
{
	painter->save();

	QColor c = color;
	c.setAlphaF(1.0 - transparency / 100.0);
	painter->setPen(c);
	painter->setBrush(QBrush(c));

	QPolygonF polygon;
	polygon.push_back(QPointF(p.x()             , p.y() + size * 0.5));
	polygon.push_back(QPointF(p.x() - size * 0.5, p.y() - size * 0.5));
	polygon.push_back(QPointF(p.x() + size * 0.5, p.y() - size * 0.5));
	polygon.push_back(QPointF(p.x()             , p.y() + size * 0.5));
	painter->drawPolygon(polygon);

	painter->restore();
}

void PointsPreProcessorViewHelper::drawCircles(int size, const QColor& color, int transparency, QPainter* painter) const
{
	painter->save();

	auto v = view();
	auto points = dynamic_cast<Points*> (dataItem());
	const auto& pvec = points->points();

	QColor c = color;
	c.setAlphaF(1.0 - transparency / 100.0);
	painter->setBrush(c);
	painter->setPen(Qt::NoPen);
	for (GeometryPoint* p : pvec) {
		QPointF p2 = v->conv(QPointF(p->x(), p->y()));

		QRectF rect(p2.x() - size * 0.5, p2.y() - size * 0.5, size, size);
		painter->drawEllipse(rect);
	}

	painter->restore();
}

void PointsPreProcessorViewHelper::drawRects(int size, const QColor& color, int transparency, QPainter* painter) const
{
	painter->save();

	auto v = view();
	auto points = dynamic_cast<Points*> (dataItem());
	const auto& pvec = points->points();

	QColor c = color;
	c.setAlphaF(1.0 - transparency / 100.0);
	for (GeometryPoint* p : pvec) {
		QPointF p2 = v->conv(QPointF(p->x(), p->y()));

		QRectF rect(p2.x() - size * 0.5, p2.y() - size * 0.5, size, size);
		painter->fillRect(rect, c);
	}

	painter->restore();
}

void PointsPreProcessorViewHelper::drawDiamonds(int size, const QColor& color, int transparency, QPainter* painter) const
{
	painter->save();

	auto v = view();
	auto points = dynamic_cast<Points*> (dataItem());
	const auto& pvec = points->points();

	QColor c = color;
	c.setAlphaF(1.0 - transparency / 100.0);
	painter->setPen(c);
	painter->setBrush(QBrush(c));
	for (GeometryPoint* p : pvec) {
		QPointF p2 = v->conv(QPointF(p->x(), p->y()));

		QPolygonF polygon;
		polygon.push_back(QPointF(p2.x()             , p2.y() - size * 0.5));
		polygon.push_back(QPointF(p2.x() - size * 0.5, p2.y()             ));
		polygon.push_back(QPointF(p2.x()             , p2.y() + size * 0.5));
		polygon.push_back(QPointF(p2.x() + size * 0.5, p2.y()             ));
		polygon.push_back(QPointF(p2.x()             , p2.y() - size * 0.5));
		painter->drawPolygon(polygon);
	}

	painter->restore();
}

void PointsPreProcessorViewHelper::drawTriangles(int size, const QColor& color, int transparency, QPainter* painter) const
{
	painter->save();

	auto v = view();
	auto points = dynamic_cast<Points*> (dataItem());
	const auto& pvec = points->points();

	QColor c = color;
	c.setAlphaF(1.0 - transparency / 100.0);
	painter->setPen(c);
	painter->setBrush(QBrush(c));
	for (GeometryPoint* p : pvec) {
		QPointF p2 = v->conv(QPointF(p->x(), p->y()));

		QPolygonF polygon;
		polygon.push_back(QPointF(p2.x()             , p2.y() - size * 0.5));
		polygon.push_back(QPointF(p2.x() - size * 0.5, p2.y() + size * 0.5));
		polygon.push_back(QPointF(p2.x() + size * 0.5, p2.y() + size * 0.5));
		polygon.push_back(QPointF(p2.x()             , p2.y() - size * 0.5));
		painter->drawPolygon(polygon);
	}

	painter->restore();
}

void PointsPreProcessorViewHelper::drawReverseTriangles(int size, const QColor& color, int transparency, QPainter* painter) const
{
	painter->save();

	auto v = view();
	auto points = dynamic_cast<Points*> (dataItem());
	const auto& pvec = points->points();

	QColor c = color;
	c.setAlphaF(1.0 - transparency / 100.0);
	painter->setPen(c);
	painter->setBrush(QBrush(c));
	for (GeometryPoint* p : pvec) {
		QPointF p2 = v->conv(QPointF(p->x(), p->y()));

		QPolygonF polygon;
		polygon.push_back(QPointF(p2.x()             , p2.y() + size * 0.5));
		polygon.push_back(QPointF(p2.x() - size * 0.5, p2.y() - size * 0.5));
		polygon.push_back(QPointF(p2.x() + size * 0.5, p2.y() - size * 0.5));
		polygon.push_back(QPointF(p2.x()             , p2.y() + size * 0.5));
		painter->drawPolygon(polygon);
	}

	painter->restore();
}

void PointsPreProcessorViewHelper::drawNames(QPainter* painter) const
{
	painter->save();

	auto v = view();
	auto points = dynamic_cast<Points*> (dataItem());
	const auto& pvec = points->points();

	QFont f;
	f.setPointSize(fontSize);
	painter->setPen(Qt::black);
	painter->setFont(f);

	for (GeometryPoint* p : pvec) {
		if (p->name().isNull()) {continue;}

		QPointF p2 = v->conv(QPointF(p->x(), p->y()));
		p2 += QPointF(labelOffset, fontSize * 0.5);

		painter->drawText(p2, p->name());
	}

	painter->restore();
}
