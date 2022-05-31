/*==========================================================================*/
/*==========================================================================*/
/*!
@file		LoaderTexture.cpp
@brief		テクスチャファイルローダー
@author		shiba
@date		2012/7/24 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "LoaderTexture.h"
using namespace DirectX;

//----------------------------------------------------------------------------
/*	@brief	テクスチャファイルロード
*/
//----------------------------------------------------------------------------
bool CLoaderTexture::LoadStart(const TCHAR* strFileName , bool bTGA)
{
	m_eLoadSeq = CLoader::eLOADSEQ_INIT;

	//テクスチャ読み込み処理
	HRESULT hr = S_OK;
	if (bTGA)
	{
		hr = DirectX::LoadFromTGAFile(strFileName, &m_metadata, m_image);
	}
	else
	{
		hr = DirectX::LoadFromWICFile(strFileName, WIC_FLAGS::WIC_FLAGS_NONE, &m_metadata, m_image);
	}
	if (FAILED(hr))
	{
		//return hr;
		return false;
	}

	//@todo ひとまずロード済みとする
	m_eLoadSeq = CLoader::eLOADSEQ_LOADEND;

	return true;
}

//EOF
