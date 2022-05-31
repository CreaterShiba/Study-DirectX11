#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		Motion.h
	@brief		モーションクラス
	@author		shiba
	@date		2012/10/31 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "FrameWork/Task/Task.h"

/*==========================================================================*/
/*!
	@class	CMotion
	@brief	モーションクラス
*/
/*==========================================================================*/
class CMotion
{
	//コピー禁止
	void operator =(const CMotion& src) { NS_UNUSED(src); }
	CMotion(const CMotion& src);

public:
	CMotion();
	virtual ~CMotion();

	bool Copy(CMotion* pcCopyMotion);

	bool Init();
	void Update(const f32 fDelta);
	void Destroy();

	enum E_KEY_TYPE
	{
		ePOS,
		eROTATE,
		eSCALE,
		eMAX
	};


	class CMotionKey
	{
	public:
		CMotionKey() :
			m_vQua(),
			m_vVec(),
			m_fKeyTime(0.0f),
			m_fKeySubTime(0.0f),
			m_eKeyType(eMAX)
		{
		}
		NSQuat m_vQua;			//モーション回転
		NSVector3 m_vVec;		//モーション移動値
		f32 m_fKeyTime;			//キータイム
		f32 m_fKeySubTime;		//前キーとの差分タイム
		E_KEY_TYPE m_eKeyType;	//タイプ
	};

	class CMotionKeyNode
	{
	public:
		CMotionKeyNode() :
			m_nPosKeyFrameNum(0),
			m_nRotKeyFrameNum(0),
			m_nScaleKeyFrameNum(0)
		{
			m_avPos.clear();
			m_avPos.reserve(KEY_SIZE_MAX);

			m_avRot.clear();
			m_avRot.reserve(KEY_SIZE_MAX);

			m_avScale.clear();
			m_avScale.reserve(KEY_SIZE_MAX);
		}
		void Term()
		{
			m_avPos.clear();
			m_avRot.clear();
			m_avScale.clear();
		}

		std::vector< CMotionKey > m_avPos;		//座標変化
		s32 m_nPosKeyFrameNum;					//キーフレームの数

		std::vector< CMotionKey > m_avRot;		//回転変化
		s32 m_nRotKeyFrameNum;					//キーフレームの数

		std::vector< CMotionKey > m_avScale;	//スケール変化
		s32 m_nScaleKeyFrameNum;				//キーフレームの数
	};

	static const s32 KEY_SIZE_MAX = 16;

private:
	enum E_ANIM_DIR
	{
		eFORWARD,
		eBACK,
	};
	
private:
	//データ
	TCHAR m_StrMotion[256];		//!< モーション名
	u32 m_unNameHash;			//!< モーションハッシュID
	u32 m_unMotionID;			//!< モーションID
	std::vector< CMotionKeyNode > m_caKeyFrameNode;	//!< ノード毎キーフレーム

	//パラメータ
	f32 m_fTimeDuration;	//!< アニメーションの長さ
	f32 m_fFrameTime;		//!< 進行時間	
	f32 m_fPastFrameTime;	//!< 前フレーム進行時間
	E_ANIM_DIR m_eAnimDir;	//!< 現在進行方向
	f32 m_fAnimPlaySpeed;	//!< アニメ速度
	bool m_bAnimLoop;		//!< ループ
	bool m_bPlayAnim;		//!< アニメ再生フラグ
	bool m_bPlayEndLoop;	//!< 終了時終端ループ

	class CMotionEvent* m_pcMotionEvent; //!< モーションイベント

public: 
	//キー追加
	void AddKeyPos(const NSVector3& vVec, f32 fTime, const s32 nNodeID);	
	void AddKeyScale(const NSVector3& vVec, f32 fTime, const s32 nNodeID);
	void AddKeyRotate(const NSQuat& vVec, f32 fTime,  const s32 nNodeID);

	//モーション再生
	void PlayMotion(const f32 fSpeed, const bool bLoop, const bool bEndLoop);
	void PlayBackMotion(const f32 fSpeed, const bool bLoop, const bool bEndLoop);
	void PlayMotionCore(const f32 fSpeed, const bool bLoop, const bool bEndLoop);

	//モーション中の数値取得
	void GetAnimPos( const s32 nNodeID, NSVector3* const pvReturnVal ) const;
	void GetAnimRot(const s32 nNodeID, NSQuat* const pvReturnVal ) const;
	void GetAnimScale(const s32 nNodeID, NSVector3* const pvReturnVal ) const;

	//モーションID管理
	inline void SetMotionID(const u32 unMotionID);
	inline u32 GetMotionID() const;

	//モーションデータ管理
	inline void SetDuration(const f32 fDuration);
	inline void SetMotionName(const TCHAR* strName);
	inline u32 GetMotionHash();
	inline bool IsPlayingAnim() const;

	//モーションイベントデータ設定
	inline void SetMotionEventData( CMotionEvent* const pcMotionEventData);

private:
	s32 GetEnableIndex( const E_KEY_TYPE eKeyType, const s32 nNodeID ) const;
	void ReserveKeyNode(const u32 nNodeSize);
	void Term();
};

void CMotion::SetMotionID(const u32 unMotionID)
{
	m_unMotionID = unMotionID;
}

u32 CMotion::GetMotionID() const
{
	return m_unMotionID;
}

void CMotion::SetDuration(const f32 fDuration)
{
	m_fTimeDuration = fDuration;
}

void CMotion::SetMotionName( const TCHAR* strName )
{
	wcscpy(m_StrMotion, strName);
	m_unNameHash = CUtilHash::calcNameHash(strName);
}

u32 CMotion::GetMotionHash()
{
	return m_unNameHash;
}

bool CMotion::IsPlayingAnim() const
{
	return m_bPlayAnim;
}

void CMotion::SetMotionEventData(CMotionEvent* const pcMotionEventData)
{
	m_pcMotionEvent = pcMotionEventData;
}

//EOF