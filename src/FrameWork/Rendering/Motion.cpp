/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		Motion.cpp
	@brief		モーションクラス
	@author		shiba
	@date		2012/10/31 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#include "Motion.h"
#include "FrameWork/MotionEvent/MotionEvent.h"

#include "FrameWork/Utility/UtilSound.h"

using namespace DirectX;

CMotion::CMotion() : 
	  m_StrMotion()
	, m_unNameHash(0)
	, m_unMotionID(0)
	, m_fTimeDuration(0.0f)
	, m_fFrameTime(0.0f)
	, m_fPastFrameTime(0.0f)
	, m_eAnimDir(eFORWARD)
	, m_fAnimPlaySpeed(1.0f)
	, m_bAnimLoop(false)
	, m_bPlayAnim(false)
	, m_bPlayEndLoop(false)
	, m_pcMotionEvent(nullptr)
{
}

CMotion::~CMotion()
{
	Term();
}

bool CMotion::Copy(CMotion* pcCopyMotion)
{
	//自分のデータを引数で渡されたCMotionに引き渡すこと。
	memcpy_s(pcCopyMotion->m_StrMotion, 
		sizeof(m_StrMotion),
		m_StrMotion,
		sizeof(m_StrMotion));//文字列コピー

	pcCopyMotion->m_unNameHash = m_unNameHash;
	pcCopyMotion->m_unMotionID = m_unMotionID;

	pcCopyMotion->m_caKeyFrameNode = m_caKeyFrameNode;

	pcCopyMotion->m_fTimeDuration = m_fTimeDuration;
	//pcCopyMotion->m_fFrameTime = m_fFrameTime;
	//pcCopyMotion->m_fPastFrameTime = m_fPastFrameTime;
	//pcCopyMotion->m_eAnimDir = m_eAnimDir;
	//pcCopyMotion->m_fAnimPlaySpeed = m_fAnimPlaySpeed;
	//pcCopyMotion->m_bAnimLoop = m_bAnimLoop;
	//pcCopyMotion->m_bPlayAnim = m_bPlayAnim;
	//pcCopyMotion->m_bPlayEndLoop = m_bPlayEndLoop;
	pcCopyMotion->m_pcMotionEvent = m_pcMotionEvent;

	return true;
}

bool CMotion::Init()
{
	return true;
}

void CMotion::Update(const f32 fDelta)
{
	NS_UNUSED(fDelta);
	if (m_bPlayAnim)
	{
		bool bAnimEnd = false;

		m_fPastFrameTime = m_fFrameTime;

		f32 fSpeed = 1.0f * m_fAnimPlaySpeed;	//スピード

		if (m_eAnimDir == eFORWARD)
		{
			m_fFrameTime += fSpeed;
			if (m_fTimeDuration <= m_fFrameTime)
			{
				m_fFrameTime = m_fTimeDuration;
				bAnimEnd = true;
			}
		}
		else
		{
			m_fFrameTime -= fSpeed;
			if (0.0f > m_fFrameTime)
			{
				m_fFrameTime = 0.0f;
				bAnimEnd = true;
			}
		}

		if (bAnimEnd)
		{
			if (!m_bPlayEndLoop)	//終端ループの場合は処理しないまま。
			{
				m_bPlayAnim = false;
				if (m_bAnimLoop)
				{
					//ループ再生
					if (m_eAnimDir == E_ANIM_DIR::eBACK)
					{
						PlayBackMotion(fSpeed, true, false);
					}
					else
					{
						PlayMotion(fSpeed, true, false);
					}
				}
			}
		}

		//モーションイベントチェック。
		if (m_pcMotionEvent != nullptr && m_fPastFrameTime != m_fFrameTime)
		{
			std::vector<CMotionEventData*> apmotEvent;
			m_pcMotionEvent->GetAvailableMotEvent(m_fPastFrameTime, m_fFrameTime, &apmotEvent);	// 有効モーションイベント取得

			for (s32 i = 0; i < static_cast<s32>(apmotEvent.size()); ++i)
			{
				CMotionEventData& motData = *apmotEvent[i];
				if (motData.m_eEventType != CMotionEventData::eMAX)
				{
					//@todo イベント発行ここでいい？
					switch (motData.m_eEventType)
					{
						case CMotionEventData::eSOUND_PLAY:
							//音再生
							//CUtilSound::PlaySE(static_cast<E_SOUND_ID>(motData.m_cEventParam.m_naEventParam[0]));
							break;
						case CMotionEventData::eCOLLISION:
							//コリジョン発生
							break;
						case CMotionEventData::eEFFECT:
							//エフェクト発生
							break;
						default:
							break;
					}
				}
			}
		}
	}
}

