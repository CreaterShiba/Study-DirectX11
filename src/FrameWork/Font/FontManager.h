
#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		FontManager.h
	@brief		フォントマネージャー
	@author		shiba
	@date		2013/1/18 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "FrameWork/Rendering/Sprite.h"

#include "FontDataType.h"
#include "FrameWork/Utility/UtilSafeArray.h"

class CAssetFontTexture;
class CSprite;
class CFontFile;

/*==========================================================================*/
/*!
	@class	CFontSprite
	@brief	フォントスプライト
*/
/*==========================================================================*/
class CFontSprite : public CSprite
{
public:
	// @brief コンストラクタ
	CFontSprite();

	// @brief デストラクタ
	virtual ~CFontSprite();

	// @brief 予約済みスプライト
	bool m_bReserved;
};

/*==========================================================================*/
/*!
	@class	CFontManager
	@brief	フォントマネージャー
	@note	あくまでデバッグ用。UIツールの導入をしたら多分いらなくなる
*/
/*==========================================================================*/
class CFontManager
{
	//シングルトン
	NS_SINGLETON( CFontManager );
	CFontManager();
	~CFontManager();

public:

	//@brief 初期化
	bool Init();

	//@brief 更新
	bool Update();

	//@brief フォント描画
	void DrawText2D( const TCHAR* str, const NSVector2& vPos, const E_FONTDATA_FILE eFontDataType, const f32 scale = 1.0f );


private:
	//@brief 空きスプライト取り出し
	const s32 GetIndexFreeSprite();

#if defined( _DEBUG )
	//@brief 使用可能フォント列挙
	void EnumFontFamilies();
#endif

private:
	CSafeArray< CAssetFontTexture*, ALLFONT_TEXTURESIZE > m_pcaFontTexture;	//!< フォントテクスチャ
	CSafeArray< CFontSprite*, DRAWFONTMAX_SIZE > m_pcaSprite;				//!< スプライト
	CSafeArray< CFontFile*, eFONTDATA_FILE_MAX > m_caFontData;				//!< フォントデータ

	CRenderingManager::E_RENDER_STAGE m_eRenderStage;

};

//EOF