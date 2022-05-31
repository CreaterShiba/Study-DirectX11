
#pragma once
/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		FontDataType.h
	@brief		フォント用データ定義
	@author		shiba
	@date		2012/9/14 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/

//@brief フォントファイル
enum E_FONTDATA_FILE
{
	eFONTDATA_FILE_Ms,		//Msゴシック
	//eFONTDATA_FILE_Azu,	//あずもじ
	eFONTDATA_FILE_MAX,
};

//@brief フォント定義
enum
{
	FONT_TEXTURE_SIZE = 2048,																	//!< フォントテクスチャサイズ(縦横一定)
	FONT_SIZE = 128,																			//!< フォントサイズ
	FONTTEXTURE_EACH_FONTDATA = 1,																//!< フォントデータ1つあたりのフォントテクスチャ数
	ALLFONT_TEXTURESIZE = eFONTDATA_FILE_MAX*FONTTEXTURE_EACH_FONTDATA,							//!< フォントテクスチャ数
	FONTTEXTURE_INFONTNUM = (FONT_TEXTURE_SIZE/FONT_SIZE)*(FONT_TEXTURE_SIZE/FONT_SIZE),		//!< 1テクスチャに文字がどれくらい入るか
	DRAWFONTMAX_SIZE = 256,																		//!< フォントの描画最大数
};

//@brief 一つあたりのフォントデータ
class CFontOneData
{
public:
	CFontOneData() : 
	  ucCode( 0 )
	, unTexID( 0 )
	, scalex( 0 )
	, scaley( 0 )
	, u( 0.0f )
	, u2( 0.0f )
	, v( 0.0f )
	, v2( 0.0f )
	{}
	~CFontOneData()
	{}

	u16 ucCode;			//!< 文字コード(UTF-8)
	u32 unTexID;		//!< テクスチャID
	s32 scalex, scaley;	//!< スケール値
	f32 u,u2;			//!< U
	f32 v,v2;			//!< V
};

// EOF
