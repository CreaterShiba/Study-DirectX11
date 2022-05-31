#include "CollisionManagerSys.h"

using namespace DirectX;

CCollisionManagerSys::CCollisionManagerSys() 
{
	m_pCollisionList.clear();
	m_pCollisionList.reserve(COLLISION_MAX);
}

CCollisionManagerSys::~CCollisionManagerSys()
{
	Term();
}

bool CCollisionManagerSys::Init()
{
	return true;
}

void CCollisionManagerSys::Destroy()
{
	Term();
}

void CCollisionManagerSys::AddCollision(CCollisionGeomBase* pCollision)
{
	m_pCollisionList.push_back(pCollision);
}

void CCollisionManagerSys::CheckAllCollision()
{
	//リストに登録されてるコリジョンチェックして通知

	//当たったやつも渡す

	//プレイヤーとステージの当たり判定
}

//AABBと位置の長さ演算
f32 SquaredDistPointAABB(const NSVector3 & p, CCollisionAABB & aabb)
{
	//ラムダ
	auto check = [&](
		const f32 pn,
		const f32 bmin,
		const f32 bmax) -> f32
	{
		f32 out = 0;
		f32 v = pn;

		if (v < bmin)
		{
			f32 val = (bmin - v);
			out += val * val;
		}

		if (v > bmax)
		{
			f32 val = (v - bmax);
			out += val * val;
		}

		return out;
	};

	// Squared distance
	f32 sq = 0.0;

	const NSVector3& min = aabb.GetMin();
	const NSVector3& max = aabb.GetMax();

	sq += check(p.x, min.x, max.x);
	sq += check(p.y, min.y, max.y);
	sq += check(p.z, min.z, max.z);

	return sq;
}

bool CCollisionManagerSys::IsIntersect(CCollisionGeomBase* pcCol1, CCollisionGeomBase* pcCol2)
{

	if (pcCol1 == nullptr || pcCol2 == nullptr)
	{
		return false;
	}

	if (pcCol1->IsCollisionEnable() == false || pcCol2->IsCollisionEnable() == false)
	{
		return false;
	}	
	
	bool bIntersect = false;

	if (pcCol1->GetColGeomType() == E_COL_GEOM_TYPE::eCOL_GEOM_SPHERE &&
		pcCol2->GetColGeomType() == E_COL_GEOM_TYPE::eCOL_GEOM_SPHERE)
	{
		//Sphere*Sphere
		CCollisionSphere* pcSphere1 = static_cast<CCollisionSphere*>(pcCol1);
		CCollisionSphere* pcSphere2 = static_cast<CCollisionSphere*>(pcCol2);

		//判定
		const NSVector3& vPos1 = pcSphere1->GetPos();
		const NSVector3& vPos2 = pcSphere2->GetPos();

		if ((vPos2.x - vPos1.x) * (vPos2.x - vPos1.x) +
			(vPos2.y - vPos1.y) * (vPos2.y - vPos1.y) +
			(vPos2.z - vPos1.z) * (vPos2.z - vPos1.z) <=
			(pcSphere1->GetScale() + pcSphere2->GetScale()) * (pcSphere1->GetScale() + pcSphere2->GetScale()))
		{
			//あたり
			bIntersect = true;
		}
	}
	else if (pcCol1->GetColGeomType() == E_COL_GEOM_TYPE::eCOL_GEOM_SPHERE &&
		pcCol2->GetColGeomType() == E_COL_GEOM_TYPE::eCOL_GEOM_AABB)
	{
		//Sphere*AABB
		CCollisionSphere* pcSphere1 = static_cast<CCollisionSphere*>(pcCol1);
		CCollisionAABB* pcAABB1 = static_cast<CCollisionAABB*>(pcCol2);

		f32 fSquaredDistance = SquaredDistPointAABB(pcSphere1->GetPos(), *pcAABB1);
		bIntersect = fSquaredDistance <= (pcSphere1->GetScale() * pcSphere1->GetScale());
	}
	else if (
		pcCol1->GetColGeomType() == E_COL_GEOM_TYPE::eCOL_GEOM_AABB &&
		pcCol2->GetColGeomType() == E_COL_GEOM_TYPE::eCOL_GEOM_AABB)
	{
		//AABB*AABB
		//CCollisionAABB* pcAABB1 = static_cast<CCollisionAABB*>(pcCol1);
		//CCollisionAABB* pcAABB2 = static_cast<CCollisionAABB*>(pcCol2);
	}
	return bIntersect;
}


