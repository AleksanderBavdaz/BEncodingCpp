#include "BValue.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>

int main(int argc, char **argv)
{
	using namespace std;

	if (argc != 2)
	{
		cout << "Pass the path to a torrent file as argument." << endl;
		return 1;
	}

	// read in file
	ifstream torrent(argv[1], ios::binary);
	torrent.seekg(0, ios::end);
	vector<char> v(torrent.tellg());
	torrent.seekg(0);
	torrent.read(v.data(), v.size());

	// create a bvalue object from the data read
	string s(v.begin(), v.end());
	BValue d = BValue::FromBEncodedString(s);
	
	// assert that bencoding our object yields the original input
	assert(s == d.ToBEncodedString());

	// print out some info about the torrent
	BValue::DictType root = d.GetDict();
	cout << "announce-url: " << root.at("announce").GetString() << endl;
	BValue::DictType::iterator it = root.find("announce-list");
	if (it != root.end())
	{
		BValue::ListType announce_list = it->second.GetList();
		cout << "announce-list: " << endl;
		BValue::ListType::iterator it;
		for (it = announce_list.begin(); it != announce_list.end(); ++it)
		{
			cout << '\t' << it->GetString() << endl;
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
	BValue::DictType info = root.at("info").GetDict();
	cout << "info: " << endl;
	cout << '\t' << "name: " << info.at("name").GetString() << endl;
	cout << '\t' << "piece length: " << info.at("piece length").GetInt() << endl;
	// the pieces value is a string of concatenated sha1 hashes
	// we will just display the number of pieces, which we obtain
	// by dividing the strings length through the length of a sha1 hash
	cout << '\t' << "pieces: " << info.at("pieces").GetString().length() / 20 << endl;
	it = info.find("private");
	if (it != info.end())
	{
		cout << '\t' << "private: " << it->second.GetInt() << endl;
	}
	it = info.find("source");
	if (it != info.end())
	{
		cout << '\t' << "source: " << it->second.GetString() << endl;
	}
	it = info.find("files");
	if (it != info.end())
	{
		// multi file
		cout << "\t" << "files:" << endl;
		BValue::ListType files_list = info.at("files").GetList();
		BValue::ListType::iterator it;
		for (it = files_list.begin(); it != files_list.end(); ++it)
		{
			BValue::DictType file = it->GetDict();
			BValue::ListType path = file.at("path").GetList();
			BValue::ListType::iterator it;
			cout << "\t\t";
			for (it = path.begin(); it != path.end(); ++it)
			{
				cout << '/' << it->GetString();
			}
			cout << endl;
			cout << "\t\t" << "length: " << file.at("length").GetInt() << endl;
		}
	}
	else
	{
		// single file
		cout << '\t' << "length: " << info.at("length").GetInt() << endl;
	}

	return 0;
}
