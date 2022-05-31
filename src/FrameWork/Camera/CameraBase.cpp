/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		CameraBase.cpp
	@brief		カメラ基底クラス
	@author		shiba
	@date		2012/9/9 create this file.

*/
/*==========================================================================*/

#include "CameraBase.h"
using namespace DirectX;

//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CCameraBase::CCameraBase() 
: m_eCameraID( CCameraManager::eCAMERAID_NONE )
, m_vCameraPos(0,0,0)
, m_vAtPos()
, m_vUpPos(0,1,0)
{
	m_View = XMMatrixIdentity();
	m_Proj = XMMatrixIdentity();
	m_camPoseMat = XMMatrixIdentity();
}

//---------------------------------------------------------------------------
/*!
	@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CCameraBase::~CCameraBase()
{
}

//---------------------------------------------------------------------------
/*!
	@brief	初期化
*/
//---------------------------------------------------------------------------
bool CCameraBase::Init()
{
	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	破棄
*/
//---------------------------------------------------------------------------
void CCameraBase::Destroy()
{
}

//---------------------------------------------------------------------------
/*!
	@brief	更新
*/
//---------------------------------------------------------------------------
void CCameraBase::Update( const f32 fDelta )
{
	NS_UNUSED(fDelta);
}

//---------------------------------------------------------------------------
/*!
	@brief		カメラ座標設定
	@param[in]	vPos	座標
*/
//---------------------------------------------------------------------------
void CCameraBase::SetCameraPos( const NSVector3& vPos )
{
	NSVector4 vEye = XMVectorSet( vPos.x, vPos.y, vPos.z, 0.0f);				//カメラの位置
	NSVector4 vAt  = XMVectorSet( m_vAtPos.x, m_vAtPos.y, m_vAtPos.z, 0.0f);	//焦点の位置
	NSVector4 vUp  = XMVectorSet( m_vUpPos.x, m_vUpPos.y, m_vUpPos.z, 0.0f);	//UP
	m_vCameraPos = vPos;

	m_View = XMMatrixLookAtLH(vEye, vAt, vUp);
}

void CCameraBase::AddCameraPos(const NSVector3& vAddPos)
{
	const NSVector3& vPos = GetCameraPos();
	SetCameraPos(vPos + vAddPos);
}

void CCameraBase::SetCameraAtPos(const NSVector3& vAtPos)
{
	NSVector4 vEye = XMVectorSet(m_vCameraPos.x, m_vCameraPos.y, m_vCameraPos.z, 0.0f);	//カメラの位置
	NSVector4 vAt = XMVectorSet(vAtPos.x, vAtPos.y, vAtPos.z, 0.0f);					//焦点の位置
	NSVector4 vUp = XMVectorSet(m_vUpPos.x, m_vUpPos.y, m_vUpPos.z, 0.0f);				//UP
	m_vAtPos = vAtPos;

	m_View = XMMatrixLookAtLH(vEye, vAt, vUp);
}

//---------------------------------------------------------------------------
/*!
	@brief		カメラ座標取得
*/
//---------------------------------------------------------------------------
const NSVector3& CCameraBase::GetCameraPos() const
{
	return m_vCameraPos;
}

NSVector3 CCameraBase::ScreenToWorldPos( const NSVector2& vScreen ) const
{
	NSVector3 world;
	//@todo 必要になったら実装
	NS_UNUSED(vScreen);

	return world;
}

NSVector2 CCameraBase::WorldToScreenPos( const NSVector3& vScreen ) const
{
	NSVector2 scrPos;
	//@todo 必要になったら実装
	NS_UNUSED(vScreen);

	return scrPos;
}

bool CCameraBase::GetCameraXDir(NSVector3& vVec) const
{
	NSVector4 Determinant;
	NSMatrix cMat = XMMatrixInverse(&Determinant, m_View);

	vVec = NSVector3(cMat._11, cMat._12, cMat._13);
	vVec.Normalize();

	return true;
}

bool CCameraBase::GetCameraZDir(NSVector3& vVec) const
{
	NSVector4 Determinant;
	NSMatrix cMat = XMMatrixInverse(&Determinant, m_View);

	vVec = NSVector3(cMat._31, cMat._32, cMat._33);
	vVec.Normalize();

	return true;
}

//EOF
