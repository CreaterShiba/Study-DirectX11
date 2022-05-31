
#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		LoaderManager.h
	@brief		ローダーマネージャー
	@author		shiba
	@date		2012/7/24 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include <list>

#include "LoaderType.h"
#include "FrameWork/Rendering/ModelDataType.h"
#include "FrameWork/Task/Task.h"

//==========================================================================*/
/*!
	@class	LoaderManager
	@brief	ローダーマネージャー
*/
//==========================================================================*/
class CLoaderManager : public CTask
{
	//シングルトン
	NS_SINGLETON( CLoaderManager );
	CLoaderManager();
	~CLoaderManager();

public:

public:
	bool Init();
	void Destroy();
	void Update(const f32 fDelta);

	bool LoadModel(const TCHAR* pStrFileName, const TCHAR* pModelSettingFileName = nullptr);
	bool LoadTexture(const SInitTextureParam& cParam);

private:
	std::list< CLoadingStruct > m_caList;

private:
	void CheckLoadEnd();
	bool EntryAsset(const CLoadingStruct& cLoader);
	void Term();
};

//EOF