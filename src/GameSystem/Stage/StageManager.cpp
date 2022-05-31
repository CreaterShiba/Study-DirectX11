#include "StageManager.h"

#include "FrameWork/Utility/UtilCsv.h"
#include "FrameWork/Asset/AssetManager.h"
#include "FrameWork/Asset/AssetTexture.h"

#include "FrameWork/Collision/CollisionGeometry.h"

#include "FrameWork/Rendering/Model.h"
#include "FrameWork/Rendering/RenderingManager.h"
#include "FrameWork/Rendering/Light.h"

#include "GameSystem/Unit/UnitPlayer.h"
#include "GameSystem/Gimmick/GimmickGoal.h"
#include "GameSystem/Gimmick/GimmickSwitch.h"

#include "GameSystem/Collision/CollisionManager.h"
using namespace std;

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		StageManager.cpp
	@brief		ステージ管理
	@author		shiba
	@date		2015/07/14 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
CStageManager::CStageManager() 
	: m_pcStageFloor(nullptr)
	, m_paCubeParam(nullptr)
	, m_nStageSize(0)
	, m_nPassableAreaColID(-1)
	, m_pcGimmickSwitch(nullptr)
	, m_pcGoalUnit(nullptr)
{
	m_paStageCubeModel.clear();
	m_paStageColData.clear();
}

CStageManager::~CStageManager()
{
	Term();
}

