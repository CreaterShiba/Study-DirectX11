#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		AssetManager.h
	@brief		Assetをここですべて一元管理
	@author		shiba
	@date		2012/7/24 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
#include <map>
#include "FrameWork/Rendering/ModelDataType.h"

class CAssetBase;
class CAssetModel;
class CAssetTexture;
class CAssetFontTexture;

class CAssetManager
{
	NS_SINGLETON(CAssetManager);
	CAssetManager();
	~CAssetManager();
public:
	bool Init();

	//アセットのロード関数
	CAssetModel* LoadModel(const TCHAR* pStrFileName, const TCHAR* pModelSettingFileName = nullptr);
	CAssetTexture* LoadTexture(const SInitTextureParam& cParam);
	CAssetTexture* LoadTexture(const TCHAR* strFileName);
	CAssetFontTexture* LoadFontTexture( const TCHAR* str );

	//アセットの取得関数
	CAssetBase* GetAsset(u32 unUID);
	CAssetBase* GetAsset(const TCHAR* strFileName);

	//アセットの削除関数
	bool RemoveAsset(CAssetBase* pcAsset);

	//アセットの登録関数
	void EntryAsset(CAssetBase* pcAsset);

private:
	void Term();

private:
	std::map < u32, CAssetBase*> m_paAssets;	//!< アセットデータ
	u32 m_nCopyCount;							//!< コピー用カウンタ

	//モーションイベントデータ保持
	std::vector<class CMotionEvent*> m_paMotionEventData;	//!< モーションイベントデータ
};

//EOF