#pragma once
#include <string>
class DataMaps
{
public:
	void NameMaps();
	//int PutSaveCode(const std::wstring& path);
	//m_NewPathSaveCode
	bool m_IsNewInfo = false;
	bool isNewMaps = false;
	std::wstring m_NameMapsFull = L"\0";
	std::wstring m_NameMaps = L"\0";
private:
};

extern DataMaps G_DATA_MAPS;

