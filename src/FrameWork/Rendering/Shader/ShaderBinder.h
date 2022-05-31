#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		ShaderBinder.h
	@brief		シェーダ変数設定
	@author		shiba
	@date		2013/4/13 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "ShaderManager.h"

/*==========================================================================*/
/*!
	@class	CShaderBinder
	@brief	シェーダ変数設定クラス
*/
/*==========================================================================*/
class CShaderBinder
{
public:
	CShaderBinder();
	~CShaderBinder();

	//@brief シェーダ変数設定
	static void BindShaderParam( class CRender* pcRender, class CShaderBase* pcShader, class CAssetMesh* pcMesh);

};

//EOF