void CMotion::Destroy()
{
	Term();
}

void CMotion::Term()
{
	for (u32 i = 0; i < m_caKeyFrameNode.size(); ++i)
	{
		m_caKeyFrameNode[i].Term();
	}
}

void CMotion::AddKeyPos(const NSVector3& vVec, f32 fTime, const s32 nNodeID)
{
	ReserveKeyNode(nNodeID);

	std::vector<CMotionKey>& avPos = m_caKeyFrameNode[nNodeID].m_avPos;
	avPos.push_back(CMotionKey());

	int nAddIndex = avPos.size() - 1;

	CMotionKey* pAddMotion = &avPos[nAddIndex];
	if (pAddMotion)
	{
		pAddMotion->m_vVec = vVec;
		pAddMotion->m_fKeyTime = fTime;
		
		if (nAddIndex > 0)
		{
			avPos[nAddIndex - 1].m_fKeySubTime = avPos[nAddIndex].m_fKeyTime - avPos[nAddIndex - 1].m_fKeyTime;
		}

		pAddMotion->m_eKeyType = ePOS;
		m_caKeyFrameNode[nNodeID].m_nPosKeyFrameNum++;
	}
}

void CMotion::AddKeyScale(const NSVector3& vVec, f32 fTime, const s32 nNodeID)
{
	ReserveKeyNode(nNodeID);

	std::vector<CMotionKey>& avScale = m_caKeyFrameNode[nNodeID].m_avScale;
	avScale.push_back(CMotionKey());

	int nAddIndex = avScale.size() - 1;

	CMotionKey* pAddMotion = &avScale[nAddIndex];
	if (pAddMotion)
	{
		pAddMotion->m_vVec = vVec;
		pAddMotion->m_fKeyTime = fTime;

		if (nAddIndex > 0)
		{
			avScale[nAddIndex - 1].m_fKeySubTime = avScale[nAddIndex].m_fKeyTime - avScale[nAddIndex - 1].m_fKeyTime;
		}

		pAddMotion->m_eKeyType = eSCALE;
		m_caKeyFrameNode[nNodeID].m_nScaleKeyFrameNum++;
	}
}

void CMotion::AddKeyRotate(const NSQuat& vVec, f32 fTime, const s32 nNodeID)
{
	ReserveKeyNode(nNodeID);

	std::vector<CMotionKey>& avRot = m_caKeyFrameNode[nNodeID].m_avRot;
	avRot.push_back(CMotionKey());

	int nAddIndex = avRot.size() - 1;

	CMotionKey* pAddMotion = &avRot[nAddIndex];
	if (pAddMotion)
	{
		pAddMotion->m_vQua = vVec;
		pAddMotion->m_fKeyTime = fTime;

		if (nAddIndex > 0)
		{
			avRot[nAddIndex - 1].m_fKeySubTime = avRot[nAddIndex].m_fKeyTime - avRot[nAddIndex - 1].m_fKeyTime;
		}

		pAddMotion->m_eKeyType = eROTATE;
		m_caKeyFrameNode[nNodeID].m_nRotKeyFrameNum++;
	}
}

void CMotion::PlayMotion(const f32 fSpeed, const bool bLoop, const bool bEndLoop)
{
	m_fFrameTime = 0.0f;
	m_fPastFrameTime = 0.0f;
	m_eAnimDir = eFORWARD;
	PlayMotionCore(fSpeed, bLoop, bEndLoop);
}

void CMotion::PlayBackMotion(const f32 fSpeed, const bool bLoop, const bool bEndLoop)
{
	m_fFrameTime = m_fTimeDuration;
	m_fPastFrameTime = m_fTimeDuration;
	m_eAnimDir = eBACK;
	PlayMotionCore(fSpeed, bLoop, bEndLoop);
}

void CMotion::PlayMotionCore(const f32 fSpeed, const bool bLoop, const bool bEndLoop)
{
	m_bAnimLoop = bLoop;
	m_fAnimPlaySpeed = fSpeed;
	m_bPlayEndLoop = bEndLoop;
	m_bPlayAnim = true;
}

