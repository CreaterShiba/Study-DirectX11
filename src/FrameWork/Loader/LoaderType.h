
#pragma once
#include "Loader.h"

enum LOADER_TYPE
{
	eLOADER_TYPE_NONE,
	eLOADER_TYPE_MODEL,
	eLOADER_TYPE_TEXTURE,
	eLOADER_TYPE_SHADER,
	eLOADER_TYPE_TEXT
};

class CLoaderStruct
{
public:
	CLoaderStruct() :
		m_unUID(0),
		m_eLoaderType(LOADER_TYPE::eLOADER_TYPE_NONE)
	{}

	CLoaderStruct(LOADER_TYPE eType, u32 uID) :
		m_eLoaderType(eType), m_unUID(uID){}

	virtual ~CLoaderStruct() {}

	u32 m_unUID;				//ユニークID
	LOADER_TYPE m_eLoaderType;	//ローダータイプ
	TCHAR m_strFileName[256];	//ファイル名
};

class CLoadingStruct : public CLoaderStruct
{
public:
	CLoadingStruct() : m_pcLoader(nullptr)
	{}

	CLoadingStruct(CLoader* pcLoader, LOADER_TYPE eType, u32 uID) :
		CLoaderStruct( eType, uID ),
		m_pcLoader(pcLoader)
	{}

	~CLoadingStruct()
	{
		SAFE_DELETE(m_pcLoader);
	}
	CLoader* m_pcLoader;
};

//EOF
