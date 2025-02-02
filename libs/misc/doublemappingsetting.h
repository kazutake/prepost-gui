#ifndef DOUBLEMAPPINGSETTING_H
#define DOUBLEMAPPINGSETTING_H

#include <QList>

/// Mapping setting container for double-precision attributes
class DoubleMappingSetting
{
public:
	/// Target node (or cell) index
	unsigned int target;

	QList<unsigned int> indices;
	QList<double> weights;
};

#endif // DOUBLEMAPPINGSETTING_H
