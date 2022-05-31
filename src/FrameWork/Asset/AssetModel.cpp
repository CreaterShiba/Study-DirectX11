/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		AssetModel.cpp
	@brief		モデルアセットクラス
	@author		shiba
	@date		2012/7/23 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/

#include "AssetModel.h"
#include "FrameWork/Asset/AssetManager.h"
#include "FrameWork/Asset/AssetMesh.h"

#include "FrameWork/Rendering/Joint.h"
#include "FrameWork/Rendering/Motion.h"

CAssetModel::CAssetModel() :
m_unRootJointID(INVALID_U32)
{
	m_paMesh.clear();
	m_paMaterial.clear();
	m_pcMotionList.clear();
	m_paJoint.clear();
}

CAssetModel::~CAssetModel()
{
	Term();
}

void CAssetModel::Term()
{
	ClearMesh();

	for (u32 i = 0; i < m_paMaterial.size(); ++i)
	{
		SAFE_DELETE(m_paMaterial[i]);
	}
	m_paMaterial.clear();

	for (u32 i = 0; i < m_paJoint.size(); ++i)
	{
		SAFE_DELETE(m_paJoint[i]);
	}
	m_paJoint.clear();

	for (u32 i = 0; i < m_pcMotionList.size(); ++i)
	{
		SAFE_DELETE(m_pcMotionList[i]);
	}
	m_pcMotionList.clear();
}

bool CAssetModel::Copy(CAssetModel* pcCopyModel)
{
	bool bSuccess = false;

	//自分のデータを引数で渡されたCAssetModelに引き渡すこと。
	//AssetMeshは共有可能。ただし、消す際にやばいので参照カウンタの対応がいる
	pcCopyModel->ReserveMesh(m_paMesh.size());
	for (u32 i = 0; i < m_paMesh.size(); ++i)
	{
		m_paMesh[i]->AddRefCount();	//カウンタ増やしておく
		pcCopyModel->AddMesh(m_paMesh[i]);
	}

	//マテリアルデータは生成した上でコピー、引き渡す
	pcCopyModel->ReserveMaterial(m_paMaterial.size());
	for (u32 i = 0; i < m_paMaterial.size(); ++i)
	{
		CMaterial* pcMat = nullptr;
		CLASS_NEW(pcMat, CMaterial());
		*pcMat = *m_paMaterial[i];
		pcCopyModel->AddMaterial(pcMat);
	}

	//モーションは生成した上でコピー、引き渡す
	for (u32 i = 0; i < m_pcMotionList.size(); ++i)
	{
		CMotion* pcMot = nullptr;
		CLASS_NEW(pcMot, CMotion());
		m_pcMotionList[i]->Copy(pcMot);
		pcCopyModel->AddMotionData(pcMot);
	}

	//ジョイントは生成した上でコピー、引き渡す
	for (u32 i = 0; i < m_paJoint.size(); ++i)
	{
		CJoint* pcJoint = nullptr;
		CLASS_NEW(pcJoint, CJoint());
		m_paJoint[i]->Copy(pcJoint);
		pcCopyModel->AddJoint(pcJoint);
	}
	//IDをもとに親子関係の構築をする
	//親子関係の構築も必要になる(idをもとに)
	for (u32 i = 0; i < m_paJoint.size(); ++i)
	{
		CJoint* pcSource = m_paJoint[i];
		if (pcSource != nullptr)
		{
			CJoint* pcDest = pcCopyModel->GetJoint(i);
			if (pcDest != nullptr)
			{
				if (pcSource->m_pFirstChild != nullptr)
				{
					pcDest->m_pFirstChild = pcCopyModel->GetJoint(pcSource->m_pFirstChild->GetJointID());
				}
				if (pcSource->m_pParent != nullptr)
				{
					pcDest->m_pParent = pcCopyModel->GetJoint(pcSource->m_pParent->GetJointID());
				}
				if (pcSource->m_pSibling != nullptr)
				{
					pcDest->m_pSibling = pcCopyModel->GetJoint(pcSource->m_pSibling->GetJointID());
				}
			}
		}
	}

	//各パラメータは単純コピー
	pcCopyModel->m_sModelData = m_sModelData;
	pcCopyModel->m_unRootJointID = m_unRootJointID;

	bSuccess = true;

	return bSuccess;
}

