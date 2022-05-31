#include "UnitEnemy.h"

#include "FrameWork/Rendering/Model.h"
#include "FrameWork/Asset/AssetManager.h"
#include "FrameWork/Asset/AssetTexture.h"
#include "FrameWork/Asset/AssetModel.h"

#include "GameSystem/Collision/CollisionManager.h"

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		UnitEnemy.cpp
	@brief		エネミーユニット操作
	@author		shiba
	@date		2015/07/14 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
CUnitEnemy::CUnitEnemy()
	: m_nTrailID(0)
	, m_nTrailBaseId(0)
	, m_fTrailCounter(0.0f)
	, m_fTrailSpeed(0.01f)
	, m_bTrailDir(false)
	, m_eEnemyType(E_ENEMY_TYPE::eNORMAL)
	, m_fLightCounter(0.0f)
{

}

CUnitEnemy::~CUnitEnemy()
{
	Term();
}

bool CUnitEnemy::Init()
{
	bool bSuccess = true;

	CLASS_NEW(m_pCollision, CCollisionSphere());
	static_cast<CCollisionSphere*>(m_pCollision)->SetScale(5.0f);

	return bSuccess;
}

void CUnitEnemy::SetupModel(const E_ENEMY_TYPE eEnemyType)
{
	if (m_pcUnitModel == nullptr)
	{
		m_eEnemyType = eEnemyType;

		CLASS_NEW(m_pcUnitModel, CModel);

		//エネミーモデルデータ読み込み
		if (m_pcUnitModel)
		{
			if (m_pcUnitModel->LoadModel(_T("resource/Graphics/Cha_Slime/Models/Cha_Slime.fbx"), _T("resource/data/Model_Slime.csv")))
			{
				m_pcUnitModel->SetScale(NSVector3(0.1f, 0.1f, 0.1f));
				m_pcUnitModel->SetRotate(NSVector3(0.0f, 90.0f, 0.0f));

				m_pcUnitModel->SetDrawEnable(true, CRenderingManager::E_RENDER_STAGE_WRITEZ);
				m_pcUnitModel->SetDrawEnable(true, CRenderingManager::E_RENDER_STAGE_MRT_POS_NOM_DIF);

				//敵用テクスチャセット
				CAssetManager* pcAssetM = CAssetManager::GetInstance();
				NS_ASSERT(pcAssetM);

				SInitTextureParam cParam;
				cParam.bTGA = true;
				if (eEnemyType == eDANGER)
				{
					wcscpy_s(cParam.strTexName, TEXTURE_FILE_PATH, _T("resource/Graphics/Cha_Slime/Textures/Cha_SlimRed.tga"));
				}
				else
				{
					wcscpy_s(cParam.strTexName, TEXTURE_FILE_PATH, _T("resource/Graphics/Cha_Slime/Textures/Cha_SlimYellow.tga"));
				}
				CAssetTexture* pcEnemyTex = pcAssetM->LoadTexture(cParam);
				if (pcEnemyTex != nullptr)
				{
					CAssetModel* pcAssetModel = m_pcUnitModel->GetModelData();
					NS_ASSERT(pcAssetModel);
					CMaterial* pcMat = pcAssetModel->GetMaterial(0);
					NS_ASSERT(pcMat);
					pcMat->SetDiffuseTextureID(pcEnemyTex->GetAssetUID());
				}
			}
		}

		m_pCollision->SetTrailCoord(m_pcUnitModel);

		ChangeState(E_ENEMY_STATE::eENEMY_STATE_STAY);
	}
}

void CUnitEnemy::InvokeMove()
{
	//エネミー起動
	ChangeState(eENEMY_STATE_WALK);
}

void CUnitEnemy::Update(const f32 fDelta)
{
	NS_UNUSED(fDelta);
	UpdateState();
}

