#include "../pre/gridcond/editwidget/gridattributerealoptioneditwidget.h"
#include "../scalarstocolors/colortransferfunctioncontainer.h"
#include "solverdefinition.h"
#include "solverdefinitiongridattributerealoptioncell.h"

SolverDefinitionGridAttributeRealOptionCell::SolverDefinitionGridAttributeRealOptionCell(const QDomElement& elem, SolverDefinition* solverDef, int order) :
	SolverDefinitionGridAttributeRealCell {elem, solverDef, true, order}
{
	loadEnumeration(elem, solverDef->buildTranslator());
}

GridAttributeVariationEditWidget* SolverDefinitionGridAttributeRealOptionCell::variationEditWidget(QWidget*)
{
	return nullptr;
}

GridAttributeEditWidget* SolverDefinitionGridAttributeRealOptionCell::editWidget(QWidget* parent)
{
	GridAttributeRealOptionEditWidget* w = new GridAttributeRealOptionEditWidget(parent, this);
	w->setEnumerations(enumerations());
	return w;
}

ScalarsToColorsEditWidget* SolverDefinitionGridAttributeRealOptionCell::createScalarsToColorsEditWidget(QWidget* parent) const
{
	return createColorTransferFunctionEditWidget(parent);
}

ScalarsToColorsContainer* SolverDefinitionGridAttributeRealOptionCell::createScalarsToColorsContainer(ProjectDataItem* d)
{
	return createColorTransferFunctionContainer(d);
}
