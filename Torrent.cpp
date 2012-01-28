#include "Torrent.h"

Torrent::Torrent(const BValue &b)
{
	auto &root = b.GetDict();

	m_Announce = root.at("announce").GetString();

	auto it = root.find("announce-list");
	if (it != root.end())
	{
		auto &announce_list = it->second.GetList();
		m_AnnounceList.reserve(announce_list.size());
		for (auto it = announce_list.begin(); it != announce_list.end(); ++it)
		{
			m_AnnounceList.push_back(it->GetString());
		}
	}
	
	it = root.find("creation date");
	m_CreationDate = it != root.end() ? it->second.GetInt() : 0;

	it = root.find("comment");
	if (it != root.end())
	{
		m_Comment = it->second.GetString();
	}

	it = root.find("created by");
	if (it != root.end())
	{
		m_CreatedBy = it->second.GetString();
	}

	it = root.find("encoding");
	if (it != root.end())
	{
		m_Encoding = it->second.GetString();
	}

	// info dictionary
	auto &binfo = root.at("info");
	auto &info = binfo.GetDict();

	m_Name = info.at("name").GetString();	

	m_PieceLength = info.at("piece length").GetInt();

	auto &pieces = info.at("pieces").GetString();
	// pieces is a string of concatenated sha1 hashes
	m_Pieces.reserve(pieces.length() / 20);
	for (std::size_t i = 0; i < pieces.length(); i += 20)
	{
		m_Pieces.push_back(pieces.substr(i, 20));
	}

	it = info.find("private");
	m_Private = it != info.end() ? it->second.GetInt() == 1 : false;

	it = info.find("source");
	if (it != info.end())
	{
		m_Source = it->second.GetString();
	}
	
	it = info.find("files");
	if (it != info.end())
	{
		// multiple file mode
		auto &files_list = info.at("files").GetList();
		m_Files.reserve(files_list.size());
		for (auto it = files_list.begin(); it != files_list.end(); ++it)
		{
			FileType f;
			auto &file = it->GetDict();
			f.first = file.at("length").GetInt();			
			auto &path = file.at("path").GetList();
			f.second.reserve(path.size());
			for (auto it = path.begin(); it != path.end(); ++it)
			{
				f.second.push_back(it->GetString());
			}			
			m_Files.push_back(f);
		}
		m_Length = 0;
	}
	else
	{
		// single file mode
		m_Length = info.at("length").GetInt();
	}

	// TODO: compute sha1 info hash of binfo.ToBEncodedString()
}
