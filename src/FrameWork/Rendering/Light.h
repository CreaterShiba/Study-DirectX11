/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		Light.h
	@brief		ライトクラス
	@author		shiba
	@date		2012/7/23 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
#include "FrameWork/Task/Task.h"
#include "FrameWork/Utility/UtilCoord.h"

//==========================================================================*/
/*!
	@class	CLight
	@brief	ライト
*/
//==========================================================================*/
class CLight : public CTask , public CUtilCoord
{
	//コピー禁止
	void operator =(const CLight& src) { NS_UNUSED(src); }
	CLight(const CLight& src);

public:
	CLight();
	virtual ~CLight();

	//@brief 初期化
	virtual bool Init();
	virtual void Update(const f32 fDelta);
	virtual void Destroy();

	//@brief カラー指定
	void SetColor(const SColor& color)
	{
		m_cLightColor = color;
	}

	const SColor& GetColor() const
	{
		return m_cLightColor;
	}

	//@brief ライト強度
	void SetIntensity(const f32 fIntensity)
	{
		m_fIntensity = fIntensity;
	}
	const f32 GetIntensity() const
	{
		return m_fIntensity;
	}

	enum E_LIGHT_TYPE
	{
		eDIRECTIONAL,
		ePOINT,
		eSPOT,
		eAMBIENT,
		eAREA,
		eMAX
	};

	void SetLightType(const E_LIGHT_TYPE eType)
	{
		m_eLightType = eType;
	}

	E_LIGHT_TYPE GetLightType() const
	{
		return m_eLightType;
	}

	const NSVector3& GetLightDir() const
	{
		return m_vDir;
	}

	//@todo Directionalのみ、updateが起きたら演算する形にする
	NSMatrix GetLightView() const
	{
		NSVector3 vPos3 = GetPos();
		NSVector3 vAt3 = GetLightDir();
		NSVector3 vUp3 = NSVector3(0, 1, 0);
		NSVector4 vEye4 = DirectX::XMVectorSet(vPos3.x, vPos3.y, vPos3.z, 0.0f);	//カメラの位置
		NSVector4 vAt4 = DirectX::XMVectorSet(vAt3.x, vAt3.y, vAt3.z, 0.0f);		//焦点の位置
		NSVector4 vUp4 = DirectX::XMVectorSet(vUp3.x, vUp3.y, vUp3.z, 0.0f);		//UP
		return DirectX::XMMatrixLookAtLH(vEye4, vAt4, vUp4);
	}

private:
	SColor m_cLightColor;		//!< ライトカラー
	E_LIGHT_TYPE m_eLightType;	//!< ライトタイプ
	NSVector3 m_vDir;			//!< ライト向きベクトル
	f32 m_fIntensity;			//!< ライト強度
#if defined(_DEBUG)
	//!< 可視テスト要ジオメトリ
	CDebugGeometry* m_pcDebugGeom;
#endif

private: 
	void Term();
};

//EOF
