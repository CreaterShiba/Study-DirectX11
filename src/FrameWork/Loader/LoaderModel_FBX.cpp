/*==========================================================================*/
/*==========================================================================*/
/*!
@file		LoaderModel_FBX.cpp
@brief		モデルローダー
@author		shiba
@date		2012/9/23 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#include "LoaderModel_FBX.h"

#include "FrameWork/Asset/AssetModel.h"
#include "FrameWork/Asset/AssetMesh.h"
#include "FrameWork/Asset/AssetManager.h"
#include "FrameWork/Asset/AssetTexture.h"

#include "FrameWork/Rendering/Material.h"
#include "FrameWork/Rendering/ModelDataType.h"
#include "FrameWork/Rendering/Joint.h"
#include "FrameWork/Rendering/Motion.h"

#include "FrameWork/Utility/UtilCsv.h"

#pragma comment ( lib, "wininet.lib" )
#pragma comment ( lib, "libfbxsdk-md.lib" )	//プロジェクト側でDEBUGとRELASEは分けてる
#pragma comment ( lib, "libxml2-md.lib" )
#pragma comment ( lib, "zlib-md.lib" )
#pragma comment ( lib, "Kernel32.lib" )

using namespace std;

//aiMatrixをNSMatrix形式に変換する
void convertXMMatrix(FbxAMatrix& aiMat, NSMatrix* pcMatrix)
{
	if (pcMatrix)
	{
		pcMatrix->_11 = (float)aiMat[0][0]; pcMatrix->_12 = (float)aiMat[0][1]; pcMatrix->_13 = (float)aiMat[0][2]; pcMatrix->_14 = (float)aiMat[0][3];
		pcMatrix->_21 = (float)aiMat[1][0]; pcMatrix->_22 = (float)aiMat[1][1]; pcMatrix->_23 = (float)aiMat[1][2]; pcMatrix->_24 = (float)aiMat[1][3];
		pcMatrix->_31 = (float)aiMat[2][0]; pcMatrix->_32 = (float)aiMat[2][1]; pcMatrix->_33 = (float)aiMat[2][2]; pcMatrix->_34 = (float)aiMat[2][3];
		pcMatrix->_41 = (float)aiMat[3][0]; pcMatrix->_42 = (float)aiMat[3][1]; pcMatrix->_43 = (float)aiMat[3][2]; pcMatrix->_44 = (float)aiMat[3][3];
	}
}

#if false
void convertNSVector3(const aiVector3D& aiVec, NSVector3* pcVec3)
{
	if (pcVec3)
	{
		pcVec3->x = aiVec.x;
		pcVec3->y = aiVec.y;
		pcVec3->z = aiVec.z;
	}
}

void convertNSQuat(const aiQuaternion& aiQuat, NSQuat* pcQuat)
{
	if (pcQuat)
	{
		*pcQuat = XMVectorSet(aiQuat.x, aiQuat.y, aiQuat.z, aiQuat.w);
	}
}

#endif

CLoaderModel_FBX::CLoaderModel_FBX() :
	m_pFBXScene(nullptr),
	m_pcModelData(nullptr)
{

}

CLoaderModel_FBX::~CLoaderModel_FBX()
{
	Term();
}

void CLoaderModel_FBX::Term()
{
	m_pcModelData = nullptr;
	m_pFBXScene = nullptr;
}

bool CLoaderModel_FBX::LoadStart(const TCHAR* strFileName, const TCHAR* pModelSettingFileName)
{
	bool bLoadSuccess = false;

	m_eLoadSeq = CLoader::eLOADSEQ_INIT;

	char strFile[256] = { 0 };
	wcstombs(strFile, strFileName, wcslen(strFileName));

	FbxManager *lSdkManager = FbxManager::Create();

	FbxIOSettings * ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	ios->SetBoolProp(IMP_FBX_MATERIAL, true);
	ios->SetBoolProp(IMP_FBX_TEXTURE, true);
	lSdkManager->SetIOSettings(ios);

	FbxImporter* pImporter = FbxImporter::Create(lSdkManager, "");
	if (pImporter->Initialize(strFile, -1, lSdkManager->GetIOSettings()))
	{
		m_pFBXScene = FbxScene::Create(lSdkManager, "");

		pImporter->Initialize(strFile, -1, ios);
		pImporter->Import(m_pFBXScene);

		//FbxNode* pRootNode = m_pFBXScene->GetRootNode();

		if (m_pFBXScene)
		{
			//座標系の修正
			FbxAxisSystem sceneAxist = m_pFBXScene->GetGlobalSettings().GetAxisSystem();
			if (sceneAxist != FbxAxisSystem::eDirectX)
			{
				FbxAxisSystem::DirectX.ConvertScene(m_pFBXScene);
			}

			//三角形ポリゴンへ変換
			FbxGeometryConverter geometryConverter(lSdkManager);
			geometryConverter.Triangulate(m_pFBXScene, true);

			// 単位の統一
			FbxSystemUnit SceneSystemUnit = m_pFBXScene->GetGlobalSettings().GetSystemUnit();
			if (SceneSystemUnit.GetScaleFactor() != 1.0)
			{
				// センチメーター単位にコンバートする
				FbxSystemUnit::cm.ConvertScene(m_pFBXScene);
			}

			//メッシュデータ生成、読み込み
			CLASS_NEW(m_pcModelData, CAssetModel());

			//シーンからメッシュデータのみを取り出しておく
			m_paTempFBXMesh.clear();
			s32 nMeshNum = m_pFBXScene->GetMemberCount<FbxMesh>();
			for (int i = 0; i < nMeshNum; ++i)
			{
				m_paTempFBXMesh.push_back(m_pFBXScene->GetMember<FbxMesh>(i));
			}

			m_pcModelData->ClearMesh();
			m_pcModelData->ReserveMesh(nMeshNum);

			m_paTempAssetMesh.clear();
			m_paTempAssetMesh.reserve(nMeshNum);

			for (s32 i = 0; i < nMeshNum; ++i)
			{
				m_paTempAssetMesh.push_back(new CAssetMesh());
			}


			//ジオメトリ情報の生成
			LoadGeometry();

			//マテリアル情報読み込み、設定
			LoadMaterial();

			//スキン情報読み込み、設定
			LoadSkin();

			//アニメーション情報取り出し
			if (pModelSettingFileName != nullptr)
			{
				//CSVファイルから各FBXを読み込む形をとる
				vector<vector<string>> animParam;
				CUtilCSV::LoadCsvData(pModelSettingFileName, animParam);

				for (u32 i = 1; i < animParam.size(); ++i)
				{
					ios->SetBoolProp(IMP_FBX_MATERIAL, false);
					ios->SetBoolProp(IMP_FBX_TEXTURE, false);
					ios->SetBoolProp(IMP_FBX_ANIMATION, true);

					FbxImporter* pAnimImporter = FbxImporter::Create(lSdkManager, "");
					pAnimImporter->Initialize((animParam[i][0].c_str()), -1, ios);

					FbxScene* pTempScene = nullptr;
					pTempScene = FbxScene::Create(lSdkManager, "");
					pAnimImporter->Import(pTempScene);

					LoadMotion(pTempScene);

					if (pAnimImporter != nullptr)
					{
						pAnimImporter->Destroy();
						pAnimImporter = nullptr;
					}
					if (pTempScene != nullptr)
					{
						pTempScene->Destroy();
						pTempScene = nullptr;
					}
				}
			}
			else
			{
				LoadMotion(m_pFBXScene);
			}

			//メッシュ情報生成
			for (u32 i = 0; i < m_paTempAssetMesh.size(); ++i)
			{
				CAssetMesh* pcMesh = m_paTempAssetMesh[i];
				bLoadSuccess = pcMesh->CreateBuffer();	//描画用バッファの作成
				if (!bLoadSuccess)
				{
					break;
				}
			}

			m_pFBXScene->Destroy();
			m_pFBXScene = nullptr;
		}

		if (bLoadSuccess)
		{
			for (u32 i = 0; i < m_paTempAssetMesh.size(); ++i)
			{
				//成功したら、モデルにアセットを登録
				m_pcModelData->AddMesh(m_paTempAssetMesh[i]);

				//マテリアルがない場合、仮マテリアル入れておく
				if (m_pcModelData->GetMaterial(i) == nullptr)
				{
					CMaterial* pMat = nullptr;
					CLASS_NEW(pMat, CMaterial());
					m_pcModelData->AddMaterial(pMat);
					m_paTempAssetMesh[i]->SetMaterialIndex(m_pcModelData->GetMaterialNum() - 1);
				}
			}
		}
		else
		{
			//失敗したのでここで消す
			for (u32 i = 0; i < m_paTempAssetMesh.size(); ++i)
			{
				SAFE_DELETE(m_paTempAssetMesh[i]);
			}
			SAFE_DELETE(m_pcModelData);
		}
	}

	pImporter->Destroy();
	pImporter = nullptr;

	ios->Destroy();
	ios = nullptr;

	lSdkManager->Destroy();
	lSdkManager = nullptr;

	//@todo ひとまずロード済みとする
	m_eLoadSeq = CLoader::eLOADSEQ_LOADEND;

	return bLoadSuccess;
}

//----------------------------------------------------------------------------
/*	@brief	ジオメトリデータ読み込み(頂点、インデックス、UV、法線)
*/
//----------------------------------------------------------------------------
void CLoaderModel_FBX::LoadGeometry()
{
	for (u32 nMeshIdx = 0; nMeshIdx < m_paTempFBXMesh.size(); ++nMeshIdx)
	{
		FbxMesh* pAIMesh = m_paTempFBXMesh[nMeshIdx];
		CAssetMesh* pcMesh = m_paTempAssetMesh[nMeshIdx];

		if (pAIMesh && pcMesh)
		{
			u32 Index = 0;
			s32 nPolyCount = pAIMesh->GetPolygonCount();
			pcMesh->m_unIndexCount = nPolyCount * 3;	//3固定
			SAFE_DELETEA(pcMesh->m_unapIndecies);
			pcMesh->m_unapIndecies = new u16[pcMesh->m_unIndexCount];


			for (s32 nPolyIndex = 0; nPolyIndex < nPolyCount; ++nPolyIndex)
			{
				int nPolygonsize = pAIMesh->GetPolygonSize(nPolyIndex);
				NS_ASSERT(nPolygonsize == 3);

				for (int j = 0; j < nPolygonsize; j++)
				{
					s32 nIndexID = pAIMesh->GetPolygonVertex(nPolyIndex, j);

					pcMesh->m_unapIndecies[Index] = static_cast<u16>(nIndexID);

					//代入ずみのとこでてこないの？
					FbxVector4 pos, nor;
					pos = pAIMesh->GetControlPointAt(nIndexID);
					pAIMesh->GetPolygonVertexNormal(nPolyIndex, j, nor);

					FbxStringList	uvsetName;
					pAIMesh->GetUVSetNames(uvsetName);
					bool unmapped = false;
					FbxString name = uvsetName.GetStringAt(0);	//とりあえずマルチUVはあとで....
					FbxVector2 texCoord;
					pAIMesh->GetPolygonVertexUV(nPolyIndex, j, name, texCoord, unmapped);

					for (s32 nNew = pcMesh->m_caVertices.size(); nNew <= nIndexID; ++nNew)
						pcMesh->m_caVertices.push_back(BasicVertex());	//あらかじめ確保

					BasicVertex& vert = pcMesh->m_caVertices[nIndexID];
					vert.v3Position.x = (f32)pos[0];
					vert.v3Position.y = (f32)pos[1];
					vert.v3Position.z = (f32)pos[2];

					vert.v3Normal.x = (f32)nor[0];
					vert.v3Normal.y = (f32)nor[1];
					vert.v3Normal.z = (f32)nor[2];

					vert.v2TexCoord.x = (f32)texCoord[0];
					vert.v2TexCoord.y = 1.0f - (f32)texCoord[1];

					Index++;	//追加分
				}
			}

			pcMesh->m_unVtxStride = sizeof(BasicVertex);
		}
	}
}

