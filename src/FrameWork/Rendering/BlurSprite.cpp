/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		BlurSprite.cpp
	@brief		ブラー用スプライト
	@author		shiba
	@date		2012/3/3 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/

#include "BlurSprite.h"

#include "FrameWork/Rendering/Shader/ShaderManager.h"
#include "FrameWork/Rendering/Shader/BlurShader.h"

void CBlurSprite::Draw()
{
	CShaderManager* pcShaderM = CShaderManager::GetInstance();
	NS_ASSERT(pcShaderM);

	CShaderBase* pcShader = pcShaderM->GetShader(CShaderManager::eSHADERID_BLUR);
	if (pcShader != nullptr)
	{
		CBlurShader* pBlurShader = NS_STATIC_CAST(CBlurShader*, pcShader);

		//タテヨコで２回描画を行う
		pBlurShader->SetBlurType(0);
		CSprite::Draw();

		pBlurShader->SetBlurType(1);
		CSprite::Draw();
	}
}

//EOF
