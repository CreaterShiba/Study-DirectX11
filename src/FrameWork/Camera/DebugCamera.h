#pragma once

#if defined( _DEBUG )

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		DebugCamera.h
	@brief		操作可能デバッグカメラ
	@author		shiba
	@date		2012/9/30 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "FrameWork/Camera/CameraBase.h"

/*==========================================================================*/
/*!
	@class	CDebugCamera
	@brief	デバッグカメラ
*/
/*==========================================================================*/
class CDebugCamera : public CCameraBase
{
public:
	CDebugCamera();
	~CDebugCamera();

	//@brief 初期化
	virtual bool Init();
	//@brief 更新
	virtual void Update( const f32 fDelta );
	//@brief デバッグカメラ用ビュー行列セットアップ
	void SetupDebugCamera( const NSMatrix& viewMat, const NSMatrix& projMat,  const NSVector3& vAt );

private:
	//@brief カメラの姿勢行列更新
	void updateCameraPose();

private:
	NSVector3 m_camPosL;			//!< 注視点空間でのカメラの位置
};

#endif // defined( _DEBUG )

//EOF
