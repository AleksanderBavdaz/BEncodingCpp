#include "BValue.h"
#include <sstream>
#include <vector>

std::string BValue::ToBEncodedString() const
{
	std::ostringstream oss;
	oss << *this;
	return oss.str();
}

BValue BValue::FromBEncodedString(const std::string &s)
{
	return FromBEncodedIStream(std::istringstream(s));
}

void BValue::ToBEncodedOStream(std::ostream &os) const
{
	switch (m_Type)
	{
		case STRING:
			os << m_pStr->length() << ':' << *m_pStr;
			break;
		case INTEGER:
			os << 'i' << m_Int << 'e';
			break;
		case LIST:
			os << 'l';
			for (auto it = m_pList->begin(); it != m_pList->end(); ++it)
			{
				os << *it;
			}
			os << 'e';
			break;
		case DICTIONARY:
			os << 'd';
			for (auto it = m_pDict->begin(); it != m_pDict->end(); ++it)
			{
				// key
				os << it->first.length() << ':' << it->first;
				// value
				os << it->second;
			}
			os << 'e';
			break;
	}
}

BValue BValue::FromBEncodedIStream(std::istream &is)
{
	try
	{
		int pos = is.tellg();
		char type = is.get();
		switch (type)
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
				is.putback(type);
				long long length;
				if (!(is >> length))
				{
					std::ostringstream oss;
					oss << "string starting at position " << pos
						<< " has invalid length";
					throw std::runtime_error(oss.str());
				}
				char delim = is.get();
				if (delim != ':')
				{
					std::ostringstream oss;
					oss << "string starting at position " << pos
						<< " has invalid delimiter '" << delim << "'";
					throw std::runtime_error(oss.str());
				}
				std::vector<char> v(length);
				is.read(v.data(), v.size());
				return std::string(v.begin(), v.end());
			}
			case 'i':
			{
				// integer
				long long i;
				if (!(is >> i))
				{
					std::ostringstream oss;
					oss << "integer starting at position " << pos
						<< " is invalid";
					throw std::runtime_error(oss.str());
				}
				char delim = is.get();
				if (delim != 'e')
				{
					std::ostringstream oss;
					oss << "integer starting at position " << pos
						<< " has invalid ending delimiter '" << delim << "'";
					std::runtime_error(oss.str());
				}
				return i;
			}
			case 'l':
			{
				// list
				ListType list;
				while (is.peek() != 'e')
				{
					list.push_back(FromBEncodedIStream(is));
				}
				is.get();
				return list;
			}
			case 'd':
			{
				// dictionary
				DictType dict;
				while (is.peek() != 'e')
				{
					BValue key = FromBEncodedIStream(is);
					BValue val = FromBEncodedIStream(is);
					dict.insert(std::make_pair(key.GetString(), val));
				}
				is.get();
				return dict;
			}
			default:
			{
				// unknown bencoded type
				std::ostringstream oss;
				oss << "unknown beginning delimiter '" << type
					<< "' at position " << pos;
				throw std::runtime_error(oss.str());
			}
		}
	}
	catch (const std::istream::failure &exc)
	{
		throw std::runtime_error("encountered unexpected end of input");
	}
}

std::ostream &operator<<(std::ostream &os, const BValue &b)
{
	b.ToBEncodedOStream(os);
	return os;
}

std::istream &operator>>(std::istream &is, BValue &b)
{
	auto exceptions = is.exceptions();
	is.exceptions(exceptions | std::ios::eofbit);
	b = BValue::FromBEncodedIStream(is);
	is.exceptions(exceptions);	
	return is;
}
