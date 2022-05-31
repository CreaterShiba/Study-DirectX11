#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		ComputeShader.h
	@brief		Computeシェーダテスト
	@author		ta.Ishii
	@date		2012/2/29 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#include "ShaderBase.h"
#include "FrameWork/Utility/UtilMacro.h"

/*==========================================================================*/
/*!
	@class	CComputeShader
	@brief	
*/
/*==========================================================================*/
class CComputeShader : public CShaderBase
{
public:
	CComputeShader();
	~CComputeShader();

	struct sComupteStr
	{
		f32 fR;
		f32 fG;
		f32 fB;
		f32 fAlpha;
	};

public:
	virtual bool Init();
	virtual HRESULT LoadShader();
	virtual bool ShaderOn();

private:
	ID3D11Buffer*	m_pVertexBuffer;

};

//EOF
