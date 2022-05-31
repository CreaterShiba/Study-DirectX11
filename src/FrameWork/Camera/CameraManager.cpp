/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		CameraManager.cpp
	@brief		カメラマネージャー
	@author		shiba
	@date		2012/9/30 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "CameraManager.h"

#include "CameraBase.h"

//@todo 再考。一応ライブラリ層なんで
//げーむかめらはライブラリ層に移植するか？
#include "GameSystem/Camera/GameCamera.h"

#if defined( _DEBUG )
#include "FrameWork/Camera/DebugCamera.h"
#endif

NS_SINGLETON_INSTANCE_DEFINE( CCameraManager );

//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CCameraManager::CCameraManager() 
: m_eActiveCameraID( eCAMERAID_MAIN )
{
	m_paCamera.ALLSet(nullptr);
}

//---------------------------------------------------------------------------
/*!
	@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CCameraManager::~CCameraManager()
{
	for( s32 i = 0; i < eCAMERAID_MAX; i++)
	{
		SAFE_DELETE_TASK(m_paCamera.Get( i ) );
	}

	m_paCamera.ALLSet(nullptr);
}

//---------------------------------------------------------------------------
/*!
	@brief	初期化
*/
//---------------------------------------------------------------------------
bool CCameraManager::Init()
{
	//--------------------------------------------------------------------
	// 全てのカメラを作成しておく
	//--------------------------------------------------------------------
	for( s32 i = 0; i < eCAMERAID_MAX; i++)
	{
		CCameraBase* pcCamera = nullptr;
		switch( i )
		{
			case eCAMERAID_MAIN:
				CLASS_NEW( pcCamera, CGameCamera() );
				break;

#if defined( _DEBUG )
			case eCAMERAID_DEBUG:
				CLASS_NEW(pcCamera, CDebugCamera());
				break;
#endif

			default:
				CLASS_NEW( pcCamera, CCameraBase() );
				break;
		}

		NS_ASSERT( !m_paCamera.Get( i ) );
		m_paCamera.Set( i, pcCamera );
	}
	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	アクティブカメラ取得
*/
//---------------------------------------------------------------------------
CCameraBase* CCameraManager::GetActiveCamera()
{
	CCameraBase* pcCamera = m_paCamera.Get( m_eActiveCameraID );
	return pcCamera;
}

//---------------------------------------------------------------------------
/*!
	@brief	カメラデータ取得
*/
//---------------------------------------------------------------------------
CCameraBase* CCameraManager::GetCameraFromID( const CCameraManager::E_CAMERAID eCameraID )
{
	return m_paCamera.Get( eCameraID );
}

//---------------------------------------------------------------------------
/*!
	@brief	アクティブカメラ切り替え
*/
//---------------------------------------------------------------------------
void CCameraManager::SetActiveCameraID(const E_CAMERAID eCamID)
{
#if defined( _DEBUG )
	CCameraBase* pcCamera = GetCameraFromID(eCAMERAID_DEBUG);
	CDebugCamera* pcDebugCamera = NS_STATIC_CAST(CDebugCamera*, pcCamera);

	if (eCamID == eCAMERAID_DEBUG)
	{
		//デバッグカメラを開始する

		//元に戻すためカメラIDは保存しておく
		m_eDebugTempCameraID = GetActiveCameraID();

		CCameraBase* pcActiveCamera = GetActiveCamera();

		NSMatrix& activeViewMat = pcActiveCamera->GetView();
		NSMatrix& activeProjMat = pcActiveCamera->GetProj();

		pcDebugCamera->SetupDebugCamera(activeViewMat, activeProjMat, pcActiveCamera->GetAt());
		pcDebugCamera->SetTaskEnable(true);
	}
	else
	{
		if (m_eActiveCameraID == eCAMERAID_DEBUG)
		{
			m_eActiveCameraID = m_eDebugTempCameraID;
			pcDebugCamera->SetTaskEnable(false);
		}
	}
#endif

	m_eActiveCameraID = eCamID;
}

//---------------------------------------------------------------------------
/*!
	@brief	アクティブカメラID取得
*/
//---------------------------------------------------------------------------
CCameraManager::E_CAMERAID CCameraManager::GetActiveCameraID() const
{
	return m_eActiveCameraID;
}

//EOF