
/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		ColladaLoader.cpp
	@brief		Colladaローダー
	@author		ta.Ishii
	@date		2012/9/22 create this file.
	@note		http://codezine.jp/article/detail/843?p=2
	@todo		インデックスへの対応
*/
/*==========================================================================*/
/*==========================================================================*/

#if 0

//#define INDEX_ENABLE

#include "ColladaLoader.h"

#include <dae.h>
#include <dom/domElements.h>
#include <dom/domCOLLADA.h>

#include "Framework/Rendering/ResourceTexture.h"
#include "FrameWork/Rendering/Model.h"
#include "FrameWork/Rendering/Joint.h"
#include "FrameWork/Rendering/TextureContext.h"
#include "FrameWork/Rendering/Joint.h"

#include "Framework/Utility/UtilMacro.h"
#include "FrameWork/Utility/UtilDX11.h"
#include "FrameWork/Resource/ResourceManager.h"

//探してくるタグ名
const static char* COLLADA_IMAGE_ID = "image";
const static char* COLLADA_EFFECT = "effect";
const static char* COLLADA_PROFILE_COMMON = "profile_COMMON";
const static char* COLLADA_GEOMETRY_ID  = "geometry";
const static char* COLLADA_MATERIAL	= "material";
const static char* COLLADA_SKIN = "skin";
const static char* COLLADA_CONTROLLER = "controller";
const static char* COLLADA_ANIMATION = "animation";
const static char* COLLADA_VISUAL_SCENE = "visual_scenes";
const static char* COLLADA_NODE = "node";

//---------------------------------------------------------------------------
/*!
	@brief	最大オフセット値を求める
*/
//---------------------------------------------------------------------------
u32 GetMaxOffset( const domInputLocalOffset_Array &inputArray )
{
   u32 unMaxOffset = 0;
   for( u32 i = 0; i < inputArray.getCount(); ++i )
   {
      if(inputArray[ i ]->getOffset() > unMaxOffset)
      {
         unMaxOffset = NS_STATIC_CAST( u32, inputArray[i]->getOffset() );
      }
   }
   return unMaxOffset;
}

//----------------------------------------------------------------------------
/*	@brief	コンストラクタ
*/
//----------------------------------------------------------------------------
CColladaLoader::CColladaLoader()
: m_pDaeFile(nullptr)
, m_pLoadModel(nullptr)
{
}

//----------------------------------------------------------------------------
/*	@brief	デストラクタ
*/
//----------------------------------------------------------------------------
CColladaLoader::~CColladaLoader()
{
	Destroy();
}

//----------------------------------------------------------------------------
/*	@brief
*/
//----------------------------------------------------------------------------
bool CColladaLoader::Init()
{	
	return true;
}

//----------------------------------------------------------------------------
/*	@brief
*/
//----------------------------------------------------------------------------
void CColladaLoader::Destroy()
{
	Term();
}

//----------------------------------------------------------------------------
/*	@brief
*/	
//----------------------------------------------------------------------------
void CColladaLoader::Calc( const f32 fDelta )
{
}

//----------------------------------------------------------------------------
/*	@brief		ロード関数
	@param[in]	strFileName ファイル名
*/	
//----------------------------------------------------------------------------
bool CColladaLoader::Load( const TCHAR* strFileName, CModel* pcModel )
{
	m_pDaeFile = new DAE();
	NS_ASSERT( m_pDaeFile );

	if( m_pDaeFile->load( strFileName ) != DAE_OK )
	{
		NS_Report(_T("Load Error Collada File :: %s ::"), strFileName );
		return false;
	}

	//ロードが完了したらモデルデータを作成しはじめる
	//モデルデータが完成したらローダー破棄
	if( m_pDaeFile )
	{
		m_pLoadModel = pcModel;

		//データロード。モデル作成
		daeDatabase* pDataBase = m_pDaeFile->getDatabase();
		NS_ASSERT( pDataBase );

		//テクスチャーロード
		LoadTexture();
		//マテリアルロード
		LoadMaterial();
		//ジオメトリロード
		LoadGeometry();
		//スキンロード
		LoadSkin();
		//モーションデータロード
		LoadMotion();
	}

	//--------------------------------------------------------------------
	// 色々と登録
	// ここは修正がいりそう
	//--------------------------------------------------------------------
	CGeometryInstance* pGeomIns = pcModel->GetGeometryInstance();

	if( pGeomIns->m_pGeomPacket )
	{
		pGeomIns->m_pGeomPacket->m_unVertexCount = pcModel->m_VertexArray.size();
		pGeomIns->m_pGeomPacket->m_unIndexCount = m_IndexArray.size();
		if( m_IndexArray.size() > 0 )
		{
			pGeomIns->m_pGeomPacket->m_unapIndecies = new u16[ m_IndexArray.size() ];
			for( u32 i = 0; i < m_IndexArray.size(); i++)
			{
				pGeomIns->m_pGeomPacket->m_unapIndecies[i] = m_IndexArray[i];
			}
		}
	}

	if( m_pDaeFile )
	{
		m_pDaeFile->cleanup();
	}
	DAE::cleanup();
	SAFE_DELETE( m_pDaeFile );

	m_bLoadEnd = true;

	return true;
}