//文字列操作。ファイル名取り出し
string GetFileName(const string &path)
{
	size_t pos1;

	pos1 = path.rfind('\\');
	if (pos1 != string::npos) {
		return path.substr(pos1 + 1, path.size() - pos1 - 1);
	}

	pos1 = path.rfind('/');
	if (pos1 != string::npos) {
		return path.substr(pos1 + 1, path.size() - pos1 - 1);
	}

	return path;
}

//----------------------------------------------------------------------------
/*	@brief	テクスチャ読み込み
@param[in]	pMaterial	マテリアル
@param[in]	pMaterialName	読み込みテクスチャタイプ
*/
//----------------------------------------------------------------------------
bool CLoaderModel_FBX::LoadTextureCore(FbxSurfaceMaterial* pMaterial, const char* pMaterialName)
{
	bool bSuccess = false;

	//TCHAR DebugStr[512];

	FbxProperty lProperty = pMaterial->FindProperty(pMaterialName);

	//レイヤーテクスチャは非対応
	int lLayeredTextureCount = lProperty.GetSrcObjectCount<FbxLayeredTexture>();
	if (lLayeredTextureCount == 0)
	{
		int lNbTextures = lProperty.GetSrcObjectCount<FbxFileTexture>();

		for (s32 i = 0; i < lNbTextures; ++i)
		{
			FbxFileTexture* lTexture = FbxCast<FbxFileTexture>(lProperty.GetSrcObject<FbxTexture>(i));
			if (lTexture)
			{
				CAssetManager* pcResM = CAssetManager::GetInstance();
				if (pcResM)
				{
					TCHAR strwideFileName[TEXTURE_FILE_PATH] = { 0 };
					mbstowcs(strwideFileName, lTexture->GetRelativeFileName(), strlen(lTexture->GetRelativeFileName()));

					CAssetTexture* pcTex = NS_STATIC_CAST(CAssetTexture*, pcResM->GetAsset(strwideFileName));
					if (!pcTex)
					{
						//ファイル名だけ取り出す
						TCHAR strfileName[TEXTURE_FILE_PATH] = { 0 };
						string filename = GetFileName(lTexture->GetRelativeFileName());
						mbstowcs(strfileName, filename.c_str(), strlen(filename.c_str()));

						//リソースマネージャに存在しないので読み込み
						TCHAR strwideFileNameFix[TEXTURE_FILE_PATH] = { 0 };
						swprintf(strwideFileNameFix, _T("resource\\Graphics\\Texture\\%s"), strfileName);

						SInitTextureParam cParam;
						wcscpy_s(cParam.strTexName, TEXTURE_FILE_PATH, strwideFileNameFix);
						cParam.bTGA = true;	//@todo 基本TGAで
						pcTex = pcResM->LoadTexture(cParam);
#if defined(_DEBUG)
						if (pcTex == nullptr)
						{
							NS_Report(_T("Error!Load Texture[%s]\n"), cParam.strTexName);
						}
#endif
					}

					if (pcTex)
					{
						//マテリアル作って設定
						CMaterial* pcMat = nullptr;
						CLASS_NEW(pcMat, CMaterial());
						if (pcMat)
						{
							pcMat->SetDiffuseTextureID(pcTex->GetAssetUID());
						}
						m_pcModelData->AddMaterial(pcMat);
					}
					bSuccess = pcTex != nullptr;
				}
			}
		}
	}

	return bSuccess;
}


