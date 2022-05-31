/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		Model.cpp
	@brief		モデルクラス
	@author		shiba
	@date		2012/7/23 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/

#include "FrameWork/Rendering/Model.h"
#include "FrameWork/Rendering/RenderingContext.h"
#include "FrameWork/Rendering/Shader/ShaderManager.h"
#include "FrameWork/Rendering/Motion.h"
#include "FrameWork/Rendering/Joint.h"
#include "FrameWork/Rendering/Shader/ShaderBinder.h"
#include "FrameWork/Rendering/Material.h"

#include "FrameWork/Asset/AssetManager.h"
#include "FrameWork/Asset/AssetModel.h"
#include "FrameWork/Asset/AssetMesh.h"

#include "FrameWork/DXUtil/UtilDX11.h"

#include "FrameWork/Camera/CameraBase.h"
#include "FrameWork/Camera/CameraManager.h"

using namespace DirectX;

//----------------------------------------------------------------------------
/*	@brief	コンストラクタ
*/
//----------------------------------------------------------------------------
CModel::CModel()
	: m_pcModelData(nullptr)
	, m_paCombMatrix(nullptr)
	, m_paInstancingMatrix(nullptr)
	, m_nInstancingMatrixNum(0)
	, m_unPlayingMotionID( INVALID_U32 )
	, m_baMeshDrawFlag(nullptr)
{
}

//----------------------------------------------------------------------------
/*	@brief	デストラクタ
*/
//----------------------------------------------------------------------------
CModel::~CModel()
{
	Term();
}

//---------------------------------------------------------------------------
/*!
	@brief	モデルロード
	@param[in]	pStrFileName		ファイル名
	@param[in]	pAnimSetCSVFileName	アニメセットCSVを使う場合は設定
*/
//---------------------------------------------------------------------------
bool CModel::LoadModel(const TCHAR* pStrFileName, const TCHAR* pAnimSetCSVFileName/* = nullptr*/)
{
	bool bSuccess = false;
	CAssetManager* pcAssetM = CAssetManager::GetInstance();

	if (pcAssetM != nullptr)
	{
		CAssetModel* pcModel = pcAssetM->LoadModel(pStrFileName, pAnimSetCSVFileName);
		if (pcModel)
		{
			SetModelData(pcModel);
			bSuccess = true;
		}
	}
	return bSuccess;
}

//----------------------------------------------------------------------------
/*	@brief	初期化
*/
//----------------------------------------------------------------------------
bool CModel::Init()
{
	bool bRet = CRender::Init();

	if (!bRet)
	{
		return bRet;
	}

	return true;
}

//----------------------------------------------------------------------------
/*	@brief	破棄
*/
//----------------------------------------------------------------------------
void CModel::Destroy()
{
	Term();
}

