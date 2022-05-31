#pragma once


/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		ModelDataType.h
	@brief		モデルデータ形式宣言
	@author		shiba
	@date		2012/9/22 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#include "FrameWork/Rendering/TextureDataType.h"

static const s32 BONE_INVALID_ID = 0;	//ボーン無効ID
static const s32 BONE_MAX = 32;			//使用できるボーン最大数
const s32 TEXTURE_FILE_PATH = 256;		//パス最大
const s32 TEXTURE_MAX_NUM = 8;			//テクスチャ最大

//--------------------------------------------------------------------
// プリミティブタイプ
//--------------------------------------------------------------------
enum E_NS_PRIMTYPE
{
	E_NS_PRIMTYPE_POINTLIST = D3D_PRIMITIVE_TOPOLOGY_POINTLIST,
	E_NS_PRIMTYPE_LINELIST = D3D_PRIMITIVE_TOPOLOGY_LINELIST,
	E_NS_PRIMTYPE_LINESTRIP = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP,
	E_NS_PRIMTYPE_TRIANGLELIST = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
	E_NS_PRIMTYPE_TRIANGLESTRIP = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
	E_NS_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST,
};

//--------------------------------------------------------------------
// 頂点宣言
// すべてのデータを内包する形。パフォーマンスに応じて分ける可能性もあり
//--------------------------------------------------------------------
struct BasicVertex
{
	//スキンなし
	BasicVertex() 
		: v3Position()
		, v2TexCoord()
		, v3Normal()
	{
		Init();
	}

	BasicVertex( NSVector3 vPos, NSVector2 tex, NSVector3 nom )
		: v3Position( vPos )
		, v2TexCoord( tex )
		, v3Normal( nom )
	{
		Init();
	}

	//スキンあり
	BasicVertex(NSVector3 vPos, NSVector2 tex, NSVector3 nom, NSVector3 weight, NSVector4 boneIndex)
		: v3Position(vPos)
		, v2TexCoord(tex)
		, v3Normal(nom)
	{
		m_faWeight[0] = weight.x;
		m_faWeight[1] = weight.y;
		m_faWeight[2] = weight.z;

		m_naBoneIndex[0] = static_cast<s32>(XMVectorGetX(boneIndex));
		m_naBoneIndex[1] = static_cast<s32>(XMVectorGetY(boneIndex));
		m_naBoneIndex[2] = static_cast<s32>(XMVectorGetZ(boneIndex));
		m_naBoneIndex[3] = static_cast<s32>(XMVectorGetW(boneIndex));
	}

	void Init()
	{
		for( s32 i = 0; i < 3; i++ )
		{
			m_faWeight[i] = 0.0f;
		}
		for( s32 i = 0; i < 4; i++ )
		{
			m_naBoneIndex[i] = BONE_INVALID_ID;
		}
	}

	NSVector3 v3Position;		//!< 座標
	NSVector2 v2TexCoord;		//!< テクスチャ座標
	NSVector3 v3Normal;			//!< 法線

	f32 m_faWeight[3];			//!< 重み
	s32 m_naBoneIndex[4];		//!< 影響をうけるボーン番号	とりあえず粗が目立たなさそうな４つで
};

// @brief	モデルデータ
struct SModelData
{ 
	SModelData() 
	{
		memset( fileName, 0, sizeof( fileName ) );
	}

	char fileName[128+1];	//!< モデルファイル名
};

// @brief	マテリアルデータ(4Byte境界に揃える)
struct SMaterialData 
{	
	u16		flags;				// 描画設定フラグ。必要に応じて追加
	// bit00:	カリング設定
	// bit01:	アルファブレンドの有無
};

//EOF