bool CStageManager::Init()
{
	bool bSuccess = true;

	//CSVからステージデータのロード、配置。
	vector<vector<string>> stageParam;
	CUtilCSV::LoadCsvData(_T("resource/data/Stage.csv"), stageParam);

	vector<vector<string>> stageAttribute;
	CUtilCSV::LoadCsvData(_T("resource/data/StageAttribute.csv"), stageAttribute);

	s32 nWidthSize = stageParam[1].size();
	s32 nHeightSize = (stageParam.size() - 1);

	m_nStageSize = nWidthSize * nHeightSize;	//４角系ステージになる
	m_paCubeParam = new CubeParam[m_nStageSize];

	s32* naInstanceSize = new s32[E_STAGE_TYPE_ID::eSTAGE_MAX];	//インスタンス数

	for (s32 i = 0; i < E_STAGE_TYPE_ID::eSTAGE_MAX; ++i)
		naInstanceSize[i] = 0;

	//データ読み出し
	for (s32 i = 0; i < m_nStageSize; ++i)
	{
		s32 nIndex1 = 1 + i / nHeightSize;
		s32 nIndex2 = i % nWidthSize;

		s32 nTypeID = std::stoi(stageParam[nIndex1][nIndex2]);
		m_paCubeParam[i].m_eTypeID = (E_STAGE_TYPE_ID)nTypeID;
		m_paCubeParam[i].m_nWidthIdx = nIndex1;
		m_paCubeParam[i].m_nHeightIdx = nIndex2;

		if (m_paCubeParam[i].m_eTypeID == eGOAL)
		{
			GlobalParam::vPlayerGoalPos.x = nIndex1 * CUBE_SIZE - nWidthSize * 0.5f * CUBE_SIZE;
			GlobalParam::vPlayerGoalPos.y = 0.0f;
			GlobalParam::vPlayerGoalPos.z = nIndex2 * CUBE_SIZE - nHeightSize * 0.5f * CUBE_SIZE;
		}
		if (m_paCubeParam[i].m_eTypeID == eSTART)
		{
			GlobalParam::vPlayerStartPos.x = nIndex1 * CUBE_SIZE - nWidthSize * 0.5f * CUBE_SIZE;
			//GlobalParam::vPlayerStartPos.y = 0.0f;
			GlobalParam::vPlayerStartPos.z = nIndex1 * CUBE_SIZE - nWidthSize * 0.5f * CUBE_SIZE;
		}
		if (nTypeID < E_STAGE_TYPE_ID::eSTAGE_MAX && nTypeID >= 0 )
		{
			naInstanceSize[nTypeID]++;	//インスタンス数増加
		}

		s32 nStageAttribute = std::stoi(stageAttribute[nIndex1][nIndex2]);
		if (nStageAttribute == 1)
		{
			m_paCubeParam[i].m_bPassable = true;
		}
	}

	CAssetManager* pcAssetM = CAssetManager::GetInstance();

	//テクスチャ名読み出し
	vector<vector<string>> blockParam;
	CUtilCSV::LoadCsvData(_T("resource/data/BlockParam.csv"), blockParam);

	//キューブ型モデル読み込み
	//インスタンシングデータの設定
	m_paStageCubeModel.reserve((size_t)E_STAGE_TYPE_ID::eSTAGE_MAX);
	for (s32 i = 0; i < (s32)E_STAGE_TYPE_ID::eSTAGE_MAX; ++i)
	{
#if true
		CModel* pcModel = nullptr;
		CLASS_NEW(pcModel, CModel);
		if (pcModel != nullptr && pcModel->LoadModel(_T("resource/Graphics/model/cube.obj")))
		{
			if (naInstanceSize[i] >= 1)
			{
				pcModel->SetDrawEnable(true, CRenderingManager::E_RENDER_STAGE_MRT_POS_NOM_DIF);
				pcModel->SetDrawEnable(true, CRenderingManager::E_RENDER_STAGE_WRITEZ);
				if (naInstanceSize[i] >= 2)
				{
					//2個以上ならインスタンシングを用いて描画
					pcModel->CreateInstancingMatrix(naInstanceSize[i]);	//インスタンシングマトリクスの生成
				}

				//テクスチャの生成、ID割当
				TCHAR strTexName[256] = { 0 };
				mbstowcs(strTexName, blockParam[1 + i][0].c_str(), strlen(blockParam[1 + i][0].c_str()));

				SInitTextureParam cParam;
				wcscpy_s(cParam.strTexName, TEXTURE_FILE_PATH, strTexName);
				cParam.bTGA = true;

				//テクスチャ割当
				const CAssetTexture* pcTexture = pcAssetM->LoadTexture(cParam);
				if (pcTexture != nullptr)
				{
					pcModel->SetDiffuseTexID(0, pcTexture->GetAssetUID());
				}

				pcModel->SetScale(CUBE_SIZE);
			}

			m_paStageCubeModel.push_back(pcModel);
		}
#endif
	}

	//床配置(適当
	CLASS_NEW(m_pcStageFloor, CModel);
	if (m_pcStageFloor->LoadModel(_T("resource/Graphics/model/cube.obj")))
	{
		m_pcStageFloor->SetDrawEnable(true, CRenderingManager::E_RENDER_STAGE_MRT_POS_NOM_DIF);
		m_pcStageFloor->SetDrawEnable(true, CRenderingManager::E_RENDER_STAGE_WRITEZ);

		//テクスチャの生成、ID割当
		TCHAR strTexName[256] = { 0 };
		char* texPath = "resource/Graphics/texture/dungeon 3.tga";
		mbstowcs(strTexName, texPath, strlen(texPath));

		SInitTextureParam cParam;
		wcscpy_s(cParam.strTexName, TEXTURE_FILE_PATH, strTexName);
		cParam.bTGA = true;

		//テクスチャ割当
		CAssetTexture* pcTexture = pcAssetM->LoadTexture(cParam);
		m_pcStageFloor->SetDiffuseTexID(0, pcTexture->GetAssetUID());
		m_pcStageFloor->SetPos(0.0f, -5.0f, 0.0f);
		m_pcStageFloor->SetScale(NSVector3(1250.0f, 0.5f, 1250.0f));

	}

	//モデルデータの配置
	s32* naMoveIndex = new s32[E_STAGE_TYPE_ID::eSTAGE_MAX];	//インスタンス数
	for (s32 i = 0; i < E_STAGE_TYPE_ID::eSTAGE_MAX; ++i)
		naMoveIndex[i] = 0;

	for (s32 i = 0; i < m_nStageSize; ++i)
	{
		CubeParam& cubeParam = m_paCubeParam[i];
		if (cubeParam.m_eTypeID >= 0 && cubeParam.m_eTypeID < E_STAGE_TYPE_ID::eSTAGE_MAX)
		{
			//モデルデータ配置
			if (m_paStageCubeModel.size() > (u32)cubeParam.m_eTypeID)
			{
				f32 fPosX = cubeParam.m_nWidthIdx * CUBE_SIZE - nWidthSize * 0.5f * CUBE_SIZE;
				f32 fPosZ = cubeParam.m_nHeightIdx * CUBE_SIZE - nHeightSize * 0.5f * CUBE_SIZE;

				cubeParam.m_vPos.x = fPosX;
				cubeParam.m_vPos.y = fPosZ;

				if (naInstanceSize[cubeParam.m_eTypeID] >= 2)
				{
					//インスタンス描画の場合はこちらで行列を作って指定
					NSMatrix cMat = DirectX::XMMatrixIdentity();
					cMat = DirectX::XMMatrixScaling(CUBE_SIZE, CUBE_SIZE, CUBE_SIZE) *
						DirectX::XMMatrixTranslation(fPosX, 0.0f, fPosZ);
					m_paStageCubeModel[cubeParam.m_eTypeID]->SetInstancingMatrix(naMoveIndex[cubeParam.m_eTypeID], cMat);
					cubeParam.m_nInstancingIndex = naMoveIndex[cubeParam.m_eTypeID];

					++naMoveIndex[cubeParam.m_eTypeID];	//インスタンス用配置インデックス更新
				}
				else
				{
					//通常モデルあれば配置するだけ
					m_paStageCubeModel[cubeParam.m_eTypeID]->SetPos(fPosX, 0.0f, fPosZ);
				}
			}
		}
	}

	SAFE_DELETEA(naMoveIndex);
	SAFE_DELETEA(naInstanceSize);

	// コリジョンデータの作成。適当に高さは持たせておく
	// ひとまず手動で入れる。後々自動生成。
	m_paStageColData.reserve(15 + 2);
	{
		f32 fColYPos = -CUBE_SIZE*0.5f;
		//左壁
		AddStageAABB(NSVector3(0.0f, 0.0f, 0.0f), NSVector3(CUBE_SIZE, CUBE_SIZE*2.0f, CUBE_SIZE*nHeightSize*2.0f), NSVector3(-57.5f, fColYPos, -CUBE_SIZE * nHeightSize));

		//右壁
		AddStageAABB(NSVector3(0.0f, 0.0f, 0.0f), NSVector3(CUBE_SIZE, CUBE_SIZE*2.0f, CUBE_SIZE*nHeightSize*2.0f), NSVector3(57.5f, fColYPos, -CUBE_SIZE * nHeightSize));

		//奥壁
		AddStageAABB(NSVector3(0.0f, 0.0f, 0.0f), NSVector3(CUBE_SIZE*nWidthSize*2.0f, CUBE_SIZE * 3, CUBE_SIZE), NSVector3(-60.0f*2.0f, fColYPos, 47.5f));

		//手前壁
		AddStageAABB(NSVector3(0.0f, 0.0f, 0.0f), NSVector3(CUBE_SIZE*nWidthSize*2.0f, CUBE_SIZE * 3, CUBE_SIZE), NSVector3(-60.0f*2.0f, fColYPos, -67.5f));

		//コリジョン追加
		AddStageAABB(NSVector3(0.0f, 0.0f, 0.0f), NSVector3(CUBE_SIZE * 1, CUBE_SIZE*2.0f, CUBE_SIZE * 4), NSVector3(-40.0f, fColYPos, -50.0f));
		AddStageAABB(NSVector3(0.0f, 0.0f, 0.0f), NSVector3(CUBE_SIZE * 1, CUBE_SIZE*2.0f, CUBE_SIZE * 4), NSVector3(-40.0f, fColYPos, 0.0f));
		AddStageAABB(NSVector3(0.0f, 0.0f, 0.0f), NSVector3(CUBE_SIZE * 1, CUBE_SIZE*2.0f, CUBE_SIZE * 10),NSVector3(-30.0f, fColYPos, -60.0f));

		AddStageAABB(NSVector3(0.0f, 0.0f, 0.0f), NSVector3(CUBE_SIZE * 1, CUBE_SIZE*2.0f, CUBE_SIZE * 3), NSVector3(-20.0f, fColYPos, -30.0f));
		AddStageAABB(NSVector3(0.0f, 0.0f, 0.0f), NSVector3(CUBE_SIZE * 1, CUBE_SIZE*2.0f, CUBE_SIZE * 3), NSVector3(-20.0f, fColYPos, 10.0f));
		AddStageAABB(NSVector3(0.0f, 0.0f, 0.0f), NSVector3(CUBE_SIZE * 1, CUBE_SIZE*2.0f, CUBE_SIZE * 2), NSVector3(-20.0f, fColYPos, -60.0f));
		
		AddStageAABB(NSVector3(0.0f, 0.0f, 0.0f), NSVector3(CUBE_SIZE * 1, CUBE_SIZE*2.0f, CUBE_SIZE * 1), NSVector3(10.0f, fColYPos, 40.0f));
		AddStageAABB(NSVector3(0.0f, 0.0f, 0.0f), NSVector3(CUBE_SIZE * 2, CUBE_SIZE*2.0f, CUBE_SIZE * 4), NSVector3(0.0f, fColYPos, 0.0f));
		AddStageAABB(NSVector3(0.0f, 0.0f, 0.0f), NSVector3(CUBE_SIZE * 2, CUBE_SIZE*2.0f, CUBE_SIZE * 4), NSVector3(0.0f, fColYPos, -50.0f));
		AddStageAABB(NSVector3(0.0f, 0.0f, 0.0f), NSVector3(CUBE_SIZE * 1, CUBE_SIZE*2.0f, CUBE_SIZE * 3), NSVector3(20.0f, fColYPos, -50.0f));
		AddStageAABB(NSVector3(0.0f, 0.0f, 0.0f), NSVector3(CUBE_SIZE * 1, CUBE_SIZE*2.0f, CUBE_SIZE * 7), NSVector3(30.0f, fColYPos, -30.0f));
		AddStageAABB(NSVector3(0.0f, 0.0f, 0.0f), NSVector3(CUBE_SIZE * 1, CUBE_SIZE*2.0f, CUBE_SIZE * 10), NSVector3(40.0f, fColYPos, -60.0f));

		AddStageAABB(NSVector3(0.0f, 0.0f, 0.0f), NSVector3(CUBE_SIZE * 2, CUBE_SIZE*2.0f, CUBE_SIZE * 1), NSVector3(0.0f, fColYPos, -10.0f));
		m_nPassableAreaColID = m_paStageColData.size() - 1;
	}

	//ゴールユニット
	CLASS_NEW(m_pcGoalUnit, CUnitPlayer());
	m_pcGoalUnit->GetModel()->SetPos(NSVector3(GlobalParam::vPlayerGoalPos.x, -5.0f, GlobalParam::vPlayerGoalPos.z));
	m_pcGoalUnit->SetLightID(6);
	
	//スイッチ
	CLASS_NEW(m_pcGimmickSwitch, CGimmickSwitch);
	m_pcGimmickSwitch->GetModel()->SetPos(35.0f, 0.0f, -35.0f); 
	std::function<void()> onFunc = [this]() {
		this->OpenPassableArea();
	};
	m_pcGimmickSwitch->SetSwitchOnEventCallback(onFunc);

	std::function<void()> offFunc = [this]() {
		this->ClosePassableArea();
	};
	m_pcGimmickSwitch->SetSwitchOffEventCallback(offFunc);

	m_pcGimmickSwitch->SetSwitchOnOff(false);	//ClosePassableAreaも呼ばれるはず
	m_pcGimmickSwitch->SetupLightID(5);

	//コリジョンデータをマネージャへ登録
	CCollisionManager* pcColM = CCollisionManager::GetInstance();
	if (pcColM != nullptr)
	{
		for (u32 i = 0; i < m_paStageColData.size(); ++i)
		{
			pcColM->AddCollision(m_paStageColData[i]);
		}
		pcColM->SetGoalUnit(m_pcGoalUnit);
		pcColM->SetGimmickSwitch(m_pcGimmickSwitch);
		pcColM->SetPassableAreaAABB(static_cast<CCollisionAABB*>(m_paStageColData[m_nPassableAreaColID]));

		pcColM->SetupGoalCollision();
	}

	return bSuccess;
}

