#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		AssetModel.h
	@brief		モデルアセットクラス
	@author		shiba
	@date		2013/4/28 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
#include "AssetBase.h"

#include "AssetMesh.h"

#include "FrameWork/Rendering/Material.h"
#include "FrameWork/Rendering/ModelDataType.h"

class CAssetMesh;
class CMotion;
class CJoint;

class CAssetModel : public CAssetBase
{
	//コピー禁止
	void operator =(const CAssetModel& src) { NS_UNUSED(src); }
	CAssetModel(const CAssetModel& src);

public:
	CAssetModel();
	virtual ~CAssetModel();
	
	//モデルコピー
	bool Copy(CAssetModel* pcCopyModel);

	//アセットの登録
	bool EntryAllMeshAsset();

	//ボーンアニメーションが使われているか？
	bool IsSkinMesh(const u32 unMeshID) const
	{
		if (unMeshID >= 0 && m_paMesh.size() > unMeshID)
		{
			return m_paMesh[unMeshID]->IsSkinMesh();
		}

		return false;
	}
	bool IsExistSkinMesh() const
	{
		bool bExistSkin = false;
		for (u32 i = 0; i < m_paMesh.size(); ++i)
		{
			if (m_paMesh[i]->IsSkinMesh())
			{
				bExistSkin = true;
				break;
			}
		}
		return bExistSkin;
	}
	// @brief メッシュデータ
	void ClearMesh();
	void ReserveMesh(const s32 nMeshNum)
	{
		m_paMesh.reserve(nMeshNum);
	}
	void AddMesh(CAssetMesh* pcMesh)
	{
		m_paMesh.push_back(pcMesh);
	}
	CAssetMesh* GetMesh(const u32 unIndex)
	{
		if (unIndex >= 0 && unIndex < m_paMesh.size())
		{
			return m_paMesh[unIndex];
		}
		return nullptr;
	}

	// @brief マテリアルデータ
	void ClearMaterial();
	void ReserveMaterial(const s32 nMatNum)
	{
		m_paMaterial.reserve(nMatNum);
	}
	void AddMaterial(CMaterial* pcMat)
	{
		m_paMaterial.push_back(pcMat);
	}
	CMaterial* GetMaterial(const u32 unIndex)
	{
		if (unIndex >= 0 && unIndex < m_paMaterial.size())
		{
			return m_paMaterial[unIndex];
		}
		return nullptr;
	}
	//CMaterial* GetMaterialBeginAddress()
	//{
	//	return &m_paMaterial[0];
	//}
	s32 GetMaterialNum() const
	{
		return m_paMaterial.size();
	}

	// @brief メッシュ数
	u32 GetMeshNum()
	{
		return m_paMesh.size();
	}

	// @brief モーション
	void AddMotionData(CMotion *pcMotionData)
	{
		m_pcMotionList.push_back(pcMotionData);
	}
	CMotion* GetMotionData( const u32 unIndex ) const
	{
		CMotion* pcMotion = nullptr;
		if (unIndex >= 0 && unIndex < m_pcMotionList.size())
		{
			pcMotion = m_pcMotionList[unIndex];
		}

		return pcMotion;
	}
	s32 GetMotionDataNum()
	{
		return m_pcMotionList.size();
	}

	//@brief ジョイント
	void SetRootJointID(const u32 unRootID)
	{
		NS_ASSERT(m_unRootJointID == INVALID_U32);
		m_unRootJointID = unRootID;
	}
	// @brief ルートジョイント獲得
	CJoint* GetRootJoint() const
	{
		if (m_unRootJointID != INVALID_U32)
		{
			return m_paJoint[m_unRootJointID];
		}
		else
		{
			return nullptr;
		}
	}

	void AddJoint(CJoint* pcJoint)
	{
		m_paJoint.push_back(pcJoint);
	}

	// @brief ジョイント取得
	CJoint* GetJoint(const u32 unJointIdx);
	CJoint* GetJoint(const TCHAR* strJointName);

	// @brief 全ジョイント数
	u32 GetJointNum() const
	{
		return m_paJoint.size();
	}

private:
	//!< 複数のメッシュを持つ。
	std::vector< class CAssetMesh* > m_paMesh;

	//!< マテリアルデータ
	std::vector< class CMaterial* > m_paMaterial;

	//!< モデルの設定データ
	SModelData m_sModelData;

	//!< モーションリスト
	std::vector<class CMotion*> m_pcMotionList;

	// ----- 以下スキン情報
	//!< モデルジョイント
	std::vector< class CJoint* > m_paJoint;

	//!< ルートID
	u32 m_unRootJointID;

private: 
	void Term();

#if defined(_DEBUG)
public:
	void OutputVertexData();
#endif
};

//EOF