#pragma once
/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		CollisionGeometry.h
	@brief		コリジョンジオメトリ
	@author		shiba
	@date		2014/07/28 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/

#include "FrameWork/Utility/UtilCoord.h"

//コリジョン用ジオメトリ
enum E_COL_GEOM_TYPE
{
	eCOL_GEOM_SPHERE,
	eCOL_GEOM_AABB,
	eCOL_GEOM_MAX,
	eCOL_GEOM_NONE = -1
};

//タイプ分け(ゲーム部分なので分離予定)
enum E_COL_OBJECT_TYPE
{
	eCOL_OBJECT_NONE,
	eCOL_OBJECT_STAGE,
	eCOL_OBJECT_MAX,
};

class CCollisionGeomBase
{
public:
	CCollisionGeomBase();
	virtual ~CCollisionGeomBase();

	bool Init()
	{
		return true;
	}
	virtual void Destroy(){}

	//@brief ジオメトリタイプの設定
	void SetColGeomType(const E_COL_GEOM_TYPE eColGeomType)
	{
		m_eColGeomType = eColGeomType;

#if defined(_DEBUG)
		//デバッグジオメトリ更新
		SAFE_DELETE(m_pDebugGeom);
#endif
	}

	//@brief ジオメトリタイプの取得
	const E_COL_GEOM_TYPE GetColGeomType() const
	{
		return m_eColGeomType;
	}

	//@brief オブジェクトタイプの設定
	void SetColObjectType(const E_COL_OBJECT_TYPE eColObjectType)
	{
		m_eColObjectType = eColObjectType;
	}

	//@brief オブジェクトタイプの取得
	const E_COL_OBJECT_TYPE GetColObjectType() const
	{
		return m_eColObjectType;
	}

	//@brief 追尾対象設定
	void SetTrailCoord(const CUtilCoord* pcCoord)
	{
		m_pTrailCoord = pcCoord;
	}

	//@brief スケール値設定
	virtual void SetScale(const f32 fScale)
	{
		m_fScale = fScale;
	}

	//@brief スケール値取得
	const f32 GetScale()
	{
		return m_fScale;
	}

	//@brief 座標設定
	void SetPos(const NSVector3& vPos)
	{
		m_cWorldMat = DirectX::XMMatrixTranslation(vPos.x, vPos.y, vPos.z);

#if defined(_DEBUG)
		if (m_pDebugGeom != nullptr)
		{
			m_pDebugGeom->SetPos(vPos);
		}
#endif
	}

	//@brief 座標取得
	NSVector3 GetPos() const
	{
		NSVector3 vPos;
		if (m_pTrailCoord != nullptr)
		{
			vPos = m_pTrailCoord->GetPos();
		}
		else
		{
			vPos = NSVector3(m_cWorldMat._41, m_cWorldMat._42, m_cWorldMat._43);
		}
		return vPos;
	}

	//@brief ワールドマトリクス取得
	const NSMatrix& GetMatrix() const
	{
		return m_cWorldMat;
	}


#if defined(_DEBUG)
	virtual void SetDrawFlagDebugGeom(const bool bDraw)
	{
		NS_UNUSED(bDraw);
	}
#endif

	//@brife コリジョン有効/無効取得、設定
	void SetCollisionEnable(const bool bEnable)
	{
		m_bColEnable = bEnable;
	}
	bool IsCollisionEnable() const
	{
		return m_bColEnable;
	}

protected:
	E_COL_GEOM_TYPE m_eColGeomType;		//!< ジオメトリ種類
	E_COL_OBJECT_TYPE m_eColObjectType;	//!< コリジョンのタイプ

	const CUtilCoord* m_pTrailCoord;	//!< 追従コリジョン対象トランスフォーム	
	f32 m_fScale;						//!< コリジョンスケール値
	NSMatrix m_cWorldMat;				//!< ワールド行列
	bool m_bColEnable;					//!< コリジョン判定有効 / 無効

	s32 m_nParam;						//!< 汎用パラメータ
#if defined(_DEBUG)
	//!< 視覚確認用のデバッグジオメトリ
	CDebugGeometry* m_pDebugGeom;
#endif

};

