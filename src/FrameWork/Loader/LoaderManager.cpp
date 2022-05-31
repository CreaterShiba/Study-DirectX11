/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		LoaderManager.cpp
	@brief		ローダーマネージャー
	@note		ローダースレッドとして別スレッドで動きます。
	@author		shiba
	@date		2012/7/24 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "LoaderManager.h"

#include "LoaderTexture.h"

#if defined(USE_FBXSDK)
#include "LoaderModel_FBX.h"
#else
#include "LoaderModel.h"
#endif

#include "FrameWork/Asset/AssetManager.h"
#include "FrameWork/Asset/AssetTexture.h"
#include "FrameWork/Asset/AssetModel.h"

#include "FrameWork/Task/TaskScheduler.h"

NS_SINGLETON_INSTANCE_DEFINE( CLoaderManager );

//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CLoaderManager::CLoaderManager()
{

}

//---------------------------------------------------------------------------
/*!
	@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CLoaderManager::~CLoaderManager()
{
	Term();
}

//---------------------------------------------------------------------------
/*!
	@brief	初期化
*/
//---------------------------------------------------------------------------
bool CLoaderManager::Init()
{
	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	破棄
*/
//---------------------------------------------------------------------------
void CLoaderManager::Destroy()
{
	Term();
}

//---------------------------------------------------------------------------
/*!
@brief	更新
*/
//---------------------------------------------------------------------------
void CLoaderManager::Update(const f32 fDelta)
{
	NS_UNUSED(fDelta);
	CheckLoadEnd();
}

void CLoaderManager::CheckLoadEnd()
{
	if (m_caList.size() > 0)
	{
		//ロード済みか
		std::list< CLoadingStruct >::iterator it;
		for (it = m_caList.begin(); it != m_caList.end(); )
		{
			//ロードが終わっているものから、AssetManagerへ登録を行い、Loaderは破棄する
			if (it->m_pcLoader->IsLoadEnd())
			{
				if (!EntryAsset(*it))
				{
					//アセットマネージャへの登録が失敗
				}
				it = m_caList.erase(it);	//破棄
				continue;
				//EntryAsset(*it);
			}
			++it;
		}
	}

}

bool CLoaderManager::LoadModel(const TCHAR* pStrFileName, const TCHAR* pModelSettingFileName /*= nullptr*/)
{
#if defined(USE_FBXSDK)
	CLoaderModel_FBX* pcLoaderModel = nullptr;
	CLASS_NEW(pcLoaderModel, CLoaderModel_FBX());
#else
	CLoaderModel* pcLoaderModel = nullptr;
	CLASS_NEW(pcLoaderModel, CLoaderModel());
#endif
	if (pcLoaderModel != nullptr && pcLoaderModel->LoadStart(pStrFileName, pModelSettingFileName))
	{
		m_caList.push_back(CLoadingStruct());

		CLoadingStruct& cLoading = m_caList.back();
		cLoading.m_eLoaderType = eLOADER_TYPE_MODEL;
		cLoading.m_pcLoader = pcLoaderModel;
		wcscpy( cLoading.m_strFileName, pStrFileName );
		cLoading.m_unUID = CUtilHash::calcNameHash(pStrFileName);

		CheckLoadEnd();
	}
	else
	{
		SAFE_DELETE(pcLoaderModel);
	}

	return true;
}

bool CLoaderManager::LoadTexture(const SInitTextureParam& cParam)
{
	CLoaderTexture* pcLoaderTexture = nullptr;
	CLASS_NEW(pcLoaderTexture, CLoaderTexture());

	if (pcLoaderTexture != nullptr && pcLoaderTexture->LoadStart(cParam.strTexName, cParam.bTGA))
	{
		m_caList.push_back(CLoadingStruct());

		CLoadingStruct& cLoading = m_caList.back();
		cLoading.m_eLoaderType = eLOADER_TYPE_TEXTURE;
		cLoading.m_pcLoader = pcLoaderTexture;
		wcscpy(cLoading.m_strFileName, cParam.strTexName);
		cLoading.m_unUID = CUtilHash::calcNameHash(cParam.strTexName);

		//@todo TextureParamどうやって反映させるか
		CheckLoadEnd();
	}
	else
	{
		SAFE_DELETE(pcLoaderTexture);
	}


	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	読み込んだAssetの登録を行う
*/
//---------------------------------------------------------------------------
bool CLoaderManager::EntryAsset(const CLoadingStruct& cLoader)
{
	bool bEntrySuccess = false;

	switch (cLoader.m_eLoaderType)
	{
	case LOADER_TYPE::eLOADER_TYPE_TEXTURE:
		{
			//テクスチャとして登録
			CLoaderTexture* pcLoader = static_cast<CLoaderTexture*>(cLoader.m_pcLoader);
			if (pcLoader != nullptr)
			{
				DXMng* pcDX = DXMng::GetInstance();
				NS_ASSERT(pcDX);

				ID3D11ShaderResourceView* pcResource = nullptr;

				HRESULT hr = S_OK;
				hr = CreateShaderResourceView(&pcDX->GetDevice(),
					pcLoader->m_image.GetImages(),
					pcLoader->m_image.GetImageCount(),
					pcLoader->m_metadata,
					&pcResource);

				if (FAILED(hr))
				{
					//AssetManagerへの登録失敗
				}
				else
				{
					//AssetManagerへ登録を行う
					CAssetTexture* pcAssetTexture = nullptr;
					CLASS_NEW(pcAssetTexture, CAssetTexture());

					CAssetManager* pcAssetM = CAssetManager::GetInstance();
					if (pcAssetM != nullptr)
					{
						pcAssetTexture->SetAssetUID(cLoader.m_unUID);
						pcAssetTexture->SetAssetName(cLoader.m_strFileName);
						pcAssetTexture->SetTextureRevi(pcResource);
						pcAssetM->EntryAsset(pcAssetTexture);
						bEntrySuccess = true;
					}
					//SAFE_RELEASE(pcResource);
				}
			}
		}
		break;


		case LOADER_TYPE::eLOADER_TYPE_MODEL:
		{
			//モデルとして登録
			//ロードした情報からモデルを生成して、アセットに登録する
#if defined(USE_FBXSDK)
			CLoaderModel_FBX* pcLoaderModel = NS_STATIC_CAST(CLoaderModel_FBX*, cLoader.m_pcLoader);
#else
			CLoaderModel* pcLoaderModel = NS_STATIC_CAST(CLoaderModel*, cLoader.m_pcLoader);
#endif
			if (pcLoaderModel)
			{
				//ローダーのアセットをそのまま入れる
				CAssetManager* pcAssetM = CAssetManager::GetInstance();
				if (pcAssetM != nullptr)
				{
					CAssetModel* pcAsset = pcLoaderModel->GetModelData();
					if (pcAsset)
					{
						pcAsset->SetAssetUID(cLoader.m_unUID);
						pcAsset->SetAssetName(cLoader.m_strFileName);
						pcAssetM->EntryAsset(pcAsset);

						//メッシュ情報も登録させておく
						pcAsset->EntryAllMeshAsset();

						bEntrySuccess = true;
					}
				}
			}
			//アセットが生成できたら、モデルとして登録する
		}
		break;

		case LOADER_TYPE::eLOADER_TYPE_SHADER:
			//@todo ShaderManagerに委託したままにするか、要検討

			break;

		default:
			break;
	}
	return bEntrySuccess;
}

//---------------------------------------------------------------------------
/*!
	@brief	破棄
*/
//---------------------------------------------------------------------------
void CLoaderManager::Term()
{
	//タスクから外させる
	CTaskScheduler* pcTaskM = CTaskScheduler::GetInstance();
	if (pcTaskM != nullptr)
	{
		pcTaskM->DelTask(this);
	}

	m_caList.clear();
}

//EOF
