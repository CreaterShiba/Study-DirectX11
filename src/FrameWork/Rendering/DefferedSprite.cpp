/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		DefferedSprite.cpp
	@brief		デファード用スプライト
	@author		shiba
	@date		2012/3/3 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#include "DefferedSprite.h"

#include "FrameWork/Asset/AssetTexture.h"
#include "FrameWork/Rendering/RenderingTexture.h"
#include "FrameWork/Rendering/RenderingStage.h"
#include "FrameWork/DXUtil/DXMng.h"

void CDefferedSprite::Draw()
{

#if true
	CRenderingManager* pcRenderM = CRenderingManager::GetInstance();
	DXMng* pcDirectX = DXMng::GetInstance();
	if (pcRenderM != nullptr)
	{
		//--------------------------------------------------------------------
		// Defferd
		//--------------------------------------------------------------------
		const CRenderingStage& cMrtStage = pcRenderM->GetRenderStage(CRenderingManager::E_RENDER_STAGE_MRT_POS_NOM_DIF);
#if true
		//んー一気に設定しないとだめみたいだ。
		std::vector< ID3D11ShaderResourceView* > pTbl;
		pTbl.clear();
		pTbl.reserve(cMrtStage.m_paRenderingTarget.size());
		for (u32 defferdMRT = 0; defferdMRT < cMrtStage.m_paRenderingTarget.size(); defferdMRT++)
		{
			pTbl.push_back(cMrtStage.m_paRenderingTarget[defferdMRT]->GetTextureRevi());
		}
		pcDirectX->GetConDevice().PSSetShaderResources(0, pTbl.size(), &pTbl[0]);
#else
		for (s32 defferdMRT = 0; defferdMRT < cMrtStage.m_paRenderingTarget.size(); defferdMRT++)
		{
			ID3D11ShaderResourceView* pcResource = cMrtStage.m_paRenderingTarget[defferdMRT]->GetTextureRevi();
			pcDirectX->GetConDevice().PSSetShaderResources(i, 1, &pcResource);
		}
#endif

		//シャドウマップ設定
	}
#endif

	CSprite::Draw();
}

//EOF
