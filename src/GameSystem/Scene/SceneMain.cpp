#include "SceneMain.h"

#include "FrameWork/Rendering/Light.h"
#include "FrameWork/Rendering/RenderingManager.h"
#include "FrameWork/Rendering/Model.h"
#include "FrameWork/Rendering/Shader/PostEffect.h"

#include "FrameWork/Camera/CameraManager.h"
#include "FrameWork/Utility/UtilInput.h"

#include "FrameWork/Font/FontManager.h"
#include "FrameWork/Utility/UtilSound.h"

#include "GameSystem/Unit/UnitPlayer.h"
#include "GameSystem/Unit/UnitEnemy.h"
#include "GameSystem/Stage/StageManager.h"
#include "GameSystem/Camera/GameCamera.h"

#include "GameSystem/Stage/StageManager.h"

#include "GameSystem/Collision/CollisionManager.h"

using namespace DirectX;

CSceneMain::CSceneMain() :
	m_pcUnitPlayer(nullptr),
	m_pcUnitEnemy(nullptr),
	m_pcUnitEnemy2(nullptr),
	m_pcUnitEnemy3(nullptr),
	m_pcStageM(nullptr),
	nPostEffectType(0),
	m_eMainState(E_SCENE_STATE::eSCENE_STATE_BEGIN)
{

}

CSceneMain::~CSceneMain()
{
	Term();
}

void CSceneMain::Term()
{
	//コリジョン設定
	//@todo とりあえずここで追加する形→自動登録＆コールバック式に変更予定
	CCollisionManager* pcColM = CCollisionManager::GetInstance();
	if (pcColM != nullptr)
	{
		pcColM->SetUnitPlayer(nullptr);
		pcColM->SetStageManager(nullptr);
		pcColM->AddUnitEnemy(nullptr);
		pcColM->AddUnitEnemy(nullptr);
		pcColM->AddUnitEnemy(nullptr);
	}

	SAFE_DELETE(m_pcStageM);
	SAFE_DELETE_TASK(m_pcUnitEnemy3);
	SAFE_DELETE_TASK(m_pcUnitEnemy2);
	SAFE_DELETE_TASK(m_pcUnitEnemy);
	SAFE_DELETE_TASK(m_pcUnitPlayer);
}

