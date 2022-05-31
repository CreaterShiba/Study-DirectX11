#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		CCollisionManagerSys.h
	@brief		コリジョン管理(ライブラリ部分)
	@author		shiba
	@date		2014/07/28 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
#include "CollisionGeometry.h"

class CCollisionManagerSys
{
public:
	CCollisionManagerSys();
	virtual ~CCollisionManagerSys();

	virtual bool Init();
	virtual void Destroy();

	//@brief コリジョンリスト追加
	void AddCollision(CCollisionGeomBase* pCollision);

	//@brief コリジョンリストのチェックを行う
	virtual void CheckAllCollision();

	//@brief コリジョンチェック
	static bool IsIntersect(CCollisionGeomBase* pCol1, CCollisionGeomBase* pcCol1);

protected:
	bool CheckRay(
		NSVector3& vPos,
		NSVector3& vAt,
		CCollisionAABB& colAABB,
		f32& fT,
		NSVector3* pHitNormal,
		NSVector3* pHitPos = 0);

	//@brief 点がAABBの中に含まれてるかチェック
	bool IsContainPointInAABB(const NSVector3& point, const CCollisionAABB& aabb);

private:
	void Term();

protected:
	//!< コリジョンリスト
	std::vector< CCollisionGeomBase* > m_pCollisionList;
	const s32 COLLISION_MAX = 256;

};

//EOF