void CStageManager::OpenPassableArea()
{
	if (m_nPassableAreaColID >= 0 && m_nPassableAreaColID < (s32)m_paStageColData.size())
	{
		CCollisionGeomBase* pCol = m_paStageColData[m_nPassableAreaColID];
		if (pCol != nullptr)
		{
			pCol->SetCollisionEnable(false);
		}
	}
	//モデルアニメーション開始
	OnOrOffPassableCubeModel(false);
}

void CStageManager::ClosePassableArea()
{
	if (m_nPassableAreaColID >= 0 && m_nPassableAreaColID < (s32)m_paStageColData.size())
	{
		CCollisionGeomBase* pCol = m_paStageColData[m_nPassableAreaColID];
		if (pCol != nullptr)
		{
			pCol->SetCollisionEnable(true);
		}
	}

	//モデルアニメーション開始
	OnOrOffPassableCubeModel(true);
}

void CStageManager::AddStageAABB(const NSVector3& vMin, const NSVector3& vMax, const NSVector3& vPos)
{
	CCollisionAABB* pAABB = nullptr;
	CLASS_NEW(pAABB, CCollisionAABB);

	pAABB->SetColObjectType(eCOL_OBJECT_STAGE);
	pAABB->SetMin(vMin);
	pAABB->SetMax(vMax);
	pAABB->SetPos(vPos);

	m_paStageColData.push_back(pAABB);
}

