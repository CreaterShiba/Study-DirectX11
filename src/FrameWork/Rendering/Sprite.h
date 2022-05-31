#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		Sprite.h
	@brief		スプライト
	@author		shiba
	@date		2012/3/3 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#include "FrameWork/Utility/UtilMacro.h"
#include "FrameWork/Rendering/Render.h"

class CSprite;
class CAssetTexture;

class CSprite : public CRender
{
public:
	CSprite();
	virtual ~CSprite();

	virtual bool Init();
	virtual void Update( const f32 fDelta );
	virtual void Draw();
	virtual void Destroy();

private:
	//破棄
	void Term();

public:
	//テクスチャ指定
	bool SetTexture(CAssetTexture* pcTexture);
#if defined( _DEBUG )
	bool SetTexture(ID3D11ShaderResourceView* pcResource);
#endif

	//UV指定
	bool SetUV( const f32 u, const f32 v, const f32 u2, const f32 v2 );

private:
	static u32 m_nSpriteCount;		//!< スプライトのカウンタ
	class CAssetMesh* m_pcMesh;		//!< メッシュ
	class CMaterial* m_pcMat;		//!< マテリアル
};

//EOF
