#include "inputconditioncontainerstring.h"

#include <misc/stringtool.h>

#include <QDomNode>

#include <iriclib.h>

void InputConditionContainerString::setup(const QDomNode& defNode)
{
	QDomElement e = defNode.toElement();
	m_default = e.attribute("default", "");
	m_value = m_default;
}

int InputConditionContainerString::load()
{
	char buffer[200];
	int ret;
	if (m_isBoundaryCondition) {
		ret = cg_iRIC_Read_BC_String(const_cast<char*>(iRIC::toStr(m_bcName).c_str()), m_bcIndex, const_cast<char*>(iRIC::toStr(m_name).c_str()), buffer);
	} else if (m_isComplexCondition) {
		ret = cg_iRIC_Read_Complex_String(const_cast<char*>(iRIC::toStr(m_complexName).c_str()), m_complexIndex, const_cast<char*>(iRIC::toStr(m_name).c_str()), buffer);
	} else {
		ret = cg_iRIC_Read_String(const_cast<char*>(iRIC::toStr(m_name).c_str()), buffer);
	}
	if (ret != 0) {
		clear();
	} else {
		m_value = buffer;
		emit valueChanged(m_value);
		emit valueChanged();
	}
	return ret;
}
int InputConditionContainerString::save()
{
	std::string value = m_value.toUtf8().constData();
	if (m_isBoundaryCondition) {
		return cg_iRIC_Write_BC_String(const_cast<char*>(iRIC::toStr(m_bcName).c_str()), m_bcIndex, const_cast<char*>(iRIC::toStr(m_name).c_str()), const_cast<char*>(value.c_str()));
	} else if (m_isComplexCondition) {
		return cg_iRIC_Write_Complex_String(const_cast<char*>(iRIC::toStr(m_complexName).c_str()), m_complexIndex, const_cast<char*>(iRIC::toStr(m_name).c_str()), const_cast<char*>(value.c_str()));
	} else {
		return cg_iRIC_Write_String(const_cast<char*>(iRIC::toStr(m_name).c_str()), const_cast<char*>(value.c_str()));
	}
}
