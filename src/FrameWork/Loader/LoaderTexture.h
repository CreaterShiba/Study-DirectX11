
#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
@file		LoaderTexture.h
@brief		テクスチャローダー
@author		shiba
@date		2012/7/24 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#include "Loader.h"
#include "FrameWork/DirectXTex/DirectXTex.h"

#pragma comment ( lib, "DirectXTex.lib" )

//==========================================================================*/
/*!
@class	LoaderTexture
@brief	テクスチャ読み込み
*/
//==========================================================================*/
class CLoaderTexture : public CLoader
{
public:
	bool  LoadStart(const TCHAR* strFileName, bool bTGA);

	//データ
	DirectX::TexMetadata m_metadata;	//!< 設定データ
	DirectX::ScratchImage m_image;		//!< テクスチャイメージ
};

//EOF
