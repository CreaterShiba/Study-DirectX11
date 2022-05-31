#ifndef __ASSIMPLOADER_H__
#define __ASSIMPLOADER_H__

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		LoaderAssimp.h
	@brief		Assimpを使用したモデルローダー
	@author		ta.Ishii
	@date		2013/4/25 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "Framework/Loader/LoaderModel.h"
#if 0
#include "FrameWork/Rendering/Mesh.h"

#include <cimport.h>
#include <scene.h>
#include <postprocess.h>
#endif

//==========================================================================*/
/*!
	@class	CLoaderAssimp
	@brief	Assimpを使用したモデルローダー
*/
//==========================================================================*/
class CLoaderAssimp : public CLoaderModel
{
#if 0
	//@note コピー禁止
	void operator =(const CLoaderAssimp& src);
	CLoaderAssimp(const CLoaderAssimp& src);
public:
	// @brief コンストラクタ
	CLoaderAssimp();

	// @brief デストラクタ
	~CLoaderAssimp();

public:
	// @brief 初期化
	virtual	bool	Init();
	
	// @brief 更新
	virtual void	Calc( const f32 fDelta );

	// @brief 破棄
	virtual void	Destroy();		
	
	// @brief ロード
	virtual bool	Load( const TCHAR* strFileName, CModel* pcModel );	

private:

	// @brief ジオメトリ読み込み
	virtual void LoadGeometry();

	// @brief マテリアル読み込み
	virtual void LoadMaterial();

	// @brief スキン情報読み込み
	virtual void LoadSkin();	

	// @brief モーション読み込み
	virtual void LoadMotion();		

	// @brief 破棄
	void Term();

	// @brief ジオメトリの初期化
	void LoadGeomCore( CMesh* pMesh, aiMesh* aMesh);
	
	// @brief マテリアルの初期化
	void LoadMaterialCore( CMesh* pMesh, aiMaterial* pMaterial );

	// @brief スキンの初期化
	void LoadSkinCore( const aiScene* pScene, CMesh* pMesh, aiMesh* aMesh);

	// @brief ノード探索
	aiNode* FindNode( aiNode* aNode, const TCHAR* nodename );

	// @brief ノードの絶対座標を算出
	void CalcWorldMatrixNode( aiMatrix4x4* pResult, aiNode* pNode );

private:
	//!< モデルシーン
	const aiScene* m_pAiScene;		

	//!< ロードモデル
	class CModel*	m_pLoadModel;	
#endif
};

////Lib読み込み
//#if defined( _DEBUG )
//#pragma comment( lib, "assimpd.lib")
//#endif
//#if defined( _RELEASE )
//#pragma comment( lib, "assimpr.lib")
//#endif

#endif // __ASSIMPLOADER_H__

//EOF