//----------------------------------------------------------------------------
/*	@brief	マテリアルデータ読み込み
*/
//----------------------------------------------------------------------------
void CLoaderModel_FBX::LoadMaterial()
{
	if (m_pFBXScene == nullptr)
	{
		return;
	}

#if true
	for (u32 i = 0; i < m_paTempFBXMesh.size(); ++i)
	{
		FbxMesh* pMesh = m_paTempFBXMesh[i];
		if (pMesh != nullptr)
		{
			FbxNode* pNode = pMesh->GetNode();
			//s32 nMaterialNum = pMesh->GetElementMaterialCount();	これだと一つのメッシュに２つ以上のマテリアルが使用されていた場合対応できず
			s32 nMaterialNum = pNode->GetMaterialCount();
			for (s32 j = 0; j < nMaterialNum; ++j)
			{
				//マテリアル読み込み
				FbxSurfaceMaterial* pMat = pNode->GetMaterial(j);
				if (pMat != nullptr)
				{
					//テクスチャロード(とりあえずDiffuseのみ)
					if (LoadTextureCore(pMat, FbxSurfaceMaterial::sDiffuse))
					{
						CAssetMesh* pcTempMesh = m_paTempAssetMesh[i];
						if (pcTempMesh != nullptr)
						{
							//どのマテリアルがどのメッシュに使われているかの情報の構築が必要
							pcTempMesh->SetMaterialIndex(m_pcModelData->GetMaterialNum() - 1);
						}
					}
				}
			}

		}
	}

#else
	s32 nMaterialCount = m_pFBXScene->GetMaterialCount();

	for (int i = 0; i < nMaterialCount; ++i)
	{
		auto fbxMaterial = m_pFBXScene->GetMaterial(i);
		if (fbxMaterial != nullptr)
		{
			//@todo テクスチャロード(とりあえずDiffuseのみ)
			LoadTextureCore(fbxMaterial, FbxSurfaceMaterial::sDiffuse);
		}
	}

#endif
}

