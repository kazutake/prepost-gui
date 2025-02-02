#include "inputconditioncontainerinteger.h"
#include "private/inputconditioncontainerinteger_impl.h"

#include <misc/stringtool.h>

#include <QDomNode>
#include <QTextStream>

#include <yaml-cpp/yaml.h>

#include <iriclib.h>

InputConditionContainerInteger::Impl::Impl() :
	m_value {0},
	m_default {0}
{}

InputConditionContainerInteger::InputConditionContainerInteger() :
	InputConditionContainer(),
	impl {new Impl {}}
{}

InputConditionContainerInteger::InputConditionContainerInteger(const std::string& n, const QString& c, const QDomNode& defNode) :
	InputConditionContainer(n, c),
	impl {new Impl {}}
{
	setup(defNode);
}

InputConditionContainerInteger::InputConditionContainerInteger(const InputConditionContainerInteger& i) :
	InputConditionContainer(i),
	impl {new Impl {}}
{
	copyValues(i);
}

InputConditionContainerInteger::~InputConditionContainerInteger()
{
	delete impl;
}

InputConditionContainerInteger& InputConditionContainerInteger::operator=(const InputConditionContainerInteger& i)
{
	copyValues(i);
	return *this;
}

int InputConditionContainerInteger::value() const
{
	return impl->m_value;
}

void InputConditionContainerInteger::setValue(int v)
{
	if (impl->m_value != v) {
		impl->m_value = v;
		emit valueChanged(impl->m_value);
		emit valueChanged();
	}
}

int InputConditionContainerInteger::defaultValue() const
{
	return impl->m_default;
}

void InputConditionContainerInteger::setDefaultValue(int d)
{
	impl->m_default = d;
}

int InputConditionContainerInteger::load()
{
	int ret;
	if (isBoundaryCondition()) {
		ret = cg_iRIC_Read_BC_Integer(toC(bcName()), bcIndex(), toC(name()), &(impl->m_value));
	} else if (isComplexCondition()) {
		ret = cg_iRIC_Read_Complex_Integer(toC(complexName()), complexIndex(), toC(name()), &(impl->m_value));
	} else {
		ret = cg_iRIC_Read_Integer(toC(name()), &(impl->m_value));
	}
	if (ret != 0) {
		clear();
	} else {
		emit valueChanged(impl->m_value);
		emit valueChanged();
	}
	return ret;
}

int InputConditionContainerInteger::save()
{
	if (isBoundaryCondition()) {
		return cg_iRIC_Write_BC_Integer(toC(bcName()), bcIndex(), toC(name()), impl->m_value);
	} else if (isComplexCondition()) {
		return cg_iRIC_Write_Complex_Integer(toC(complexName()), complexIndex(), toC(name()), impl->m_value);
	} else {
		return cg_iRIC_Write_Integer(toC(name()), impl->m_value);
	}
}

void InputConditionContainerInteger::clear()
{
	if (impl->m_value != impl->m_default) {
		impl->m_value = impl->m_default;
		emit valueChanged(impl->m_value);
		emit valueChanged();
	}
}

QVariant InputConditionContainerInteger::variantValue() const
{
	return QVariant(impl->m_value);
}

void InputConditionContainerInteger::importFromYaml(const YAML::Node& doc, const QDir&)
{
	if (doc[name()]) {
		impl->m_value = doc[name()].as<int>();
		emit valueChanged(impl->m_value);
		emit valueChanged();
	}
}

void InputConditionContainerInteger::exportToYaml(QTextStream* stream, const QDir&)
{
	*stream << name().c_str() << ": " << impl->m_value << "\t#[integer] " << caption() << "\r\n";
}

void InputConditionContainerInteger::setup(const QDomNode& defNode)
{
	QDomElement e = defNode.toElement();
	impl->m_default = e.attribute("default", "0").toInt();
	impl->m_value = impl->m_default;
}

void InputConditionContainerInteger::copyValues(const InputConditionContainerInteger& i)
{
	InputConditionContainer::copyValues(i);
	setValue(i.value());
	setDefaultValue(i.defaultValue());
}
