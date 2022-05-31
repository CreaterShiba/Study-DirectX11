/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		ShaderBinder.cpp
	@brief		シェーダ変数設定
	@author		shiba
	@date		2013/4/13 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#include "FrameWork/DXUtil/DXMng.h"
#include "FrameWork/Camera/CameraBase.h"
#include "FrameWork/Camera/CameraManager.h"

#include "FrameWork/Rendering/Shader/ShaderBinder.h"
#include "FrameWork/Rendering/Model.h"
#include "FrameWork/Rendering/Light.h"

#include "FrameWork/Rendering/Shader/ZRenderShader.h"
#include "FrameWork/Rendering/Shader/SkinTransShader.h"
#include "FrameWork/Rendering/Shader/SpriteShader.h"
#include "FrameWork/Rendering/Shader/MrtShader.h"
#include "FrameWork/Rendering/Shader/DefferdShader.h"
#include "FrameWork/Rendering/Shader/PostEffect.h"
#include "FrameWork/Rendering/Shader/GetBright.h"
#include "FrameWork/Rendering/Shader/BlurShader.h"

#include "FrameWork/Asset/AssetManager.h"
#include "FrameWork/Asset/AssetMesh.h"

using namespace DirectX;

//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CShaderBinder::CShaderBinder()
{

}

