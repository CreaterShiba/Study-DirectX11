
#if defined( _DEBUG )

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		DebugCamera.cpp
	@brief		操作可能デバッグカメラ
	@author		shiba
	@date		2012/9/30 create this file.
	@memo		http://marupeke296.com/DXG_No56_CameraBaseTrans.html
*/
/*==========================================================================*/
/*==========================================================================*/

#include "DebugCamera.h"
#include "Framework/Utility/UtilInput.h"
#include "Framework/Input/InputManager.h"

using namespace DirectX;

//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CDebugCamera::CDebugCamera()
	: m_camPosL()
{
}

//---------------------------------------------------------------------------
/*!
	@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CDebugCamera::~CDebugCamera()
{
}

//---------------------------------------------------------------------------
/*!
	@brief	初期化
*/
//---------------------------------------------------------------------------
bool CDebugCamera::Init()
{
	bool bRet = CCameraBase::Init();

	DXMng* pcDirectX = DXMng::GetInstance();
	NS_ASSERT( pcDirectX );

	//ビューポート作成
	m_Viewport.TopLeftX	= 0.0f;
	m_Viewport.TopLeftY	= 0.0f;
	m_Viewport.Width	= NS_STATIC_CAST( f32, pcDirectX->GetSwapChainDesc().BufferDesc.Width ); 
	m_Viewport.Height	= NS_STATIC_CAST( f32, pcDirectX->GetSwapChainDesc().BufferDesc.Height );
	m_Viewport.MinDepth	= 0.0f;
	m_Viewport.MaxDepth	= 1.0f;

	//最初は稼働しない
	SetTaskEnable(false);

	//ポーズレベルは最高
	SetLockLv(CTask::e_Task_Lock_07);

	return bRet;
}

//---------------------------------------------------------------------------
/*!
	@brief	更新
*/
//---------------------------------------------------------------------------
void CDebugCamera::Update( const f32 fDelta )
{	
	NS_UNUSED( fDelta );

	bool bUpdate = false;

	//カメラの注視点変更(移動)
	if( CUtilInput::IsPush(eMOUSE_LEFT, eINPUT_DEVICE_TYPE_MOUSE ) )
	{
		//マウスのスクリーン座標系で動かした値。
		NSVector2 vec = CUtilInput::GetMouseCursorDir();

		NSVector4 vec2 = XMVectorSet( vec.x, -vec.y, 0.0f, 0.0f );
		NSVector3 vecLen = m_camPosL - m_vAtPos;
		vec2 *= vecLen.GetLength() / 500.0f;	//

		//移動値をカメラの姿勢系に変換
		vec2 = XMVector3TransformCoord( vec2, m_camPoseMat );
		NSVector3 vAddVec = NSVector3(XMVectorGetX(vec2), XMVectorGetY(vec2), XMVectorGetZ(vec2));

		m_camPosL += vAddVec;
		m_vAtPos += vAddVec;

		bUpdate = true;
	}

	//カメラ回転
	if( CUtilInput::IsPush( eMOUSE_RIGHT, eINPUT_DEVICE_TYPE_MOUSE) )
	{	
		//マウスのスクリーン座標系で動かした値。
		const NSVector2 vec = CUtilInput::GetMouseCursorDir();

		//マウスを動かした方向で、カメラ姿勢行列から姿勢を取り出す。(割った係数はトライアンドエラー。)
		NSVector2 vDirMove = NSVector2(vec.x * 0.01f, vec.y * 0.01f);
		float fRotateSpeed = vDirMove.GetLength();

		if( fRotateSpeed > 0.0f )
		{
			//あくまでスクリーン座標系なので、カメラ姿勢にする。
			NSVector4 v4Dir = XMVectorSet(vDirMove.x, vDirMove.y, 0.0f, 0.0f);
			NSVector4 DL = XMVector3TransformCoord(v4Dir, m_camPoseMat );

			//マウスを動かした方向と、常に注視点を向いていると想定したZ軸で外積を取る。
			//ここで取った外積が、回転軸になる。
			NSVector4& vecCameraZAxis = m_camPoseMat.r[2];
			NSVector4 vecRotAxis = XMVector3Cross( DL, vecCameraZAxis );

			//回転軸を基にした、回転行列を作成
			NSVector4 TransQ = XMQuaternionRotationAxis( vecRotAxis, fRotateSpeed );

			NSMatrix TransRotMat = XMMatrixRotationQuaternion( TransQ );

			//座標に回転行列をかける。
			NSVector4 vec2 = XMVector3TransformCoord(XMVectorSet(m_camPosL.x, m_camPosL.y, m_camPosL.z, 1.0f), TransRotMat);
			m_camPosL = NSVector3(XMVectorGetX(vec2), XMVectorGetY(vec2), XMVectorGetZ(vec2));

			updateCameraPose();

			bUpdate = true;
		}
	}

	//カメラのズーム値調整
	//bool bZoom = false;
	CInputManager* pInput = CInputManager::GetInstance();
	if( pInput )
	{
		long move = pInput->GetMouseWheelMove();
		if( move != 0 )
		{
			f32 fZoomLength = (f32)move * 0.05f;

			//Z軸の方向へ移動させる
			NSVector3 Z;
			GetCameraZDir(Z);
			Z.Normalize();
			Z *= fZoomLength;
			m_camPosL += Z;

			//注視点も移動分、更新しておく。
			m_vAtPos += Z;

			//updateCameraPose();

			bUpdate = true;
		}
	}

	if( bUpdate )
	{
		//ビュー行列を更新する
		NSVector3 posW = m_camPosL;

		m_View = m_camPoseMat;
		m_View.r[3] = XMVectorSet(posW.x, posW.y, posW.z, 1.0f);	//座標
		m_View._44 = 1.0f;

		//NS_Report(_T("x.%f:y.%f:z.%f\n"), posW.x, posW.y, posW.z);

		//姿勢行列を逆行列にしてビュー行列の作成
		NSVector4 Determinant;
		m_View = XMMatrixInverse(&Determinant, m_View);
	}
}

