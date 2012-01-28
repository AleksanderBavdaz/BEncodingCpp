#include "Torrent.h"
#include <fstream>
#include <iostream>
#include <cassert>

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
	if (!file)
	{
		cout << "Could not open file \"" << argv[1] << "\"." << endl;
		return 1;
	}
	file.seekg(0, ios::end);
	vector<char> v(file.tellg());
	file.seekg(0);
	file.read(v.data(), v.size());

	// create a bvalue object from the data read
	string s(v.begin(), v.end());	
	BValue b(0); // a little hack as we don't allow default construction
	try
	{
		b = BValue::FromBEncodedString(s);
	}
	catch (const std::exception &exc)
	{
		cout << exc.what();
		return 1;
	}

	// assert that bencoding our object yields the original input
	assert(s == b.ToBEncodedString());

	// construct and print out some info about the torrent
	Torrent torrent(b);
	cout << "Announce: " << torrent.GetAnnounce() << endl;
	if (!torrent.GetAnnounceList().empty())
	{
		cout << "AnnounceList:" << endl;
		auto &announce_list = torrent.GetAnnounceList();
		for (auto it = announce_list.begin(); it < announce_list.end(); ++it)
		{
			cout << '\t' << *it << endl;
		}
	}	
	time_t cd = torrent.GetCreationDate();
	cout << "CreationDate: " << ctime(&cd);
	cout << "Comment: " << torrent.GetComment() << endl;
	cout << "CreatedBy: " << torrent.GetCreatedBy() << endl;
	cout << "Encoding: " << torrent.GetEncoding() << endl;
	cout << "Name: " << torrent.GetName() << endl;
	cout << "PieceLength: " << torrent.GetPieceLength() << endl;
	cout << "Pieces (count): " << torrent.GetPieces().size() << endl;
	cout << "Private: " << torrent.GetPrivate() << endl;
	if (!torrent.GetFiles().empty())
	{
		// multi file torrent
		cout << "Files:" << endl;
		auto &files = torrent.GetFiles();
		for (auto it = files.begin(); it < files.end(); ++it)
		{
			cout << '\t' << "Path: ";
			for (auto it_p = it->second.begin(); it_p < it->second.end(); ++it_p)
			{
				cout << '/' << *it_p;
			}
			cout << endl;
			cout << '\t' << "Length: " << it->first << endl;
		}
	}
	else
	{
		// single file torrent
		cout << "Length: " << torrent.GetLength() << endl;
	}
	
	return 0;
}
