#include <string>
#include <map>

#include "common_typedef.h"

#pragma once
class Field
{
public:
	enum class FieldType
	{ 
		varchar = -1,
		bit = 0,
		int8 = 1,
		int16 = 2,
		int32 = 4,
		int64 = 8,
	};
	Field(FieldType t):m_nFieldType(t) {};
	FieldType type();
	void name(const std::string& name) { m_sName = name; };
	std::string name() { return m_sName; };

	virtual int size() = 0;

	virtual int write(buffer_t*) = 0;
	virtual int parse(buffer_t*, size_t) = 0;
	virtual std::string toString() = 0;
protected:				
	std::string m_sName;
	FieldType m_nFieldType;
};

class StringField: public Field
{
public:
	StringField();
	StringField(buffer_t*, size_t);
	virtual ~StringField();

	virtual int size() { return m_nSize; };
	virtual int32_t write(buffer_t*);
	virtual int32_t parse(buffer_t*, size_t);
	virtual std::string toString();
protected:
	buffer_t* m_pBuffer;
	size_t m_nSize;
};

class IntField: public Field
{
public:

	IntField(Field::FieldType);
	virtual ~IntField();

	virtual int size() { return m_nSize; };
	virtual int write(buffer_t*);
	virtual int parse(buffer_t*, size_t);
	virtual std::string toString();

	int addBitField(const std::string& name, uint32_t from, size_t sz);
	int set(uint32_t value, uint32_t from, size_t sz);
	int set(uint32_t value, const std::string& name);

	 int get(uint32_t&, uint32_t from, size_t sz);
	 int get(uint32_t&, const std::string& name);

	bool needReverse();
	void needReverse(bool);

	bool useNetByteOrder();
	void useNetByteOrder(bool);


	bool hasBitField();
	void clearBitField();
protected:
	buffer_t* m_pValue;
	size_t m_nSize;
	bool m_bReverse;
	bool m_bNetByteOrder;

	class BitField
	{
	public:
		BitField(const std::string& name, IntField&, uint32_t from, size_t sz);
		~BitField() {};

		std::string name() const;
		uint32_t from() const;
		size_t size() const;
		int get(uint32_t&) const;
		int set(uint32_t);
	protected:
		std::string m_strName;
		IntField& m_refField;
		uint32_t m_nFrom;
		size_t m_nSize;
	};

	typedef std::map<uint32_t, BitField*> BitFieldMap;
	BitFieldMap m_mapBitField;
};

