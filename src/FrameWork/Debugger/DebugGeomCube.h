
#ifndef __DEBUGGEOMCUBE_H__
#define __DEBUGGEOMCUBE_H__

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		DebugGeomCube.h
	@brief		デバッグ用ジオメトリ（キューブ）
	@author		ta.Ishii
	@date		2012/9/9 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#if defined( _DEBUG )

#if false

#include "DebugGeomBase.h"

//==========================================================================*/
/*!
	@class	CDebugGeomCube
	@brief	デバッグ用ジオメトリ（キューブ）
*/
//==========================================================================*/
class CDebugGeomCube : public CDebugGeomBase
{
public:
	CDebugGeomCube() {};
	virtual ~CDebugGeomCube() {};

protected:
	//メッシュ作成関数
	virtual bool CreateMesh();
};

#endif

#endif	//	_DEBUG
#endif	//	__DEBUGGEOMCUBE_H__

//EOF