//---------------------------------------------------------------------------
/*!
	@brief		アニメ再生時の移動値
	@param[in]	nNodeID	モデルノードID
	@param[out] pvReturnVal 移動値
*/
//---------------------------------------------------------------------------
void CMotion::GetAnimPos( const s32 nNodeID, NSVector3* const pvReturnVal ) const
{
	if (pvReturnVal == nullptr)
	{
		return;
	}

	NSVector3 vPos;

	if (m_bPlayAnim && nNodeID >= 0 && nNodeID < static_cast<s32>(m_caKeyFrameNode.size()))
	{
		const CMotionKeyNode& cKeyNode = m_caKeyFrameNode[nNodeID];

		int nKeyIndex = GetEnableIndex(ePOS, nNodeID);
		if (nKeyIndex >= 0)
		{
			if (cKeyNode.m_avPos.size() <= 1)
			{
				//キーが一個なので数値は固定
				vPos = cKeyNode.m_avPos[0].m_vVec;
			}
			else
			{
				if (nKeyIndex + 1 >= cKeyNode.m_nPosKeyFrameNum)
				{
					//最終フレームは数値を確定
					vPos = cKeyNode.m_avPos[nKeyIndex].m_vVec;
				}
				else
				{
					//最終フレームではないので補間で処理を行う
					const NSVector3& vStartPos = cKeyNode.m_avPos[nKeyIndex].m_vVec;
					NSVector3 vEndPos = vStartPos;

					if (nKeyIndex + 1 < cKeyNode.m_nPosKeyFrameNum)
					{
						vEndPos = cKeyNode.m_avPos[nKeyIndex + 1].m_vVec;
					}

					//キーフレームの間の進行差分時間を出す
					f32 fTime = m_fFrameTime - cKeyNode.m_avPos[nKeyIndex].m_fKeyTime;

					NS_ASSERT(cKeyNode.m_avPos[nKeyIndex].m_fKeySubTime != 0.0f);
					//キーフレーム間の補間処理
					vPos = NSVector3::Lerp(vStartPos, vEndPos, fTime / cKeyNode.m_avPos[nKeyIndex].m_fKeySubTime);
				}
			}
		}
	}

	*pvReturnVal = vPos;
}

//---------------------------------------------------------------------------
/*!
	@brief	アニメ再生時の回転値
	@param[in]	nNodeID	モデルノードID
	@param[out] pvReturnVal 移動値
*/
//---------------------------------------------------------------------------
void CMotion::GetAnimRot(const s32 nNodeID, NSQuat* const pvReturnVal ) const
{
	if (pvReturnVal == nullptr)
	{
		return;
	}

	NSQuat quat = XMQuaternionIdentity();

	if (m_bPlayAnim && nNodeID >= 0 && nNodeID < static_cast<s32>(m_caKeyFrameNode.size()))
	{
		const CMotionKeyNode& cKeyNode = m_caKeyFrameNode[nNodeID];

		int nKeyIndex = GetEnableIndex(eROTATE, nNodeID);
		if (nKeyIndex >= 0)
		{
			if (cKeyNode.m_avRot.size() <= 1)
			{
				//キーが一個なので数値は固定
				quat = cKeyNode.m_avRot[0].m_vQua;
			}
			else
			{
				if (nKeyIndex + 1 >= cKeyNode.m_nRotKeyFrameNum)
				{
					//最終フレームは数値を確定
					quat = cKeyNode.m_avRot[nKeyIndex].m_vQua;
				}
				else
				{
					NSQuat vStartRot = cKeyNode.m_avRot[nKeyIndex].m_vQua;

					NSQuat vEndRot = vStartRot;
					//float fSubTime = 1.0f;

					if (nKeyIndex + 1 < cKeyNode.m_nRotKeyFrameNum)
					{
						vEndRot = cKeyNode.m_avRot[nKeyIndex + 1].m_vQua;
					}

					//キーフレームの間の進行差分時間を出す
					f32 fTime = m_fFrameTime - cKeyNode.m_avRot[nKeyIndex].m_fKeyTime;

					NSQuat vStartRotNom = XMQuaternionNormalize(vStartRot);
					NSQuat vEndRotNom = XMQuaternionNormalize(vEndRot);

					//キーフレーム間の補間処理
					NS_ASSERT(cKeyNode.m_avRot[nKeyIndex].m_fKeySubTime != 0.0f);
					f32 fLerpTime = fTime / cKeyNode.m_avRot[nKeyIndex].m_fKeySubTime;
					quat = XMQuaternionSlerp(vStartRotNom, vEndRotNom, fLerpTime);
				}
			}
		}
	}
	*pvReturnVal = quat;
}

