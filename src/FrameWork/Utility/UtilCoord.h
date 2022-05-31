#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		UtilCoord.h
	@brief		座標系操作インターフェイス
	@author		shiba
	@date		2012/9/9 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

//==========================================================================*/
/*!
	@class	CUtilCoord
	@brief	座標系操作インターフェイス
*/
//==========================================================================*/
class CUtilCoord
{
public:
	CUtilCoord() :
		m_vPos(NSVector3(0.0f, 0.0f, 0.0f)),
		m_vRotate(NSVector3(0.0f, 0.0f, 0.0f)),
		m_vScale(NSVector3(1.0f, 1.0f, 1.0f)),
		m_bUpdate(true)
	{};
	virtual ~CUtilCoord(){};

private:
	//座標
	NSVector3 m_vPos;
	
	//回転
	NSVector3 m_vRotate;
	
	//スケール
	NSVector3 m_vScale;
	
protected:
	//チェンジフラグ。必要なら継承先クラスでマトリクス更新など
	bool m_bUpdate;

public:
	// @brief 3D座標設定
	void SetPos(const NSVector3& vPos)
	{
		SetPos(vPos.x, vPos.y, vPos.z);
	}

	void SetPos(f32 x, f32 y, f32 z)
	{
		m_vPos.x = x;
		m_vPos.y = y;
		m_vPos.z = z;
		m_bUpdate = true;
	}

	// @brief 2D座標設定
	void Set2DPos( const NSVector2& vPos )
	{
		m_vPos = NSVector3(vPos.x, vPos.y, 0.0f);
		m_bUpdate = true;
	}

	// @brief 3D座標獲得
	const NSVector3& GetPos() const
	{
		return m_vPos;
	}

	// @brief Xの方向ベクトルを返す
	NSVector3 GetXAt()
	{
		//Z方向のベクトルを返す
		NSMatrix matRot = DirectX::XMMatrixRotationRollPitchYaw(
			m_vRotate.x * UtilMath::DegToRad,
			m_vRotate.y * UtilMath::DegToRad,
			m_vRotate.z * UtilMath::DegToRad);
		NSVector3 vZAt = NSVector3(matRot._11, matRot._12, matRot._13);
		vZAt.Normalize();

		return vZAt;
	}


	// @brief Yの方向ベクトルを返す
	NSVector3 GetYAt()
	{
		//Z方向のベクトルを返す
		NSMatrix matRot = DirectX::XMMatrixRotationRollPitchYaw(
			m_vRotate.x * UtilMath::DegToRad,
			m_vRotate.y * UtilMath::DegToRad,
			m_vRotate.z * UtilMath::DegToRad);
		NSVector3 vZAt = NSVector3(matRot._21, matRot._22, matRot._23);
		vZAt.Normalize();

		return vZAt;
	}


	// @brief Zの方向ベクトルを返す
	NSVector3 GetZAt()
	{
		//Z方向のベクトルを返す
		NSMatrix matRot = DirectX::XMMatrixRotationRollPitchYaw(
			m_vRotate.x * UtilMath::DegToRad, 
			m_vRotate.y * UtilMath::DegToRad, 
			m_vRotate.z * UtilMath::DegToRad);
		NSVector3 vZAt = NSVector3(matRot._31, matRot._32, matRot._33);
		vZAt.Normalize();

		return vZAt;
	}

	void AddPos(const NSVector3& vPos)
	{
		m_vPos += vPos;
		m_bUpdate = true;
	}

	// @brief 2D座標獲得
	const NSVector2 Get2DPos()
	{
		return NSVector2( m_vPos.x, m_vPos.y );
	}

	// @brief 3軸オイラー回転設定
	void SetRotate( const NSVector3& vRotate )
	{
		m_vRotate = vRotate;
		m_bUpdate = true;
	}
	void SetRotate(f32 x, f32 y, f32 z)
	{
		m_vRotate = NSVector3(x, y, z);
		m_bUpdate = true;
	}

	// @brief X軸追加
	void AddRotateX(const f32 nAddDegree)
	{
		m_vRotate.x += nAddDegree;
		m_bUpdate = true;
	}

	// @brief Y軸追加
	void AddRotateY(const f32 nAddDegree)
	{
		m_vRotate.y += nAddDegree;
		m_bUpdate = true;
	}

	// @brief Z軸追加
	void AddRotateZ(const f32 nAddDegree)
	{
		m_vRotate.z += nAddDegree;
		m_bUpdate = true;
	}

	// @brief 3軸オイラー回転獲得
	const NSVector3& GetRotate()
	{
		return m_vRotate;
	}

	// @brief 3軸スケール値設定
	void SetScale( const NSVector3& vScale )
	{
		m_vScale = vScale;
		m_bUpdate = true;
	}

	void SetScale(const f32 fScale)
	{
		m_vScale.x = fScale;
		m_vScale.y = fScale;
		m_vScale.z = fScale;
		m_bUpdate = true;
	}

	// @brief 3軸スケール値獲得
	const NSVector3& GetScale()
	{
		return m_vScale;
	}

	// @brief 比率でスケール値増加
	void AddScale( const f32 fAddScale )
	{
		m_vScale.x = m_vScale.x * fAddScale;
		m_vScale.y = m_vScale.y * fAddScale;
		m_vScale.z = m_vScale.z * fAddScale;
		m_bUpdate = true;
	}
};

//EOF