//----------------------------------------------------------------------------
/*	@brief	ジオメトリデータの読み込み
*/
//----------------------------------------------------------------------------
void CColladaLoader::LoadGeometry()
{ 
	// ジオメトリの数の取得
	// Collada内の独立したメッシュ数。
	s32 nGeomNum = NS_STATIC_CAST( s32, (m_pDaeFile->getDatabase()->getElementCount(nullptr, COLLADA_GEOMETRY_ID, nullptr ) ) );
	if( nGeomNum == 0)
	{
		//NS_Report("ColladaFile GeometryNum is 0\n" );
		return;
	}

	for( s32 nCurrentGeom = 0; nCurrentGeom < nGeomNum; ++nCurrentGeom )
	{
		domGeometry* pCurrentGeometry = nullptr;

		// ジオメトリデータの取得
		m_pDaeFile->getDatabase()->getElement( (daeElement**)&pCurrentGeometry, nCurrentGeom, nullptr, COLLADA_GEOMETRY_ID);
		if( pCurrentGeometry != nullptr)
		{
			// メッシュの取得
			domMesh* pCurrentMesh = nullptr;
			pCurrentMesh = pCurrentGeometry->getMesh();

			if( !pCurrentMesh )
			{
				//NS_Report("ColladaFile Mesh is nullptr\n" );
				return;
			}

			//--------------------------------------------------------------------
			// 頂点データを格納する。
			//--------------------------------------------------------------------
			const domSource_Array& vSourceArray = pCurrentMesh->getSource_array();
			u16 vSourceTypeNum = 0;
			vSourceTypeNum = NS_STATIC_CAST( u16, vSourceArray.getCount() );	//頂点種類の数を取得
			NS_ASSERT( vSourceTypeNum > 0 );

			//--------------------------------------------------------------------
			// 頂点座標（Colladaエクスポートの時点で、重複省いてくれるかも？）
			u32 unVertexSize =  NS_STATIC_CAST( u32, pCurrentMesh->getSource_array()[0]->getFloat_array()->getValue().getCount() );

			std::vector< NSVector3 > vPosArray;
			u32 unVertexNum = unVertexSize / 3;		//重複がない実質の頂点数。
			vPosArray.reserve( unVertexSize );

			for( u32 i = 0; i < unVertexNum; i++ )
			{
				//座標はXYZ前提
				f32 x = NS_STATIC_CAST( f32, pCurrentMesh->getSource_array()[0]->getFloat_array()->getValue().get( i*3 + 0 ) );
				f32 y = NS_STATIC_CAST( f32, pCurrentMesh->getSource_array()[0]->getFloat_array()->getValue().get( i*3 + 1 ) );
				f32 z = NS_STATIC_CAST( f32, pCurrentMesh->getSource_array()[0]->getFloat_array()->getValue().get( i*3 + 2 ) );

				NSVector3 vPos( x, y, z );
				//NS_Report("X:%f Y:%f Z:%f idx:%d\n", x, y, z, i );
				vPosArray.push_back( vPos );
			}

			//--------------------------------------------------------------------
			std::vector< NSVector3 > vNormArray;
			if( vSourceTypeNum >= 2 )
			{
				// 法線（Colladaエクスポートの時点で、重複省いてくれるかも？）
				u32 unNormalSize =  NS_STATIC_CAST( u32, pCurrentMesh->getSource_array()[1]->getFloat_array()->getValue().getCount() );

				u32 unNormNum = unNormalSize / 3;		//実質の法線数
				vNormArray.reserve( unNormNum );

				for( u32 i = 0; i < unNormNum; i++ )
				{
					//法線
					f32 x = NS_STATIC_CAST( f32, pCurrentMesh->getSource_array()[1]->getFloat_array()->getValue().get( i*3 + 0 ) );
					f32 y = NS_STATIC_CAST( f32, pCurrentMesh->getSource_array()[1]->getFloat_array()->getValue().get( i*3 + 1 ) );
					f32 z = NS_STATIC_CAST( f32, pCurrentMesh->getSource_array()[1]->getFloat_array()->getValue().get( i*3 + 2 ) );

					NSVector3 vNorm( x, y, z );
					//NS_Report("Norm X:%f Norm Y:%f Norm Z:%f idx:%d\n", x, y, z, i );
					vNormArray.push_back( vNorm );
				}
			}

			//--------------------------------------------------------------------
			// UV
			std::vector< NSVector2 > vUVArray;
			if( vSourceTypeNum >= 3 )
			{
				u32 unUVSize = NS_STATIC_CAST( u32, pCurrentMesh->getSource_array()[2]->getFloat_array()->getValue().getCount() );

				u32 unUVNum = unUVSize / 2;			//UV数
				vUVArray.reserve( unUVNum );

				for( u32 i = 0; i < unUVNum; i++ )
				{
					//UV
					f32 u = NS_STATIC_CAST( f32, pCurrentMesh->getSource_array()[2]->getFloat_array()->getValue().get( i*2 + 0 ) );
					f32 v = NS_STATIC_CAST( f32, pCurrentMesh->getSource_array()[2]->getFloat_array()->getValue().get( i*2 + 1 ) );

					NSVector2 vUV( u, v);
					//NS_Report("UV U:%f UV V:%f idx:%d\n", u, v, i );
					vUVArray.push_back( vUV );
				}
			}

			//--------------------------------------------------------------------
			// インデックス周りの情報を取得
			// @note バグが残ってる悪寒・・・
			//--------------------------------------------------------------------
			std::vector< u16 > vIndexArray;	//	インデックスをまとめて保存しておく
			vIndexArray.clear();
			u32 unTriangles = 0;			//	三角形に分割したときの数
			s32 nNumberOfInputs = 0;		//	インデックスオフセット

			//--------------------------------------------------------------------
			// PolyListポリゴン処理
			//--------------------------------------------------------------------
			s32 nPolyListSize = NS_STATIC_CAST( s32, pCurrentMesh->getPolylist_array().getCount() );	
			for( s32 nCurrentPolyList = 0; nCurrentPolyList < nPolyListSize; ++nCurrentPolyList )
			{
				domPolylist* pPolyList = pCurrentMesh->getPolylist_array().get( nCurrentPolyList );

				//--------------------------------------------------------------------
				// 四角形を三角形に。
				nNumberOfInputs = NS_STATIC_CAST( s32, GetMaxOffset( pPolyList->getInput_array() ) ) + 1;		//	オフセット
				s32 nNumberOfPrim	= NS_STATIC_CAST( s32, pPolyList->getVcount()->getValue().getCount() );		//	PolyList面数

				u32 unOffSet = 0;

				//面数ループをまわす。
				for( s32 j = 0; j < nNumberOfPrim; j++ )
				{
					//nTriCount が三角形ならそのままでよいが、nTriCount が四角形なら変換する必要がある。
					s32 nTriCount = NS_STATIC_CAST( s32, pPolyList->getVcount()->getValue()[j] ) - 2;	//角形数

					s32 nIdx = nNumberOfInputs;

					//三角形ごとにデータコンバート
					for( s32 k = 0; k < nTriCount; k++ )
					{
						//頂点1
						for( s32 l = 0; l < nNumberOfInputs; l++ )
						{
							u16 ulNum = NS_STATIC_CAST( u16, pPolyList->getP()->getValue()[ unOffSet + l ] );
							vIndexArray.push_back( ulNum );
						}

						//頂点2
						for( s32 l = 0; l < nNumberOfInputs; l++ )
						{
							u16 ulNum = NS_STATIC_CAST( u16, pPolyList->getP()->getValue()[ unOffSet + nIdx + l ] );
							vIndexArray.push_back( ulNum );
						}

						//頂点3
						nIdx += nNumberOfInputs;
						for( s32 l = 0; l < nNumberOfInputs; l++ )
						{
							u16 ulNum = NS_STATIC_CAST( u16, pPolyList->getP()->getValue()[ unOffSet + nIdx + l ] );
							vIndexArray.push_back( ulNum );
						}
						unTriangles++;	//三角形の数を算出できる。
					}
					unOffSet += NS_STATIC_CAST( s32, pPolyList->getVcount()->getValue()[ j ] * nNumberOfInputs );
				}
			}

#if 0
			//--------------------------------------------------------------------
			// Polygonsポリゴン処理
			//--------------------------------------------------------------------
			s32 nPolygonsSize = NS_STATIC_CAST( s32, pCurrentMesh->getPolygons_array().getCount());
			for( s32 nCurrentPoly = 0; nCurrentPoly < nPolygonsSize; ++nCurrentPoly )
			{

			}
			//--------------------------------------------------------------------
			// Trianglesポリゴン処理
			//--------------------------------------------------------------------
			s32 nTriangles = NS_STATIC_CAST( s32, pCurrentMesh->getTriangles_array().getCount() );
			//これ基本1じゃないと困るけど・・・
			//一応とめておいて、とまったら対応しよう
			//NS_ASSERT( nTriangles <= 1 );

			for( s32 nCurrentTri = 0; nCurrentTri < nTriangles; nCurrentTri++ )
			{
				domTriangles* pTriangle = pCurrentMesh->getTriangles_array().get( nCurrentTri );

				nNumberOfInputs = NS_STATIC_CAST( s32, GetMaxOffset( pTriangle->getInput_array() ) ) + 1;		//	オフセット
				//s32 nInputCount = pTriangle->getInput_array().getCount();
				s32 nNumberOfPrim = NS_STATIC_CAST( s32, pTriangle->getCount() );								//	三角ポリゴン数

				for( s32 i = 0; i < nNumberOfPrim; i++ )
				{
					vIndexArray.push_back( pTriangle->getP()->getValue()[ i ] );

					if( (i+1) % nNumberOfInputs == 0 )
					{
						unTriangles++;
					}
				}
			}
#endif
			//インデックス格納
			if( nNumberOfInputs > 0  && vIndexArray.size() > 0 )
			{
				//頂点インデックスの格納に関してはフィクスでOKかなあ
				u32 unIndexSize = unTriangles;	//インデックスサイズ
				//m_IndexArray.clear();
				//m_IndexArray.reserve( unIndexSize );
				//頂点のインデックス情報だけは格納しておく。
				for( u32 i = 0; i < unIndexSize; i++ )
				{
					m_IndexArray.push_back( vIndexArray[ i * nNumberOfInputs ] );
				}

				//頂点座標を代入
				//法線、UVなどをインデックスに沿って頂点情報として代入
			}

			//--------------------------------------------------------------------
			// @todo 情報整理する必要はありそうだ
			//--------------------------------------------------------------------
			s32 nMaxIndex = vIndexArray.size() / nNumberOfInputs;
			//m_pLoadModel->m_VertexArray.reserve( nMaxIndex );

			for( s32 i = 0; i < nMaxIndex; i++)
			{
				VertexFormat vVertex;
#if defined( INDEX_ENABLE )
				vVertex.v3Position = vPosArray[ i ];		//頂点
#else
				s32 idx = i * nNumberOfInputs;
				u32 unIdx = vIndexArray[ idx ];	
				vVertex.v3Position = vPosArray[ unIdx ];		//頂点
				//NSVector4SetXPtr( vVertex.v3Position, &vPosArray[ unIdx ].x);
				//NSVector4SetYPtr( vVertex.v3Position, &vPosArray[ unIdx ].y);
				//NSVector4SetZPtr( vVertex.v3Position, &vPosArray[ unIdx ].z);
#endif				
				//--------------------------------------------------------------------
				// @todo ここは要直しやな
				// ここをインデックス対応にする必要があるっぽいけど・・・
				// @note 頂点はインデックス対応出来るんだけど・・・
				// サイズは頂点インデックス数のはず。
				// UVと法線をどういう感じで頂点座標にいれていけばいいか分からない。

				if( vSourceTypeNum >= 2 )
				{
					s32 idx = i * nNumberOfInputs + 1;
					u32 unIdx = vIndexArray[ idx ];	
					vVertex.v3Normal = vNormArray[ unIdx ];	//法線

					//バンプマッピングしたいなら、法線から接ベクトル算出
					NSVector4 vNom;
					vNom = NSVector4Set( vVertex.v3Normal.x, vVertex.v3Normal.y, vVertex.v3Normal.z, 0.0f );
					//vNom = NSVector4Set( 1.0f, 0.0f, 0.0f, 0.0f );
					NSVector4 vTan = UtilMath::ConvertNomToTangent( vNom );

					f32 fx = NSVector4GetX( vTan );
					f32 fy = NSVector4GetY( vTan );
					f32 fz = NSVector4GetZ( vTan );
					NS_Report(_T("Norm :: x:%f y:%f z:%f\n"), vVertex.v3Normal.x, vVertex.v3Normal.y, vVertex.v3Normal.z );
					NS_Report(_T("Tangent :: x:%f y:%f z:%f\n"), fx, fy, fz );

					vVertex.v3Tangent = NSVector3( fx, fy, fz );
				}
				if( vSourceTypeNum >= 3 )
				{
					s32 idx = i * nNumberOfInputs + 2;
					u32 unIdx = vIndexArray[ idx ];
					vVertex.v2TexCoord = vUVArray[ unIdx ];	//UVはない場合あるけど
				}
				m_pLoadModel->m_VertexArray.push_back( vVertex );	//コピー発生するけどまあいいか
			}
		}
	}
	//これだとうまく描画できませんが。
	//NS_ASSERT( m_VertexArray.size() > 0 && m_IndexArray.size() > 0 );
}

