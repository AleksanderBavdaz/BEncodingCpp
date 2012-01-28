#include "BValue.h"
#include <sstream>

using namespace std;

string BValue::ToBEncodedString() const
{
	ostringstream oss;
	switch (m_Type)
	{
		case STRING:
			oss << m_pStr->length() << ':' << *m_pStr;
			break;
		case INTEGER:
			oss << 'i' << m_Int << 'e';
			break;
		case LIST:
			oss << 'l';
			for (auto it = m_pList->begin(); it != m_pList->end(); ++it)
			{
				oss << it->ToBEncodedString();
			}
			oss << 'e';
			break;
		case DICTIONARY:
			oss << 'd';
			for (auto it = m_pDict->begin(); it != m_pDict->end(); ++it)
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

BValue BValue::FromBEncodedString(const std::string &s, int &idx)
{
	if (idx + 1 >= s.length())
	{
		throw runtime_error("end of input encountered unexpectedly");
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
			if (delim_pos == string::npos)
			{
				ostringstream oss;
				oss << "delimiter for string starting at position "
					<< idx << " was not found";
				throw runtime_error(oss.str());
			}
			int len;
			istringstream iss(s.substr(idx, delim_pos - idx));
			if (!(iss >> len))
			{
				ostringstream oss;
				oss << "string at position " << idx << " has invalid length";
				throw runtime_error(oss.str());
			}
			if (!iss.eof())
			{
				ostringstream oss;
				oss << "string at position " << idx
					<< " appears to have valid length " << len
					<< " but has additional data before the delimiter";
				throw runtime_error(oss.str());
			}
			if (delim_pos + len >= s.length())
			{
				ostringstream oss;
				oss << "string at position " << idx
					<< " with length " << len
					<< " would go outside of the input data bounds";
				throw runtime_error(oss.str());
			}
			idx += delim_pos - idx + len + 1;
			return std::string(s.substr(delim_pos + 1, len));
		}
		case 'i':
		{
			// integer
			int end = s.find('e', idx + 1);
			if (end == string::npos)
			{
				ostringstream oss;
				oss << "ending delimiter for integer starting at position "
					<< idx << " was not found";
				throw runtime_error(oss.str());
			}
			istringstream iss(s.substr(idx + 1, end - idx - 1));
			long long i;
			if (!(iss >> i))
			{
				ostringstream oss;
				oss << "integer at position " << idx << " is invalid";
				throw runtime_error(oss.str());
			}
			if (!iss.eof())
			{
				ostringstream oss;
				oss << "integer at position " << idx
					<< " appears to be valid with value " << i
					<< " but has additional trailing data";
				throw runtime_error(oss.str());
			}
			idx = end + 1;
			return i;
		}
		case 'l':
		{
			// list
			idx++;
			ListType list;
			while (s[idx] != 'e')
			{
				list.push_back(FromBEncodedString(s, idx));
				if (idx == s.length())
				{
					throw runtime_error("end of input encountered in list");
				}
			}
			idx++;
			return list;
		}
		case 'd':
		{
			// dictionary
			idx++;
			DictType dict;
			while (s[idx] != 'e')
			{				
				BValue key = FromBEncodedString(s, idx);
				BValue val = FromBEncodedString(s, idx);
				if (idx == s.length())
				{
					throw runtime_error("end of input encountered in map");
				}
				dict.insert(make_pair(key.GetString(), val));
			}
			idx++;
			return dict;
		}
		default:
		{
			// unknown bencoded type
			ostringstream oss;
			oss << "unknown beginning delimiter '" << s[idx]
				<< "' at position " << idx;
			throw runtime_error(oss.str());
		}
	}
}
