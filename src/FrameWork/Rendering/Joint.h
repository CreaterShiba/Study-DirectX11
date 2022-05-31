#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		Joint.h
	@brief		ジョイントクラス
	@author		shiba
	@date		2012/10/14 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "FrameWork/Utility/UtilDefine.h"
#include "ModelDataType.h"

#define JOINT_NAME_SIZE 128

/*==========================================================================*/
/*!
	@class	CJoint
	@brief	ジョイントクラス
*/
/*==========================================================================*/
class CJoint
{
	//コピー禁止
	void operator =(const CJoint& src) { NS_UNUSED(src); }
	CJoint(const CJoint& src);

public:
	CJoint() 
	: m_unJointID( 0 )
#if defined( _DEBUG )
	, m_bDebugDraw( false )
#endif
	, m_pParent( nullptr )
	, m_pFirstChild( nullptr )
	, m_pSibling( nullptr )
	, m_pCombMatArray( nullptr )
	, m_bJoint(false)
	{
		memset( m_aStrJointName, 0, sizeof( m_aStrJointName ) );

		m_InitMat = DirectX::XMMatrixIdentity();
		m_invMatrix = DirectX::XMMatrixIdentity();
		m_BoneMat = DirectX::XMMatrixIdentity();
	}
	~CJoint(){};

	bool Init()
	{
		return true;
	}

	bool Copy(CJoint* pcCopyJoint)
	{
		if (pcCopyJoint == nullptr)
		{
			return false;
		}

		pcCopyJoint->SetJointName(m_aStrJointName);
		pcCopyJoint->m_unJointID = m_unJointID;

#if defined(_DEBUG)
		pcCopyJoint->m_bDebugDraw = m_bDebugDraw;
#endif

		pcCopyJoint->m_InitMat = m_InitMat;
		pcCopyJoint->m_invMatrix = m_invMatrix;
		pcCopyJoint->m_BoneMat = m_BoneMat;
		pcCopyJoint->m_bJoint = m_bJoint;

		return true;
	}

	// @brief ジョイントID設定
	inline void SetJointID( const u32 uwJointID );

	// @brief ジョイントID獲得
	inline u32 GetJointID() const;

	// @brief ジョイント名設定
	inline void SetJointName( const TCHAR* name );

	// @brief ジョイント名の取得
	inline const TCHAR* GetJointName() const;

	void SetJointFlag(const bool bFlag)
	{
		m_bJoint = bFlag;
	}

	bool IsJoint()
	{
		return m_bJoint;
	}

private:
	//!< ジョイントネーム	
	TCHAR m_aStrJointName[ JOINT_NAME_SIZE ];

	//!< ジョイントID
	u32			 m_unJointID;

#if defined(_DEBUG)
	//!< デバッグ用レンダリング
	bool m_bDebugDraw;
#endif

public:

	//!< 親
	CJoint*		m_pParent;

	//!< 第1子
	CJoint*		m_pFirstChild;		

	//!< 兄弟
	CJoint*		m_pSibling;	

	//!< ジョイントのローカル行列(InData)
	NSMatrix	m_InitMat;			

	//!< ジョイントをローカルに戻す逆行列(InData)
	NSMatrix	m_invMatrix;			
	
	//!< 計算行列
	NSMatrix	m_BoneMat;			

	//!< 行列
	NSMatrix*	m_pCombMatArray;	

	//!< ジョイント or ノード
	bool m_bJoint;
};

//---------------------------------------------------------------------------
/*!
	@brief	ジョイントID設定
	@param[in]	uwJointID	ジョイントID
*/
//---------------------------------------------------------------------------
void CJoint::SetJointID( const u32 uwJointID )
{
	m_unJointID = uwJointID;
}

//---------------------------------------------------------------------------
/*!
	@brief	ジョイントID獲得
	@return u16 ホイントID
*/
//---------------------------------------------------------------------------
u32 CJoint::GetJointID() const
{
	return m_unJointID;
}

//---------------------------------------------------------------------------
/*!
	@brief	ジョイント名の設定
	@param[in]	name	ジョイント名
*/
//---------------------------------------------------------------------------
void CJoint::SetJointName( const TCHAR* name )
{
	wcscpy(m_aStrJointName, name);
}

//---------------------------------------------------------------------------
/*!
	@brief	ジョイント名の獲得
	@return conts TCHAR* ジョイント名
*/
//---------------------------------------------------------------------------
const TCHAR* CJoint::GetJointName() const
{
	return m_aStrJointName;
}

//EOF