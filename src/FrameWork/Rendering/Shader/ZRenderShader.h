#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		ZRenderShader.h
	@brief		深度レンダリング
	@author		shiba
	@date		2012/10/7 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "MrtShader.h"

class CZRenderShader : public CMrtShader
{
public:
	CZRenderShader();
	~CZRenderShader();

	//@brief シェーダロード
	virtual HRESULT LoadShader();
};

//EOF
