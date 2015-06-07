#include "gridcreatingconditiontriangleholepolygon.h"

#include <vtkProperty.h>

GridCreatingConditionTriangleHolePolygon::GridCreatingConditionTriangleHolePolygon(GridCreatingConditionTriangle* parent)
	: GridCreatingConditionTriangleAbstractPolygon(parent)
{
	m_paintActor->GetProperty()->SetColor(0, 0, 1);
	m_paintActor->GetProperty()->SetOpacity(0.1);
	m_edgeActor->GetProperty()->SetColor(0, 0, 1);
	m_vertexActor->GetProperty()->SetColor(0, 0, 1);
}