#define USE_NODE	//親子関係の構築に、ノード情報も使用する

//----------------------------------------------------------------------------
/*
@brief	スキン情報の読み込み
@return	bool	スキン情報有無フラグ
*/
//----------------------------------------------------------------------------
void CLoaderModel_FBX::LoadSkin()
{
	for (u32 i = 0; i < m_paTempAssetMesh.size(); ++i)
	{
		LoadSkinCore(m_pFBXScene, m_paTempAssetMesh[i], m_paTempFBXMesh[i]);
	}

#if defined(USE_NODE)
	// BoneとNodeを組み合わせて関係を構築するため、Nodeも仮想ボーンとして扱うことにする
	// @todo データ量がもったいないので要検討
	// @todo ノードデータも読み込むので、できれば必要なモデルデータのみとしたいところ
	struct BuildBoneHierarchy
	{
		static void run(CAssetModel* pcAssetModel, FbxNode* pNode)
		{
			if (pNode != nullptr)
			{
				TCHAR strBoneName[256] = { 0 };
				mbstowcs(strBoneName, pNode->GetName(), strlen(pNode->GetName()));
				CJoint* pcJoint = pcAssetModel->GetJoint(strBoneName);

				if (pcJoint == nullptr)
				{
					//ノードとして追加
					CJoint* pcNewJoint = new CJoint;
					pcNewJoint->SetJointName(strBoneName);
					pcNewJoint->SetJointID(pcAssetModel->GetJointNum());
					pcNewJoint->SetJointFlag(false);

					//メッシュ空間からのローカル座標へのオフセット行列を設定しておく
					fbxsdk::FbxAMatrix cMat;
					//cluster->GetTransformMatrix(cMat);
					cMat = pNode->EvaluateGlobalTransform();
#if true
					convertXMMatrix(cMat, &pcNewJoint->m_InitMat);
					NSVector4 Determinant;
					pcNewJoint->m_invMatrix = XMMatrixInverse(&Determinant, pcNewJoint->m_InitMat);
#else
					convertXMMatrix(cMat, &pJoint->m_invMatrix);
					NSVector4 Determinant;
					pJoint->m_InitMat = XMMatrixInverse(&Determinant, pJoint->m_invMatrix);
#endif

					//あくまでノードなので、何も行わない行列として登録
					//FbxMatrix mat;
					//convertXMMatrix(mat, &pcNewJoint->m_InitMat);	//@todo これいる？
					//NSVector4 Determinant;
					//pcNewJoint->m_invMatrix = XMMatrixInverse(&Determinant, pcNewJoint->m_InitMat);

					pcAssetModel->AddJoint(pcNewJoint);
				}

				for (s32 i = 0; i < pNode->GetChildCount(); ++i)
				{
					run(pcAssetModel, pNode->GetChild(i));
				}
			}
		}
	};
	BuildBoneHierarchy::run(m_pcModelData, m_pFBXScene->GetRootNode());
#endif

	//親子関係構築
	if (m_pcModelData->GetJointNum() > 0)
	{
		for (u32 i = 0; i < m_pcModelData->GetJointNum(); ++i)
		{
			CJoint* pcJoint = m_pcModelData->GetJoint(i);
			FbxNode* pNode = FindNode(m_pFBXScene->GetRootNode(), pcJoint->GetJointName());	//ボーン名から、ノードを探索する
			if (pNode != nullptr)
			{
				//親ボーンの設定。
				if (pNode->GetParent())
				{
					TCHAR strBoneName[1024] = { 0 };
					mbstowcs(strBoneName, pNode->GetParent()->GetName(), strlen(pNode->GetParent()->GetName()));
					CJoint* pParent = m_pcModelData->GetJoint(strBoneName);

					if (pParent)
					{
						pcJoint->m_pParent = pParent;
					}
				}

				//子供、兄弟ボーンの設定
				CJoint* pcTempJoint = NULL;
				for (s32 k = 0; k < pNode->GetChildCount(); k++)
				{
					TCHAR strBoneName[1024] = { 0 };
					mbstowcs(strBoneName, pNode->GetChild(k)->GetName(), strlen(pNode->GetChild(k)->GetName()));
					CJoint* pcChild = m_pcModelData->GetJoint(strBoneName);

					if (!pcJoint->m_pFirstChild)
					{
						//第1子設定
						pcJoint->m_pFirstChild = pcChild;
						pcTempJoint = pcChild;
					}
					else
					{
						//他にも兄弟がいるなら、それらは兄弟設定
						pcTempJoint->m_pSibling = pcChild;
						pcTempJoint = pcChild;
					}
				}

			}
#if defined(_DEBUG)
			else
			{
				NS_Report(_T("can't find node[%s]\n"), pcJoint->GetJointName());
			}
#endif
		}

		for (u32 i = 0; i < m_pcModelData->GetJointNum(); ++i)
		{
			CJoint* pcJoint = m_pcModelData->GetJoint(i);
			if (pcJoint->m_pFirstChild && !pcJoint->m_pParent)
			{
				//ルートジョイント確定。
				m_pcModelData->SetRootJointID(pcJoint->GetJointID());
			}
		}

#if !defined(USE_NODE)
		m_pcModelData->m_paJoint[0]->m_pSibling = m_pcModelData->m_paJoint[1];
		m_pcModelData->m_paJoint[1]->m_pSibling = m_pcModelData->m_paJoint[2];

		m_pcModelData->m_unRootJointID = 0;
#endif

#if true
		//各ジョイントのローカル座標を親からの相対位置に設定
		struct CalcRelativeMat
		{
			static void run(CJoint* me, NSMatrix *parentoffsetMat)
			{
				if (me->m_pFirstChild)
				{
					run(me->m_pFirstChild, &me->m_invMatrix);
				}
				if (me->m_pSibling)
				{
					run(me->m_pSibling, parentoffsetMat);
				}
				if (parentoffsetMat)
				{
					me->m_InitMat *= *parentoffsetMat;
				}
			}
		};
		if (m_pcModelData->GetJointNum() > 0)
		{
			CJoint* rootJoint = m_pcModelData->GetRootJoint();
			if (rootJoint)
			{
				CalcRelativeMat::run(rootJoint, 0);
			}
		}
#endif
	}
}

