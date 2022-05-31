/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		AssimpLoader.cpp
	@brief		Assimpを使用したモデルローダー
	@author		ta.Ishii
	@date		2013/4/25 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#include "LoaderAssimp.h"

#if 0
#include "FrameWork/Resource/ResourceManager.h"
#include "FrameWork/Utility/UtilityStructure/UtilHash.h"
#include "FrameWork/Rendering/Model.h"
#include "FrameWork/Rendering/ResourceTexture.h"

//#define DRAW_GEOMETORY_INFO

//////////////////////////////////////////////////////////////////////////

/*
 * Some extra matrix, vector and quaternion functions.
 *
 * Many of these already exist in some variant in assimp,
 * but not exported to the C interface.
 */

// convert 4x4 to column major format for opengl
void transposematrix(f32 m[16],  aiMatrix4x4 *p)
{
	m[0] = p->a1; m[4] = p->a2; m[8] = p->a3; m[12] = p->a4;
	m[1] = p->b1; m[5] = p->b2; m[9] = p->b3; m[13] = p->b4;
	m[2] = p->c1; m[6] = p->c2; m[10] = p->c3; m[14] = p->c4;
	m[3] = p->d1; m[7] = p->d2; m[11] = p->d3; m[15] = p->d4;
}

void extract3x3( aiMatrix3x3 *m3,  aiMatrix4x4 *m4)
{
	m3->a1 = m4->a1; m3->a2 = m4->a2; m3->a3 = m4->a3;
	m3->b1 = m4->b1; m3->b2 = m4->b2; m3->b3 = m4->b3;
	m3->c1 = m4->c1; m3->c2 = m4->c2; m3->c3 = m4->c3;
}

void mixvector( aiVector3D *p,  aiVector3D *a,  aiVector3D *b, f32 t)
{
	p->x = a->x + t * (b->x - a->x);
	p->y = a->y + t * (b->y - a->y);
	p->z = a->z + t * (b->z - a->z);
}

f32 dotquaternions( aiQuaternion *a,  aiQuaternion *b)
{
	return a->x*b->x + a->y*b->y + a->z*b->z + a->w*b->w;
}

void normalizequaternion( aiQuaternion *q)
{
	f32 d = sqrt(dotquaternions(q, q));
	if (d >= 0.00001) {
		d = 1 / d;
		q->x *= d;
		q->y *= d;
		q->z *= d;
		q->w *= d;
	} else {
		q->x = q->y = q->z = 0;
		q->w = 1;
	}
}

void mixquaternion( aiQuaternion *q,  aiQuaternion *a,  aiQuaternion *b, f32 t)
{
	if (dotquaternions(a, b) < 0) {
		 aiQuaternion tmp;
		tmp.x = -a->x; tmp.y = -a->y; tmp.z = -a->z; tmp.w = -a->w;
		a = &tmp;
	}
	q->x = a->x + t * (b->x - a->x);
	q->y = a->y + t * (b->y - a->y);
	q->z = a->z + t * (b->z - a->z);
	q->w = a->w + t * (b->w - a->w);
	normalizequaternion(q);
}

void composematrix( aiMatrix4x4 *m,
	 aiVector3D *t,  aiQuaternion *q,  aiVector3D *s)
{
	// quat to rotation matrix
	m->a1 = 1 - 2 * (q->y * q->y + q->z * q->z);
	m->a2 = 2 * (q->x * q->y - q->z * q->w);
	m->a3 = 2 * (q->x * q->z + q->y * q->w);
	m->b1 = 2 * (q->x * q->y + q->z * q->w);
	m->b2 = 1 - 2 * (q->x * q->x + q->z * q->z);
	m->b3 = 2 * (q->y * q->z - q->x * q->w);
	m->c1 = 2 * (q->x * q->z - q->y * q->w);
	m->c2 = 2 * (q->y * q->z + q->x * q->w);
	m->c3 = 1 - 2 * (q->x * q->x + q->y * q->y);

	// scale matrix
	m->a1 *= s->x; m->a2 *= s->x; m->a3 *= s->x;
	m->b1 *= s->y; m->b2 *= s->y; m->b3 *= s->y;
	m->c1 *= s->z; m->c2 *= s->z; m->c3 *= s->z;

	// set translation
	m->a4 = t->x; m->b4 = t->y; m->c4 = t->z;

	m->d1 = 0; m->d2 = 0; m->d3 = 0; m->d4 = 1;
}