//---------------------------------------------------------------------------
/*!
	@brief	デバッグカメラ用ビュー行列セットアップ
*/
//---------------------------------------------------------------------------
void CDebugCamera::SetupDebugCamera( const NSMatrix& viewMat, const NSMatrix& projMat, const NSVector3& vAt )
{
	m_vAtPos = NSVector3( vAt.x, vAt.y, vAt.z);

	//そのままコピー。
	m_View = viewMat;
	m_Proj = projMat;

	//逆行列にして、カメラ座標系にする
	NSVector4 Determinant;
	m_camPoseMat = XMMatrixInverse( &Determinant, m_View);

	//カメラ座標系から色々情報を取り出しておく
	m_vCameraPos = NSVector3(m_camPoseMat._41, m_camPoseMat._42, m_camPoseMat._43);
	m_camPosL = NSVector3( m_vCameraPos.x, m_vCameraPos.y, m_vCameraPos.z);

	m_camPoseMat._41 = m_camPoseMat._42 = m_camPoseMat._43 = 0.0f;
}

//---------------------------------------------------------------------------
/*!
	@brief	カメラの姿勢行列更新
*/
//---------------------------------------------------------------------------
void CDebugCamera::updateCameraPose()
{
	//座標が移動したので、カメラの姿勢は求めなおす
	NSVector3 X, Y, Z;

	//Z軸は常に注視点を向く。
	Z = m_vAtPos - m_camPosL;
	Z.Normalize();

	//前フレームのY方向を使用する。
	NSVector4 YVec = m_camPoseMat.r[1];
	Y = NSVector3(XMVectorGetX(YVec), XMVectorGetY(YVec), XMVectorGetZ(YVec));

	//YとZの外積でXを確定
	X = Y.Cross(Z);
	X.Normalize();

	//Yを再度確定
	Y = Z.Cross(X);
	Y.Normalize();

	//姿勢更新
	m_camPoseMat = XMMatrixIdentity();

	m_camPoseMat.r[0] = XMVectorSet(X.x, X.y, X.z, 0.0f);
	m_camPoseMat.r[1] = XMVectorSet(Y.x, Y.y, Y.z, 0.0f);
	m_camPoseMat.r[2] = XMVectorSet(Z.x, Z.y, Z.z, 0.0f);
}

#endif // defined( _DEBUG )

//EOF
