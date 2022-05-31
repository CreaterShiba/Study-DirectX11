#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		CameraManager.h
	@brief		カメラマネージャ
	@author		shiba
	@date		2012/3/4 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "FrameWork/Utility/UtilSafeArray.h"

class CCameraBase;
/*==========================================================================*/
/*!
	@class	CCameraManager
	@brief	カメラマネージャクラス
*/
/*==========================================================================*/
class CCameraManager
{
	//シングルトン
	NS_SINGLETON( CCameraManager );
	CCameraManager();
	~CCameraManager();

public:

	//カメラID
	enum E_CAMERAID
	{
		eCAMERAID_NONE,
		eCAMERAID_MAIN,		//メインカメラ

#if defined( _DEBUG )
		eCAMERAID_DEBUG,	//デバッグカメラ用ID
#endif

		eCAMERAID_MAX,
		eCAMERAID_NULL = -1	//
	};

	//@brief 初期化
	bool Init();

	//@brief アクティブ(レンダリング)カメラ取得
	CCameraBase* GetActiveCamera();
	//@brief カメラをID取得
	CCameraBase* GetCameraFromID( const CCameraManager::E_CAMERAID eCameraID );

	//@brief アクティブカメラ切り替え
	void SetActiveCameraID(const E_CAMERAID eCamID);
	//@brief アクティブカメラID取得
	CCameraManager::E_CAMERAID GetActiveCameraID() const;

private:
	CSafeArray< CCameraBase*, eCAMERAID_MAX > m_paCamera;		//!< カメラデータ配列
	E_CAMERAID m_eActiveCameraID;								//!< アクティブなカメラID

#if defined( _DEBUG )
private:
	E_CAMERAID m_eDebugTempCameraID;							//!< デバッグカメラ切り替え保存用
#endif
};

//EOF
