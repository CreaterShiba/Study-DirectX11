#include "SceneTitle.h"
#include "FrameWork/Rendering/Sprite.h"
#include "FrameWork/Asset/AssetManager.h"
#include "FrameWork/Utility/UtilInput.h"
#include "FrameWork/Scene/SceneManager.h"
#include "FrameWork/Utility/UtilSound.h"

CSceneTitle::CSceneTitle() :
	m_TestSprite(nullptr)
{

}

CSceneTitle::~CSceneTitle()
{
	Term();
}

void CSceneTitle::Term()
{
	SAFE_DELETE_TASK(m_TestSprite);
}

bool CSceneTitle::Init()
{
	CAssetManager* pcAssetM = CAssetManager::GetInstance();

	CLASS_NEW(m_TestSprite, CSprite());
	if (m_TestSprite)
	{
		m_TestSprite->SetScale(NSVector3(480.0f, 320.0f, 0.0f));
		m_TestSprite->SetPos(NSVector3(WINDOW_WIDTH*0.5f, WINDOW_HEIGHT*0.5f, 0.0f));
		if (pcAssetM != nullptr)
		{
			CAssetTexture* pcTex = pcAssetM->LoadTexture(_T("resource/Graphics/test.png"));
			m_TestSprite->SetTexture(pcTex);

			CRenderingContext cContext;
			cContext.SetShader(CShaderManager::eSHADERID_SPRITE);
			m_TestSprite->SetRenderContext(cContext);
		}
		m_TestSprite->SetDrawEnable(true, CRenderingManager::E_RENDER_STAGE_SPRITE);
	}

	return true;
}

void CSceneTitle::Update(const f32 fDelta)
{
	NS_UNUSED(fDelta);

	//入力があったらメインシーンに移行させてみる
	if (CUtilInput::IsPush(eKEYBOARD_SPACE, eINPUT_DEVICE_TYPE_KEYBOARD))
	{
		CSceneManager* pcSM = CSceneManager::GetInstance();
		if (pcSM != nullptr)
		{
			pcSM->LoadScene(CSceneManager::eMAIN);
		}
	}
}

void CSceneTitle::Destroy()
{
	Term();
}

//EOF