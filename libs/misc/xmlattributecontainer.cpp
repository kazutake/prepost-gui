#include "xmlattributecontainer.h"
#include "private/xmlattributecontainer_impl.h"

#include <QString>

XmlAttributeContainer::XmlAttributeContainer() :
	impl {new Impl {}}
{}

XmlAttributeContainer::XmlAttributeContainer(const XmlAttributeContainer& c) :
	XmlAttributeContainer {}
{
	*(impl) = *(c.impl);
}

XmlAttributeContainer::~XmlAttributeContainer()
{
	delete impl;
}

XmlAttributeContainer& XmlAttributeContainer::operator=(const XmlAttributeContainer& c)
{
	*(impl) = *(c.impl);
	copyValue(c);
	return *this;
}

const QString& XmlAttributeContainer::prefix() const
{
	return impl->m_prefix;
}

void XmlAttributeContainer::setPrefix(const QString& prefix)
{
	impl->m_prefix = prefix;
}

QString XmlAttributeContainer::attName(const QString& name) const
{
	if (impl->m_prefix.isEmpty()) {return name;}

	QString fullName = impl->m_prefix;
	fullName.append(name.left(1).toUpper());
	fullName.append(name.mid(1));
	return fullName;
}

void XmlAttributeContainer::copyValue(const XmlAttributeContainer&)
{}