//---------------------------------------------------------------------------
/*!
@brief		スキンの初期化
@param[in]	pScene
@param[in]	pMesh(独自メッシュ)
@param[in]	aMesh(Assimpメッシュ)
*/
//---------------------------------------------------------------------------
void CLoaderModel_FBX::LoadSkinCore(const FbxScene* pScene, CAssetMesh* pMesh, FbxMesh* aMesh)
{
	if (aMesh == nullptr || pScene == nullptr || pMesh == nullptr)
	{
		return;
	}

	s32 DeformerCount = aMesh->GetDeformerCount(FbxDeformer::eSkin);
	if (DeformerCount == 0)
	{
		//スキン情報なし
		return;
	}

	//この時点でスキンメッシュが確定
	pMesh->SetSkinMeshFlag(true);

	for (s32 nDefIndex = 0; nDefIndex < DeformerCount; ++nDefIndex)
	{
		// i番目のスキンを取得
		FbxSkin* pSkin = (FbxSkin*)aMesh->GetDeformer(nDefIndex, FbxDeformer::eSkin);

		// クラスター(ボーン)の数を取得
		s32 clusterNum = pSkin->GetClusterCount();

		for (s32 nBoneIdx = 0; nBoneIdx < clusterNum; ++nBoneIdx)
		{
			//ボーンデータの取得
			FbxCluster* cluster = pSkin->GetCluster(nBoneIdx);

			//ボーン情報の登録
			TCHAR strBoneName[1024] = { 0 };
			mbstowcs(strBoneName, cluster->GetLink()->GetName(), strlen(cluster->GetLink()->GetName()));

			CJoint* pcExistJoint = m_pcModelData->GetJoint(strBoneName);
			if (!pcExistJoint)
			{
				CJoint* pJoint = new CJoint;
				pJoint->SetJointID(m_pcModelData->GetJointNum());
				pJoint->SetJointFlag(true);

				pJoint->SetJointName(strBoneName);

				//メッシュ空間からのローカル座標へのオフセット行列を設定しておく
				fbxsdk::FbxAMatrix cMat;
				//cluster->GetTransformMatrix(cMat);
				cMat = cluster->GetLink()->EvaluateGlobalTransform();
#if true
				convertXMMatrix(cMat, &pJoint->m_InitMat);
				NSVector4 Determinant;
				pJoint->m_invMatrix = XMMatrixInverse(&Determinant, pJoint->m_InitMat);
#else
				convertXMMatrix(cMat, &pJoint->m_invMatrix);
				NSVector4 Determinant;
				pJoint->m_InitMat = XMMatrixInverse(&Determinant, pJoint->m_invMatrix);
#endif
				m_pcModelData->AddJoint(pJoint);
				pcExistJoint = pJoint;
			}

			//ボーンが与える頂点インデックス値と、影響度を頂点バッファへ
			int	pointNum = cluster->GetControlPointIndicesCount();
			int*	pointAry = cluster->GetControlPointIndices();
			double* weightAry = cluster->GetControlPointWeights();

			for (s32 nPtIdx = 0; nPtIdx < pointNum; ++nPtIdx)
			{
				// 頂点インデックスとウェイトを取得
				s32 vID = pointAry[nPtIdx];				//頂点インデクス
				f32 weight = (f32)weightAry[nPtIdx];	//ウェイト

				s32 nAvailableIndex = -1;
				for (s32 k = 0; k < 4; ++k)
				{
					if (pMesh->m_caVertices[vID].m_faWeight[k] <= 0.0f)
					{
						//ウェイトがまだ入っていない箇所を空きインデックスとする
						nAvailableIndex = k;
						break;
					}

					//@todo 大きなウェイト対応もいるかな
				}
				NS_ASSERT(nAvailableIndex < 4);
				//最後のおもさはシェーダ内で、1.0からひいたもので計算するので入れる必要はない
				if (nAvailableIndex < 4)
				{
					pMesh->m_caVertices[vID].m_faWeight[nAvailableIndex] = weight;
				}

				pMesh->m_caVertices[vID].m_naBoneIndex[nAvailableIndex] = pcExistJoint->GetJointID();	//使用するボーンインデックス

			}
		}
	}
}

