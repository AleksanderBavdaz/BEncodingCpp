#pragma once

#include <string>
#include <list>
#include <map>
#include <exception>

class BValue
{
public:

	typedef std::list<BValue> ListType;
	typedef std::map<std::string, BValue> DictType;
	
	enum ValueType
	{
		STRING,
		INTEGER,
		LIST,
		DICTIONARY
	};

	BValue(const std::string &str) :
		m_pStr(new std::string(str)),
		m_Type(STRING) { }

	BValue(long long i) :
		m_Int(i),
		m_Type(INTEGER) { }

	BValue(const ListType &list) :
		m_pList(new ListType(list)),
		m_Type(LIST) { }

	BValue(const DictType &dict) :
		m_pDict(new DictType(dict)),
		m_Type(DICTIONARY) { }
	
	ValueType GetType() const
	{
		return m_Type;
	}

	const std::string &GetString() const
	{
		static std::string s;
		return m_Type == STRING ? *m_pStr : s;
	}

	long long GetInt() const
	{
		return m_Type == INTEGER ? m_Int : 0;
	}

	const ListType &GetList() const
	{
		static ListType l;
		return m_Type == LIST ? *m_pList : l;
	}

	const DictType &GetDict() const
	{
		static DictType d;
		return m_Type == DICTIONARY ? *m_pDict : d;
	}

	std::string ToBEncodedString() const;
	static BValue FromBEncodedString(const std::string &s);

private:

	void ToBEncodedOStream(std::ostream &os) const;
	friend std::ostream &operator<<(std::ostream &os, const BValue &b);

	static BValue FromBEncodedIStream(std::istream &is);
	friend std::istream &operator>>(std::istream &os, BValue &b);
	
	long long m_Int;
	std::shared_ptr<std::string> m_pStr;
	std::shared_ptr<ListType> m_pList;
	std::shared_ptr<DictType> m_pDict;
	ValueType m_Type;
};

// bencode
std::ostream &operator<<(std::ostream &os, const BValue &b);
// bdecode, may throw std::runtime_error
std::istream &operator>>(std::istream &is, BValue &b);
