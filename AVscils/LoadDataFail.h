#pragma once
#include <string>
class LoadDataFail
{
public:
	std::string loadDataFail(const std::wstring& pathFail);
	std::vector<std::string> loadDataFailTokens(const std::wstring& pathFail);
	std::wstring loadDataFailW(const std::wstring& pathFail);
	std::vector<std::wstring> loadDataFailTokensW(const std::wstring& pathFail);
};

