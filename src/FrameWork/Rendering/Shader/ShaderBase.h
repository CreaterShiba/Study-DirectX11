#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		ShaderBase.h
	@brief		シェーダーベースクラス
	@author		shiba
	@date		2012/3/4 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include <D3D11Shader.h>

#include "FrameWork/DXUtil/UtilDX11.h"
#include "FrameWork/Rendering/Shader/ShaderManager.h"
#include "FrameWork/Rendering/Shader/ShaderDefine.h"

/*==========================================================================*/
/*!
	@class	CShaderBase
	@brief	シェーダベースクラス
*/
/*==========================================================================*/
class CShaderBase
{
public:
	CShaderBase();
	virtual ~CShaderBase();

	virtual bool Init();
	//シェーダをロードする
	virtual HRESULT LoadShader() = 0;
	virtual bool ShaderOn();
	virtual bool ShaderOff();

	// @brief シェーダーID獲得
	inline const CShaderManager::E_SHADERID GetShaderID() const;
	
	// @brief シェーダの設定情報セット
	inline void SetShaderParam( const CShaderParam& cParam );

	// @brief シェーダの設定情報取得
	inline const CShaderParam& GetShaderParam() const;

private:
	// @brief 破棄
	void Term();

protected:

#if defined(SHADER_PRE_COMPILE)

	//シェーダー生成
	bool CreateShader(
		const TCHAR* pFileName,
		CShaderManager::E_SHADERTYPE eShaderType,
		D3D11_INPUT_ELEMENT_DESC Layout[] = 0,
		UINT LayoutSize = 0);

#else

	//シェーダのコンパイル
	bool CompileShaderFromFile( 
		const TCHAR* pFileName, 
		const TCHAR* pEntryPoint,
		const TCHAR* pShaderModel,
		CShaderManager::E_SHADERTYPE ShaderType,
		D3D11_INPUT_ELEMENT_DESC Layout[] = 0,
		UINT LayoutSize = 0);

#endif

protected:
	//!< シェーダーID
	CShaderManager::E_SHADERID m_eShaderID;	

	//!< シェーダの設定情報
	CShaderParam m_cShaderParam;	

	//!< デバッグ用シェーダパラメータ
#if defined( _DEBUG )
	ID3D11ShaderReflection* m_DebugShaderParam;
#endif

private:
	//シェーダ
	ID3D11VertexShader*		m_pVertexShader;
	ID3D11PixelShader*		m_pPixelShader;
	ID3D11GeometryShader*	m_pGeometryShader;
	ID3D11HullShader*		m_pHullShader;
	ID3D11DomainShader*		m_pDomainShader;

	//頂点レイアウト渡す情報とシェーダが受け取る情報の整合性を保つ
	//複数持つべき？（スキンメッシュとか色々頂点情報が変わるもの用）
	ID3D11InputLayout* m_pInputLayout;
};

//---------------------------------------------------------------------------
/*!
	@brief	シェーダーID
*/
//---------------------------------------------------------------------------
const CShaderManager::E_SHADERID CShaderBase::GetShaderID() const
{
	return m_eShaderID;
}

//---------------------------------------------------------------------------
/*!
	@brief	シェーダ設定情報のセット
*/
//---------------------------------------------------------------------------
void CShaderBase::SetShaderParam( const CShaderParam& cParam )
{
	m_cShaderParam = cParam;
}

//---------------------------------------------------------------------------
/*!
	@brief	シェーダ設定情報獲得
*/
//---------------------------------------------------------------------------
const CShaderParam& CShaderBase::GetShaderParam() const
{
	return m_cShaderParam;
}

//EOF
