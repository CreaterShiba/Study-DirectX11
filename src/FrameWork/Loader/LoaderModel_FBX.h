#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		LoaderModel_FBX.h
	@brief		モデルローダー(FBXSDK使用)
	@author		shiba
	@date		2012/9/23 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
#include "Framework/Loader/Loader.h"
#include <fbxsdk.h>


class CModel;
class CAssetMesh;
class CAssetModel;
class CMotion;

class CLoaderModel_FBX : public CLoader
{
public:
	CLoaderModel_FBX();
	virtual ~CLoaderModel_FBX();

	bool LoadStart(const TCHAR* strFileName, const TCHAR* pModelSettingFileName /*= nullptr*/);
	CAssetModel* GetModelData() const { return m_pcModelData; }
private:
	// @brief 破棄
	void Term();

	// @brief ジオメトリ読み込み
	void LoadGeometry();

	// @brief マテリアル読み込み
	void LoadMaterial();

	// @brief スキン情報読み込み
	void LoadSkin();
	void LoadSkinCore(const FbxScene* pScene, CAssetMesh* pMesh, FbxMesh* aMesh);

	// @brief モーション読み込み
	void LoadMotion(FbxScene* pScene);
	//void LoadMotion(FbxImporter* pImport);

	// @brief ノード探索
	FbxNode* FindNode(FbxNode* aNode, const TCHAR* nodename);

private:
	//!< モデルシーン
	FbxScene* m_pFBXScene;

	//!< モデルデータ
	class CAssetModel* m_pcModelData;

	//!< メッシュ一時バッファ
	std::vector<CAssetMesh*> m_paTempAssetMesh;

	//!< FBXSDK用メッシュ一時バッファ
	std::vector<FbxMesh*> m_paTempFBXMesh;

private:
	//!< テクスチャ読み込みこあ
	bool LoadTextureCore(FbxSurfaceMaterial* pMaterial, const char* pMaterialName);

	//!< キーパラメータ取り込み
	bool SetKeyParam(
		s32 nTimeSpan,
		CMotion* pAddMot,
		FbxNode* pNode, 
		FbxAnimLayer* pLayer, 
		const char* type);
};


//EOF
