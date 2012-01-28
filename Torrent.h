#pragma once

#include "BValue.h"
#include <vector>
#include <ctime>

class Torrent
{
public:

	typedef std::pair<long long, std::vector<std::string>> FileType;
	
	Torrent(const BValue &);
	
	// inline getters
	const std::string &GetAnnounce() const
	{
		return m_Announce;
	}

	const std::vector<std::string> &GetAnnounceList() const
	{
		return m_AnnounceList;
	}

	time_t GetCreationDate() const
	{
		return m_CreationDate;
	}

	const std::string &GetComment() const
	{
		return m_Comment;
	}

	const std::string &GetCreatedBy() const
	{
		return m_CreatedBy;
	}

	const std::string &GetEncoding() const
	{
		return m_Encoding;
	}

	const std::string &GetName() const
	{
		return m_Name;
	}

	long long GetPieceLength() const
	{
		return m_PieceLength;
	}

	const std::vector<std::string> &GetPieces() const
	{
		return m_Pieces;
	}

	bool GetPrivate() const
	{
		return m_Private;
	}

	const std::string &GetSource() const
	{
		return m_Source;
	}

	const std::vector<FileType> &GetFiles() const
	{
		return m_Files;
	}

	long long GetLength() const
	{
		return m_Length;
	}

private:
	// mandatory fields
	std::string m_Announce;	
	// optional fields	
	std::vector<std::string> m_AnnounceList;
	std::time_t m_CreationDate;
	std::string m_Comment;
	std::string m_CreatedBy;
	std::string m_Encoding;
	// mandatory info fields
	std::string m_Name;
	int m_PieceLength;
	std::vector<std::string> m_Pieces;
	// optional info fields
	bool m_Private;
	std::string m_Source;
	// only set in multiple file mode
	std::vector<FileType> m_Files;
	// only set in single file mode
	long long m_Length;
};
