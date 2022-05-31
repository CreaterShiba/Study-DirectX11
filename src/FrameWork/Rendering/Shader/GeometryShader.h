#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		GeometryShader.h
	@brief		ジオメトリシェーダテスト
	@author		ta.Ishii
	@date		2013/1/22 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#include "FrameWork/Rendering/Shader/ShaderBase.h"

/*==========================================================================*/
/*!
	@class	CGeometryShader
	@brief	ジオメトリシェーダテスト
*/
/*==========================================================================*/
class CGeometryShader : public CShaderBase
{
public:
	CGeometryShader();
	~CGeometryShader();

public:
	//@brief シェーダロード
	virtual HRESULT LoadShader();
	//@brief シェーダを有効化
	virtual bool ShaderOn();

	void SetWVP( NSMatrix& WVPMat );
	void SetVecLight( NSVector4& vecLight );

private:

	struct cbMatrixBuffer
	{
		NSMatrix MatWVP;
	};

	struct cbLightVec
	{
		NSVector4 vecLight;
	};

	const static s32 CONBUFFERNUM = 2;	//バッファ数
	ID3D11Buffer* mpMatrixBuffer;
	ID3D11Buffer* mpLightVecBuffer;
};

//EOF