bool CSceneMain::Init()
{
	//CAssetManager* pcAssetM = CAssetManager::GetInstance();

	//ステージ管理
	CLASS_NEW(m_pcStageM, CStageManager());

	//プレイヤーユニット生成
	CLASS_NEW(m_pcUnitPlayer, CUnitPlayer());
	m_pcUnitPlayer->SetLightID(1);

	CCameraManager* pcCameraM = CCameraManager::GetInstance();
	if (pcCameraM != nullptr)
	{
		//カメラ追従指定
		CGameCamera* pcGameCamera = static_cast<CGameCamera*>(pcCameraM->GetCameraFromID(CCameraManager::eCAMERAID_MAIN));
		pcGameCamera->SetTarget(m_pcUnitPlayer->GetModel());
	}

	//エネミーユニット生成
	CLASS_NEW(m_pcUnitEnemy, CUnitEnemy());
	m_pcUnitEnemy->SetupModel(CUnitEnemy::eNORMAL);
	m_pcUnitEnemy->GetModel()->SetPos(NSVector3(-45.0f, -5.0f, 45.0f));
	m_pcUnitEnemy->SetLightID(2);
	m_pcUnitEnemy->GetModel()->SetRotate(0.0f, 180.0f, 0.0f);
	m_pcUnitEnemy->AddTrailPos(NSVector2(-45.0f, 45.0f));
	m_pcUnitEnemy->AddTrailPos(NSVector2(-45.0f, -55.0f));
	m_pcUnitEnemy->SetTrailSpeed(0.5f);
	//m_pcUnitEnemy->InvokeMove();

	CLASS_NEW(m_pcUnitEnemy2, CUnitEnemy());
	m_pcUnitEnemy2->SetupModel(CUnitEnemy::eNORMAL);
	m_pcUnitEnemy2->GetModel()->SetPos(NSVector3(5.0f, -5.0f, 45.0f));
	m_pcUnitEnemy2->SetLightID(3);
	m_pcUnitEnemy2->AddTrailPos(NSVector2(5.0f, 45.0f));
	m_pcUnitEnemy2->AddTrailPos(NSVector2(-5.0f, 45.0f));
	m_pcUnitEnemy2->AddTrailPos(NSVector2(-5.0f, -55.0f));
	m_pcUnitEnemy2->AddTrailPos(NSVector2(35.0f, -55.0f));
	m_pcUnitEnemy2->SetTrailSpeed(0.8f);

	CLASS_NEW(m_pcUnitEnemy3, CUnitEnemy());
	m_pcUnitEnemy3->SetupModel(CUnitEnemy::eDANGER);
	m_pcUnitEnemy3->GetModel()->SetPos(NSVector3(25.0f, -5.0f, -18.0f));
	m_pcUnitEnemy3->SetLightID(4);
	m_pcUnitEnemy3->AddTrailPos(NSVector2(25.0f,-20.0f));
	m_pcUnitEnemy3->AddTrailPos(NSVector2(25.0f, 45.0f));
	m_pcUnitEnemy3->AddTrailPos(NSVector2(55.0f, 45.0f));
	m_pcUnitEnemy3->AddTrailPos(NSVector2(55.0f, -25.0f));
	m_pcUnitEnemy3->SetTrailSpeed(0.87f);
#if 1
	//ライトセットアップ
	CRenderingManager* pcRenderM = CRenderingManager::GetInstance();
	if (pcRenderM != nullptr)
	{
		CLight* pcLight = pcRenderM->GetLightData(0);
		if (pcLight)
		{
			//ディレクショナルです
			pcLight->SetColor(XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f));
			f32 rotateCounter = 170.0f;
			f32 fLength = 200.0f;
			pcLight->SetPos(
				UtilMath::Cos(UtilMath::DegToRad * rotateCounter)*fLength * 0.5f,
				fLength,
				UtilMath::Sin(UtilMath::DegToRad * rotateCounter)*fLength * 0.5f);
		}

		//プレイヤー
		pcLight = pcRenderM->GetLightData(1);
		if (pcLight)
		{
			pcLight->SetColor(XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f));
			pcLight->SetIntensity(1.0f);
		}

		//エネミー
		pcLight = pcRenderM->GetLightData(2);
		if (pcLight)
		{
			pcLight->SetColor(XMVectorSet(1.0f, 1.0f, 0.0f, 1.0f));
			pcLight->SetIntensity(1.0f);
		}
		pcLight = pcRenderM->GetLightData(3);
		if (pcLight)
		{
			pcLight->SetColor(XMVectorSet(1.0f, 1.0f, 0.0f, 1.0f));
			pcLight->SetIntensity(1.0f);
		}
		pcLight = pcRenderM->GetLightData(4);
		if (pcLight)
		{
			pcLight->SetColor(XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f));
			pcLight->SetIntensity(1.5f);
		}

		//スイッチ
		pcLight = pcRenderM->GetLightData(5);
		if (pcLight)
		{
			pcLight->SetColor(XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f));
			pcLight->SetIntensity(1.0f);
		}

		//ゴールユニット
		pcLight = pcRenderM->GetLightData(6);
		if (pcLight)
		{
			pcLight->SetColor(XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f));
			pcLight->SetIntensity(1.0f);
		}
	}
