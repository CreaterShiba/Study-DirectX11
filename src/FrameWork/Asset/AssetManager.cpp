/*==========================================================================*/
/*==========================================================================*/
/*!
@file		CAssetManager.cpp
@brief		Assetをここですべて一元管理
@author		shiba
@date		2012/7/24 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include <iostream>

#include "AssetManager.h"

#include "AssetBase.h"
#include "AssetTexture.h"
#include "AssetModel.h"
#include "AssetFontTexture.h"

#include "FrameWork/Loader/LoaderManager.h"
#include "FrameWork/Loader/LoaderTexture.h"

#include "FrameWork/Utility/UtilCsv.h"
#include "FrameWork/MotionEvent/MotionEvent.h"

#include "FrameWork/Rendering/Motion.h"

NS_SINGLETON_INSTANCE_DEFINE(CAssetManager);
using namespace std;
CAssetManager::CAssetManager():
	m_nCopyCount(0)
{
}

CAssetManager::~CAssetManager()
{
	Term();
}

bool CAssetManager::Init()
{
	m_paMotionEventData.clear();

	//モーションイベントデータ読み込み、保持。
	vector<vector<string>> motionEventParam;
	if (CUtilCSV::LoadCsvData(_T("resource/data/MotionEvent.csv"), motionEventParam))
	{
		m_paMotionEventData.reserve(motionEventParam.size());
		for (u32 i = 1; i < motionEventParam.size(); ++i)
		{
			m_paMotionEventData.push_back(new CMotionEvent());

			u32 unArrayIndex = i - 1;
			CMotionEvent* pMotionEvent = m_paMotionEventData[unArrayIndex];
			if (pMotionEvent != nullptr)
			{
				for (s32 k = 0; k < CMotionEvent::MOTION_EVENT_MAX; ++k)
				{
					//モーションイベントのデータセット
					CMotionEventData::E_MOT_EVENT_TYPE eEvent = static_cast<CMotionEventData::E_MOT_EVENT_TYPE>(std::stoi( motionEventParam[i][1 + k * 4] ));
					f32 fTriggerTime = static_cast<f32>(std::atof(motionEventParam[i][1 + 1 + k * 4].c_str()));
					s32 nParam1 = std::stoi(motionEventParam[i][1 + 2 + k * 4].c_str());
					s32 nParam2 = std::stoi(motionEventParam[i][1 + 3 + k * 4].c_str());
					pMotionEvent->SetupMotionEventParam(k, eEvent, fTriggerTime, nParam1, nParam2);
				}
			}
		}
	}

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	モデルロード
	@param[in]	pStrFileName			ファイル名
	@param[in]	pModelSettingFileName	モデルセッティングファイル
*/
//---------------------------------------------------------------------------
CAssetModel* CAssetManager::LoadModel(const TCHAR* pStrFileName, const TCHAR* pModelSettingFileName/* = nullptr*/)
{
	CAssetBase* pcAssetModel = GetAsset(pStrFileName);
	if (pcAssetModel)
	{
#if true
		//すでにモデルデータが存在するのでコピーをして返す。
		CAssetModel* pcNewAssetModel = nullptr;
		CLASS_NEW(pcNewAssetModel, CAssetModel());

		CAssetModel* pcSourceAssetModel = static_cast<CAssetModel*>(pcAssetModel);
		bool bSuccess = pcSourceAssetModel->Copy(static_cast<CAssetModel*>(pcNewAssetModel));

		if (bSuccess)
		{
			pcAssetModel = pcNewAssetModel;

			TCHAR strFileNameFix[CAssetBase::ASSET_NAME_LENGTH] = { 0 };
			wsprintf(strFileNameFix, _T("%s[%d]\n"), pStrFileName, m_nCopyCount++);

			//@todo IDを無理やり変えて登録を行うので、IDを変えていないデータが消されるとコピーが生成されなくなる
			pcAssetModel->SetAssetUID(CUtilHash::calcNameHash(strFileNameFix) );
			pcAssetModel->SetAssetName(pStrFileName);
			EntryAsset(pcAssetModel);

			//メッシュ情報も登録させておく
			//CAssetMeshは共有できるので登録がいらない。
			//pcAsset->EntryAllMeshAsset();
		}
		else
		{
			SAFE_DELETE(pcNewAssetModel);
		}
#else
		//ただ返すだけだと、マテリアルやジョイントが共有されるため、
		//独自処理ができなくなる。
		return NS_STATIC_CAST(CAssetModel*, pcAssetModel);
#endif
	}
	else
	{

		//ローダーマネージャーにロード発行
		CLoaderManager* pcLoaderM = CLoaderManager::GetInstance();
		if (pcLoaderM != nullptr && pcLoaderM->LoadModel(pStrFileName, pModelSettingFileName))
		{
			//@todo 現状同期なのでAssetに登録されているはず。
			pcAssetModel = GetAsset(CUtilHash::calcNameHash(pStrFileName));

			if (pModelSettingFileName != nullptr)
			{
				//モーション設定ファイルからデータを取り出す
				vector<vector<string>> animParam;
				CUtilCSV::LoadCsvData(pModelSettingFileName, animParam);

				//モーションイベントデータID
				CAssetModel* pcAsset = NS_STATIC_CAST(CAssetModel*, pcAssetModel);
				for (s32 j = 0; j < pcAsset->GetMotionDataNum(); ++j)
				{
					CMotion* pcMot = pcAsset->GetMotionData(j);
					if (pcMot != nullptr)
					{
						s32 nIndex = std::atoi(animParam[j + 1][1].c_str());
						if (nIndex >= 0 && nIndex < NS_STATIC_CAST(s32, m_paMotionEventData.size()))
						{
							pcMot->SetMotionEventData(m_paMotionEventData[nIndex]);
						}
					}
				}
			}
		}
	}

	return NS_STATIC_CAST(CAssetModel*, pcAssetModel);
}

