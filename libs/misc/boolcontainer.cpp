#include "boolcontainer.h"
#include "xmlsupport.h"

BoolContainer::BoolContainer(const QString& name) :
	SimpleValueContainerT<bool> {name, false}
{}

BoolContainer::BoolContainer(const QString& name, bool defaultVal) :
	SimpleValueContainerT<bool> {name, defaultVal}
{}

BoolContainer::BoolContainer(const BoolContainer& c) :
	SimpleValueContainerT<bool> {c}
{}

BoolContainer::~BoolContainer()
{}

void BoolContainer::load(const QDomNode& node)
{
	m_value = iRIC::getBooleanAttribute(node, attName(m_name), m_defaultValue);
}

void BoolContainer::save(QXmlStreamWriter& writer) const
{
	iRIC::setBooleanAttribute(writer, attName(m_name), m_value);
}

BoolContainer& BoolContainer::operator=(bool val)
{
	SimpleValueContainerT<bool>::operator =(val);
	return *this;
}

void BoolContainer::copyValue(const XmlAttributeContainer& c)
{
	const BoolContainer& c2 = dynamic_cast<const BoolContainer&> (c);
	m_value = c2.m_value;
}
