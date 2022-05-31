#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		AssetTexture.h
	@brief		メッシュ管理
	@author		shiba
	@date		2013/4/28 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
#include "AssetBase.h"
#include "FrameWork/Rendering/ModelDataType.h"

class CRender;
class CAssetMesh : public CAssetBase
{
	//コピー禁止
	void operator =(const CAssetMesh& src) { NS_UNUSED(src);  };
	CAssetMesh(const CAssetMesh& src);

public:
	friend class CLoaderModel;
	friend class CLoaderModel_FBX;

#if defined(_DEBUG)
	friend class CAssetModel;
#endif

	CAssetMesh();
	virtual ~CAssetMesh();

	bool CreateBuffer();																//!< メッシュ情報の生成
	bool CopyMesh(CAssetMesh* pcCopyMesh);												//!< メッシュコピー
	bool SetVertexData(BasicVertex caVertexs[], const s32 nArraySize);					//!< 頂点データ外部指定
	bool SetIndexData(u16 caIndecies[], const s32 nArraySize);							//!< インデックスデータ外部指定
	bool Draw( CRender* pcRender, const bool bInstanceDraw, class CMaterial* pcMat[]);	//!< メッシュ描画命令
	void SetMaterialIndex(const s32 nMaterialIndex);									//!< マテリアルインデックスデータの設定
	
	//!< スキン使用フラグ
	void SetSkinMeshFlag( const bool bSkin )
	{
		m_bSkinMesh = bSkin;
	}
	bool IsSkinMesh() const
	{
		return m_bSkinMesh;
	}

private:
	//!< トポロジータイプ
	D3D_PRIMITIVE_TOPOLOGY		m_ePrimType;

	//!< 頂点情報
	std::vector< BasicVertex > m_caVertices;

	//!< インデックス情報配列
	u16* m_unapIndecies;

	//!< インデックス数
	u32	m_unIndexCount;

	//!< 頂点バッファ
	ID3D11Buffer*	m_pVtxBuffer;

	//!< インデックスバッファ
	ID3D11Buffer*	m_pIdxBuffer;

	//!< 頂点情報幅
	u32 m_unVtxStride;

public:
	//!< 使用するマテリアルインデックス
	s32* m_naMaterialIndex;

	//!< マテリアルインデックスカウンタ
	s32 m_nMaterialIndexCount;

private:
	//!< スキンメッシュか否か
	bool m_bSkinMesh;

private: 
	void Term();	//破棄
};

//EOF