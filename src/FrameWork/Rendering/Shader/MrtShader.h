#pragma once
/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		MrtShader.h
	@brief		Defferd用のデータ書き込みシェーダ
	@author		shiba
	@date		2013/5/25 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "ShaderBase.h"

class CMrtShader : public CShaderBase
{

public:
	CMrtShader();
	~CMrtShader();

public:
	//@brief シェーダロード
	virtual HRESULT LoadShader();
	//@brief シェーダを有効化
	virtual bool ShaderOn();

	//@brief 各種値設定
	void setWorld( const NSMatrix& World );
	void SetView( const NSMatrix& ViewMat );
	void SetProjection( const NSMatrix& ProjMat );
	void SetTexProj(const NSMatrix& TexProj);

	//@brief スキン用合成行列設定
	void SetCombMatrix(NSMatrix* pCombMat, const s32 nMatSize);

	//@brief インスタンス描画用合成行列設定
	void SetInstanceDrawMatrix(NSMatrix* pMatrix, const s32 nMatrixSize);

	//@brief 処理タイプの設定
	void SetVertShaderType(const s32 nVertShaderType );

	void SetLightPos(const NSVector3& vPos);

	ID3D11Buffer* GetInstanceMatBuffer()
	{
		return m_paBuffer[eINSTANCE_MATRIX];
	}

protected:
	void LoadVertexShader();

protected:

	//マトリクスデータ
	struct cbMatrix 
	{
		NSMatrix World;			//ワールド
		NSMatrix WorldSkin[32];	//ワールド変換行列(スキン用)
		NSMatrix View;			//ビュー変換
		NSMatrix Projection;	//透視変換
		NSMatrix TexProj;		//テクスチャ変換行列
	};

	//パラメータ
	struct cbParam
	{
		int m_nShaderType;		//!< 頂点シェーダでの処理タイプ
		NSVector3 m_vLightPos;	//!< ライト座標
	};

	enum E_BUFFER_PARAM
	{
		eMATRIX,
		ePARAM,
		eINSTANCE_MATRIX,
		eMAX
	};
	ID3D11Buffer* m_paBuffer[(s32)E_BUFFER_PARAM::eMAX];

	//!< データ
	cbMatrix m_cbMatrix;
	cbParam m_cParam;

	ID3D11SamplerState* m_pCompareSmp;

private:
	void writeBuffer(const E_BUFFER_PARAM eBufferParamID);


};

//EOF