void convertNSMatrix( const aiMatrix4x4& aiMat, NSMatrix* pcMatrix )
{
	if( pcMatrix )
	{
		pcMatrix->_11 = aiMat.a1; pcMatrix->_12 = aiMat.a2; pcMatrix->_13 = aiMat.a3; pcMatrix->_14 = aiMat.a4;
		pcMatrix->_21 = aiMat.b1; pcMatrix->_22 = aiMat.b2; pcMatrix->_23 = aiMat.b3; pcMatrix->_24 = aiMat.b4;
		pcMatrix->_31 = aiMat.c1; pcMatrix->_32 = aiMat.c2; pcMatrix->_33 = aiMat.c3; pcMatrix->_34 = aiMat.c4;
		pcMatrix->_41 = aiMat.d1; pcMatrix->_42 = aiMat.d2; pcMatrix->_43 = aiMat.d3; pcMatrix->_44 = aiMat.d4;
	}
}

//////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
/*	@brief	コンストラクタ
*/
//----------------------------------------------------------------------------
CAssimpLoader::CAssimpLoader()
: m_pAiScene( nullptr )
, m_pLoadModel( nullptr )
{
}

//----------------------------------------------------------------------------
/*	@brief	デストラクタ
*/
//----------------------------------------------------------------------------
CAssimpLoader::~CAssimpLoader()
{
	Term();
}

//----------------------------------------------------------------------------
/*	@brief
*/
//----------------------------------------------------------------------------
bool CAssimpLoader::Init()
{	
	return true;
}

//----------------------------------------------------------------------------
/*	@brief
*/
//----------------------------------------------------------------------------
void CAssimpLoader::Destroy()
{
	Term();
}

//----------------------------------------------------------------------------
/*	@brief
*/	
//----------------------------------------------------------------------------
void CAssimpLoader::Calc( const f32 fDelta )
{
	NS_UNUSED( fDelta );
}

//----------------------------------------------------------------------------
/*	@brief		ロード関数(同期読み込み)
	@param[in]	strFileName ファイル名
*/	
//----------------------------------------------------------------------------
bool CAssimpLoader::Load( const TCHAR* strFileName, CModel* pcModel )
{
	//ロードが完了したらモデルデータを作成しはじめる
	//モデルデータが完成したらローダー破棄
	NS_ASSERT( pcModel );
	m_pLoadModel = pcModel;
	strcpy_s( m_pLoadModel->m_sModelData.fileName, 128, strFileName );
	
	//データロード。モデル作成
	NS_ASSERT( !m_pAiScene );
	m_pAiScene = aiImportFile( strFileName, aiProcessPreset_TargetRealtime_MaxQuality );

	bool bRet = false;

	if( m_pAiScene )
	{
		//メッシュ作っておく
		m_pLoadModel->m_unMeshNum = m_pAiScene->mNumMeshes;
		m_pLoadModel->m_paMesh.clear();
		m_pLoadModel->m_paMesh.reserve( m_pLoadModel->m_unMeshNum );

		for( u32 i = 0; i < m_pLoadModel->m_unMeshNum; ++i )
		{
			CMesh* pMesh = new CMesh;
			m_pLoadModel->m_paMesh.push_back( pMesh );
		}

		//ジオメトリロード
		LoadGeometry();
		//マテリアルロード
		LoadMaterial();
		//スキン情報ロード
		LoadSkin();

		aiReleaseImport( m_pAiScene );

		bRet = true;
	}
	else
	{
		bRet = false;
	}
	m_bLoadEnd = true;

	return bRet;
}

//----------------------------------------------------------------------------
/*	@brief	ジオメトリデータの読み込み
*/
//----------------------------------------------------------------------------
void CAssimpLoader::LoadGeometry()
{
	for( u32 i = 0; i < m_pLoadModel->m_paMesh.size(); ++i )
	{
		LoadGeomCore( m_pLoadModel->m_paMesh[ i ], m_pAiScene->mMeshes[ i ] );
	}
}

//----------------------------------------------------------------------------
/*	@brief	マテリアルロード
*/
//----------------------------------------------------------------------------
void CAssimpLoader::LoadMaterial()
{
	for( u32 i = 0; i < m_pLoadModel->m_paMesh.size(); ++i )
	{
		LoadMaterialCore( m_pLoadModel->m_paMesh[ i ], m_pAiScene->mMaterials[ m_pAiScene->mMeshes[i]->mMaterialIndex ] );
	}
}