void CAssetModel::ClearMesh()
{
	//すべてのメッシュアセットの参照解除
	CAssetManager* pcAssetM = CAssetManager::GetInstance();
	if (pcAssetM)
	{
		for (u32 i = 0; i < m_paMesh.size(); ++i)
		{
			pcAssetM->RemoveAsset(m_paMesh[i]);
		}
	}
	m_paMesh.clear();
}

void CAssetModel::ClearMaterial()
{
	for (u32 i = 0; i < m_paMaterial.size(); ++i)
	{
		SAFE_DELETE(m_paMaterial[i]);
	}
	m_paMaterial.clear();
}

//---------------------------------------------------------------------------
/*!
	@brief		ジョイント取得
	@param[in]	unJointIdx
*/
//---------------------------------------------------------------------------
CJoint* CAssetModel::GetJoint(const u32 unJointIdx)
{
	if (m_paJoint.size() > unJointIdx && unJointIdx >= 0)
	{
		return m_paJoint[unJointIdx];
	}
	else
	{
		return nullptr;
	}
}

//---------------------------------------------------------------------------
/*!
	@brief	ジョイント取得
	@param[in]	strJointName	ジョイント名
*/
//---------------------------------------------------------------------------
CJoint* CAssetModel::GetJoint(const TCHAR* strJointName)
{
	std::vector< CJoint* >::iterator it = m_paJoint.begin();

	for (; it != m_paJoint.end(); ++it)
	{
		if (!wcscmp((*it)->GetJointName(), strJointName))
		{
			return (*it);
		}
	}

	return nullptr;
}

//---------------------------------------------------------------------------
/*!
	@brief	MeshAssetのAssetManagerへの登録
*/
//---------------------------------------------------------------------------
bool CAssetModel::EntryAllMeshAsset()
{
	//すべてのメッシュアセットの登録
	CAssetManager* pcAssetM = CAssetManager::GetInstance();
	if (pcAssetM)
	{
		std::vector<CAssetMesh*>::iterator it = m_paMesh.begin();
		int nCount = 1;
		for (; it != m_paMesh.end(); ++it)
		{
			TCHAR strFileName[CAssetBase::ASSET_NAME_LENGTH] = { 0 };
			wcscat(strFileName, m_StrAssetName);

			TCHAR mesh[] = _T("/Mesh");
			wcscat(strFileName, mesh);

			TCHAR figure[4] = { 0 };
			swprintf(figure, _T("%d"), nCount);

			wcscat(strFileName, figure);

			(*it)->SetAssetName(strFileName);
			(*it)->SetAssetUID(CUtilHash::calcNameHash(strFileName));

			pcAssetM->EntryAsset(*it);

			++nCount;
		}
	}

	return true;
}

#if defined(_DEBUG)


//デバッグ用出力
void CAssetModel::OutputVertexData()
{
	for (u32 i = 0; i < m_paMesh.size(); ++i)
	{
		CAssetMesh* pcMesh = m_paMesh[i];
		if (pcMesh != nullptr)
		{
			for (u32 j = 0; j < pcMesh->m_caVertices.size(); ++j)
			{
				BasicVertex& vert = pcMesh->m_caVertices[j];
				NS_Report(_T("MID[%d] VID[%d] : Pos[%f,%f,%f] : UV[%f,%f] : Norm[%f,%f,%f] : Weight[%f,%f,%f] : BI[%d,%d,%d,%d]\n"),
					i,j,vert.v3Position.x, vert.v3Position.y, vert.v3Position.z, vert.v2TexCoord.x, vert.v2TexCoord.y,
					vert.v3Normal.x, vert.v3Normal.y, vert.v3Normal.z, 
					vert.m_faWeight[0], vert.m_faWeight[1], vert.m_faWeight[2],
					vert.m_naBoneIndex[0], vert.m_naBoneIndex[1], vert.m_naBoneIndex[2], vert.m_naBoneIndex[3]);
			}
		}
	}
}

#endif

//EOF

