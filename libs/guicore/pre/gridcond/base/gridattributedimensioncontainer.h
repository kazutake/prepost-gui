#ifndef GRIDATTRIBUTEDIMENSIONCONTAINER_H
#define GRIDATTRIBUTEDIMENSIONCONTAINER_H

#include "../../../guicore_global.h"
#include "gridattributebaseobject.h"

#include <QObject>
#include <QVariant>
#include <QList>

class SolverDefinitionGridAttributeDimension;

class GUICOREDLL_EXPORT GridAttributeDimensionContainer : public GridAttributeBaseObject
{
	Q_OBJECT

public:
	GridAttributeDimensionContainer(SolverDefinitionGridAttributeDimension* def);
	~GridAttributeDimensionContainer();
	SolverDefinitionGridAttributeDimension* definition() const {return m_definition;}
	const std::string& name() const;
	const QString& caption() const;
	int currentIndex() const {return m_currentIndex;}
	virtual int count() const = 0;
	virtual QVariant variantValue(int index) const = 0;
	virtual QList<QVariant> variantValues() const = 0;
	virtual void setVariantValues(const QList<QVariant>& vals) = 0;
	QVariant currentVariantValue() const {return variantValue(currentIndex());}
	virtual bool loadFromExternalFile(const QString& filename) = 0;
	virtual bool saveToExternalFile(const QString& filename) = 0;
	virtual bool loadFromCgnsFile(int fn, int B, int Z) = 0;
	virtual bool saveFromCgnsFile(int fn, int B, int Z) = 0;

public slots:
	void setCurrentIndex(int index, bool noDraw = false);

signals:
	void valuesChanged();
	void valuesChanged(const QList<QVariant> before, const QList<QVariant> after);
	void currentIndexChanged(bool noDraw);
	void currentIndexChanged(int newIndex, bool noDraw);

protected:
	SolverDefinitionGridAttributeDimension* m_definition;

private:
	int m_currentIndex;

};

#endif // GRIDATTRIBUTEDIMENSIONCONTAINER_H