#endif

	//コリジョン設定
	//@todo とりあえずここで追加する形→自動登録＆コールバック式に変更予定
	CCollisionManager* pcColM = CCollisionManager::GetInstance();
	if (pcColM != nullptr)
	{
		pcColM->SetUnitPlayer(m_pcUnitPlayer);
		pcColM->SetStageManager(m_pcStageM);
		pcColM->AddUnitEnemy(m_pcUnitEnemy);
		pcColM->AddUnitEnemy(m_pcUnitEnemy2);
		pcColM->AddUnitEnemy(m_pcUnitEnemy3);
	}

	return true;
}

void CSceneMain::Update(const f32 fDelta)
{
	NS_UNUSED(fDelta);


	switch (m_eMainState)
	{
	case CSceneMain::eSCENE_STATE_BEGIN:
	{

		//説明画面
		if (CUtilInput::IsTrigger(eKEYBOARD_X, eINPUT_DEVICE_TYPE_KEYBOARD))
		{
			ChangeState(eSCENE_STATE_MAIN);
		}
		CFontManager* pcFontM = CFontManager::GetInstance();
		if (pcFontM != nullptr)
		{
			pcFontM->DrawText2D(_T("敵のスライムをよけて仲間がいる所を目指しましょう"), NSVector2(WINDOW_WIDTH*0.25f, WINDOW_HEIGHT*0.25f), eFONTDATA_FILE_Ms, 0.3f);
			pcFontM->DrawText2D(_T("W:上に進む"), NSVector2(WINDOW_WIDTH*0.25f, WINDOW_HEIGHT*0.25f + FONT_SIZE*0.5f), eFONTDATA_FILE_Ms, 0.3f);
			pcFontM->DrawText2D(_T("A:左に進む"), NSVector2(WINDOW_WIDTH*0.25f+FONT_SIZE*2*1, WINDOW_HEIGHT*0.25f + FONT_SIZE*0.5f), eFONTDATA_FILE_Ms, 0.3f);
			pcFontM->DrawText2D(_T("D:右に進む"), NSVector2(WINDOW_WIDTH*0.25f + FONT_SIZE * 2 * 2, WINDOW_HEIGHT*0.25f + FONT_SIZE*0.5f), eFONTDATA_FILE_Ms, 0.3f);
			pcFontM->DrawText2D(_T("S:下に進む"), NSVector2(WINDOW_WIDTH*0.25f + FONT_SIZE * 2 * 3, WINDOW_HEIGHT*0.25f + FONT_SIZE*0.5f), eFONTDATA_FILE_Ms, 0.3f);
			pcFontM->DrawText2D(_T("※Xキーを押すと開始します"), NSVector2(WINDOW_WIDTH*0.25f, WINDOW_HEIGHT*0.25f + FONT_SIZE*2*0.5f), eFONTDATA_FILE_Ms, 0.3f);
		}
		break;
	}
	case CSceneMain::eSCENE_STATE_MAIN:
		//ゲーム処理
		if (m_pcUnitPlayer->IsDead())
		{
			ChangeState(eSCENE_STATE_GAMEOVER);
		}
		if (m_pcUnitPlayer->IsGoal())
		{
			ChangeState(eSCENE_STATE_CLEAR);
		}

		break;
	case CSceneMain::eSCENE_STATE_GAMEOVER:
	{
		CFontManager* pcFontM = CFontManager::GetInstance();
		if (pcFontM != nullptr)
		{
			//ゲームオーバー
			pcFontM->DrawText2D(_T("GameOver..."), NSVector2(WINDOW_WIDTH*0.35f, WINDOW_HEIGHT*0.5f), eFONTDATA_FILE_Ms, 0.3f);
			break;
		}
		break;
	}
	case CSceneMain::eSCENE_STATE_CLEAR:
	{
		CFontManager* pcFontM = CFontManager::GetInstance();
		if (pcFontM != nullptr)
		{
			//クリア
			pcFontM->DrawText2D(_T("GameClear!"), NSVector2(WINDOW_WIDTH*0.35f, WINDOW_HEIGHT*0.5f), eFONTDATA_FILE_Ms, 0.3f);
		}
		break;
	}
	default:
		break;
	}

#if 0
	static f32 rotateCounter = 170.0f;
	//rotateCounter += 0.1f;
	//検証用に回転させてみる
	CRenderingManager* pcRenderM = CRenderingManager::GetInstance();
	if (pcRenderM != nullptr)
	{
		CLight* pcLight = pcRenderM->GetLightData(0);
		if (pcLight)
		{
			f32 fLength = 200.0f;
			pcLight->SetPos(
				UtilMath::Cos(UtilMath::DegToRad * rotateCounter)*fLength * 0.5f,
				fLength,
				UtilMath::Sin(UtilMath::DegToRad * rotateCounter)*fLength * 0.5f);
		}
	}
#endif

#if 1
	if (CUtilInput::IsTrigger(eKEYBOARD_F, eINPUT_DEVICE_TYPE_KEYBOARD))
	{
		//ポストエフェクトテスト
		nPostEffectType++;
		if (nPostEffectType >= (int)CPostEffectShader::eMAX)
		{
			nPostEffectType = 0;
		}

		CShaderManager* pcShaderM = CShaderManager::GetInstance();
		if (pcShaderM != nullptr)
		{
			CPostEffectShader* pcPostEffectShader = NS_STATIC_CAST(CPostEffectShader*, pcShaderM->GetShader(CShaderManager::eSHADERID_POSTEFFECT));
			if (pcPostEffectShader != nullptr)
			{
				pcPostEffectShader->SetPostEffectType(nPostEffectType);
			}
		}
	}
	if (CUtilInput::IsTrigger(eKEYBOARD_B, eINPUT_DEVICE_TYPE_KEYBOARD))
	{
		//ブルーム切り替え
		CShaderManager* pcShaderM = CShaderManager::GetInstance();
		if (pcShaderM != nullptr)
		{
			CPostEffectShader* pcPostEffectShader = NS_STATIC_CAST(CPostEffectShader*, pcShaderM->GetShader(CShaderManager::eSHADERID_POSTEFFECT));
			if (pcPostEffectShader != nullptr)
			{
				pcPostEffectShader->ChangeBloomEnable();
			}
		}
	}

#endif
}