void CUnitEnemy::UpdateState()
{
	if (m_pcUnitModel != nullptr)
	{
		f32 fLightAddSub = 0.0f;

		switch (m_eEnemyState)
		{
		case CUnitEnemy::eENEMY_STATE_STAY:
			fLightAddSub = 3.0f;
			//待機(なにもしない)
			break;
		case CUnitEnemy::eENEMY_STATE_WALK:
			fLightAddSub = 6.0f;
			m_vBeforeMovingPos = m_pcUnitModel->GetPos();

			if (m_vaRootPos.size() >= 2)
			{
				//座標に沿って往復
				
				f32 fSpeed = m_fTrailSpeed / NSVector2(m_vaRootPos[m_nTrailBaseId] - m_vaRootPos[m_nTrailID]).GetLength();	//軽く速度補正
				m_fTrailCounter += fSpeed;
				if (m_fTrailCounter >= 1.0f)
				{
					m_nTrailBaseId = m_nTrailID;
					//追跡ID切り替え
					if (m_bTrailDir)
					{
						if ((s32)m_vaRootPos.size()-1 > m_nTrailID)
						{
							m_nTrailID++;
						}
						else
						{
							m_nTrailID--;
							m_bTrailDir = false;
						}
					}
					else
					{
						m_nTrailID--;
						if (m_nTrailID < 0)
						{
							m_nTrailID = 1;
							m_bTrailDir = true;
						}
					}
					m_fTrailCounter = 0.0f;
				}
				NSVector3 vPos = m_pcUnitModel->GetPos();

#if 1
				NSVector2 vUpdatePos = NSVector2::Lerp(
					m_vaRootPos[m_nTrailBaseId], m_vaRootPos[m_nTrailID], m_fTrailCounter);
#else
				NSVector2 dirA, dirB;
				dirB.SetZero();

				dirA = NSVector2(m_vaRootPos[m_nTrailID] - m_vaRootPos[m_nTrailBaseId]);
				dirA.Normalize();

				if (m_nTrailID + 1 < (s32)m_vaRootPos.size())
				{
					dirB = m_vaRootPos[m_nTrailID + 1] - m_vaRootPos[m_nTrailID];
				}
				dirB.Normalize();

				NSVector2 vUpdatePos = NSVector2::Spline(
					m_vaRootPos[m_nTrailBaseId], 
					m_vaRootPos[m_nTrailID], 
					dirA,
					dirB,
					m_fTrailCounter);
#endif
				m_pcUnitModel->SetPos(vUpdatePos.x, vPos.y, vUpdatePos.y);

				NSVector3 vMoveSub = m_pcUnitModel->GetPos() - m_vBeforeMovingPos;
				f32 fDegree = UtilMath::ATan2(vMoveSub.x, vMoveSub.z) * UtilMath::RadToDeg;
				m_pcUnitModel->SetRotate(0.0f, fDegree, 0.0f);
			}
			break;
		default:
			break;
		}
		TrailLightPos();

		m_fLightCounter += fLightAddSub;
		m_fLightIntensity = (UtilMath::Sin(UtilMath::DegToRad * m_fLightCounter) + 1.0f) * 0.5f;
		m_fLightIntensity = UtilMath::Clamp(m_fLightIntensity, 0.3f, 1.0f);
		SetLightIntensity();
	}
}

void CUnitEnemy::Term()
{
}

void CUnitEnemy::Destroy()
{
	Term();
}

void CUnitEnemy::OnState()
{
	switch (m_eEnemyState)
	{
	case CUnitEnemy::eENEMY_STATE_STAY:
		if (m_pcUnitModel)
		{
			m_pcUnitModel->PlayMotionForward(E_MOTION_ID::eMOTION_ID_STAY, true, false, 0.5f);
		}
		break;
	case CUnitEnemy::eENEMY_STATE_WALK:
		if (m_pcUnitModel)
		{
			m_pcUnitModel->PlayMotionForward(E_MOTION_ID::eMOTION_ID_WALK, true, false, 1.0f);
		}
		ResetTrailParam();
		break;
	default:
		break;
	}
}

void CUnitEnemy::OffState()
{
	switch (m_eEnemyState)
	{
	case CUnitEnemy::eENEMY_STATE_STAY:
		break;
	case CUnitEnemy::eENEMY_STATE_WALK:
		break;
	default:
		break;
	}
}

void CUnitEnemy::ChangeState(const E_ENEMY_STATE eState)
{
	OffState();
	m_eEnemyState = eState;
	OnState();
}

void CUnitEnemy::ResetTrailParam()
{
	m_nTrailBaseId = 0;
	m_nTrailID = 1;
	m_fTrailCounter = 0.0f;
	m_bTrailDir = true;
}

//EOF