//---------------------------------------------------------------------------
/*!
	@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CShaderBinder::~CShaderBinder()
{

}

//---------------------------------------------------------------------------
/*!
	@brief	シェーダ毎の変数を設定
	@todo	スッキリさせたい
*/
//---------------------------------------------------------------------------
void CShaderBinder::BindShaderParam( CRender* pcRender, CShaderBase* pcShader, CAssetMesh* pcMesh)
{
	if (!pcRender || !pcShader)
	{
		return;
	}

	CCameraManager* pcCamM = CCameraManager::GetInstance();
	NS_ASSERT(pcCamM);

	CCameraBase* pcActiveCamera = pcCamM->GetActiveCamera();
	NS_ASSERT(pcActiveCamera);

	CRenderingManager* pcRenderM = CRenderingManager::GetInstance();
	NS_ASSERT(pcRenderM);

	NSMatrix& wMat = pcRender->GetWorldMatrix();
	CShaderManager::E_SHADERID eShaderID = pcShader->GetShaderID();
	
	if (eShaderID == CShaderManager::eSHADERID_MRT)
	{
		CMrtShader* pcMRT = NS_STATIC_CAST(CMrtShader*, pcShader);

		pcMRT->SetView(pcActiveCamera->GetView());
		pcMRT->SetProjection(pcActiveCamera->GetProj());

		const CLight* pcLight = pcRenderM->GetDirectionalLight();
		if (pcLight != nullptr)
		{
			//NSMatrix lightmat = pcActiveCamera->GetView();
			NSMatrix lightmat = pcLight->GetLightView();
			f32 aspect = WINDOW_WIDTH / WINDOW_HEIGHT;
			NSMatrix proj = XMMatrixPerspectiveFovLH(UtilMath::DegreeToRadian(45.0f), aspect, 70.0f, 300.0f);	//ある程度でかくしないとZうまく書き込めない
			NSMatrix vp = lightmat*proj;
			pcMRT->SetTexProj( vp * UtilMath::GetTextureTransFormMatrix());	//射影空間→テクスチャ座標系への変換
		}

		pcMRT->SetVertShaderType(0);
		pcMRT->setWorld(wMat);

		CModel* pcModel = static_cast<CModel*>(pcRender);
		if (pcMesh->IsSkinMesh())
		{
			pcMRT->SetVertShaderType(1);
			pcMRT->SetCombMatrix(pcModel->GetCombMatrix(), pcModel->GetJointNum());
		}
		else if (pcModel->IsInstancingModel())
		{
			pcMRT->SetVertShaderType(2);
			pcMRT->SetInstanceDrawMatrix(pcModel->GetInstancingMatrix(), pcModel->GetInstancingNum());
		}
	}
	else if (eShaderID == CShaderManager::eSHADERID_ZRENDER)
	{
		CZRenderShader* pcZRenderShader = NS_STATIC_CAST(CZRenderShader*, pcShader);

		const CLight* pcLight = pcRenderM->GetDirectionalLight();
		if (pcLight != nullptr)
		{
			//pcZRenderShader->SetView(pcActiveCamera->GetView());	//ライトビュー行列	
			pcZRenderShader->SetView(pcLight->GetLightView());	//ライトビュー行列			
		}
		f32 aspect = WINDOW_WIDTH / WINDOW_HEIGHT;
		NSMatrix proj = XMMatrixPerspectiveFovLH(UtilMath::DegreeToRadian(45.0f), aspect, 70.0f, 300.0f);	//ある程度でかくしないとZうまく書き込めない
		pcZRenderShader->SetProjection(proj);

		CModel* pcModel = static_cast<CModel*>(pcRender);

		pcZRenderShader->SetVertShaderType(0);
		pcZRenderShader->setWorld(wMat);

		if (pcMesh->IsSkinMesh())
		{
			pcZRenderShader->SetVertShaderType(1);
			pcZRenderShader->SetCombMatrix(pcModel->GetCombMatrix(), pcModel->GetJointNum());
		}
		else if (pcModel->IsInstancingModel())
		{
			pcZRenderShader->SetVertShaderType(2);
			pcZRenderShader->SetInstanceDrawMatrix(pcModel->GetInstancingMatrix(), pcModel->GetInstancingNum());
		}
	}
	else if( eShaderID == CShaderManager::eSHADERID_SPRITE )
	{
		NSMatrix view = XMMatrixIdentity();
		view._41 = -WINDOW_WIDTH/2;
		view._42 = -WINDOW_HEIGHT/2;
		NSMatrix proj = UtilMath::GetOrthoMatrix();

		CSpriteShader* pcSpriteShader = NS_STATIC_CAST( CSpriteShader*, pcShader );

		pcSpriteShader->SetView( view );
		pcSpriteShader->SetProjection( proj );
		pcSpriteShader->SetLocal( wMat );
	}
	else if (eShaderID == CShaderManager::eSHADERID_DEFFERD)
	{
		CDefferdShader* pcDefferd = NS_STATIC_CAST(CDefferdShader*, pcShader);

		//アクティブカメラを指定
		NSVector3 vCameraPos = pcActiveCamera->GetCameraPos();
		NSVector4 camPos = XMVectorSet(vCameraPos.x, vCameraPos.y, vCameraPos.z, 0.0f);
		pcDefferd->setCamPos(camPos);

		for (s32 i = 0; i < (int)CRenderingManager::eLIGHT_MAX; ++i)
		{
			const CLight* pcLight = pcRenderM->GetLightData(i);
			if (pcLight && pcLight->GetLightType() == CLight::ePOINT)
			{
				//とりあえずポイントライトのみ
				pcDefferd->setLightPos(i, pcLight->GetPos());
				pcDefferd->setLightCol(i, pcLight->GetColor());
				pcDefferd->SetLightPow(i, pcLight->GetIntensity());
			}
		}
	}
	else if (
		eShaderID == CShaderManager::eSHADERID_POSTEFFECT || 
		eShaderID == CShaderManager::eSHADERID_BLUR || 
		eShaderID == CShaderManager::eSHADERID_GETBRIGHT)
	{
		//正射影で
		NSMatrix view = XMMatrixIdentity();
		view._41 = -WINDOW_WIDTH / 2;
		view._42 = -WINDOW_HEIGHT / 2;
		NSMatrix proj = UtilMath::GetOrthoMatrix();

		if (eShaderID == CShaderManager::eSHADERID_POSTEFFECT)
		{
			CPostEffectShader* pcPEShader = NS_STATIC_CAST(CPostEffectShader*, pcShader);
			pcPEShader->SetView(view);
			pcPEShader->SetProjection(proj);
			pcPEShader->SetLocal(wMat);
		}
		if (eShaderID == CShaderManager::eSHADERID_BLUR)
		{
			CBlurShader* pcPEShader = NS_STATIC_CAST(CBlurShader*, pcShader);
			pcPEShader->SetView(view);
			pcPEShader->SetProjection(proj);
			pcPEShader->SetLocal(wMat);
		}
		if (eShaderID == CShaderManager::eSHADERID_GETBRIGHT)
		{
			CGetBrightShader* pcPEShader = NS_STATIC_CAST(CGetBrightShader*, pcShader);
			pcPEShader->SetView(view);
			pcPEShader->SetProjection(proj);
			pcPEShader->SetLocal(wMat);
		}

	}
}

//EOF