//----------------------------------------------------------------------------
/*	@brief	マテリアルロード
*/
//----------------------------------------------------------------------------
void CColladaLoader::LoadMaterial()
{
	daeDatabase* pDataBase = m_pDaeFile->getDatabase();
	NS_ASSERT( pDataBase );

	//マテリアル
	s32 nMaterialCount = NS_STATIC_CAST( s32, pDataBase->getElementCount(nullptr, COLLADA_PROFILE_COMMON, nullptr) );

	for( s32 i = 0; i < nMaterialCount; i++)
	{
		domEffect* pEffect = nullptr;

		pDataBase->getElement((daeElement**)&pEffect, i, nullptr, COLLADA_EFFECT);

		CMaterial info;
		//info.name = pEffect->getName();

		/*
		domProfile_COMMON* pProfile = nullptr;
		pDataBase->getElement((daeElement**)&pProfile, i, nullptr, COLLADA_PROFILE_COMMON);

		if( pProfile != nullptr )
		{
			const domProfile_COMMON::domTechniqueRef rTech = pProfile->getTechnique();
			const domProfile_COMMON::domTechnique::domPhongRef rPhone = rTech->getPhong();
			
			if( rPhone != nullptr )
			{
				if( rPhone->getDiffuse() != nullptr && 
					rPhone->getDiffuse()->getTexture() != nullptr )
				{
					info.samplerName = rPhone->getDiffuse()->getTexture()->getAttribute("texture");	
				}
			}
		}
		*/
	}
}