//スフィア
class CCollisionSphere : public CCollisionGeomBase
{
public:
	CCollisionSphere()
	{
		m_eColGeomType = E_COL_GEOM_TYPE::eCOL_GEOM_SPHERE;
	}
	~CCollisionSphere() {}
};

//AABB
class CCollisionAABB : public CCollisionGeomBase
{
public:
	CCollisionAABB()
	{
		m_eColGeomType = E_COL_GEOM_TYPE::eCOL_GEOM_AABB;
	}
	~CCollisionAABB() {}

#if defined(_DEBUG)
	void SetDrawFlagDebugGeom(const bool bDraw)
	{
		if (m_pDebugGeom == nullptr)
		{
			CLASS_NEW(m_pDebugGeom, CDebugGeometry);
			m_pDebugGeom->SetupGeom(CDebugGeometry::eCUBE);
		}

		if (m_pDebugGeom != nullptr)
		{
			m_pDebugGeom->SetDrawEnable(bDraw, CRenderingManager::E_RENDER_STAGE_MRT_POS_NOM_DIF);
		}
	}
#endif

	//AABBを構成する８頂点を返す。
	bool GetAABBVertex(NSVector3* paPos)
	{
		if (paPos == nullptr)
		{
			return false;
		}

		//8頂点
		paPos[0] = min;								//左下奥
		paPos[1] = NSVector3(min.x, min.y, max.z);	//左下手前
		paPos[2] = NSVector3(min.x, max.y, min.z);	//左上奥
		paPos[3] = NSVector3(min.x, max.y, max.z);	//左上手前

		paPos[4] = NSVector3(max.x, min.y, min.z);	//右下奥
		paPos[5] = NSVector3(max.x, min.y, max.z);	//右下手前
		paPos[6] = NSVector3(max.x, max.y, min.z);	//右上奥
		paPos[7] = max;								//右上手前

		return true;
	}

	//AABBを構成する８頂点を返す（ワールド系）
	bool GetAABBVertexWorld(NSVector3* paPos)
	{
		if (paPos == nullptr)
		{
			return false;
		}

		NSVector3 aVec3[8];
		if (GetAABBVertex(aVec3))
		{
			NSVector4 aVec4[8];
			for (s32 i = 0; i < 8; ++i)
			{
				aVec4[i] = XMVector3TransformCoord( DirectX::XMVectorSet(aVec3[i].x, aVec3[i].y, aVec3[i].z, 1.0f), m_cWorldMat);
				paPos[i] = NSVector3(XMVectorGetX(aVec4[i]), XMVectorGetY(aVec4[i]), XMVectorGetZ(aVec4[i]));
			}
		}

		return true;
	}

	//@brief スケール設定
	virtual void SetScale(const f32 fScale)
	{
		CCollisionGeomBase::SetScale(fScale);
		max = max * m_fScale;
		min = min * m_fScale;
	}
	//@brief AABBサイズ設定
	void SetMax(NSVector3 vmax)
	{
		vmax = vmax * m_fScale;
		max = vmax;
	}
	//@brief AABBサイズ取得
	const NSVector3& GetMax() const
	{
		return max;
	}
	//@brief AABBサイズ設定
	void SetMin(NSVector3 vmin)
	{
		vmin = vmin * m_fScale;
		min = vmin;
	}
	//@brief AABBサイズ取得
	const NSVector3& GetMin() const
	{
		return min;
	}

private:

	NSVector3 max;
	NSVector3 min;
};

//レイ
class CCollisionRay : public CCollisionGeomBase
{
public:
	CCollisionRay()
		: m_fRayLength(0.0f)
	{
		m_vRayDir.SetZero();
	}
	~CCollisionRay() {}

	virtual void SetScale(const f32 fScale)
	{
		CCollisionGeomBase::SetScale(fScale);
		m_fRayLength *= fScale;
	}

	f32 m_fRayLength;
	NSVector3 m_vRayDir;
};

//三角形
class CCollisionTriangle : public CCollisionGeomBase
{
public:
	CCollisionTriangle()
	{
		m_vPoint1.SetZero();
		m_vPoint2.SetZero();
		m_vPoint3.SetZero();
	}
	NSVector3 m_vPoint1;
	NSVector3 m_vPoint2;
	NSVector3 m_vPoint3;
};

//EOF
