
#if defined( _DEBUG )

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		DebugGeometry.cpp
	@brief		デバッグ用ジオメトリ基底クラス
	@author		shiba
	@date		2012/9/9 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#include "DebugGeometry.h"

#include "FrameWork/Rendering/Model.h"

#include "FrameWork/Asset/AssetManager.h"
#include "FrameWork/Asset/AssetMesh.h"
#include "FrameWork/Asset/AssetModel.h"

#include "FrameWork/Rendering/Joint.h"

using namespace DirectX;

//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CDebugGeometry::CDebugGeometry()
: m_pcDebugGeomModel(nullptr)
{
}

//---------------------------------------------------------------------------
/*!
	@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CDebugGeometry::~CDebugGeometry()
{
	Term();
}

//---------------------------------------------------------------------------
/*!
@brief	破棄
*/
//---------------------------------------------------------------------------
void CDebugGeometry::Destroy()
{
	Term();
}

void CDebugGeometry::Term()
{
	SAFE_DELETE_TASK(m_pcDebugGeomModel);
}

//---------------------------------------------------------------------------
/*!
	@brief	初期化
*/
//---------------------------------------------------------------------------
bool CDebugGeometry::Init()
{
	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	ジオメトリセットアップ
*/
//---------------------------------------------------------------------------
void CDebugGeometry::SetupGeom(E_GEOM_TYPE eGeomType)
{
	if (m_pcDebugGeomModel == nullptr)
	{
		//メッシュの作成関数を呼び出し
		CLASS_NEW(m_pcDebugGeomModel, CModel());

		if (eGeomType == E_GEOM_TYPE::eCUBE)
		{
			//バッファ生成
			struct BasicVertex VertexDataA[] =
			{
				{ NSVector3(-1.0f, -1.0f,-1.0f),	 NSVector2(0.0f, 1.0f),	NSVector3(1.0f, 1.0f, 1.0f), },
				{ NSVector3(1.0f,	 -1.0f,-1.0f),	 NSVector2(0.0f, 1.0f),	NSVector3(1.0f, 1.0f, 1.0f), },
				{ NSVector3(-1.0f, 1.0f, -1.0f),	 NSVector2(0.0f, 1.0f),	NSVector3(1.0f, 1.0f, 1.0f), },
				{ NSVector3(1.0f,	 1.0f, -1.0f),	 NSVector2(0.0f, 1.0f),	NSVector3(1.0f, 1.0f, 1.0f), },
				{ NSVector3(-1.0f, -1.0f, 1.0f),	 NSVector2(0.0f, 1.0f),	NSVector3(1.0f, 1.0f, 1.0f), },
				{ NSVector3(1.0f,	 -1.0f, 1.0f),	 NSVector2(0.0f, 1.0f),	NSVector3(1.0f, 1.0f, 1.0f), },
				{ NSVector3(-1.0f, 1.0f,	1.0f),	 NSVector2(0.0f, 1.0f),	NSVector3(1.0f, 1.0f, 1.0f), },
				{ NSVector3(1.0f,	 1.0f,	1.0f),	 NSVector2(0.0f, 1.0f),	NSVector3(1.0f, 1.0f, 1.0f), },
			};

			//インデックスバッファの数
			// 板ポリ = 12個 * 3頂点 = 36
			u16 idxVertexID[] =
			{
				0,1,2,1,3,2,
				4,0,6,0,2,6,
				5,4,7,4,6,7,
				1,5,3,5,7,3,
				4,5,0,5,1,0,
				6,7,2,7,3,2,
			};
			
			//メッシュを追加,モデルへ割当
			CAssetModel* pcAssetModel = nullptr;
			CLASS_NEW(pcAssetModel, CAssetModel());
			if (pcAssetModel != nullptr)
			{
				CAssetMesh* pcMesh = nullptr;
				CLASS_NEW(pcMesh, CAssetMesh());
				if (pcMesh != nullptr)
				{
					pcMesh->SetVertexData(VertexDataA, 8);
					pcMesh->SetIndexData(idxVertexID, 36);
					pcMesh->CreateBuffer();
					pcAssetModel->AddMesh(pcMesh);
				}
			}
			m_pcDebugGeomModel->SetModelData(pcAssetModel);

			//アセットマネージャへの登録がいるはず
			//@todo 

			//CRenderingContext cContext;
			//cContext.SetShader(CShaderManager::eSHADERID_MRT);
			//SetRenderContext(cContext);

			SetDrawEnable(true, CRenderingManager::E_RENDER_STAGE_MRT_POS_NOM_DIF);
		}
		else if (eGeomType == E_GEOM_TYPE::eSKIN)
		{
			//テストスキンメッシュセットアップ
			//AssetMeshを作成して、登録する
			struct BasicVertex caVertex[15] =
			{
				//Pos/TexCorrd/Normal/Weight/bone
				{ NSVector3(-0.5000f, -2.2887f, 0.0f), NSVector2(0.0f, 1.0f),NSVector3(0.0f, 0.0f, 1.0f),NSVector3(1.00f, 0.00f, 0.00f),{ 2, 0, 0, 0 } },
				{ NSVector3(-0.5000f, -1.2887f, 0.0f), NSVector2(0.0f, 1.0f),NSVector3(0.0f, 0.0f, 1.0f),NSVector3(0.50f, 0.50f, 0.00f),{ 1, 2, 0, 0 } },
				{ NSVector3(-0.5000f, -0.2887f, 0.0f), NSVector2(0.0f, 1.0f),NSVector3(0.0f, 0.0f, 1.0f),NSVector3(1.00f, 0.00f, 0.00f),{ 0, 0, 0, 0 } },
				{ NSVector3(-1.3660f,  0.2113f, 0.0f), NSVector2(0.0f, 1.0f),NSVector3(0.0f, 0.0f, 1.0f),NSVector3(0.50f, 0.50f, 0.00f),{ 3, 4, 0, 0 } },
				{ NSVector3(-2.2321f,  0.7113f, 0.0f), NSVector2(0.0f, 1.0f),NSVector3(0.0f, 0.0f, 1.0f),NSVector3(1.00f, 0.00f, 0.00f),{ 4, 0, 0, 0 } },
				{ NSVector3(-1.7321f,  1.5774f, 0.0f), NSVector2(0.0f, 1.0f),NSVector3(0.0f, 0.0f, 1.0f),NSVector3(1.00f, 0.00f, 0.00f),{ 4, 0, 0, 0 } },
				{ NSVector3(-0.8660f,  1.0774f, 0.0f), NSVector2(0.0f, 1.0f),NSVector3(0.0f, 0.0f, 1.0f),NSVector3(0.50f, 0.50f, 0.00f),{ 3, 4, 0, 0 } },
				{ NSVector3(0.0000f,  0.5774f, 0.0f), NSVector2(0.0f, 1.0f),NSVector3(0.0f, 0.0f, 1.0f),NSVector3(1.00f, 0.00f, 0.00f),{ 0, 0, 0, 0 } },
				{ NSVector3(0.8660f,  1.0774f, 0.0f), NSVector2(0.0f, 1.0f),NSVector3(0.0f, 0.0f, 1.0f),NSVector3(0.50f, 0.50f, 0.00f),{ 5, 6, 0, 0 } },
				{ NSVector3(1.7321f,  1.5774f, 0.0f), NSVector2(0.0f, 1.0f),NSVector3(0.0f, 0.0f, 1.0f),NSVector3(1.00f, 0.00f, 0.00f),{ 6, 0, 0, 0 } },
				{ NSVector3(2.2321f,  0.7113f, 0.0f), NSVector2(0.0f, 1.0f),NSVector3(0.0f, 0.0f, 1.0f),NSVector3(1.00f, 0.00f, 0.00f),{ 6, 0, 0, 0 } },
				{ NSVector3(1.3660f,  0.2113f, 0.0f), NSVector2(0.0f, 1.0f),NSVector3(0.0f, 0.0f, 1.0f),NSVector3(0.50f, 0.50f, 0.00f),{ 5, 6, 0, 0 } },
				{ NSVector3(0.5000f, -0.2887f, 0.0f), NSVector2(0.0f, 1.0f),NSVector3(0.0f, 0.0f, 1.0f),NSVector3(1.00f, 0.00f, 0.00f),{ 0, 0, 0, 0 } },
				{ NSVector3(0.5000f, -1.2887f, 0.0f), NSVector2(0.0f, 1.0f),NSVector3(0.0f, 0.0f, 1.0f),NSVector3(0.50f, 0.50f, 0.00f),{ 1, 2, 0, 0 } },
				{ NSVector3(0.5000f, -2.2887f, 0.0f), NSVector2(0.0f, 1.0f),NSVector3(0.0f, 0.0f, 1.0f),NSVector3(1.00f, 0.00f, 0.00f),{ 2, 0, 0, 0 } },

			};

			// 頂点インデックス
			// ポリゴン数は13枚で三角形リストなので13×3 = 39個あります
			u16 idx[39] = {
				0, 1, 14,
				1, 13, 14,
				1, 2, 13,
				2, 12, 13,
				2, 7, 12,
				2, 6, 7,
				2, 3, 6,
				3, 5, 6,
				3, 4, 5,
				7, 8, 12,
				8, 11, 12,
				8, 9, 11,
				9, 10, 11,
			};

			//メッシュを追加,モデルへ割当
			CAssetModel* pcAssetModel = nullptr;
			CLASS_NEW(pcAssetModel, CAssetModel());
			if (pcAssetModel != nullptr)
			{
				pcAssetModel->AddMesh(new CAssetMesh());
				CAssetMesh* pcMesh = pcAssetModel->GetMesh(0);
				if (pcMesh != nullptr)
				{
					pcMesh->SetVertexData(caVertex, 15);
					pcMesh->SetIndexData(idx, 39);
					pcMesh->CreateBuffer();
				}
			}

			m_pcDebugGeomModel->SetModelData(pcAssetModel);

			//--------------------------------------------------------------------
			// ジョイントの作成
			//--------------------------------------------------------------------
			CJoint* caJoint[7];
			for (s32 i = 0; i < 7; ++i)
			{
				caJoint[i] = new CJoint();
			}

			////親子関係構築
			pcAssetModel->SetRootJointID(0);

			caJoint[0]->m_pFirstChild = caJoint[1];
			caJoint[1]->m_pFirstChild = caJoint[2];
			caJoint[3]->m_pFirstChild = caJoint[4];
			caJoint[5]->m_pFirstChild = caJoint[6];
			caJoint[1]->m_pSibling = caJoint[3];
			caJoint[3]->m_pSibling = caJoint[5];

			// 初期姿勢の計算
			// まずはローカル姿勢を設定して
			// 最終的に自分の親からの相対姿勢に直します。
			caJoint[0]->m_InitMat = XMMatrixRotationZ(UtilMath::DegreeToRadian(-90.0f));
			caJoint[1]->m_InitMat = XMMatrixRotationZ(UtilMath::DegreeToRadian(-90.0f));
			caJoint[2]->m_InitMat = XMMatrixRotationZ(UtilMath::DegreeToRadian(-90.0f));
			caJoint[3]->m_InitMat = XMMatrixRotationZ(UtilMath::DegreeToRadian(150.0f));
			caJoint[4]->m_InitMat = XMMatrixRotationZ(UtilMath::DegreeToRadian(150.0f));
			caJoint[5]->m_InitMat = XMMatrixRotationZ(UtilMath::DegreeToRadian(30.0f));
			caJoint[6]->m_InitMat = XMMatrixRotationZ(UtilMath::DegreeToRadian(30.0f));
			caJoint[0]->m_InitMat._41 = 0.0000f; caJoint[0]->m_InitMat._42 = 0.0000f;
			caJoint[1]->m_InitMat._41 = 0.0000f; caJoint[1]->m_InitMat._42 = -1.0000f;
			caJoint[2]->m_InitMat._41 = 0.0000f; caJoint[2]->m_InitMat._42 = -2.0000f;
			caJoint[3]->m_InitMat._41 = -0.6830f; caJoint[3]->m_InitMat._42 = 0.3943f;
			caJoint[4]->m_InitMat._41 = -1.5490f; caJoint[4]->m_InitMat._42 = 0.8943f;
			caJoint[5]->m_InitMat._41 = 0.6830f; caJoint[5]->m_InitMat._42 = 0.3943f;
			caJoint[6]->m_InitMat._41 = 1.5490f; caJoint[6]->m_InitMat._42 = 0.8943f;

			// ボーンオフセット行列を計算しておきます。
			// オフセット行列は各ボーンの「ローカル姿勢」の逆行列です。
			m_pcDebugGeomModel->m_paCombMatrix = new NSMatrix[7];    // 合成変換行列。これがシェーダに渡ります
			for (int i = 0; i < 7; i++)
			{
				caJoint[i]->SetJointID((u16)i);

				NSVector4 determinant;
				caJoint[i]->m_invMatrix = XMMatrixInverse(&determinant, caJoint[i]->m_InitMat);
			}

			// 初期姿勢を親の姿勢からの相対姿勢に直します。
			// まず子の末端まで下りて、自分のローカル空間での初期姿勢 × 親のボーンオフセット行列で相対姿勢が出ます
			// 親子関係を辿るので再起関数が必要です。
			struct CalcRelativeMat
			{
				static void run(CJoint* me, NSMatrix *parentoffsetMat)
				{
					if (me->m_pFirstChild)
						run(me->m_pFirstChild, &me->m_invMatrix);
					if (me->m_pSibling)
						run(me->m_pSibling, parentoffsetMat);
					if (parentoffsetMat)
						me->m_InitMat *= *parentoffsetMat;
				}
			};
			CalcRelativeMat::run(caJoint[0], 0);

			for (int i = 0; i < 7; i++)
			{
				pcAssetModel->AddJoint(caJoint[i]);
			}
		}
	}
}


#endif	//	_DEBUG

//EOF