void CStageManager::OnOrOffPassableCubeModel(const bool bOnOff)
{
	for (s32 i = 0; i < m_nStageSize; ++i)
	{
		CubeParam* pCubeParam = &m_paCubeParam[i];
		if (pCubeParam != nullptr && pCubeParam->m_bPassable)
		{
			if (m_paStageCubeModel.size() <= (int)pCubeParam->m_eTypeID)
				continue;

			//引っ込める
			NSVector3 vPos;
			if(bOnOff)
				vPos = NSVector3(pCubeParam->m_vPos.x, 0.0f, pCubeParam->m_vPos.y);
			else
				vPos = NSVector3(pCubeParam->m_vPos.x, -10.0f, pCubeParam->m_vPos.y);

			if (pCubeParam->m_nInstancingIndex >= 0)
			{
				//インスタンシング
				NSMatrix cMat = DirectX::XMMatrixIdentity();
				cMat =
					DirectX::XMMatrixScaling(CUBE_SIZE, CUBE_SIZE, CUBE_SIZE) *
					DirectX::XMMatrixTranslation(vPos.x, vPos.y, vPos.z);
				m_paStageCubeModel[pCubeParam->m_eTypeID]->SetInstancingMatrix(pCubeParam->m_nInstancingIndex, cMat);
			}
			else
			{
				m_paStageCubeModel[pCubeParam->m_eTypeID]->SetPos(vPos);
			}
		}
	}
}

void CStageManager::Term()
{
	CCollisionManager* pcColM = CCollisionManager::GetInstance();
	if (pcColM != nullptr)
	{
		pcColM->SetGoalUnit(nullptr);
		pcColM->SetGimmickSwitch(nullptr);
	}

	SAFE_DELETE_TASK(m_pcGoalUnit);
	SAFE_DELETE_TASK(m_pcGimmickSwitch);

	SAFE_DELETEA(m_paCubeParam);
	SAFE_DELETE_TASK(m_pcStageFloor);
	
	for (std::vector<CCollisionGeomBase*>::iterator it = m_paStageColData.begin(); it != m_paStageColData.end(); ++it)
	{
		SAFE_DELETE(*it);
	}
	m_paStageColData.clear();

	for (std::vector<CModel*>::iterator it = m_paStageCubeModel.begin(); it != m_paStageCubeModel.end(); ++it)
	{
		SAFE_DELETE_TASK(*it);
	}
	m_paStageCubeModel.clear();
}

//EOF