//
//----------------------------------------------------------------------------
/*	@brief	テクスチャ読み込み
*/
//----------------------------------------------------------------------------
void CColladaLoader::LoadTexture()
{
	daeDatabase* pDataBase = m_pDaeFile->getDatabase();
	NS_ASSERT( pDataBase );

	CResourceManager* pcResM = CResourceManager::GetInstance();
	NS_ASSERT( pcResM );

	//テクスチャ枚数分
	s32 nTextureCount = NS_STATIC_CAST( s32, pDataBase->getElementCount(nullptr, COLLADA_IMAGE_ID, nullptr) );

	for( s32 i = 0; i < nTextureCount; i++)
	{
		CResourceTexture* pcTexture = nullptr;

		domImage* pImage = nullptr;
		pDataBase->getElement( (daeElement**)(&pImage), i, nullptr, COLLADA_IMAGE_ID);
		NS_ASSERT( pImage );

		//テクスチャファイル名取り出し
		const char* pFilepath = pImage->getInit_from()->getValue().getPath();

		// テクスチャファイルパスの取得
		std::string filepath = pFilepath;
		//m_TexturePathArray.push_back(filepath);

		TCHAR drive[_MAX_DRIVE];	// ドライブ名
		TCHAR dir[_MAX_DIR];		// ディレクトリ名
		TCHAR fname[_MAX_FNAME];	// ファイル名
		TCHAR ext[_MAX_EXT];		// 拡張子
		TCHAR fixFilePath[ MAX_PATH ];
		_splitpath_s( filepath.c_str(), drive, dir, fname, ext);
		//sprintf_s( fixFilePath, MAX_PATH, "%s%s%s", "resource/model/" , fname, ext);

#if 0		
		//テクスチャサイズうまく動かない
		u16 nSizeX = NS_STATIC_CAST( u16, pImage->getWidth() );
		u16 nSizeY = NS_STATIC_CAST( u16, pImage->getHeight() );
#else
		u16 nSizeX = 512;
		u16 nSizeY = 512;
#endif
		// テクスチャロード
		pcTexture = pcResM->LoadTexture( fixFilePath, nSizeX, nSizeY );
		if( !pcTexture )
		{
			SAFE_DELETE( pcTexture );
		}
		else
		{
			m_pLoadModel->GetTextureContext()->AddTexture( pcTexture );
		}
	}
}