//----------------------------------------------------------------------------
/*	@brief	更新
*/
//----------------------------------------------------------------------------
void CModel::Update(f32 fDelta)
{
	NS_UNUSED(fDelta);

	//座標更新
	CRender::Update(fDelta);

	//スキンメッシュなら処理
	//@todo この部分をメッシュ毎にやる必要がある…？
	if ( m_pcModelData != nullptr && m_pcModelData->IsExistSkinMesh() )
	{
		const int nBoneSize = m_pcModelData->GetJointNum();
		if (m_paCombMatrix == nullptr)
		{
			//合成行列が未作成なら作っておく
			m_paCombMatrix = new NSMatrix[nBoneSize];

			for (u32 i = 0; i < m_pcModelData->GetJointNum(); i++)
			{
				CJoint* pcJoint = m_pcModelData->GetJoint(i);
				if (pcJoint != nullptr)
				{
					pcJoint->m_pCombMatArray = m_paCombMatrix;
				}
			}
		}

		for (int i = 0; i < nBoneSize; ++i)
		{
			m_paCombMatrix[i] = XMMatrixIdentity();
		}
		//キーフレームデータの適用
		NSMatrix defBone[BONE_MAX];
		for (s32 i = 0; i < BONE_MAX; ++i)
		{
			defBone[i] = XMMatrixIdentity();
		}
#if false
		static  f32 fCounter = 0.0f;
		//defBone[8] = XMMatrixTranslation(0.0f, UtilMath::Cos(fCounter)*30.0f, 0.0f);
		defBone[8] = XMMatrixScaling(UtilMath::Cos(fCounter), UtilMath::Cos(fCounter), UtilMath::Cos(fCounter));
		//defBone[8] = XMMatrixRotationY(UtilMath::Cos(fCounter)*30.0f);
		fCounter += 0.05f;
#else

		if (m_unPlayingMotionID != INVALID_U32)
		{
			CMotion* pcMot = m_pcModelData->GetMotionData(m_unPlayingMotionID);
			if (pcMot != nullptr)
			{
				//モーション更新
				pcMot->Update(fDelta);

				//@todo 必要があれば加算モーション等
				for (u32 j = 0; j < m_pcModelData->GetJointNum(); ++j)
				{
					CJoint* pcJoint = m_pcModelData->GetJoint(j);

					if (pcJoint->IsJoint()) //ジョイントのみアニメ対応
					{
						//スケール
						NSVector3 vScale;
						pcMot->GetAnimScale(pcJoint->GetJointID(), &vScale);

						//回転
						NSQuat qRot = XMQuaternionIdentity();
						pcMot->GetAnimRot(pcJoint->GetJointID(), &qRot);
						qRot = XMQuaternionNormalize(qRot);

						if (BONE_MAX > j)
						{
							defBone[j] = XMMatrixScaling(vScale.x, vScale.y, vScale.z) * XMMatrixRotationQuaternion(qRot);

							//座標
							NSVector3 vPos;
							pcMot->GetAnimPos(pcJoint->GetJointID(), &vPos);
							//defBone[j]._41 = vPos.x;
							//defBone[j]._42 = vPos.y;
							//defBone[j]._43 = vPos.z;
						}
						else
						{
							NS_Report(_T("defBone isn't enough to calc boneMatrix"));
						}
					}
				}

				//モーション終了チェック
				if (!pcMot->IsPlayingAnim())
				{
					m_unPlayingMotionID = INVALID_U32;
				}
			}
		}
#endif
		// 各ボーン行列の親空間ベースでの姿勢を更新
		// 差分姿勢×初期姿勢（共に親空間ベース）です。
		for (u32 i = 0; i < m_pcModelData->GetJointNum(); i++)
		{
			//アニメーションデータへの更新
			CJoint* pcJoint = m_pcModelData->GetJoint(i);
			if (pcJoint != nullptr)
			{
				pcJoint->m_BoneMat = XMMatrixIdentity();

				//Jointとして登録されている場合のみ
				if (BONE_MAX > i)
				{
					pcJoint->m_BoneMat = defBone[i] * pcJoint->m_InitMat;
				}
				else
				{
					NS_Report(_T("defBone isn't enough to calc boneMatrix"));
				}
			}
		}

		// 親空間ベースにある各ボーン行列をローカル空間ベースの姿勢に変換
		// ここは親子関係に従って行列を掛ける必要があります
		// 掛ける順番は 子 × 親 です。
		struct UpdateBone
		{
			static void run(CJoint* me, NSMatrix *parentWorldMat) {
				if (parentWorldMat != nullptr && me != nullptr)
				{
					//if (me->IsJoint())
					me->m_BoneMat *= *parentWorldMat;

					if (me->m_pCombMatArray != nullptr)
					{
						//一旦逆行列で引き込む
						//if (me->IsJoint())
						me->m_pCombMatArray[me->GetJointID()] = me->m_invMatrix * me->m_BoneMat;

						if (me->m_pFirstChild)
							run(me->m_pFirstChild, &me->m_BoneMat);
						if (me->m_pSibling)
							run(me->m_pSibling, parentWorldMat);
					}
				}
			};
		};

		//ルートジョイントからかけていく
  		CJoint* pcRootJoint = GetRootJoint();
		UpdateBone::run(pcRootJoint, &m_WorldMat);

		//Test
#if false
		for (u32 i = 0; i < m_pcModelData->GetJointNum(); i++)
		{
			if (m_paCombMatrix != nullptr)
			{
				m_paCombMatrix[i]._41 = 0.0f;
				m_paCombMatrix[i]._42 = 0.0f;
				m_paCombMatrix[i]._43 = 0.0f;
			}
		}
#endif
	}
}

