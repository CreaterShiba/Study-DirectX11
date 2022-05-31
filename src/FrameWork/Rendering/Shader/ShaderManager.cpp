/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		ShaderManager.h
	@brief		シェーダーマネージャー
	@author		shiba
	@date		2012/3/4 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "ShaderManager.h"

#include "MrtShader.h"
#include "ZRenderShader.h"
#include "SpriteShader.h"
#include "DefferdShader.h"
#include "GetBright.h"
#include "BlurShader.h"
#include "PostEffect.h"

NS_SINGLETON_INSTANCE_DEFINE( CShaderManager );

//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CShaderManager::CShaderManager()
{
	m_aShader.ALLSet(nullptr);
}

//---------------------------------------------------------------------------
/*!
	@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CShaderManager::~CShaderManager()
{
	Term();
}

//---------------------------------------------------------------------------
/*!
	@brief	初期化
*/
//---------------------------------------------------------------------------
bool CShaderManager::Init()
{
	for( s32 i = 0; i < eSHADERID_END; i++)
	{
		AddShader( NS_STATIC_CAST( E_SHADERID, i ) );
	}

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	シェーダ獲得
*/
//---------------------------------------------------------------------------
CShaderBase* CShaderManager::GetShader( const E_SHADERID eShaderID )
{
	return m_aShader.Get( eShaderID );
}

//---------------------------------------------------------------------------
/*!
	@brief	シェーダ追加
*/
//---------------------------------------------------------------------------
bool CShaderManager::AddShader( const E_SHADERID eShaderID )
{
	if( m_aShader.Get( eShaderID ) )
	{
		return false;
	}

	CShaderBase* pcShader = nullptr;
	switch( eShaderID )
	{
	case eSHADERID_MRT:
		CLASS_NEW(pcShader, CMrtShader());
		break;
	case eSHADERID_ZRENDER:
		CLASS_NEW(pcShader, CZRenderShader());
		break;
	case eSHADERID_SPRITE:
		CLASS_NEW(pcShader, CSpriteShader());
		break;
	case eSHADERID_DEFFERD:
		CLASS_NEW(pcShader, CDefferdShader());
		break;
	case eSHADERID_GETBRIGHT:
		CLASS_NEW(pcShader, CGetBrightShader());
		break;
	case eSHADERID_BLUR:
		CLASS_NEW(pcShader, CBlurShader());
		break;
	case eSHADERID_POSTEFFECT:
		CLASS_NEW(pcShader, CPostEffectShader());
		break;
		default:
			break;
	}

	if( pcShader )
	{
		//シェーダーロード
		pcShader->LoadShader();
	}
	//シェーダ追加
	m_aShader.Set( eShaderID, pcShader );

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	シェーダ削除
*/
//---------------------------------------------------------------------------
bool CShaderManager::RemoveShader( const E_SHADERID eShaderID )
{
	SAFE_DELETE( m_aShader.Get( eShaderID ) );
	m_aShader.Set( eShaderID, nullptr);

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	破棄
*/
//---------------------------------------------------------------------------
void CShaderManager::Term()
{
	for( s32 i = 0; i < eSHADERID_END; i++)
	{
		SAFE_DELETE( m_aShader.Get( i ) );
		m_aShader.Set( i, nullptr);
	}
}

//EOF