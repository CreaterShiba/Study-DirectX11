#include "UnitPlayer.h"

#include "FrameWork/Utility/UtilInput.h"
#include "FrameWork/Camera/CameraManager.h"

#include "FrameWork/Rendering/Light.h"
#include "FrameWork/Rendering/Model.h"

#include "FrameWork/Collision/CollisionManagerSys.h"

#include "GameSystem/Camera/GameCamera.h"

CUnitPlayer::CUnitPlayer()
	: m_ePlayerState(E_PLAYER_STATE::ePLAYER_STATE_STAY)
	, m_fJumpF(0.0f)
	, m_fGoalRoateCount(0.0f)
{

}

CUnitPlayer::~CUnitPlayer()
{
	Term();
}

bool CUnitPlayer::Init()
{
	bool bSuccess = false;

	CLASS_NEW(m_pcUnitModel, CModel);

#if true
	//プレイヤーモデルデータ読み込み
	if (m_pcUnitModel)
	{
		if (m_pcUnitModel->LoadModel(_T("resource/Graphics/Cha_Slime/Models/Cha_Slime.fbx"), _T("resource/data/Model_Slime.csv")))
		{
			m_pcUnitModel->SetScale(NSVector3(0.1f, 0.1f, 0.1f));
			m_pcUnitModel->SetRotate(NSVector3(0.0f, 0.0f, 0.0f));

			m_pcUnitModel->SetDrawEnable(true, CRenderingManager::E_RENDER_STAGE_MRT_POS_NOM_DIF);
			m_pcUnitModel->SetDrawEnable(true, CRenderingManager::E_RENDER_STAGE_WRITEZ);

			bSuccess = true;
		}
	}
#endif

	CLASS_NEW(m_pCollision, CCollisionSphere());
	if (m_pCollision != nullptr)
	{
		m_pCollision->SetTrailCoord(m_pcUnitModel);
		static_cast<CCollisionSphere*>(m_pCollision)->SetScale(m_pcUnitModel->GetScale().x * 10.0f);
	}

	m_pcUnitModel->SetPos(GlobalParam::vPlayerStartPos);
	ChangeState(E_PLAYER_STATE::ePLAYER_STATE_WAIT);

	return bSuccess;
}

void CUnitPlayer::Update(const f32 fDelta)
{
	NS_UNUSED(fDelta);
	if (m_pcUnitModel == nullptr)
		return;

	m_bMoving = false;
	m_vBeforeMovingPos = m_pcUnitModel->GetPos();

	UpdateState();
}

void CUnitPlayer::UpdateState()
{
	switch (m_ePlayerState)
	{
		case CUnitPlayer::ePLAYER_STATE_WAIT:
			break;

		case CUnitPlayer::ePLAYER_STATE_STAY:
		{
			NSVector2 vec = GetMovingPos();
			if (UtilMath::Abs(vec.x) > 0.0f || UtilMath::Abs(vec.y) > 0.0f)
			{
				ChangeState(E_PLAYER_STATE::ePLAYER_STATE_WALK);
			}
			break;
		}

		case CUnitPlayer::ePLAYER_STATE_WALK:
		{
			NSVector2 vAddPos2 = GetMovingPos();
			NSVector3 vAddPos = NSVector3(vAddPos2.x, 0.0f, vAddPos2.y);

			if (UtilMath::Abs(vAddPos.x) > 0.0f || UtilMath::Abs(vAddPos.z) > 0.0f)
			{
				//カメラの進行方向に補正
				CCameraManager* pcCameraM = CCameraManager::GetInstance();
				if (pcCameraM != nullptr)
				{
					CCameraBase* pcCamera = pcCameraM->GetCameraFromID(CCameraManager::eCAMERAID_MAIN);
					if (pcCamera != nullptr)
					{
						NSVector3 vCameraDirX, vCameraDirZ;
						if (pcCamera->GetCameraXDir(vCameraDirX) && pcCamera->GetCameraZDir(vCameraDirZ))
						{
							vCameraDirX.y = 0.0f;
							vCameraDirZ.y = 0.0f;

							NSVector3 vAddDir = vAddPos;
							vAddPos.x = vCameraDirZ.x * vAddDir.z + vCameraDirX.x * vAddDir.x;
							vAddPos.z = vCameraDirZ.z * vAddDir.z + vCameraDirX.z * vAddDir.x;
						}
					}
				}

				m_pcUnitModel->AddPos(vAddPos);

				f32 fDegree = UtilMath::ATan2(vAddPos.x, vAddPos.z) * UtilMath::RadToDeg;
				m_pcUnitModel->SetRotate(0.0f, fDegree, 0.0f);
				m_bMoving = true;
			}
			else
			{
				ChangeState(E_PLAYER_STATE::ePLAYER_STATE_STAY);
			}
		}
		break;

		case CUnitPlayer::ePLAYER_STATE_DEAD:
			//ゲーム終了。
			break;

		case CUnitPlayer::ePLAYER_STATE_GOAL:
		{
			//適当にぴょんぴょん
			const NSVector3& pos = m_pcUnitModel->GetPos();
			if (pos.y < GlobalParam::vPlayerStartPos.y)
			{
				//ジャンプ
				m_fJumpF = 1.0f;
				m_pcUnitModel->SetPos(pos.x, GlobalParam::vPlayerStartPos.y, pos.z);
			}
			else
			{
				m_fJumpF -= 0.03f;
				m_pcUnitModel->SetPos(pos.x, pos.y + m_fJumpF, pos.z);
			}
			//適当にグルグル
			m_pcUnitModel->AddRotateY(3.0f);
			m_bMoving = true;
			break;
		}

		default:
			break;

	}

	if (m_bMoving)
	{
		TrailLightPos();
	}
}