bool CLoaderModel_FBX::SetKeyParam(s32 nTimeSpan, CMotion* pAddMot, FbxNode* pNode, FbxAnimLayer* pLayer, const char* type)
{
	if (pAddMot == nullptr || pNode == nullptr || pLayer == nullptr || type == nullptr)
	{
		return false;
	}

	TCHAR strJointName[1024] = { 0 };
	mbstowcs(strJointName, pNode->GetName(), strlen(pNode->GetName()));
	CJoint* pcJoint = m_pcModelData->GetJoint(strJointName);
	NS_ASSERT(pcJoint);	//ジョイントは前処理で追加済みのはず

	s32 nNodeID = pcJoint->GetJointID();
	s32 nTypeMax = 3;	//XYZなので3固定
	s32 nCurveType = 0;	//0::Trans 1::Rot 2::Scale

						//XYZのカーブをまとめて処理
	std::vector<FbxAnimCurve*> paCurve;
	paCurve.reserve(nTypeMax);
	for (s32 i = 0; i < nTypeMax; ++i)
		paCurve.push_back(nullptr);

	f32 fConf = 1.0f;
	if (!strcmp(type, FBXSDK_CURVENODE_TRANSLATION))
	{
		//座標のキーフレーム
		nCurveType = 0;
		paCurve[0] = pNode->LclTranslation.GetCurve(pLayer, FBXSDK_CURVENODE_COMPONENT_X);
		paCurve[1] = pNode->LclTranslation.GetCurve(pLayer, FBXSDK_CURVENODE_COMPONENT_Y);
		paCurve[2] = pNode->LclTranslation.GetCurve(pLayer, FBXSDK_CURVENODE_COMPONENT_Z);
		fConf = 0.01f;	//入ってくる値大きいので係数かける
	}
	else if (!strcmp(type, FBXSDK_CURVENODE_ROTATION))
	{
		//回転キーフレーム
		nCurveType = 1;
		paCurve[0] = pNode->LclRotation.GetCurve(pLayer, FBXSDK_CURVENODE_COMPONENT_X);
		paCurve[1] = pNode->LclRotation.GetCurve(pLayer, FBXSDK_CURVENODE_COMPONENT_Y);
		paCurve[2] = pNode->LclRotation.GetCurve(pLayer, FBXSDK_CURVENODE_COMPONENT_Z);
		fConf = -1.0f;	//反転
	}
	else if (!strcmp(type, FBXSDK_CURVENODE_SCALING))
	{
		//スケールキーフレーム
		nCurveType = 2;
		paCurve[0] = pNode->LclScaling.GetCurve(pLayer, FBXSDK_CURVENODE_COMPONENT_X);
		paCurve[1] = pNode->LclScaling.GetCurve(pLayer, FBXSDK_CURVENODE_COMPONENT_Y);
		paCurve[2] = pNode->LclScaling.GetCurve(pLayer, FBXSDK_CURVENODE_COMPONENT_Z);
	}

	//キーの最大数確定(XYZのうち一番キーを持っているものを基準とする)
	s32 nKeyCount = 0;
	for (s32 j = 0; j < nTypeMax; ++j)
	{
		if (paCurve[j] != nullptr)
		{
			s32 nTemp = paCurve[j]->KeyGetCount();
			if (nKeyCount < nTemp)
				nKeyCount = nTemp;
		}
	}

	if (nKeyCount == 0)
	{
		//アニメーション情報が見当たらなかった
		return false;
	}

	std::vector<NSVector3> vaValue;	//キー数分用意
	vaValue.reserve(nKeyCount);
	std::vector<f32> faValueTime;	//タイム
	faValueTime.reserve(nKeyCount);

	for (s32 j = 0; j < nKeyCount; ++j)
	{
		NSVector3 vec;
		vaValue.push_back(vec);
		faValueTime.push_back(0.0f);

		NS_ASSERT(paCurve[0]);
		NS_ASSERT(paCurve[1]);
		NS_ASSERT(paCurve[2]);

		faValueTime[j] = (f32)paCurve[0]->KeyGetTime(j).GetSecondDouble();	//キータイムはXYZ固定のハズなので0番用いる

		//係数かけて計算
		vaValue[j].x = paCurve[0]->KeyGetValue(j) * fConf;
		vaValue[j].y = paCurve[1]->KeyGetValue(j) * fConf;
		vaValue[j].z = paCurve[2]->KeyGetValue(j) * fConf;

		//@note SlimeのJoint2のスケールZに、なぜ0.0が入ってくるかわからないので臨時対応。
		if (vaValue[j].z <= 0.0f)
			vaValue[j].z = 1.0f;
	}

	for (s32 j = 0; j < nKeyCount; ++j)
	{
		//データ追加
		if (nCurveType == 0 || nCurveType == 2)
		{
			if (nCurveType == 0)
			{
				//座標
				pAddMot->AddKeyPos(vaValue[j], faValueTime[j] * nTimeSpan, nNodeID);
			}
			else if (nCurveType == 2)
			{
				//スケール
				pAddMot->AddKeyScale(vaValue[j], faValueTime[j] * nTimeSpan, nNodeID);
			}
		}
		else if (nCurveType == 1)
		{
			//回転キーフレーム
			NSQuat vRot = DirectX::XMQuaternionRotationRollPitchYaw(
				UtilMath::DegToRad * vaValue[j].x,
				UtilMath::DegToRad * vaValue[j].y,
				UtilMath::DegToRad * vaValue[j].z);

			pAddMot->AddKeyRotate(vRot, faValueTime[j] * nTimeSpan, nNodeID);
		}
	}

	return true;
}