//----------------------------------------------------------------------------
/*	@brief	描画
*/
//----------------------------------------------------------------------------
void CModel::Draw()
{
	// 描画処理。
	// 今後の拡張も考えてModelクラスが情報を集めて描画する形にする。
	if (m_pcModelData != nullptr)
	{
		CRenderingContext& conText = GetRenderContext();
		conText.ApplyRenderDesc();

		u32 unMeshNum = m_pcModelData->GetMeshNum();
		for (u32 i = 0; i < unMeshNum; ++i)
		{
			if(m_baMeshDrawFlag[i] == false)
				continue;

			CAssetMesh* pcMesh = m_pcModelData->GetMesh(i);
			NS_ASSERT(pcMesh);

			//@todo 要検討
			//現状メッシュ毎ですべて同じシェーダが前提になってしまう
			CMaterial* pcMat = m_pcModelData->GetMaterial(i);
			if (pcMat != nullptr)
			{
				pcMat->SetShader(conText.GetShader());
			}

			if (pcMesh->m_nMaterialIndexCount > 0)
			{
				//CMaterial* pcMatBeginPointer = m_pcModelData->GetMaterial(0);

				//@todo マルチマテリアル準備臨時対応
				//CMaterial* pcMatBeginPointer = m_pcModelData->GetMaterial(pcMesh->m_naMaterialIndex[0]);
				//pcMesh->Draw(this, IsInstancingModel(), &pcMatBeginPointer);
				pcMesh->Draw(this, IsInstancingModel(), &pcMat);
			}
			else
			{
				pcMesh->Draw(this, IsInstancingModel(), nullptr);
			}

		}

		CUtilDX11::SetAllDefaultState();
	}
}

//---------------------------------------------------------------------------
/*!
	@brief		ジョイント取得
	@param[in]	unJointIdx
*/
//---------------------------------------------------------------------------
CJoint* CModel::GetJointByIdx(const u32 unJointIdx)
{
	if (m_pcModelData != nullptr)
	{
		return m_pcModelData->GetJoint(unJointIdx);
	}

	return nullptr;
}

//---------------------------------------------------------------------------
/*!
	@brief	ジョイント取得
	@param[in]	strJointName	ジョイント名
*/
//---------------------------------------------------------------------------
CJoint* CModel::GetJoint(const TCHAR* strJointName)
{
	if (m_pcModelData != nullptr)
	{
		return m_pcModelData->GetJoint(strJointName);
	}

	return nullptr;
}

CJoint* CModel::GetRootJoint() const
{
	if (m_pcModelData != nullptr)
	{
		return m_pcModelData->GetRootJoint();
	}

	return nullptr;
}

u32 CModel::GetJointNum() const
{
	if (m_pcModelData != nullptr)
	{
		return m_pcModelData->GetJointNum();
	}

	return 0;
}

//----------------------------------------------------------------------------
/*	@brief	モデルを指定ボーンにアタッチ
*/
//----------------------------------------------------------------------------
void CModel::AttachModelToJoint(CModel *pcAttachModel, const TCHAR* strBoneName)
{
	//@todo ジョイントのマトリクスを演算に加えるようにする
	NS_UNUSED(strBoneName);
	NS_UNUSED(pcAttachModel);
}

//----------------------------------------------------------------------------
/*	@brief	モデルデータセット
*/
//----------------------------------------------------------------------------
void CModel::SetModelData(CAssetModel* pcModelData)
{
	m_pcModelData = pcModelData;

	if (m_pcModelData)
	{
		SAFE_DELETEA(m_baMeshDrawFlag);
		if (m_baMeshDrawFlag == nullptr)
		{
			m_baMeshDrawFlag = new bool[m_pcModelData->GetMeshNum()];
			for (u32 i = 0; i < m_pcModelData->GetMeshNum(); ++i)
			{
				m_baMeshDrawFlag[i] = true;
			}
		}
	}
}

//----------------------------------------------------------------------------
/*	@brief	モーションデータ取得（文字列からのハッシュ値使用）
*/
//----------------------------------------------------------------------------
CMotion* CModel::GetMotion(const TCHAR* strMotionName)
{
	if (strMotionName != nullptr && m_pcModelData != nullptr)
	{
		u32 unHash = CUtilHash::calcNameHash(strMotionName);
		for (s32 i = 0; i < m_pcModelData->GetMotionDataNum(); ++i)
		{
			CMotion* pcMotion = m_pcModelData->GetMotionData(i);
			if (pcMotion != nullptr)
			{
				if (unHash == pcMotion->GetMotionHash())
				{
					return pcMotion;
				}
			}
		}
	}
	return nullptr;
}