void CUnitPlayer::Term()
{
}

void CUnitPlayer::Destroy()
{
	Term();
}

void CUnitPlayer::InvokeMove()
{
	ChangeState(ePLAYER_STATE_STAY);
}

void CUnitPlayer::OnDead()
{
	ChangeState(ePLAYER_STATE_DEAD);
}

void CUnitPlayer::OnGoal()
{
	ChangeState(ePLAYER_STATE_GOAL);
}

void CUnitPlayer::OnState()
{
	switch (m_ePlayerState)
	{
	case CUnitPlayer::ePLAYER_STATE_WAIT:
		m_pcUnitModel->PlayMotionForward(eMOTION_ID_STAY, true, false, 1.0f);
		break;

	case CUnitPlayer::ePLAYER_STATE_STAY:
		break;
	case CUnitPlayer::ePLAYER_STATE_WALK:
		m_pcUnitModel->PlayMotionForward(eMOTION_ID_WALK, true, false, 1.0f);
		break;
	case CUnitPlayer::ePLAYER_STATE_DEAD:
		m_pcUnitModel->PlayMotionForward(eMOTION_ID_DEAD, false, true, 0.5f);
		break;
	case CUnitPlayer::ePLAYER_STATE_GOAL:
	{
		m_pcUnitModel->PlayMotionForward(eMOTION_ID_STAY, true, false, 1.0f);

		//カメラ追尾解除
		CCameraManager* pcCamM = CCameraManager::GetInstance();
		NS_ASSERT(pcCamM);

		CGameCamera* pcCamera = NS_STATIC_CAST(CGameCamera*,pcCamM->GetCameraFromID(CCameraManager::eCAMERAID_MAIN));
		if (pcCamera != nullptr)
		{
			pcCamera->SetTarget(nullptr);
		}
		break;
	}
	default:
		break;
	}

}

void CUnitPlayer::OffState()
{
	switch (m_ePlayerState)
	{
	case CUnitPlayer::ePLAYER_STATE_STAY:
		m_pcUnitModel->StopPlayingMotion();
		break;
	case CUnitPlayer::ePLAYER_STATE_WALK:
		m_pcUnitModel->StopPlayingMotion();
		break;
	case CUnitPlayer::ePLAYER_STATE_DEAD:
		break;
	default:
		break;
	}
}

void CUnitPlayer::ChangeState(const E_PLAYER_STATE eState)
{
	OffState();
	m_ePlayerState = eState;
	OnState();
}

NSVector2 CUnitPlayer::GetMovingPos() const
{
	NSVector2 vMoving;

	if (CUtilInput::IsPush(eKEYBOARD_A))
	{
		vMoving.x = -1;
	}

	if (CUtilInput::IsPush(eKEYBOARD_D))
	{
		vMoving.x = 1;
	}

	if (CUtilInput::IsPush(eKEYBOARD_W))
	{
		vMoving.y = 1;
	}

	if (CUtilInput::IsPush(eKEYBOARD_S))
	{
		vMoving.y = -1;
	}

	return vMoving;
}

//EOF
