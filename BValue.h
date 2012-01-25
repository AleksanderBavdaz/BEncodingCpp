#ifndef BVALUE_H
#define BVALUE_H

#include <string>
#include <list>
#include <map>
#include <sstream>
#include <cassert>

class BValue
{
public:

	typedef std::list<BValue> ListType;
	typedef std::map<std::string, BValue> DictType;

	enum ValueType
	{
		NONE,
		STRING,
		INTEGER,
		LIST,
		DICTIONARY
	};

	BValue(const std::string &str)
	{
		m_Str = str;
		m_Type = STRING;
	}

	BValue(long long i)
	{
		m_Int = i;
		m_Type = INTEGER;
	}

	BValue(const ListType &list)
	{
		m_List = list;
		m_Type = LIST;
	}

	BValue(const DictType &dict)
	{
		m_Dict = dict;
		m_Type = DICTIONARY;
	}
	
	ValueType GetType() const
	{
		return m_Type;
	}

	const std::string &GetString() const
	{
		assert(m_Type == STRING);
		return m_Str;
	}

	long long GetInt() const
	{
		assert(m_Type == INTEGER);
		return m_Int;
	}

	const ListType &GetList() const
	{
		assert(m_Type == LIST);
		return m_List;
	}

	const DictType &GetDict() const
	{
		assert(m_Type == DICTIONARY);
		return m_Dict;
	}
	
	std::string ToBEncodedString() const
	{
		std::ostringstream oss;
		assert(m_Type != NONE);		
		switch (m_Type)
		{
			case STRING:
				oss << m_Str.length() << ':' << m_Str;
				break;
			case INTEGER:
				oss << 'i' << m_Int << 'e';
				break;
			case LIST:
				oss << 'l';
				for (ListType::const_iterator it = m_List.begin();
					it != m_List.end();
					++it)
				{
					oss << it->ToBEncodedString();
				}
				oss << 'e';
				break;
			case DICTIONARY:
				oss << 'd';
				for (DictType::const_iterator it = m_Dict.begin();
					it != m_Dict.end();
					++it)
				{
					// key
					oss << BValue(it->first).ToBEncodedString();
					// value
					oss << BValue(it->second).ToBEncodedString();
				}
				oss << 'e';
				break;
		}
		return oss.str();
	}

	static BValue FromBEncodedString(const std::string &s)
	{
		int idx = 0;
		return FromBEncodedString(s, idx);
	}

private:

	// friendship with containers allows them
	// to access the default constructor
	friend DictType;
	friend ListType;

	// default constructor intended for the containers
	BValue() { m_Type = NONE; }

	static BValue FromBEncodedString(const std::string &s, int &idx)
	{		
		switch (s[idx])
		{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				{
					// string
					int delim_pos = s.find_first_of(':', idx);
					int len;
					std::istringstream iss(s.substr(idx, delim_pos - idx));
					iss >> len;					
					idx += delim_pos - idx + len + 1;
					return BValue(s.substr(delim_pos + 1, len));
				}
			case 'i':
				{
					// integer
					idx += 1;
					int end = s.find_first_of('e', idx);					
					std::istringstream iss(s.substr(idx, end - idx));
					long long i;
					iss >> i;
					idx = end + 1;
					return i;
				}
			case 'l':
				{
					// list
					idx += 1;
					ListType list;
					while (s[idx] != 'e')
					{
						list.push_back(FromBEncodedString(s, idx));
					}
					idx++;
					return BValue(list);
				}				
			case 'd':
				{
					// dictionary
					idx += 1;
					DictType dict;
					while (s[idx] != 'e')
					{
						BValue key = FromBEncodedString(s, idx);
						BValue val = FromBEncodedString(s, idx);
						dict[key.GetString()] = val;
					}
					idx++;
					return BValue(dict);
				}
			default:
				// unknown bencoded type
				assert(0);
		}
	}

	ValueType m_Type;
	std::string m_Str;
	long long m_Int;
	ListType m_List;
	DictType m_Dict;
};

#endif // BVALUE_H