void CSceneMain::Destroy()
{
	Term();
}

void CSceneMain::OnState()
{
	switch (m_eMainState)
	{
	case CSceneMain::eSCENE_STATE_BEGIN:
		break;
	case CSceneMain::eSCENE_STATE_MAIN:
		//コントロール解除
		if (m_pcUnitEnemy != nullptr)
		{
			m_pcUnitEnemy->InvokeMove();
		}
		if (m_pcUnitPlayer != nullptr)
		{
			m_pcUnitPlayer->InvokeMove();
		}
		CUtilSound::PlayBGM(E_SOUND_ID::eSOUNDID_BGM);

		break;
	case CSceneMain::eSCENE_STATE_GAMEOVER:
		CUtilSound::PlaySE(eSOUNDID_GAMEOVER);
		break;
	case CSceneMain::eSCENE_STATE_CLEAR:
		CUtilSound::PlaySE(eSOUNDID_CLEAR);
		break;
	case CSceneMain::eSCENE_STATE_MAX:
		break;
	default:
		break;
	}
}

void CSceneMain::OffState()
{
	switch (m_eMainState)
	{
	case CSceneMain::eSCENE_STATE_BEGIN:
		break;
	case CSceneMain::eSCENE_STATE_MAIN:
		break;
	case CSceneMain::eSCENE_STATE_GAMEOVER:
		break;
	case CSceneMain::eSCENE_STATE_CLEAR:
		break;
	case CSceneMain::eSCENE_STATE_MAX:
		break;
	default:
		break;
	}
}

void CSceneMain::ChangeState(const E_SCENE_STATE eState)
{
	OffState();
	m_eMainState = eState;
	OnState();
}
//EOF