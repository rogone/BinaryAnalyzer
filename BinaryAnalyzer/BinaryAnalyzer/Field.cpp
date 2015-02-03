#include "Field.h"

void reverse(buffer_t* pBuff, size_t sz)
{
	buffer_t b;
	buffer_t* p = pBuff;
	buffer_t* q = pBuff + sz - 1;
	for(;p < q; ++p, --q)
	{
		b = *p;
		*p = *q;
		*q = b;
	}
}

StringField::StringField():
Field(Field::FieldType::varchar),
m_pBuffer(nullptr),
m_nSize(0)
{

}

StringField::StringField(buffer_t* pBuffer, size_t sz):
Field(Field::FieldType::varchar)
{
	m_nSize = sz;
	m_pBuffer = new buffer_t[m_nSize];
	memcpy(m_pBuffer, pBuffer, m_nSize);
}

StringField::~StringField()
{

}

int32_t StringField::parse(buffer_t* pBuffer, size_t sz)
{
	if (!m_pBuffer)
	{
		delete[] m_pBuffer;
	}

	m_nSize = sz;
	m_pBuffer = new buffer_t[m_nSize];
	memcpy(m_pBuffer, pBuffer, m_nSize);
	return m_nSize;
}

std::string StringField::toString()
{
	if (0 == m_nSize)
	{
		return "";
	}

	buffer_t* pBuffer = new buffer_t[m_nSize + 1];
	memcpy(pBuffer, m_pBuffer, m_nSize);
	pBuffer[m_nSize] = 0x00;

	for(size_t i = 0; i < m_nSize; ++i)
	{
		if (isalnum(pBuffer[i]))
		{
		} 
		else if (isspace(pBuffer[i]))
		{
		} 
		else
		{
			pBuffer[i] = '.';
		}
	}

	std::string s(reinterpret_cast<char*>(pBuffer));
	delete[] pBuffer;
	return s;
}

IntField::IntField(Field::FieldType t):
Field(t),
m_bReverse(false),
m_bNetByteOrder(false)
{
	m_nSize = size_t(m_nFieldType);
	m_pValue = new buffer_t[m_nSize];
}
IntField::~IntField()
{
	if (!m_pValue)
	{
		delete[] m_pValue;
	}
}

int IntField::write(buffer_t* pBuffer)
{
	buffer_t* p = m_pValue;
	int step = 1;
	if(m_bReverse)
	{
		p = m_pValue + m_nSize - 1;
		step = -1;
	}

	for(size_t i = 0; i < m_nSize; i++)
	{
		pBuffer[i] = *p;
	}

	return m_nSize;
}

int IntField::parse(buffer_t* pBuffer, size_t sz)
{
	if (sz < m_nSize)
	{
		return 0;
	}
	else
	{
		memcpy(m_pValue, pBuffer, m_nSize);
		return m_nSize;
	}
}

std::string IntField::toString()
{
	return "";
}

int IntField::addBitField(const std::string& name, uint32_t from, size_t sz)
{

}

int IntField::set(uint32_t value, uint32_t from, size_t sz)
{

}

int IntField::set(uint32_t value, const std::string& name)
{

}

uint32_t IntField::get(uint32_t from, size_t sz)
{

}

uint32_t IntField::get(const std::string& name)
{

}

bool IntField::needReverse()
{

}

void IntField::needReverse(bool)
{

}

bool useNetByteOrder()
{

}

void useNetByteOrder(bool)
{

}

bool hasBitField()
{

}

void clearBitField()
{

}