//---------------------------------------------------------------------------
/*!
	@brief	スキン情報の読み込み
*/
//---------------------------------------------------------------------------
void CColladaLoader::LoadSkin()
{
	daeDatabase* pDataBase = m_pDaeFile->getDatabase();
	NS_ASSERT( pDataBase );

	//ウェイト情報を入れる
	std::vector<float> weightArray;
	weightArray.clear();

	s32 nController = NS_STATIC_CAST( s32, (m_pDaeFile->getDatabase()->getElementCount(nullptr, COLLADA_CONTROLLER, nullptr ) ) );
	NS_ASSERT( nController <= 1 );

	for( s32 i = 0; i < nController; ++i )
	{
		domController* pController = nullptr;
		pDataBase->getElement( (domElement**)( &pController ), i, nullptr, COLLADA_CONTROLLER );

		if( pController )
		{
			const domSkinRef pSkinRef = pController->getSkin();
			const domSkin::domJointsRef rJoints = pSkinRef->getJoints();
			const domInputLocal_Array& rInputLocalArray = rJoints->getInput_array();

			std::string strJointURI;
			std::string strBasePoseURI;

			s32 nInputArrayCount = NS_STATIC_CAST( s32, rInputLocalArray.getCount() );
			for( s32 i = 0; i < nInputArrayCount; ++i)
			{
				const domInputLocalRef rInputLocal = rInputLocalArray.get(i);
				const char* pSemantic = rInputLocal->getSemantic();
				if( strcmp("JOINT", pSemantic) == 0)
				{
					const domURIFragmentType& rSource = rInputLocal->getSource();
					// 先頭の#をとばすため1足す
					strJointURI = (rSource.getOriginalURI() + 1);
				}
				else if( strcmp("INV_BIND_MATRIX", pSemantic) == 0 )
				{
					const domURIFragmentType& rSource = rInputLocal->getSource();
					// 先頭の#をとばすため1足す 
					strBasePoseURI = (rSource.getOriginalURI() + 1);
				}
			}

			const domSource_Array& vSourceArray = pSkinRef->getSource_array();
			for(UINT i = 0; i < vSourceArray.getCount(); ++i)
			{
				if( strJointURI == vSourceArray[i]->getID() )
				{
					const domIDREF_arrayRef rIDREFArray = vSourceArray[i]->getIDREF_array();
					m_pLoadModel->m_paJoint.reserve(static_cast<u32>(rIDREFArray->getCount()));
					
					// ジョイント名の取得
					for(UINT j = 0; j < rIDREFArray->getCount(); ++j)
					{
						CJoint* pJoint = new CJoint;
						m_pLoadModel->m_paJoint.push_back( pJoint );
						m_pLoadModel->m_paJoint[j]->m_uwJointID = j;

						xsIDREFS& refs = rIDREFArray->getValue();
						daeIDRef& idRef = refs.get(j);
						size_t size = strlen( idRef.getID() );
						memcpy( m_pLoadModel->m_paJoint[j]->m_aStrJointName, idRef.getID(), size );
						m_pLoadModel->m_paJoint[j]->m_aStrJointName[ 31 ] = 0;
					}
					break;
				}
			}

			// 上記のボーンサイズを取得してからベースポーズ行列の探索
			for(UINT i = 0; i < vSourceArray.getCount(); ++i){
				if( strBasePoseURI == vSourceArray[i]->getID() ){
					const domFloat_arrayRef rFloatArray = vSourceArray[i]->getFloat_array();
					for(UINT j = 0; j < m_pLoadModel->m_paJoint.size(); ++j){
						// 行列要素の取得
						for(UINT k = 0; k < 4; ++k){
							for(UINT l = 0; l < 4; ++l){
								m_pLoadModel->m_paJoint[j]->m_InitMat.m[l][k] = static_cast<float>(rFloatArray->getValue().get((j * 16) + (k * 4) + l));
							}
						}

						NSVector4 Determinant;
						m_pLoadModel->m_paJoint[j]->m_invMatrix = NSMatrixInverse( &Determinant, m_pLoadModel->m_paJoint[j]->m_InitMat );

					}
					break;
				}
			}
			//f32 fPos = (f32)pController->getSkin()->getSource_array()[0]->getFloat_array()->getValue().get( 0 );

			//--------------------------------------------------------------------
			// ウェイト情報取得
			//--------------------------------------------------------------------
			// ウェイト情報の取得(ここで一時的に保存しておいて頂点配列作成時に使用する)
			const domSkin::domVertex_weightsRef rWeights = pSkinRef->getVertex_weights();
			// ウェイト情報の配列の場所を取得
			const domInputLocalOffset_Array& rWeightLocalinputArray = rWeights->getInput_array();

			// 本当はここでJOINTの示す配列と関連付ける必要があるがサンプルモデルではジョイントのルートが等しいので無視する
			// WEIGHT情報の参照先の取得
			std::string strWeithsArrayURI;
			for(UINT i = 0; i < rWeightLocalinputArray.getCount(); ++i)
			{
				if( strcmp("WEIGHT", rWeightLocalinputArray[i]->getSemantic()) == 0 )
				{
					strWeithsArrayURI = rWeightLocalinputArray[i]->getSource().getOriginalURI() + 1;
					break;
				}
			}

			// ウェイトの情報元の探索　これ関数化したほうがいいよねてかid値からひっぱるのないかなぁ・・・
			for(UINT i = 0; i < vSourceArray.getCount(); ++i)
			{
				if( strWeithsArrayURI == vSourceArray[i]->getID() )
				{
					const domFloat_arrayRef rWeightsFloatArray = vSourceArray[i]->getFloat_array();
					weightArray.resize(static_cast<u32>(rWeightsFloatArray->getCount()));
					for(UINT j = 0; j < rWeightsFloatArray->getCount(); ++j)
					{
						weightArray[j] = static_cast<float>(rWeightsFloatArray->getValue().get(j));
					}
					break;
				}
			}

		}
	}

	s32 nSkinNum = NS_STATIC_CAST( s32, (m_pDaeFile->getDatabase()->getElementCount(nullptr, COLLADA_SKIN, nullptr) ) );
	NS_ASSERT( nSkinNum <= 1 );

	for( s32 i = 0; i < nSkinNum; ++i)
	{
		domSkin* pSkin = nullptr;
		pDataBase->getElement( (domElement**)(&pSkin), i, nullptr, COLLADA_SKIN );

		if ( pSkin )
		{
			domSkin::domVertex_weightsRef vertWeights = pSkin->getVertex_weights();
			u32 unWeightCount = NS_STATIC_CAST( u32, vertWeights->getCount() );
			domSkin::domVertex_weights::domVRef vRef = vertWeights->getV();
			domSkin::domVertex_weights::domVcountRef vCount = vertWeights->getVcount();

			UINT vArrayIndex = 0;
			// 頂点配列に対応したボーンインデックスを取得
			for(UINT i = 0; i < unWeightCount; ++i)
			{
				for(UINT j = 0; j < vCount->getValue().get(i); ++j)
				{
					if( j >= 4 )
					{
						//頂点に影響するボーン数は最大４つとする。
						vArrayIndex += 2;
						break;
					}

					//ジョイントインデックス
					UINT ui32BoneIndex = static_cast<u32>( vRef->getValue().get(vArrayIndex) );
					m_pLoadModel->m_VertexArray[i].boneIndex[j] = (u8)ui32BoneIndex;
					++vArrayIndex;

					//重みインデックス
					if( j < 3 )
					{
						UINT ui32WeightIndex = static_cast<u32>(vRef->getValue().get(vArrayIndex));
						m_pLoadModel->m_VertexArray[i].weight[j] = weightArray[ ui32WeightIndex ];
					}
					++vArrayIndex;
				}
			}
		}
	}

	//親子関係を形成(NODE)
	s32 nNodeCount = NS_STATIC_CAST( s32, (m_pDaeFile->getDatabase()->getElementCount(nullptr, COLLADA_NODE, nullptr) ) );
	for( s32 i = 0; i < nNodeCount; ++i)
	{
		domNode* pNode = nullptr;
		m_pDaeFile->getDatabase()->getElement((daeElement**)&pNode, i, nullptr, COLLADA_NODE);

		// 取得したエレメントのIdがすでに取得済みのボーン名と一致するかどうか
		for(UINT i = 0; i < m_pLoadModel->m_paJoint.size(); ++i)
		{
			CJoint* thisJoint = m_pLoadModel->m_paJoint[i];
			if( strcmp( pNode->getID(), thisJoint->m_aStrJointName ) == 0 )
			{

#if 0
				domFloat3 scale = pNode->getScale_array().get(0)->getValue();
				m_pLoadModel->m_paJoint[i]. = D3DXVECTOR3(scale.get(0), scale.get(1), scale.get(2));

				const domRotate_Array& rotationArray = pNode->getRotate_array();

				m_pLoadModel->m_paJoint[i].rotaion.z = rotationArray.get(0)->getValue().get(3);
				m_pLoadModel->m_paJoint[i].rotaion.y = rotationArray.get(1)->getValue().get(3);
				m_pLoadModel->m_paJoint[i].rotaion.x = rotationArray.get(2)->getValue().get(3);

				domFloat3 trans = pNode->getTranslate_array().get(0)->getValue();
				m_pLoadModel->m_paJoint[i].translation = D3DXVECTOR3(trans.get(0), trans.get(1), trans.get(2));

				m_pLoadModel->m_paJoint[i].parentIndex = -1;

#endif

#if 0
				//親が存在する。
				if( pNode->getParent() )
				{ 
					for(UINT j = 0; j < m_pLoadModel->m_paJoint.size(); ++j)
					{
						if( strcmp( pNode->getParent()->getID(), m_pLoadModel->m_paJoint[j]->m_aStrJointName ) == 0 )
						{
							CJoint* pcParentJoint = m_pLoadModel->m_paJoint[j];	//親。

							if( !pcParentJoint->m_pFirstChild )
							{
								pcParentJoint->m_pFirstChild = thisJoint;	//子ジョイント設定
							}
							else
							{
								struct NodeSystem
								{
									//---------------------------------------------------------------------------
									/*!
										@brief	兄弟関係の構築
										@param[in] 子ポインタ
										@param[in] 兄弟ポインタ
									*/
									//---------------------------------------------------------------------------
									static void SetSibling( CJoint* pcChild, CJoint* pcSib ) 
									{
										if( pcChild->m_pSibling )
										{
											NodeSystem::SetSibling( pcChild->m_pSibling, pcSib );
										}
										else
										{
											pcChild->m_pSibling = pcSib;
										}
									}
								};

								//すでに子供いる！
								NodeSystem::SetSibling( pcParentJoint->m_pFirstChild, thisJoint );
							}
							
							break;
						}
					}
				}
#endif

				break;
			}
		}
	}

	// 初期姿勢を親の姿勢からの相対姿勢に直します。
	// まず子の末端まで下りて、自分のローカル空間での初期姿勢 × 親のボーンオフセット行列で相対姿勢が出ます
	// 親子関係を辿るので再起関数が必要です。
	// ボーンの初期姿勢をローカル空間→親空間に変換しておくらしい
	struct CalcRelativeMat {
		static void run(CJoint* me, NSMatrix *parentoffsetMat) {
			if ( me->m_pFirstChild )
				run( me->m_pFirstChild, &me->m_invMatrix );
			if ( me->m_pSibling )
				run( me->m_pSibling, parentoffsetMat );
			if ( parentoffsetMat )
				me->m_InitMat *= *parentoffsetMat;
		}
	};

	u32 size = m_pLoadModel->m_paJoint.size();
	for( u32 i = 0; i < size; ++i)
	{
		CalcRelativeMat::run( m_pLoadModel->m_paJoint[ i ], 0 );
	}

	if( size > 0 )
	{
		m_pLoadModel->m_paMatrix = new NSMatrix[ size ];
		for( u32 i = 0; i < size; ++i)
		{
			m_pLoadModel->m_paJoint[ i ]->m_pCombMatArray = &m_pLoadModel->m_paMatrix[i];
		}
	}

	//s32 nVisualScene = NS_STATIC_CAST( s32, (m_pDaeFile->getDatabase()->getElementCount( nullptr, COLLADA_VISUAL_SCENE, nullptr ) ) );
}

//---------------------------------------------------------------------------
/*!
	@brief	モーションデータ情報読み込み
*/
//---------------------------------------------------------------------------
void CColladaLoader::LoadMotion()
{
	/*
	u32 unAnimCount = NS_STATIC_CAST( s32, (m_pDaeFile->getDatabase()->getElementCount( nullptr, COLLADA_ANIMATION, nullptr ) ) );

	for( s32 i = 0; i < unAnimCount; ++i)
	{
		domAnimation* pAnimation = nullptr;
		m_pDaeFile->getDatabase()->getElement( (daeElement**)pAnimation, i, nullptr, COLLADA_ANIMATION );
	}
	*/
}

//----------------------------------------------------------------------------
/*	@brief	破棄
*/
//----------------------------------------------------------------------------
void CColladaLoader::Term()
{
	m_pLoadModel = nullptr;
}

#endif

//EOF

