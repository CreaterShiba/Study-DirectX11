/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		GameCamera.cpp
	@brief		ゲームカメラ
	@author		shiba
	@date		2013/2/16 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#include "GameCamera.h"
#include "FrameWork/Utility/UtilInput.h"
#include "FrameWork/Utility/UtilCoord.h"

using namespace DirectX;

//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CGameCamera::CGameCamera() 
	: m_fCameraMoveDir(270.0f)
	, m_fCameraZoom(90.0f)
	, m_pTargetUnit(nullptr)
{

}

//---------------------------------------------------------------------------
/*!
	@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CGameCamera::~CGameCamera()
{
	m_pTargetUnit = nullptr;
}

//---------------------------------------------------------------------------
/*!
	@brief	初期化
*/
//---------------------------------------------------------------------------
bool CGameCamera::Init()
{
	bool bSuccess = CCameraBase::Init();

	DXMng* pcDirectX = DXMng::GetInstance();
	NS_ASSERT( pcDirectX );

	//ビューポート作成
	m_Viewport.TopLeftX	= 0.0f;
	m_Viewport.TopLeftY	= 0.0f;
	m_Viewport.Width	= NS_STATIC_CAST( f32, pcDirectX->GetSwapChainDesc().BufferDesc.Width ); 
	m_Viewport.Height	= NS_STATIC_CAST( f32, pcDirectX->GetSwapChainDesc().BufferDesc.Height );
	m_Viewport.MinDepth	= 0.0f;
	m_Viewport.MaxDepth	= 1.0f;

	//ビュー、射影行列の設定（カメラクラス）初期設定
	NSMatrix view;
	//m_vCameraPos.x = 5.0f;
	//m_vCameraPos.y = -2.0f;
	m_vCameraPos.y = 90.0f;
	m_vCameraPos.z = -90.0f;

	NSVector4 vEye = XMVectorSet( m_vCameraPos.x, m_vCameraPos.y, m_vCameraPos.z, 0.0f);	//カメラの位置
	NSVector4 vAt  = XMVectorSet( m_vAtPos.x, m_vAtPos.y, m_vAtPos.z, 0.0f);				//焦点の位置
	NSVector4 vUp  = XMVectorSet( m_vUpPos.x, m_vUpPos.y, m_vUpPos.z, 0.0f);				//UP

	m_View = XMMatrixLookAtLH(vEye, vAt, vUp);
	
	f32 aspect = m_Viewport.Width / m_Viewport.Height;
	m_Proj = XMMatrixPerspectiveFovLH( UtilMath::DegreeToRadian(45.0f), aspect, 20.0f, 300.0f);

	return bSuccess;
}

//---------------------------------------------------------------------------
/*!
	@brief	更新
*/
//---------------------------------------------------------------------------
void CGameCamera::Update( const f32 fDelta )
{
	NS_UNUSED(fDelta);
	if (m_pTargetUnit == nullptr)
	{
		return;
	}

	//注視点を維持したままカメラを回転(LR)
	bool bUpdate = true;

	//ターゲットの座標が移動した場合も変更
	
	//カメラ回転
	if (CUtilInput::IsPush(E_KEYBOARD_BUTTON::eKEYBOARD_LEFT))
	{
		m_fCameraMoveDir--;
		bUpdate = true;
	}
	if (CUtilInput::IsPush(E_KEYBOARD_BUTTON::eKEYBOARD_RIGHT))
	{
		m_fCameraMoveDir++;
		bUpdate = true;
	}

	if (bUpdate)
	{
		UpdateCamera();
	}
}


void CGameCamera::SetTarget(CUtilCoord* pCoord)
{
	m_pTargetUnit = pCoord;
	UpdateCamera();
}

void CGameCamera::UpdateCamera()
{
	if (m_pTargetUnit)
	{
		//カメラズーム
		NSVector3 dirVec;
		dirVec.x = UtilMath::Cos(m_fCameraMoveDir * UtilMath::DegToRad);
		dirVec.y = 0.85f;
		dirVec.z = UtilMath::Sin(m_fCameraMoveDir * UtilMath::DegToRad);
		dirVec.Normalize();
		dirVec *= m_fCameraZoom;

		//座標更新
		dirVec += m_pTargetUnit->GetPos();
		//dirVec.y = m_fCameraZoom;	//とりあえずYは固定

		SetCameraPos(dirVec);
		SetCameraAtPos(m_pTargetUnit->GetPos());
	}
}

//EOF
