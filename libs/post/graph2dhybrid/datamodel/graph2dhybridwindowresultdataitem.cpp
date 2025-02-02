#include "../graph2dhybridwindowresultsetting.h"
#include "../graph2dhybridwindowview.h"
#include "graph2dhybridwindowresultcopydataitem.h"
#include "graph2dhybridwindowresultcopygroupdataitem.h"
#include "graph2dhybridwindowresultdataitem.h"
#include "graph2dhybridwindowresultgroupdataitem.h"

#include <guibase/qwtplotcustomcurve.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/stringtool.h>

#include <QStandardItem>
#include <QVector3D>

#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkSmartPointer.h>
#include <vtkStructuredGrid.h>

#include <cmath>

Graph2dHybridWindowResultDataItem::Graph2dHybridWindowResultDataItem(const QString& title, int index, const Graph2dHybridWindowResultSetting::Setting& setting, Graph2dWindowDataItem* parent)
	: Graph2dHybridWindowDataItem(title, QIcon(":/libs/guibase/images/iconPaper.png"), parent)
{
	Q_UNUSED(index)

	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();
	m_isDeletable = false;

	m_attached = true;
	m_curve = new QwtPlotCustomCurve(title);
	m_curve->setXAxis(QwtPlot::xBottom);

//	m_data = 0;
	setSetting(setting);

	Graph2dHybridWindowView* view = dataModel()->view();
	m_curve->attach(view);
}

Graph2dHybridWindowResultDataItem::~Graph2dHybridWindowResultDataItem()
{
	delete m_curve;
//	if (m_data != 0){delete m_data;}
}

void Graph2dHybridWindowResultDataItem::setVisible(bool visible)
{
	if (visible && ! m_attached) {
		m_attached = true;
		m_curve->attach(dataModel()->view());
	}
	if (! visible && m_attached) {
		m_attached = false;
		m_curve->detach();
	}
}

void Graph2dHybridWindowResultDataItem::update(int fn)
{
	updateValues(fn);

	QVector<double> xvals;
	QVector<double> yvals;

	const Graph2dHybridWindowResultSetting& s = dataModel()->setting();
	Graph2dHybridWindowDataItem::buildData(
		m_xValues, m_yValues, s, m_setting, xvals, yvals);

	double* x, *y;
	buildXY(xvals, yvals, &x, &y);

	m_curve->setSamples(x, y, xvals.count());
	delete x;
	delete y;
}

Graph2dHybridWindowResultCopyDataItem* Graph2dHybridWindowResultDataItem::copy(Graph2dHybridWindowResultCopyGroupDataItem* parent)
{
	Graph2dHybridWindowResultCopyDataItem* ret = new Graph2dHybridWindowResultCopyDataItem(parent);
	ret->setData(m_xValues, m_yValues);
	ret->setSetting(m_setting);
	return ret;
}

void Graph2dHybridWindowResultDataItem::setSetting(const Graph2dHybridWindowResultSetting::Setting& s)
{
	m_setting = s;
	m_setting.setupCurve(m_curve);
}

QString Graph2dHybridWindowResultDataItem::title() const
{
	return m_standardItem->text();
}

bool Graph2dHybridWindowResultDataItem::axisNeeded(Graph2dHybridWindowResultSetting::AxisSide as) const
{
	return (m_setting.axisSide() == as);
}
