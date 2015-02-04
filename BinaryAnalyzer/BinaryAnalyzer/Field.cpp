#include <cmath>

#include "Error.h"
#include "Field.h"

using namespace error;

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
    if (m_bReverse)
    {
        p = m_pValue + m_nSize - 1;
        step = -1;
    }

    for (size_t i = 0; i < m_nSize; i++)
    {
        pBuffer[i] = *p;
		p += step;
    }

    return m_nSize;
}

int IntField::parse(buffer_t* pBuffer, size_t sz)
{
    if (sz < m_nSize)
    {
        return ERR_INPUT_TOO_SHORT;
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
	if (from > m_nSize * 8
		|| (from + sz) > m_nSize * 8)
	{
		return ERR_BIT_OVERFLOW;
	}

	BitFieldMap::const_iterator it = m_mapBitField.find(from);
	if (it == m_mapBitField.end())
	{
		BitField* p = new BitField(name, this, from, sz);
		m_mapBitField[from] = p;
		return SUCCESS;
	} 
	else
	{
		return ERR_BIT_FIELD_EXIST;
	}
}

int IntField::set(uint32_t value, uint32_t from, size_t sz)
{
	if (0 == sz)
	{
		return ERR_BIT_ZERO_SIZE;
	}	                  
	else if (from + sz > m_nSize * 8)
	{
		return ERR_INT_OVERFLOW;
	} 
	else if ( value > pow(2, sz) - 1)
	{
		return ERR_BIT_OVERFLOW;
	} 
	else
	{
		switch (m_nFieldType)
		{
		case Field::FieldType::int8:
			uint8_t v(value);
			v << (8 - from - sz);
			uint8_t* p = reinterpret_cast<uint8_t*>(m_pValue);
			*p |= v;
			break;
		case Field::FieldType::int16:
			uint16_t v(value);
			v << (16 - from - sz);
			uint16_t* p = reinterpret_cast<uint16_t*>(m_pValue);
			*p |= v;
			break;
		case Field::FieldType::int32:
			value << (32 - from - sz);
			uint32_t* p = reinterpret_cast<uint32_t*>(m_pValue);
			*p |= value;
			break;
		case Field::FieldType::int64:
			uint64_t v(value);
			v << (64 - from - sz);
			uint64_t* p = reinterpret_cast<uint64_t*>(m_pValue);
			*p |= v;
			break;
		default:
			return ERR_INT_UNSUPPORTED;
			break;
		}
		return SUCCESS;
	}
}

int IntField::set(uint32_t value, const std::string& name)
{
	if (name.empty())
	{
		return ERR_FIELD_EMPTY_NAME;
	}

	 BitFieldMap::iterator it = m_mapBitField.begin();
	 for (; it != m_mapBitField.end(); ++it)
	{
		if (it->second.name() == name)
		{
			set(value, it->second.from(), it->second.size());
			return SUCCESS;
		}
	}
	
	 if (it == m_mapBitField.end())
	 {
		 return ERR_BIT_NO_SUCH_FIELD;
	 }
}

 int IntField::get(uint32_t& value, uint32_t from, size_t sz)
{
	 if (0 == sz)
	 {
		 return ERR_BIT_ZERO_SIZE;
	 }
	 else if (from + sz > m_nSize * 8)
	 {
		 return ERR_INT_OVERFLOW;
	 }
	 else if (value > pow(2, sz) - 1)
	 {
		 return ERR_BIT_OVERFLOW;
	 }
	 else
	 {
		 switch (m_nFieldType)
		 {
		 case Field::FieldType::int8:
			 uint8_t v = *reinterpret_cast<uint8_t*>(m_pValue);
			 v << from;
			 v >> (8 - from - sz);
			 value = v;
			 break;
		 case Field::FieldType::int16:
			 uint16_t v = *reinterpret_cast<uint16_t*>(m_pValue);
			 v << from;
			 v >> (16 - from - sz);
			 value = v;
			 break;
		 case Field::FieldType::int32:
			 uint32_t v = *reinterpret_cast<uint32_t*>(m_pValue);
			 v << from;
			 v >> (32 - from - sz);
			 value = v;
			 break;
		 default:
			 return ERR_INT_UNSUPPORTED;
			 break;
		 }
	 }
		 return SUCCESS;
}

int IntField::get(uint32_t& value, const std::string& name)
{
	 if (name.empty())
	 {
		 return ERR_FIELD_EMPTY_NAME;
	 }

	 BitFieldMap::const_iterator it = m_mapBitField.begin();
	 for (; it != m_mapBitField.end(); ++it)
	 {
		 if (it->second.name() == name)
		 {
			 get(value, it->second.from(), it->second.size());
			 return SUCCESS;
		 }
	 }

	 if (it == m_mapBitField.end())
	 {
		 return ERR_BIT_NO_SUCH_FIELD;
	 }
}

bool IntField::needReverse()
{
	return m_bReverse;
}

void IntField::needReverse(bool b)
{
	m_bReverse = b;
}

bool IntField::useNetByteOrder()
{
	return m_bNetByteOrder;
}

void IntField::useNetByteOrder(bool b)
{
	m_bNetByteOrder = b;
}

bool IntField::hasBitField()
{
	return !m_mapBitField.empty();
}

void IntField::clearBitField()
{
	for (BitFieldMap::iterator it = m_mapBitField.begin();
		it != m_mapBitField.end(); ++it)
	{
		delete it->second;
	}

	m_mapBitField.clear();
}