//---------------------------------------------------------------------------
/*!
@brief	モーション、アニメーションデータ情報読み込み
@param[in]	pScene	シーン情報
*/
//---------------------------------------------------------------------------
void CLoaderModel_FBX::LoadMotion(FbxScene* pScene)
{
	if (pScene == nullptr)
	{
		return;
	}

	// アニメーション数取得
	s32 nAnimStackCount = pScene->GetSrcObjectCount<FbxAnimStack>();
	if (nAnimStackCount <= 0)
	{
		return;
	}

	// ボーン名からアニメの対象を確定
	for (s32 nAnimStackIdx = 0; nAnimStackIdx < nAnimStackCount; ++nAnimStackIdx)
	{
		FbxAnimStack* pAnimStack = pScene->GetSrcObject<FbxAnimStack>(nAnimStackIdx);
		if (pAnimStack != nullptr)
		{
			CMotion* pcMotion = nullptr;
			CLASS_NEW(pcMotion, CMotion());

			TCHAR strAnimName[128] = { 0 };
			mbstowcs(strAnimName, pAnimStack->GetName(), strlen(pAnimStack->GetName()));

			pcMotion->SetMotionName(strAnimName);

			//時間周りの設定
			FbxTime fbxPeriodTime;	//! 単位時間
			FbxTime fbxStartTime;	//! スタート時間
			FbxTime fbxStopTime;	//! ストップ時間

									// テイクから基準時間を取得
			FbxArray<FbxString*> takeNameAry;
			pScene->FillAnimStackNameArray(takeNameAry);

			bool isTakeExist = false;
			for (s32 i = 0; i < takeNameAry.GetCount(); ++i)
			{
				FbxTakeInfo* currentTakeInfo = pScene->GetTakeInfo(*(takeNameAry[i]));
				if (currentTakeInfo)
				{
					fbxStartTime = currentTakeInfo->mLocalTimeSpan.GetStart();
					fbxStopTime = currentTakeInfo->mLocalTimeSpan.GetStop();
					isTakeExist = true;
					break;
				}
			}

			// テイクが確認できなかったらデフォルト時間を取得
			if (isTakeExist == false) {
				FbxTimeSpan timeLineTimeSpan;
				pScene->GetGlobalSettings().GetTimelineDefaultTimeSpan(timeLineTimeSpan);
				fbxStartTime = timeLineTimeSpan.GetStart();
				fbxStopTime = timeLineTimeSpan.GetStop();
			}

			// 時間モードから単位時間を算出
			FbxGlobalSettings &globalTimeSettings = pScene->GetGlobalSettings();
			FbxTime::EMode timeMode = globalTimeSettings.GetTimeMode();
			fbxPeriodTime.SetTime(0, 0, 0, 1, 0, timeMode);

			s32 startFrame_ = (int)(fbxStartTime.Get() / fbxPeriodTime.Get());
			s32 stopFrame_ = (int)(fbxStopTime.Get() / fbxPeriodTime.Get());

			// 1フレームの時間を算出
			FbxTime framePerSecTime;
			framePerSecTime.SetTime(0, 0, 1, 0, 0, timeMode);
			s32 framePerSec_ = (int)(framePerSecTime.Get() / fbxPeriodTime.Get());

			pcMotion->SetDuration(static_cast<f32>(stopFrame_ - startFrame_));	//アニメーションの長さ
			FbxArrayDelete(takeNameAry);

			pcMotion->SetMotionID(m_pcModelData->GetMotionDataNum());

			m_pcModelData->AddMotionData(pcMotion);

			//レイヤーが1を超える場合はどういったアニメーションになるのか？
			////とりあえずレイヤーは0のみ対応
			int nAnimLayers = pAnimStack->GetMemberCount<FbxAnimLayer>();
			NS_ASSERT(nAnimLayers == 1);
			//if (nAnimLayers > 0)
			{
				FbxAnimLayer* pAnimLayer = pAnimStack->GetMember<FbxAnimLayer>(0);
				if (pAnimLayer != nullptr)
				{
					//ジョイント分のキーデータ・セット
					for (u32 i = 0; i < m_pcModelData->GetJointNum(); ++i)
					{
						char strBoneName[128] = { 0 };
						mbstowcs(strAnimName, pAnimStack->GetName(), strlen(pAnimStack->GetName()));

						sprintf(strBoneName, "%S", m_pcModelData->GetJoint(i)->GetJointName());
						FbxString str;
						str.Append(strBoneName, strlen(strBoneName));
						FbxNode* pNode = pScene->FindNodeByName(str);

						if (pNode != nullptr)
						{
							SetKeyParam(framePerSec_, pcMotion, pNode, pAnimLayer, FBXSDK_CURVENODE_TRANSLATION);
							SetKeyParam(framePerSec_, pcMotion, pNode, pAnimLayer, FBXSDK_CURVENODE_ROTATION);
							SetKeyParam(framePerSec_, pcMotion, pNode, pAnimLayer, FBXSDK_CURVENODE_SCALING);
						}
					}
				}
			}
		}
	}
}

//---------------------------------------------------------------------------
/*!
@brief		ノード探索
@param[in]	aNode	ノード情報
@param[in]	nodename	ノード文字列
@return aiNode 一致ノード
*/
//---------------------------------------------------------------------------
FbxNode* CLoaderModel_FBX::FindNode(FbxNode* pNode, const TCHAR* nodename)
{
	TCHAR nodeSourceName[1024] = { 0 };
	mbstowcs(nodeSourceName, pNode->GetName(), strlen(pNode->GetName()));
	if (!wcscmp(nodename, nodeSourceName))
	{
		return pNode;	//名前が一致した
	}

	for (s32 i = 0; i < pNode->GetChildCount(); i++)
	{
		//ノードの子を探索
		FbxNode* FoundNode = FindNode(pNode->GetChild(i), nodename);
		if (FoundNode)
		{
			return FoundNode;
		}
	}

	return NULL;
}

//EOF
