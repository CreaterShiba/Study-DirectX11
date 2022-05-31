#include "SceneDebug.h"
#include "FrameWork/Asset/AssetManager.h"

#include "GameSystem/Unit/UnitPlayer.h"
#include "GameSystem/Unit/UnitEnemy.h"
#include "FrameWork/Camera/CameraManager.h"
#include "GameSystem/Camera/GameCamera.h"
#include "FrameWork/Rendering/Model.h"
#include "FrameWork/Utility/UtilInput.h"
#include "FrameWork/Utility/UtilSound.h"

CSceneDebug::CSceneDebug()
{
}

CSceneDebug::~CSceneDebug()
{
	Term();
}

void CSceneDebug::Term()
{
}

bool CSceneDebug::Init()
{
	return true;
}

void CSceneDebug::Update(const f32 fDelta)
{
	NS_UNUSED(fDelta);

	if (CUtilInput::IsTrigger(eKEYBOARD_SPACE, eINPUT_DEVICE_TYPE_KEYBOARD))
	{
		CUtilSound::PlaySE(E_SOUND_ID::eSOUNDID_SWITCH);
	}
}

void CSceneDebug::Destroy()
{
	Term();
}


//EOF