//---------------------------------------------------------------------------
/*!
	@brief	スキン情報の読み込み
*/
//---------------------------------------------------------------------------
void CAssimpLoader::LoadSkin()
{
	for( u32 i = 0; i < m_pLoadModel->m_paMesh.size(); ++i )
	{
		LoadSkinCore( m_pAiScene, m_pLoadModel->m_paMesh[ i ], m_pAiScene->mMeshes[ i ] );
	}

#if defined( DRAW_GEOMETORY_INFO )
	CResourceManager* pcResM = CResourceManager::GetInstance();
	NS_ASSERT( pcResM );

	u32 geomID = 0;
	for( u32 i = 0; i < m_pLoadModel->m_paMesh.size(); i++ )
	{
		CMesh* pMesh = m_pLoadModel->m_paMesh[ i ];
		CGeometryPacket* pcPacket = pcResM->GetGeometryPacket( pMesh->GetGeometryID() );
		if( pcPacket )
		{
			for( u32 j = 0; j < pcPacket->m_pVertices.size(); j++)
			{
				for( s32 k = 0; k < 4; k++ )
				{
					if( k == 3 )
					{
						NS_Report("MyGeomID[%d] BoneIndex[%d] Weight[%f]\n", 
							geomID, 
							pcPacket->m_pVertices[j].boneIndex[k],
							1.0f-pcPacket->m_pVertices[j].weight[0]-pcPacket->m_pVertices[j].weight[1]-pcPacket->m_pVertices[j].weight[2] );
					}
					else
					{
						NS_Report("MyGeomID[%d] BoneIndex[%d] Weight[%f]\n", 
							geomID, 
							pcPacket->m_pVertices[j].boneIndex[k],
							pcPacket->m_pVertices[j].weight[k] );
					}
				}
				geomID++;
			}
		}
	}

#endif

	//親子関係構築
	if( m_pLoadModel->m_paJoint.size() > 0 )
	{
		m_pLoadModel->m_sModelData.bSkin = true;
		m_pLoadModel->m_paCombMatrix = new NSMatrix[ BONE_MAX ];
#if 1
		std::vector< CJoint* >::iterator it = m_pLoadModel->m_paJoint.begin();
		for( u32 unRootIdx = 0; it != m_pLoadModel->m_paJoint.end(); ++it )
		{
			(*it)->m_pCombMatArray = m_pLoadModel->m_paCombMatrix;

			aiNode* pNode = FindNode( m_pAiScene->mRootNode, (*it)->GetJointName() );	//ボーン名から、ノードを探索する
			NS_ASSERT( pNode );

			//親設定
			if( pNode->mParent )
			{
				CJoint* pParent = m_pLoadModel->GetJoint( pNode->mParent->mName.C_Str() );
				if( pParent )
				{
					(*it)->m_pParent = pParent;
				}
			}

			//1子設定
			CJoint* pcTempJoint = nullptr;
			for( u32 i = 0; i < pNode->mNumChildren; i++ )
			{
				CJoint* pcChild = m_pLoadModel->GetJoint( pNode->mChildren[ i ]->mName.C_Str() );
				if( !(*it)->m_pFirstChild )
				{
					(*it)->m_pFirstChild = pcChild;
					pcTempJoint = pcChild;
				}
				else
				{
					//兄弟設定
					pcTempJoint->m_pSibling = pcChild;
					pcTempJoint = pcChild;
				}
			}

			if( (*it)->m_pFirstChild && !(*it)->m_pParent )
			{
				//ルートジョイント確定
				m_pLoadModel->m_unRootJointID = unRootIdx;
			}
			unRootIdx++;
		}
#endif   

#if 1
		//各ジョイントのローカル座標を親からの相対位置に設定
		struct CalcRelativeMat 
		{
			static void run(CJoint* me, NSMatrix *parentoffsetMat) 
			{
				if ( me->m_pFirstChild )
				{
					run( me->m_pFirstChild, &me->m_invMatrix);
				}
				if ( me->m_pSibling )
				{
					run( me->m_pSibling, parentoffsetMat );
				}
				if ( parentoffsetMat )
				{
					me->m_InitMat *= *parentoffsetMat;
				}
			}	
		};
		if( m_pLoadModel->m_paJoint.size() > 0 )
		{
			CJoint* rootJoint = m_pLoadModel->GetRootJoint();
			if( rootJoint )
			{
				CalcRelativeMat::run( rootJoint, 0 );
			}
		}
#endif
	}
}