void CModel::PlayMotionForward(const u32 unMotionID, const bool bLoop, const bool bEndLoop /* = false */, const f32 fSpeed /* = 1.0f */)
{
	if (m_pcModelData != nullptr && m_unPlayingMotionID != unMotionID)
	{
		CMotion* pcMotion = m_pcModelData->GetMotionData(unMotionID);
		if (pcMotion != nullptr)
		{
			pcMotion->PlayMotion(fSpeed, bLoop, bEndLoop);
			m_unPlayingMotionID = pcMotion->GetMotionID();
		}
	}
}

void CModel::PlayMotionBack(const u32 unMotionID, const bool bLoop, const bool bEndLoop /* = false */, const f32 fSpeed /* = 1.0f */)
{
	if (m_pcModelData != nullptr)
	{
		CMotion* pcMotion = m_pcModelData->GetMotionData(unMotionID);
		if (pcMotion != nullptr)
		{
			pcMotion->PlayBackMotion(fSpeed, bLoop, bEndLoop);
			m_unPlayingMotionID = pcMotion->GetMotionID();
		}
	}
}

void CModel::StopPlayingMotion()
{
	//@todo 他に処理いるかも？
	m_unPlayingMotionID = INVALID_U32;
}

void CModel::SetDiffuseTexID(const s32 nMeshID, const u32 unTexUIID)
{
	//@todo 
	//メッシュが使用するマテリアルに、テクスチャIDを割り当てる。
	//メッシュにマテリアルが指定されていない場合は新しく生成する必要がある。
	CMaterial* pMat = m_pcModelData->GetMaterial(nMeshID);
	if (pMat != nullptr)
	{
		pMat->SetDiffuseTextureID(unTexUIID);
	}
}

void CModel::SetNormalTexID(const s32 nMeshID, const u32 unTexUIID)
{
	//@todo 
	//メッシュが使用するマテリアルに、テクスチャIDを割り当てる。
	//メッシュにマテリアルが指定されていない場合は新しく生成する必要がある。
	CMaterial* pMat = m_pcModelData->GetMaterial(nMeshID);
	if (pMat != nullptr)
	{
		pMat->SetNormalTextureID(unTexUIID);
	}
}

//----------------------------------------------------------------------------
/*	@brief	メッシュ単位の描画On/Off切り替え。
*/
//----------------------------------------------------------------------------
void CModel::SetFlagDrawMesh(const u32 nMeshID, const bool bDrawFlag)
{
	if (m_pcModelData == nullptr)
	{
		return;
	}
	if (m_pcModelData->GetMeshNum() <= nMeshID || nMeshID < 0)
	{
		return;
	}

	m_baMeshDrawFlag[nMeshID] = bDrawFlag;
}

//----------------------------------------------------------------------------
/*	@brief	インスタンシング用マトリクスの生成
	@param[in]	nNum	マトリクス生成数
*/
//----------------------------------------------------------------------------
void CModel::CreateInstancingMatrix(s32 nNum)
{
	if (m_paInstancingMatrix == nullptr && nNum > 0)
	{
		if (nNum > INSTANCE_MODEL_MAX)
		{
			nNum = INSTANCE_MODEL_MAX;
		}

		m_paInstancingMatrix = new NSMatrix[nNum];
		m_nInstancingMatrixNum = nNum;

		for (s32 i = 0; i < nNum; ++i)
		{
			m_paInstancingMatrix[i] = XMMatrixIdentity();
		}
	}
}

//----------------------------------------------------------------------------
/*	@brief	インスタンシング用マトリクスの設定
*/
//----------------------------------------------------------------------------
void CModel::SetInstancingMatrix(const s32 nIndex, NSMatrix& cMat)
{
	if (m_paInstancingMatrix != nullptr && m_nInstancingMatrixNum > nIndex && nIndex >= 0)
	{
		m_paInstancingMatrix[nIndex] = cMat;
	}
}

//----------------------------------------------------------------------------
/*	@brief	破棄
*/
//----------------------------------------------------------------------------
void CModel::Term()
{
	SAFE_DELETEA(m_baMeshDrawFlag);
	CAssetManager* pcAssetM = CAssetManager::GetInstance();
	if (pcAssetM != nullptr)
	{
		pcAssetM->RemoveAsset(m_pcModelData);
	}

	if (m_paCombMatrix)
	{
		SAFE_DELETEA(m_paCombMatrix);
	}

	if (m_paInstancingMatrix)
	{
		SAFE_DELETEA(m_paInstancingMatrix);
	}
}

#if defined(_DEBUG)
void CModel::OutputVertexData()
{
	if (m_pcModelData != nullptr)
	{
		m_pcModelData->OutputVertexData();
	}
}
#endif

//EOF
