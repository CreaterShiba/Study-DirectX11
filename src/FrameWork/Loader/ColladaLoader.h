#ifndef __COLLADALOADER_H__
#define __COLLADALOADER_H__

#if 0
/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		ColladaLoader.h
	@brief		Colladaローダー
	@author		ta.Ishii
	@date		2012/9/22 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

//--------------------------------------------------------------------
//Collada関連ライブラリリンク 
//--------------------------------------------------------------------

#pragma comment ( lib, "wsock32.lib" )
#pragma comment ( lib, "zlib.lib" )
#pragma comment ( lib, "libxml2_a.lib" )

#if defined( _DEBUG )

#pragma comment ( lib, "libcollada14dom22-sd.lib" )
#pragma comment ( lib, "libboost_filesystem-d.lib" )
#pragma comment ( lib, "libboost_system-d.lib" )
#pragma comment ( lib, "minizip-d.lib" )
#pragma comment ( lib, "pcrecpp-d.lib" )
#pragma comment ( lib, "pcre-d.lib" )

#endif

#if defined( _RELEASE )

#pragma comment ( lib, "libcollada14dom22-s.lib" )
#pragma comment ( lib, "libboost_filesystem.lib" )
#pragma comment ( lib, "libboost_system.lib" )
#pragma comment ( lib, "minizip.lib" )
#pragma comment ( lib, "pcrecpp.lib" )
#pragma comment ( lib, "pcre.lib" )

#endif

#include "Framework/Loader/ModelLoader.h"

#include <vector>
#include <string>

class domTriangles;
class domMesh;

//==========================================================================*/
/*!
	@class	CColladaLoader
	@brief	Colladaファイルローダー
			マルチプラットフォームで使いたい。
			ひとまず、頂点、法線、テクスチャ機能を使えれば。
			とりあえずすべてここにぶち込みます。
			@todo ハーフエッジ・・・？（by Computer Graphics Gems JP 2012 )
*/
//==========================================================================*/
class CColladaLoader : public CModelLoader
{
	//@note コピー禁止
	void operator =(const CColladaLoader& src);
	CColladaLoader(const CColladaLoader& src);

public:
	CColladaLoader();
	virtual ~CColladaLoader();

public:
	virtual	bool	Init();												//初期化
	virtual void	Calc( const f32 fDelta );							//更新
	virtual void	Destroy();											//破棄
	virtual bool	Load( const TCHAR* strFileName, CModel* pcModel );	//ロード

private:

	virtual void LoadGeometry();
	virtual void LoadMaterial();
	virtual void LoadTexture();
	virtual void LoadSkin();		
	virtual void LoadMotion();		

	void Term();

private:
	class DAE*		m_pDaeFile;		//!< DAEファイル
	class CModel*	m_pLoadModel;	//!< ロードモデル
	// インデックスデータ
	std::vector< u16 >			m_IndexArray;
};
#endif

#endif	//__COLLADALOADER_H__

//EOF