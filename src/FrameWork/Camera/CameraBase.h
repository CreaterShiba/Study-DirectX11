#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		CameraBase.h
	@brief		カメラ基底クラス
	@author		shiba
	@date		2012/9/9 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#include "CameraManager.h"
#include "FrameWork/Task/Task.h"
#include "FrameWork/DxUtil/DXMng.h"

class CCameraManager;
/*==========================================================================*/
/*!
	@class	CCameraBase
	@brief	カメラ規定クラス
*/
/*==========================================================================*/
class CCameraBase : public CTask
{
public:
	CCameraBase();
	virtual ~CCameraBase();

	virtual bool Init();
	virtual void Update( const f32 fDelta );
	virtual void Destroy();

public:

	//@brief カメラ座標設定
	void SetCameraPos( const NSVector3& vPos );
	void AddCameraPos( const NSVector3& vAddPos );
	//@brief カメラの注視点
	void SetCameraAtPos(const NSVector3& vAtPos);

	//@brief カメラ座標を取得
	const NSVector3& GetCameraPos() const;

	//@brief カメラ行列取得
	inline NSMatrix& GetView();
	inline const NSMatrix& GetViewConst() const;

	//@brief 射影行列取得
	inline NSMatrix& GetProj();
	inline const NSMatrix& GetProjConst() const;

	//@brief ビューポートの取得
	inline const D3D11_VIEWPORT& GetViewport() const;

	//@brief 注視点取得
	inline const NSVector3& GetAt() const;

	// @brief スクリーン座標からワールド座標を算出する
	NSVector3 ScreenToWorldPos( const NSVector2& vScreen ) const;

	// @brief ワールド座標からスクリーン座標を算出する
	NSVector2 WorldToScreenPos( const NSVector3& vScreen ) const;

	//@ brief カメラのX軸ベクトルの取得
	bool GetCameraXDir( NSVector3& vVec ) const;

	//@ brief カメラのZ軸ベクトルの取得
	bool GetCameraZDir( NSVector3& vVec ) const;

protected:
	//!< カメラユニークID
	CCameraManager::E_CAMERAID m_eCameraID;	

	//!< ビュー行列
	NSMatrix m_View;						

	//!< プロジェクション行列
	NSMatrix m_Proj;						

	//!< ビューポート
	D3D11_VIEWPORT m_Viewport;				

	//!< 座標
	NSVector3 m_vCameraPos;	

	//!< 注視点
	NSVector3 m_vAtPos;

	//!< 上を示すベクトル
	NSVector3 m_vUpPos;		

	//!< カメラ座標系
	NSMatrix m_camPoseMat;	
};

//--------------------------------------------------------------------
// inline
//--------------------------------------------------------------------
inline NSMatrix& CCameraBase::GetView() 
{
	return m_View;
}
inline const NSMatrix& CCameraBase::GetViewConst() const
{
	return m_View;
}

inline NSMatrix& CCameraBase::GetProj()
{
	return m_Proj;
}
inline const NSMatrix& CCameraBase::GetProjConst() const
{
	return m_Proj;
}

inline const D3D11_VIEWPORT& CCameraBase::GetViewport() const
{
	return m_Viewport;
}

inline const NSVector3& CCameraBase::GetAt() const
{
	return m_vAtPos;
}

//EOF

