
#if defined( _DEBUG )

#if false
/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		DebugGeomCube.cpp
	@brief		デバッグ用ジオメトリ（キューブ）
	@author		ta.Ishii
	@date		2012/9/9 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#include "DebugGeomCube.h"
#include "FrameWork/Asset/AssetMesh.h"

using namespace DirectX;
//---------------------------------------------------------------------------
/*!
	@brief	メッシュ作成関数
*/
//---------------------------------------------------------------------------
bool CDebugGeomCube::CreateMesh()
{
	//バッファ生成
	struct BasicVertex VertexDataA[] = 
	{
		{ NSVector3(-1.0f, -1.0f,-1.0f),	 NSVector2(0.0f, 1.0f),	NSVector3(1.0f, 1.0f, 1.0f),	},
		{ NSVector3(1.0f,	 -1.0f,-1.0f),	 NSVector2(0.0f, 1.0f),	NSVector3(1.0f, 1.0f, 1.0f),	},
		{ NSVector3(-1.0f, 1.0f, -1.0f),	 NSVector2(0.0f, 1.0f),	NSVector3(1.0f, 1.0f, 1.0f),	},
		{ NSVector3(1.0f,	 1.0f, -1.0f),	 NSVector2(0.0f, 1.0f),	NSVector3(1.0f, 1.0f, 1.0f),	},
		{ NSVector3(-1.0f, -1.0f, 1.0f),	 NSVector2(0.0f, 1.0f),	NSVector3(1.0f, 1.0f, 1.0f),	},
		{ NSVector3(1.0f,	 -1.0f, 1.0f),	 NSVector2(0.0f, 1.0f),	NSVector3(1.0f, 1.0f, 1.0f),	},
		{ NSVector3(-1.0f, 1.0f,	1.0f),	 NSVector2(0.0f, 1.0f),	NSVector3(1.0f, 1.0f, 1.0f),	},
		{ NSVector3(1.0f,	 1.0f,	1.0f),	 NSVector2(0.0f, 1.0f),	NSVector3(1.0f, 1.0f, 1.0f),	},
	};

	//インデックスバッファの数
	// 板ポリ = 12個 * 3頂点 = 36
	u16 idxVertexID[] = 
	{
		0,1,2,1,2,3,
		4,0,6,0,6,2,
		5,4,7,4,7,6,
		1,5,3,5,3,7,
		4,5,0,5,0,1,
		6,7,2,7,2,3,
	};

	if (m_pcDebugGeomMesh != nullptr)
	{
		m_pcDebugGeomMesh->SetVertexData(VertexDataA, 8);
		m_pcDebugGeomMesh->SetIndexData(idxVertexID, 36);
		m_pcDebugGeomMesh->CreateBuffer();
	}
	return true;
}

#endif

#endif	//	_DEBUG

//EOF