//---------------------------------------------------------------------------
/*!
	@brief	テクスチャ読み込み
	@param[in]	cParam	テクスチャデータ
*/
//---------------------------------------------------------------------------
CAssetTexture* CAssetManager::LoadTexture(const SInitTextureParam& cParam)
{
	u32 unUID = CUtilHash::calcNameHash(cParam.strTexName);
	CAssetBase* pcResTex = GetAsset(unUID);
	if (!pcResTex)
	{
		//パス調整

		//ローダーマネージャーにロード発行
		CLoaderManager* pcLoaderM = CLoaderManager::GetInstance();
		if (pcLoaderM != nullptr && pcLoaderM->LoadTexture(cParam))
		{
			//@todo 現状同期なのでAssetに登録されているはず。
			pcResTex = GetAsset(CUtilHash::calcNameHash(cParam.strTexName));
		}
	}

	return static_cast<CAssetTexture*>(pcResTex);
}

CAssetTexture* CAssetManager::LoadTexture(const TCHAR* strFileName)
{
	SInitTextureParam cParam;	//デフォルトパラメータ
	wcscpy(cParam.strTexName, strFileName);

	return LoadTexture(cParam);
}

//---------------------------------------------------------------------------
/*!
	@brief	フォントテクスチャ読み込み
	@param[in]	cParam	テクスチャデータ
*/
//---------------------------------------------------------------------------
CAssetFontTexture* CAssetManager::LoadFontTexture(const TCHAR* str)
{
	//作ってリストに追加しておくだけでよさそうかな？
	CAssetFontTexture* pcAssetFontTexture = nullptr;
	CLASS_NEW(pcAssetFontTexture, CAssetFontTexture());

	if (pcAssetFontTexture != nullptr)
	{
		pcAssetFontTexture->SetAssetName(str);
		u32 unUID = CUtilHash::calcNameHash(str);
		pcAssetFontTexture->SetAssetUID(unUID);

		pcAssetFontTexture->LoadFontTexture();

		EntryAsset(pcAssetFontTexture);
	}


	return pcAssetFontTexture;
}


void CAssetManager::EntryAsset(CAssetBase* pcAsset)
{
	//@todo mapである意味はあるか？
	auto pib = m_paAssets.insert(std::pair< u32, CAssetBase* >(pcAsset->GetAssetUID(), pcAsset));

	if (!pib.second)
	{
		//ここに来た場合、管理されない宙ぶらりんなアセットになる
		NS_ASSERT(0);
		NS_Report(_T("Asset Insert Error!!\n"));
	}
	else
	{
		pcAsset->AddRefCount();	//カウンタ増やしておく
	}
}

//---------------------------------------------------------------------------
/*!
	@brief		アセットデータ取得
	@param[in]	u32	unUID	アセットのユニークID
	@return		CAssetBase*	対象アセットのポインタ
*/
//---------------------------------------------------------------------------
CAssetBase* CAssetManager::GetAsset(u32 unUID)
{
	if (unUID == INVALID_U32)
	{
		return nullptr;
	}

	CAssetBase* pcAsset = nullptr;
	if (m_paAssets.size() > 0)
	{
		std::map< u32, CAssetBase* >::iterator it = m_paAssets.find(unUID);
		if (it != m_paAssets.end())
		{
			pcAsset = it->second;
		}
	}
	return pcAsset;
}

CAssetBase* CAssetManager::GetAsset(const TCHAR* strFileName)
{
	u32 unUID = CUtilHash::calcNameHash(strFileName);
	return GetAsset(unUID);
}

//---------------------------------------------------------------------------
/*!
	@brief		アセットの削除
	@param[in]	pcAsset	アセット
*/
//---------------------------------------------------------------------------
bool CAssetManager::RemoveAsset(CAssetBase* pcAsset)
{
	bool bSuccess = false;

	//@todo ポインタの実体を渡すように調整
	//pcAssetがnullptrになるように
	if (m_paAssets.size() > 0 && pcAsset)
	{
		u32 unUID = pcAsset->GetAssetUID();
		std::map< u32, CAssetBase* >::iterator it = m_paAssets.find(unUID);
		if (it != m_paAssets.end())
		{
			//参照カウンタ方式
			if (it->second != nullptr)
			{
				it->second->DecRefCount();
				if (it->second->GetRefCount() <= 0)
				{
					SAFE_DELETE(it->second);
					m_paAssets.erase(it);
					pcAsset = nullptr;
				}
			}
			bSuccess = true;
		}
	}
	return bSuccess;
}

//---------------------------------------------------------------------------
/*!
@brief	破棄
*/
//---------------------------------------------------------------------------
void CAssetManager::Term()
{
	{
		std::vector<CMotionEvent*>::iterator it = m_paMotionEventData.begin();
		for (; it != m_paMotionEventData.end(); ++it)
		{
			SAFE_DELETE(*it);
		}
		m_paMotionEventData.clear();
	}

	{
		std::map< u32, CAssetBase* >::iterator it = m_paAssets.begin();
		for (; it != m_paAssets.end(); ++it)
		{
			SAFE_DELETE(it->second);
		}
		m_paAssets.clear();
	}
}


//EOF