#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		UtilCsv.h
	@brief		CSV読み込み
	@author		shiba
	@date		2014/9/9 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

class CUtilCSV
{
	public:
	static bool LoadCsvData(const TCHAR* filename, std::vector<std::vector<std::string>>& out)
	{
		std::ifstream file(filename);

		if (file.fail()) 
		{
			NS_Report(_T("%s LoadError!\n"), filename);
			return false;
		}

		std::string str;
		u32 p;

		while (getline(file, str)) 
		{
			//コメント行は除く
			if ((p = str.find("//")) != str.npos) continue;
			std::vector<std::string> inner;

			//コンマがあるかを探し、そこまでをvaluesに格納
			while ((p = str.find(",")) != str.npos) {
				inner.push_back(str.substr(0, p));
				str = str.substr(p + 1);
			}
			inner.push_back(str);
			out.push_back(inner);
		}

		return true;
	}
};