//---------------------------------------------------------------------------
/*!
	@brief	モーションデータ情報読み込み
*/
//---------------------------------------------------------------------------
void CAssimpLoader::LoadMotion()
{
}

//----------------------------------------------------------------------------
/*	@brief	破棄
*/
//----------------------------------------------------------------------------
void CAssimpLoader::Term()
{
	m_pLoadModel = nullptr;
}

//---------------------------------------------------------------------------
/*!
	@brief		Meshの初期化
	@param[in]	pMesh(独自メッシュ)
	@param[in]	aMesh(Assimpメッシュ)
*/
//---------------------------------------------------------------------------
void CAssimpLoader::LoadGeomCore( CMesh* pMesh, aiMesh* aMesh)
{
	CResourceManager* pcResM = CResourceManager::GetInstance();
	NS_ASSERT( pcResM );

	//リソースデータベースに既にジオメトリが存在しないか問い合わせる。
	CGeometryPacket* pcPacket = nullptr;
	char packetName[256] = {0};
	sprintf( packetName, "%s/%s", m_pLoadModel->m_sModelData.fileName, aMesh->mName.C_Str() );

	//ハッシュ値計算してジオメトリ問い合わせ。
	u32 unUIID = CUtilHash::calcNameHash( packetName );
	pcPacket = pcResM->GetGeometryPacket( unUIID );

	if( !pcPacket )
	{
		//存在しないので、ジオメトリを作成する。
		pcPacket = new CGeometryPacket();
		pcPacket->m_unVertexCount = aMesh->mNumVertices;
		m_pLoadModel->m_unVertexCount += pcPacket->m_unVertexCount;

		//Pos,Nom,UVセットアップ
#if !defined( NO_USEVOID )
		//頂点バッファの領域を確保する
		pcPacket->m_pVertices = static_cast< VertexFormatPtr* >( calloc( 1, sizeof( VertexFormatPtr ) ) );
		VertexFormatPtr* pcCP = static_cast< VertexFormatPtr* >( pcPacket->m_pVertices );
		pcCP->pPos = new f32[ pcPacket->m_unVertexCount * 3 ];			//f32*3を頂点数分
		pcCP->pTexCoord = new f32[ pcPacket->m_unVertexCount * 2 ];		//f32*2を頂点数分
		pcCP->pNom = new f32[ pcPacket->m_unVertexCount * 3 ];			//f32*3を頂点数分
#else
		pcPacket->m_pVertices.reserve( pcPacket->m_unVertexCount );
#endif
		//情報を代入
		for( u32 i = 0; i < pcPacket->m_unVertexCount; ++i )
		{
#if defined( NO_USEVOID )
			pcPacket->m_pVertices.push_back( BasicVertex() );
			BasicVertex& vert = pcPacket->m_pVertices[ i ];
#endif
			//現状、再変換するので。
			//if( aMesh->mNumBones == 0 )
			{
#if defined( NO_USEVOID )
				vert.v3Position.x = aMesh->mVertices[i].x;
				vert.v3Position.y = aMesh->mVertices[i].y;
				vert.v3Position.z = aMesh->mVertices[i].z;
#else								
				pcCP->pPos[i*3+0] = aMesh->mVertices[i].x;
				pcCP->pPos[i*3+1] = aMesh->mVertices[i].y;
				pcCP->pPos[i*3+2] = aMesh->mVertices[i].z;
				NS_Report("PosX:%f PosY:%f PosZ:%f\n", pcCP->pPos[i*3+0], pcCP->pPos[i*3+1], pcCP->pPos[i*3+2]);
#endif

				//法線あれば
				if( aMesh->mNormals )
				{
#if defined( NO_USEVOID )
					vert.v3Normal.x = aMesh->mNormals[i].x;
					vert.v3Normal.y = aMesh->mNormals[i].y;
					vert.v3Normal.z = aMesh->mNormals[i].z;
#else				
					pcCP->pNom[i*3+0] = aMesh->mNormals[i].x;
					pcCP->pNom[i*3+1] = aMesh->mNormals[i].y;
					pcCP->pNom[i*3+2] = aMesh->mNormals[i].z;
					NS_Report("NomX:%f NomY:%f NomZ:%f\n", pcCP->pNom[i*3+0], pcCP->pNom[i*3+1], pcCP->pNom[i*3+2]);
#endif
				}
			}

			//テクスチャ座標あれば
			if( aMesh->mTextureCoords[0] )
			{
#if defined( NO_USEVOID )
				vert.v2TexCoord.x = aMesh->mTextureCoords[0][i].x;
				vert.v2TexCoord.y = 1.0f-aMesh->mTextureCoords[0][i].y;	//V反転
#else				
				pcCP->pTexCoord[i*2+0] = aMesh->mTextureCoords[0][i].x;
				pcCP->pTexCoord[i*2+1] = 1.0f-aMesh->mTextureCoords[0][i].y;	//V反転
				NS_Report("TexU:%f TexV:%f\n", pcCP->pTexCoord[i*2+0], pcCP->pTexCoord[i*2+1]);
#endif
			}
		}

		//インデックス情報の作成
		pcPacket->m_unIndexCount = aMesh->mNumFaces*3;	//インデックスバッファ？
		m_pLoadModel->m_unIndexCount += pcPacket->m_unIndexCount;

		NS_ASSERT( !pcPacket->m_unapIndecies );
		pcPacket->m_unapIndecies = new u16[ pcPacket->m_unIndexCount ];

		for( u32 i = 0; i < aMesh->mNumFaces; ++i )
		{
			aiFace* pFace = aMesh->mFaces + i;
			pcPacket->m_unapIndecies[i*3+0] = (u16)pFace->mIndices[0];
			pcPacket->m_unapIndecies[i*3+1] = (u16)pFace->mIndices[1];
			pcPacket->m_unapIndecies[i*3+2] = (u16)pFace->mIndices[2];

#if defined( DRAW_GEOMETORY_INFO )
			NS_Report("Face is %d Idx1:%d Idx2:%d Idx3:%d\n", 
				i,
				pcPacket->m_unapIndecies[i*3+0], 
				pcPacket->m_unapIndecies[i*3+1], 
				pcPacket->m_unapIndecies[i*3+2]);
#endif
		}
		pcPacket->m_unVtxStride = sizeof( BasicVertex );
		pcResM->AddGeometryPacket( packetName, pcPacket);
	}
	else
	{
		//既にリソースに存在していた場合はIDだけ参照する。
	}

	pMesh->m_unGeometryID = unUIID;
	bool bRet = pMesh->CreateBuffer();	//描画用バッファの作成
	NS_ASSERT( bRet );
}

