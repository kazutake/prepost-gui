#include "gridcreatingconditioncreatorgridgenerator.h"
#include "gridcreatingconditiongridgenerator.h"

GridCreatingConditionCreatorGridGenerator::GridCreatingConditionCreatorGridGenerator()
	: GridCreatingConditionCreator()
{
	m_name = "grid_generator";
	m_caption = tr("Create geometric shape grids");
	m_description = tr(
		"You can create straight grids or sine curve grids by "
		"specifying parameters."
	    );
}

GridCreatingCondition* GridCreatingConditionCreatorGridGenerator::create(ProjectDataItem* parent)
{
	return new GridCreatingConditionGridGenerator(parent, this);
}