//---------------------------------------------------------------------------
/*!
	@brief		レイとAABBの交差判定
	@param[in]	vPos		ワールド座標でのレイの視点
	@oaram[in]	vAt			ワールド空間でのレイの向き
	@param[in]	colAABB		境界ボックス（ローカル座標系）
	@param[out] fT			衝突点との距離
	@param[out] pHitNormal	衝突位置法線(出力)
	@param[out] pHitPos		衝突位置(出力)
	@return		レイがAABBにヒットした時点でTrueが
*/
//---------------------------------------------------------------------------
bool CCollisionManagerSys::CheckRay(NSVector3& vPos, NSVector3& vAt, CCollisionAABB& colAABB, f32& fT, NSVector3* pHitNormal, NSVector3* pHitPos /* = 0 */)
{
	// 光線を境界ボックスの空間へ移動
	NSMatrix invMat;
	NSVector4 Determinant;
	invMat = XMMatrixInverse(&Determinant, colAABB.GetMatrix());
	
	// 座標と方向を、AABBボックスの座標系に引き込む形
	NSVector4 p_l4, dir_l4;
	p_l4 = XMVector3TransformCoord(XMVectorSet(vPos.x, vPos.y, vPos.z, 1.0f), invMat);
	invMat._41 = 0.0f;
	invMat._42 = 0.0f;
	invMat._43 = 0.0f;
	dir_l4 = XMVector3TransformCoord(XMVectorSet(vAt.x, vAt.y, vAt.z, 1.0f), invMat);

	NSVector3 p_l, dir_l;
	p_l = NSVector3(XMVectorGetX(p_l4), XMVectorGetY(p_l4), XMVectorGetZ(p_l4));
	dir_l = NSVector3(XMVectorGetX(dir_l4), XMVectorGetY(dir_l4), XMVectorGetZ(dir_l4));

	// 交差判定
	f32 p[3], d[3], min[3], max[3];
	memcpy(p, &p_l, sizeof(NSVector3));
	memcpy(d, &dir_l, sizeof(NSVector3));
	memcpy(min, &colAABB.GetMin(), sizeof(NSVector3));
	memcpy(max, &colAABB.GetMax(), sizeof(NSVector3));

	fT = -FLT_MAX;
	f32 t_max = FLT_MAX;

	//3つのスラブにたいしての判定(X,Y,Z)
	for (int i = 0; i < 3; ++i) 
	{
		if (abs(d[i]) < FLT_EPSILON)
		{
			//スラブの軸に対して並行なら、iの軸に関しては交差することはない
			//minやmaxにマイナスが入ることを想定していない
			if (p[i] < min[i] || p[i] > max[i])
				//スラブ範囲から光線原点がはみでてるんで、交差はない
				return false; // 交差していない
		}
		else 
		{
			// 光線原点とスラブとの距離を算出
			// t1が近スラブ、t2が遠スラブとの距離
			f32 ood = 1.0f / d[i];	//正規化
			f32 t1 = (min[i] - p[i]) * ood;
			f32 t2 = (max[i] - p[i]) * ood;

			if (t1 < 0.0f && t2 < 0.0f)
			{
				//向きも考慮しておく
				//いるか？
				return false;
			}

			if (t1 > t2) 
			{
				//計算しやすくするためSwap
				f32 tmp = t1;
				t1 = t2; 
				t2 = tmp;
			}

			if (t1 > fT) 
				fT = t1;
			if (t2 < t_max) 
				t_max = t2;

			// スラブ交差チェック
			if (fT >= t_max)
				//スラブに交差がないため、即リターン
				return false;
		}
	}

	// 交差している
	NSVector3 vHitPos;
	vAt.x *= fT;
	vAt.y *= fT;
	vAt.z *= fT;
	vHitPos = vPos + vAt;

	if (fT <= 0.0f)
	{
		//衝突した場合のみ
		//衝突点から法線を出す

		//AABB8点の直近点を２つ出す。
		//衝突点と得た頂点２つで法線を出す
		//AABBを構成する８頂点のXZと比較
		//minのxzとmaxのxzを比較すればいけそう

		NSVector3 boxmin = colAABB.GetMin();
		NSVector4 minworld = XMVector3TransformCoord(XMVectorSet(boxmin.x, boxmin.y, boxmin.z, 1.0f), colAABB.GetMatrix());

		NSVector3 boxmax = colAABB.GetMax();
		NSVector4 maxworld = XMVector3TransformCoord(XMVectorSet(boxmax.x, boxmax.y, boxmax.z, 1.0f), colAABB.GetMatrix());

		s32 nNearestPointIdx = -1;
		//0,1,2,3のポイント
		//左 = 0, 手前 = 1, 右 = 2, 奥 = 3とする
		f32 fLength = 100000.0f;
		if (UtilMath::Abs(vHitPos.x - XMVectorGetX(minworld)) < fLength)
		{
			nNearestPointIdx = 0;
			fLength = UtilMath::Abs(vHitPos.x - XMVectorGetX(minworld));
		}
		if (UtilMath::Abs(vHitPos.z - XMVectorGetZ(minworld)) < fLength)
		{
			nNearestPointIdx = 1;
			fLength = UtilMath::Abs(vHitPos.z - XMVectorGetZ(minworld));
		}
		if (UtilMath::Abs(vHitPos.x - XMVectorGetX(maxworld)) < fLength)
		{
			nNearestPointIdx = 2;
			fLength = UtilMath::Abs(vHitPos.x - XMVectorGetX(maxworld));
		}
		if (UtilMath::Abs(vHitPos.z - XMVectorGetZ(maxworld)) < fLength)
		{
			nNearestPointIdx = 3;
			fLength = UtilMath::Abs(vHitPos.z - XMVectorGetZ(maxworld));
		}

		//もっとも近い、衝突した面がわかったので、面の頂点から法線をだす
		NSVector3 vert1, vert2;
		NSVector3 aVert[8];
		if (colAABB.GetAABBVertexWorld(aVert))
		{
			if (nNearestPointIdx == 0)
			{
				vert1 = aVert[3];
				vert2 = aVert[0];
			}
			if (nNearestPointIdx == 1)
			{
				vert1 = aVert[0];
				vert2 = aVert[6];
			}
			if (nNearestPointIdx == 2)
			{
				vert1 = aVert[4];
				vert2 = aVert[7];
			}
			if (nNearestPointIdx == 3)
			{
				vert1 = aVert[7];
				vert2 = aVert[1];
			}

			if (pHitNormal)
			{
				*pHitNormal = UtilMath::CalcNormal(vert1, vert2, vHitPos);
			}
		}
	}

	if (pHitPos)
	{
		*pHitPos = vHitPos;
	}

	return true;
}

//点がAABBの中に含まれてるかチェック
bool CCollisionManagerSys::IsContainPointInAABB(const NSVector3& point, const CCollisionAABB& aabb)
{
	NSMatrix invMat;
	NSVector4 Determinant;
	invMat = XMMatrixInverse(&Determinant, aabb.GetMatrix());

	NSVector4 p_l4;
	p_l4 = XMVector3TransformCoord(XMVectorSet(point.x, point.y, point.z, 1.0f), invMat);

	const NSVector3& max = aabb.GetMax();
	const NSVector3& min = aabb.GetMin();

	if (XMVectorGetX(p_l4) > max.x || XMVectorGetX(p_l4) < min.x)
	{
		return false;
	}
	if (XMVectorGetY(p_l4) > max.y || XMVectorGetY(p_l4) < min.y)
	{
		return false;
	}
	if (XMVectorGetZ(p_l4) > max.z || XMVectorGetZ(p_l4) < min.z)
	{
		return false;
	}

	return true;
}

void CCollisionManagerSys::Term()
{
	m_pCollisionList.clear();
}

//EOF

