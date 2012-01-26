#include "BValue.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>

// custom indentation manipulator
struct t
{
	t(int tabs) :
		tabs(tabs) { }
	int tabs;
};

std::ostream &operator<<(std::ostream &out, const t &m)
{
	for (int i = 0; i < m.tabs; i++)
	{
		out << '\t';
	}
	return out;
}

int main(int argc, char **argv)
{
	using namespace std;

	if (argc != 2)
	{
		cout << "Pass the path to a torrent file as argument." << endl;
		return 1;
	}

	// read in file
	ifstream file(argv[1], ios::binary);
	file.seekg(0, ios::end);
	vector<char> v(file.tellg());
	file.seekg(0);
	file.read(v.data(), v.size());

	// create a bvalue object from the data read
	string s(v.begin(), v.end());	
	BValue torrent(0); // a little hack as we don't have a default constructor
	try
	{
		torrent = BValue::FromBEncodedString(s);
	}
	catch (const std::exception &exc)
	{
		cout << exc.what();
		return 1;
	}
	
	// assert that bencoding our object yields the original input
	assert(s == torrent.ToBEncodedString());

	// print out some info about the torrent
	const BValue::DictType &root = torrent.GetDict();
	cout << "announce-url: " << root.at("announce").GetString() << endl;
	BValue::DictType::const_iterator it = root.find("announce-list");
	if (it != root.end())
	{
		const BValue::ListType &announce_list = it->second.GetList();
		cout << "announce-list: " << endl;
		BValue::ListType::const_iterator it;
		for (it = announce_list.begin(); it != announce_list.end(); ++it)
		{
			cout << t(1) << it->GetString() << endl;
		}
	}
	it = root.find("creation date");
	if (it != root.end())
	{
		time_t t = it->second.GetInt();
		cout << "creation date: " << ctime(&t);
	}
	it = root.find("comment");
	if (it != root.end())
	{
		cout << "comment: " << it->second.GetString() << endl;
	}
	it = root.find("created by");
	if (it != root.end())
	{
		cout << "created by: " << it->second.GetString() << endl;
	}
	it = root.find("encoding");
	if (it != root.end())
	{
		cout << "encoding: " << it->second.GetString() << endl;
	}
	const BValue::DictType &info = root.at("info").GetDict();
	cout << "info: " << endl;
	cout << t(1) << "name: " << info.at("name").GetString() << endl;
	cout << t(1) << "piece length: " << info.at("piece length").GetInt() << endl;
	// the pieces value is a string of concatenated sha1 hashes
	// we will just display the number of pieces, which we obtain
	// by dividing the strings length through the length of a sha1 hash
	cout << t(1) << "pieces: " << info.at("pieces").GetString().length() / 20 << endl;
	it = info.find("private");
	if (it != info.end())
	{
		cout << t(1) << "private: " << it->second.GetInt() << endl;
	}
	it = info.find("source");
	if (it != info.end())
	{
		cout << t(1) << "source: " << it->second.GetString() << endl;
	}
	it = info.find("files");
	if (it != info.end())
	{
		// multi file
		cout << t(1) << "files:" << endl;
		const BValue::ListType &files_list = info.at("files").GetList();
		BValue::ListType::const_iterator it;
		for (it = files_list.begin(); it != files_list.end(); ++it)
		{
			const BValue::DictType &file = it->GetDict();
			const BValue::ListType &path = file.at("path").GetList();
			BValue::ListType::const_iterator it;
			cout << t(2);
			for (it = path.begin(); it != path.end(); ++it)
			{
				cout << '/' << it->GetString();
			}
			cout << endl;
			cout << t(2) << "length: " << file.at("length").GetInt() << endl;
		}
	}
	else
	{
		// single file
		cout << t(1) << "length: " << info.at("length").GetInt() << endl;
	}

	return 0;
}