//---------------------------------------------------------------------------
/*!
	@brief	アニメ再生時のスケール値
	@param[in]	nNodeID	モデルノードID
	@param[out] pvReturnVal 移動値
*/
//---------------------------------------------------------------------------
void CMotion::GetAnimScale(const s32 nNodeID, NSVector3* const pvReturnVal ) const
{
	if (pvReturnVal == nullptr)
	{
		return;
	}

	NSVector3 vScale;
	vScale.SetOne();

	if (m_bPlayAnim && nNodeID >= 0 && nNodeID < static_cast<s32>(m_caKeyFrameNode.size()))
	{
		const CMotionKeyNode& cKeyNode = m_caKeyFrameNode[nNodeID];

		int nKeyIndex = GetEnableIndex(eSCALE, nNodeID);
		if (nKeyIndex >= 0)
		{
			if (cKeyNode.m_avScale.size() <= 1)
			{
				//キーが一個なので数値は固定
				vScale = cKeyNode.m_avScale[0].m_vVec;
			}
			else
			{
				if (nKeyIndex + 1 >= cKeyNode.m_nScaleKeyFrameNum)
				{
					//最終フレームは数値を確定
					vScale = cKeyNode.m_avScale[nKeyIndex].m_vVec;
				}
				else
				{
					const NSVector3& vStartScale = cKeyNode.m_avScale[nKeyIndex].m_vVec;

					NSVector3 vEndScale = vStartScale;
					//float fSubTime = 1.0f;

					if (nKeyIndex + 1 < cKeyNode.m_nScaleKeyFrameNum)
					{
						vEndScale = cKeyNode.m_avScale[nKeyIndex + 1].m_vVec;
					}

					//キーフレームの間の進行差分時間を出す
					f32 fTime = m_fFrameTime - cKeyNode.m_avScale[nKeyIndex].m_fKeyTime;

					NS_ASSERT(cKeyNode.m_avScale[nKeyIndex].m_fKeySubTime != 0.0f);
					//キーフレーム間の補間処理
					vScale = NSVector3::Lerp(vStartScale, vEndScale, fTime / cKeyNode.m_avScale[nKeyIndex].m_fKeySubTime);
				}
			}
		}
	}

	*pvReturnVal = vScale;
}

s32 CMotion::GetEnableIndex(const E_KEY_TYPE eKeyType, const s32 nNodeID) const
{
	s32 nKeyIndex = -1;

	if (nNodeID >= 0 && nNodeID < static_cast<s32>(m_caKeyFrameNode.size()))
	{
		CMotionKeyNode cKeyNode = m_caKeyFrameNode[nNodeID];

		if (eKeyType == ePOS)
		{
			for (u32 i = 0; i < cKeyNode.m_avPos.size(); ++i)
			{
				if (m_fFrameTime >= cKeyNode.m_avPos[i].m_fKeyTime)
				{
					//インデックス確定
					nKeyIndex = i;
				}
			}
		}
		else if (eKeyType == eROTATE)
		{
			for (u32 i = 0; i < cKeyNode.m_avRot.size(); ++i)
			{
				if (m_fFrameTime >= cKeyNode.m_avRot[i].m_fKeyTime)
				{
					//インデックス確定
					nKeyIndex = i;
				}
			}
		}
		else if (eKeyType == eSCALE)
		{
			for (u32 i = 0; i < cKeyNode.m_avScale.size(); ++i)
			{
				if (m_fFrameTime >= cKeyNode.m_avScale[i].m_fKeyTime)
				{
					//インデックス確定
					nKeyIndex = i;
				}
			}
		}
	}

	return nKeyIndex;
}


//---------------------------------------------------------------------------
/*!
	@brief	ノードの拡張
	@param[in]	nNodeID	拡張サイズ
*/
//---------------------------------------------------------------------------
void CMotion::ReserveKeyNode(const u32 nNodeSize)
{
	if (nNodeSize >= m_caKeyFrameNode.size())
	{
		for (u32 i = m_caKeyFrameNode.size(); i < nNodeSize + 1; ++i)
		{
			m_caKeyFrameNode.push_back(CMotionKeyNode());
		}
	}
}

//EOF