//---------------------------------------------------------------------------
/*!
	@brief	マテリアル情報読み込みコア
	@param[in] pMaterial	マテリアル
*/
//---------------------------------------------------------------------------
void CAssimpLoader::LoadMaterialCore( CMesh* pMesh, aiMaterial* pMaterial )
{
	char filename[1024] = { 0 };
	aiString fileName;

	if ( !aiGetMaterialString( pMaterial, AI_MATKEY_TEXTURE_DIFFUSE(0), &fileName) )
	{
		//テクスチャファイルの読み込み。
		char *s = strrchr(fileName.data, '/');
		if (!s) s = strrchr(fileName.data, '\\');
		if (!s) s = fileName.data; else s++;
		strcpy(filename, "resource/image/");
		strcat_s(filename, s);

		//とりあえずDiffuseのみとなる。
		CResourceManager* pcResM = CResourceManager::GetInstance();
		if( pcResM )
		{
			CTextureBase* pcTex = pcResM->GetTexture( filename );
			if( !pcTex )
			{
				//リソースマネージャに存在しないので読み込み
				pcTex = pcResM->LoadTexture( filename );
			}

			if( pcTex )
			{
				u32* pTexID = pMesh->cMaterial.GetTextureIDs();
				//テクスチャ設定をしておく
#if defined( DRAW_GEOMETORY_INFO )
				NS_Report("Assimp:LoadMaterialCore:%s\n", filename);
#endif
				pTexID[ 0 ] = pcTex->GetTexUIID();
			}
		}
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
void CAssimpLoader::LoadSkinCore(const aiScene* pScene, CMesh* pMesh, aiMesh* aMesh)
{
	if( aMesh->mNumBones == 0 )
	{
		//スキン情報なし
		return;
	}

	CResourceManager* pcResM = CResourceManager::GetInstance();
	NS_ASSERT( pcResM );
	CGeometryPacket* pcPacket = pcResM->GetGeometryPacket( pMesh->GetGeometryID() );
	NS_ASSERT( pcPacket );

	aiMatrix3x3 skin3;
	aiMatrix4x4 skin4;
	for( u32 k = 0; k < aMesh->mNumBones; k++ )
	{
		aiBone* pBone = aMesh->mBones[ k ];
		NS_Report("JointListup:%s\n", pBone->mName.data );
		aiNode* pNode = FindNode( pScene->mRootNode, pBone->mName.data );	//ボーン名から、ノードを探索する
		NS_Report("NodeListup:%s parent is %s\n", pNode->mName.data, pNode->mParent->mName.data );
		NS_ASSERT( pNode );

		for( u32 t = 0; t < pNode->mNumMeshes; ++t )
		{
			NS_Report("MeshIndex:%lld", pNode->mMeshes[t] );
		}

#if 0

		CalcWorldMatrixNode( &skin4, pNode );
		//オフセットかけちゃったよ！！オレの認識ではオフセット行列は原点に戻す行列のものなんだが
		aiMultiplyMatrix4( &skin4, &pBone->mOffsetMatrix );	
		extract3x3( &skin3, &skin4 );

#else
		//影響を与える頂点の集合
		for( u32 i = 0; i < pBone->mNumWeights; i++ )
		{
			s32 v = pBone->mWeights[i].mVertexId;	//影響を受ける頂点ID(ボーンIDじゃないのか？)
			f32 w = pBone->mWeights[i].mWeight;		//重み

			//インデックス、重みを入れる
			for( s32 j = 0; j < 4; ++j )
			{
				if( pcPacket->m_pVertices[ v ].boneIndex[j] == 0 )
				{
					pcPacket->m_pVertices[ v ].boneIndex[j] = k;
					if( j < 3 )
					{
						pcPacket->m_pVertices[ v ].weight[j] = w;
					}
					break;
				}
			}
		}

		//ボーン情報読み込み
		CJoint* pJoint = new CJoint;
		m_pLoadModel->m_paJoint.push_back( pJoint );
		pJoint->SetJointID( (u16)k );
		pJoint->SetJointName( pNode->mName.data );

		//初期マトリクス
		//pNode->mTransformation;	//ローカル行列？
		//mOffsetMatrixメッシュ空間→ボーン
		convertNSMatrix( pBone->mOffsetMatrix, &pJoint->m_InitMat );
		NSVector4 Determinant;
		pJoint->m_invMatrix = NSMatrixInverse( &Determinant, pJoint->m_InitMat );

		//行列の親空間ベース姿勢更新
#endif
	}

	pMesh->CreateBuffer();
}

//---------------------------------------------------------------------------
/*!
	@brief		ノード探索
	@param[in]	aNode	ノード情報
	@param[in]	nodename	ノード文字列
	@return aiNode 一致ノード
*/
//---------------------------------------------------------------------------
aiNode* CAssimpLoader::FindNode( aiNode* Node, const TCHAR* nodename )
{
	if( !strcmp( nodename, Node->mName.data ) )
	{
		return Node;	//名前が一致した
	}

	for( u32 i = 0; i < Node->mNumChildren; i++ )
	{
		//ノードの子を探索
		aiNode* FoundNode = FindNode( Node->mChildren[i], nodename );
		if( FoundNode )
		{
			return FoundNode;
		}
	}
	return nullptr;
}

//---------------------------------------------------------------------------
/*!
	@brief		ノードの絶対座標算出
	@param[in]	pResult	絶対座標行列
	@param[in]	pNode	算出対象ノード
*/
//---------------------------------------------------------------------------
void CAssimpLoader::CalcWorldMatrixNode( aiMatrix4x4* pResult, aiNode* pNode )
{
	if( pNode->mParent )
	{
		//親がいる
		CalcWorldMatrixNode( pResult, pNode->mParent );
		aiMultiplyMatrix4( pResult, &pNode->mTransformation );
	}
	else
	{
		//算出しきった
		*pResult = pNode->mTransformation;
	}
}

#endif

//EOF

