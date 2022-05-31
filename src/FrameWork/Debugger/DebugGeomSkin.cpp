/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		DebugGeomSkin.cpp
	@brief		検証用スキン
	@author		ta.Ishii
	@date		2012/11/4 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#if defined( _DEBUG )

#if false

#include "DebugGeomSkin.h"

#include "FrameWork/Asset/AssetMesh.h"
#include "FrameWork/Asset/AssetManager.h"
#include "FrameWork/Asset/AssetModel.h"

#include "FrameWork/Camera/CameraManager.h"
#include "FrameWork/Camera/CameraBase.h"

#include "FrameWork/Rendering/Model.h"
#include "FrameWork/Rendering/Joint.h"
#include "FrameWork/Rendering/Shader/SkinTransShader.h"
#include "FrameWork/Rendering/ModelDataType.h"

using namespace DirectX;
//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CDebugGeomSkin::CDebugGeomSkin()
	: m_pcSkinModel(nullptr)
{
}

//---------------------------------------------------------------------------
/*!
	@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CDebugGeomSkin::~CDebugGeomSkin()
{
}

//---------------------------------------------------------------------------
/*!
	@brief	メッシュ作成関数
	http://marupeke296.com/DXGSmp_No61_WhiteBoxSkinMeshAnimation.html
	から借用
*/
//---------------------------------------------------------------------------
bool CDebugGeomSkin::CreateMesh()
{
	//AssetMeshを作成して、登録する
	struct BasicVertex caVertex[15] =
	{
		//Pos/TexCorrd/Normal/Weight/bone
		{ NSVector3(-0.5000f, -2.2887f, 0.0f), NSVector2(0.0f, 1.0f),NSVector3(0.0f, 0.0f, 1.0f),NSVector3(1.00f, 0.00f, 0.00f), {2, 0, 0, 0} },
		{ NSVector3(-0.5000f, -1.2887f, 0.0f), NSVector2(0.0f, 1.0f),NSVector3(0.0f, 0.0f, 1.0f),NSVector3(0.50f, 0.50f, 0.00f), {1, 2, 0, 0} },
		{ NSVector3(-0.5000f, -0.2887f, 0.0f), NSVector2(0.0f, 1.0f),NSVector3(0.0f, 0.0f, 1.0f),NSVector3(1.00f, 0.00f, 0.00f), {0, 0, 0, 0} },
		{ NSVector3(-1.3660f,  0.2113f, 0.0f), NSVector2(0.0f, 1.0f),NSVector3(0.0f, 0.0f, 1.0f),NSVector3(0.50f, 0.50f, 0.00f), {3, 4, 0, 0} },
		{ NSVector3(-2.2321f,  0.7113f, 0.0f), NSVector2(0.0f, 1.0f),NSVector3(0.0f, 0.0f, 1.0f),NSVector3(1.00f, 0.00f, 0.00f), {4, 0, 0, 0} },
		{ NSVector3(-1.7321f,  1.5774f, 0.0f), NSVector2(0.0f, 1.0f),NSVector3(0.0f, 0.0f, 1.0f),NSVector3(1.00f, 0.00f, 0.00f), {4, 0, 0, 0} },
		{ NSVector3(-0.8660f,  1.0774f, 0.0f), NSVector2(0.0f, 1.0f),NSVector3(0.0f, 0.0f, 1.0f),NSVector3(0.50f, 0.50f, 0.00f), {3, 4, 0, 0} },
		{ NSVector3(0.0000f,  0.5774f, 0.0f), NSVector2(0.0f, 1.0f),NSVector3(0.0f, 0.0f, 1.0f),NSVector3(1.00f, 0.00f, 0.00f), {0, 0, 0, 0} },
		{ NSVector3(0.8660f,  1.0774f, 0.0f), NSVector2(0.0f, 1.0f),NSVector3(0.0f, 0.0f, 1.0f),NSVector3(0.50f, 0.50f, 0.00f), {5, 6, 0, 0} },
		{ NSVector3(1.7321f,  1.5774f, 0.0f), NSVector2(0.0f, 1.0f),NSVector3(0.0f, 0.0f, 1.0f),NSVector3(1.00f, 0.00f, 0.00f), {6, 0, 0, 0} },
		{ NSVector3(2.2321f,  0.7113f, 0.0f), NSVector2(0.0f, 1.0f),NSVector3(0.0f, 0.0f, 1.0f),NSVector3(1.00f, 0.00f, 0.00f), {6, 0, 0, 0} },
		{ NSVector3(1.3660f,  0.2113f, 0.0f), NSVector2(0.0f, 1.0f),NSVector3(0.0f, 0.0f, 1.0f),NSVector3(0.50f, 0.50f, 0.00f), {5, 6, 0, 0} },
		{ NSVector3(0.5000f, -0.2887f, 0.0f), NSVector2(0.0f, 1.0f),NSVector3(0.0f, 0.0f, 1.0f),NSVector3(1.00f, 0.00f, 0.00f), {0, 0, 0, 0} },
		{ NSVector3(0.5000f, -1.2887f, 0.0f), NSVector2(0.0f, 1.0f),NSVector3(0.0f, 0.0f, 1.0f),NSVector3(0.50f, 0.50f, 0.00f), {1, 2, 0, 0} },
		{ NSVector3(0.5000f, -2.2887f, 0.0f), NSVector2(0.0f, 1.0f),NSVector3(0.0f, 0.0f, 1.0f),NSVector3(1.00f, 0.00f, 0.00f), {2, 0, 0, 0} },

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

	if (m_pcDebugGeomMesh != nullptr)
	{
		m_pcDebugGeomMesh->SetVertexData(caVertex, 15);
		m_pcDebugGeomMesh->SetIndexData(idx, 39);
		m_pcDebugGeomMesh->CreateBuffer();
	}

	//@note DebugGeomBaseのデストラクタでRemoveされるから、注意。
	CLASS_NEW(m_pcSkinModel, CModel());

	if (m_pcSkinModel != nullptr)
	{


		CLASS_NEW(m_pcSkinModel->m_pcModelData, CAssetModel());

		m_pcSkinModel->m_pcModelData->m_paMesh.push_back(m_pcDebugGeomMesh);

		//--------------------------------------------------------------------
		// ジョイントの作成
		//--------------------------------------------------------------------
		for (s32 i = 0; i < 7; ++i)
		{
			CJoint* pJoint = new CJoint;
			m_pcSkinModel->m_pcModelData->m_paJoint.push_back(pJoint);
		}

		std::vector<CJoint*>& caJoint = m_pcSkinModel->m_pcModelData->m_paJoint;

		////親子関係構築
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
		m_pcSkinModel->m_paCombMatrix = new NSMatrix[7];    // 合成変換行列。これがシェーダに渡ります
		for (int i = 0; i < 7; i++)
		{
			caJoint[i]->SetJointID((u16)i);
			caJoint[i]->m_pCombMatArray = m_pcSkinModel->m_paCombMatrix;

			caJoint[i]->m_invMatrix = caJoint[i]->m_InitMat;

			NSVector4 vec;
			caJoint[i]->m_invMatrix = XMMatrixInverse(&vec, caJoint[i]->m_InitMat);
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

		////AssetManagerへ登録
		//CAssetManager* pcAssetM = CAssetManager::GetInstance();
		//if (pcAssetM != nullptr)
		//{
		//	pcAssetM->EntryAsset(pcTestMesh);
		//}
	}

	return true;

}

//---------------------------------------------------------------------------
/*!
	@brief	更新
*/
//---------------------------------------------------------------------------
void CDebugGeomSkin::Calc( const f32 fDelta )
{
	CDebugGeomBase::Calc(fDelta);

	if (m_pcSkinModel == nullptr)
	{
		return;
	}

	static float val = 0.0f;
	val += 0.03f;

	///////////////////////////////////////
	// ボーンの姿勢を更新
	//  最終的には、
	//   [ボーンオフセット行列] × [ワールド空間でのボーンの姿勢]
	//  を計算します。

	// 各ボーンの初期姿勢からの差分姿勢（親空間ベース）を更新
	//  これは適当にぐにぐに動かして構わない部分です
	NSMatrix defBone[7];
	defBone[0] = XMMatrixIdentity();
	for ( int i = 1; i < 7; i++ ) 
	{
		defBone[i] = XMMatrixRotationY( UtilMath::DegreeToRadian(UtilMath::Sin(val) * 70.0f) );
	}

	std::vector<CJoint*>& caJoint = m_pcSkinModel->m_pcModelData->m_paJoint;
	// 各ボーン行列の親空間ベースでの姿勢を更新
	// 差分姿勢×初期姿勢（共に親空間ベース）です。
	for ( int i = 0; i < 7; i++ )
	{
		caJoint[i]->m_BoneMat = defBone[i] * caJoint[i]->m_InitMat;
	}

	// 親空間ベースにある各ボーン行列をローカル空間ベースの姿勢に変換
	// ここは親子関係に従って行列を掛ける必要があります
	// 掛ける順番は 子 × 親 です。
	NSMatrix globalMat;
	globalMat = XMMatrixIdentity();

	//global = XMMatrixRotationZ( val * 0.1f );
	struct UpdateBone {
		static void run( CJoint* me, NSMatrix *parentWorldMat ) {
			me->m_BoneMat *= *parentWorldMat;
			me->m_pCombMatArray[ me->GetJointID() ] = me->m_invMatrix * me->m_BoneMat;
			if ( me->m_pFirstChild )
				run( me->m_pFirstChild, &me->m_BoneMat);
			if ( me->m_pSibling )
				run( me->m_pSibling, parentWorldMat );
		};
	};
	UpdateBone::run(caJoint[0], &globalMat);
}

//---------------------------------------------------------------------------
/*!
	@brief	シェーダに沿って、パラメタ設定(virtual)
*/
//---------------------------------------------------------------------------
void CDebugGeomSkin::ShaderCalc( const CShaderManager::E_SHADERID eShaderID )
{	
#if false
	CShaderManager* pcShaderM = CShaderManager::GetInstance();
	NS_ASSERT( pcShaderM );
	
	//シェーダにパラメータの設定
	CShaderBase* pcShader = pcShaderM->GetShader( eShaderID );
	if( pcShader )
	{
		if( pcShader->GetShaderID() == CShaderManager::eSHADERID_SKINTRANS )
		{
			CSkinTransShader* pcSkinShader = NS_STATIC_CAST( CSkinTransShader*, pcShader );

			CCameraManager* pcCamM = CCameraManager::GetInstance();
			NS_ASSERT( pcCamM );

			CCameraBase* pcCam = pcCamM->GetActiveCamera();
			NS_ASSERT( pcCam );

			pcSkinShader->SetView( pcCam->GetView() );
			pcSkinShader->SetProjection( pcCam->GetProj() );
			pcSkinShader->SetCombMatrix( m_paCombMatrix, 7 );
		}
	}
#endif
}

#endif

#endif //defined( _DEBUG )

//EOF