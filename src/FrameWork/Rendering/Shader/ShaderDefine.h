#pragma once

class CShaderParam
{
public:
	CShaderParam() 
	: m_unUseTexNum( 0 )
	{}
	~CShaderParam()
	{}

	u32 m_unUseTexNum;			//!< シェーダにセットするテクスチャ数
};

static const s32 INSTANCE_MODEL_MAX = 128;	//インスタンス最大数

//EOF