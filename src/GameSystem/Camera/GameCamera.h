#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		GameCamera.h
	@brief		ゲームカメラ
	@author		shiba
	@date		2013/2/16 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "FrameWork/Camera/CameraBase.h"

/*==========================================================================*/
/*!
	@class	CGameCamera
	@brief	ゲームカメラ
*/
/*==========================================================================*/
class CUtilCoord;
class CGameCamera : public CCameraBase
{
public:
	CGameCamera();
	~CGameCamera();

	virtual bool Init();
	virtual void Update( const f32 fDelta );
	void SetTarget(CUtilCoord* pCoord);

private:
	f32 m_fCameraMoveDir;
	f32 m_fCameraZoom;

	const f32 CAMERA_LENGTH = 50.0f;
	CUtilCoord* m_pTargetUnit;	//!< 追尾先ターゲット

private:
	void UpdateCamera();
};

//EOF
