#ifndef BVALUE_H
#define BVALUE_H

#include <string>
#include <list>
#include <map>
#include <sstream>
#include <cassert>
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
		m_Str(str),
		m_Type(STRING) { }

	BValue(long long i) :
		m_Int(i),
		m_Type(INTEGER) { }

	BValue(const ListType &list) :
		m_List(list),
		m_Type(LIST) { }

	BValue(const DictType &dict) :
		m_Dict(dict),
		m_Type(DICTIONARY) { }
	
	ValueType GetType() const
	{
		return m_Type;
	}

	// all getters return const references
	// getting the wrong type causes a debug assertion failure
	// in release a default constructed object is guaranteed	
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

	static BValue FromBEncodedString(const std::string &s, int &idx)
	{
		if (idx + 1 >= s.length())
		{
			throw std::runtime_error("end of input encountered unexpectedly");
		}
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
				int delim_pos = s.find(':', idx + 1);
				if (delim_pos == std::string::npos)
				{
					std::ostringstream oss;
					oss << "delimiter for string starting at position "
						<< idx << " was not found";
					throw std::runtime_error(oss.str());
				}
				int len;
				std::istringstream iss(s.substr(idx, delim_pos - idx));
				if (!(iss >> len))
				{
					std::ostringstream oss;
					oss << "string at position " << idx << " has invalid length";
					throw std::runtime_error(oss.str());
				}
				idx += delim_pos - idx + len;
				idx++;
				return s.substr(delim_pos + 1, len);
			}
			case 'i':
			{
				// integer
				idx += 1;
				int end = s.find('e', idx);
				if (end == std::string::npos)
				{
					std::ostringstream oss;
					oss << "ending delimiter for integer starting at position "
						<< idx << " was not found";
					throw std::runtime_error(oss.str());
				}
				std::istringstream iss(s.substr(idx, end - idx));
				long long i;
				if (!(iss >> i))
				{
					std::ostringstream oss;
					oss << "integer at position " << idx << " is invalid";
					throw std::runtime_error(oss.str());
				}
				idx = end;
				idx++;
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
					if (idx == s.length())
					{
						throw std::runtime_error("end of input encountered in list");
					}
				}
				idx++;
				return list;
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
					if (idx == s.length())
					{
						throw std::runtime_error("end of input encountered in map");
					}
					dict.insert(std::make_pair(key.GetString(), val));
				}
				idx++;
				return dict;
			}
			default:
			{
				// unknown bencoded type
				std::ostringstream oss;
				oss << "unknown beginning delimiter '" << s[idx]
					<< "' at position " << idx;
				throw std::runtime_error(oss.str());
			}
		}
	}
		
	long long m_Int;
	std::string m_Str;	
	ListType m_List;
	DictType m_Dict;
	ValueType m_Type;
};

#endif // BVALUE_H
