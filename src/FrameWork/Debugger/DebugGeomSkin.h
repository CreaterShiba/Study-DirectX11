#ifndef __DEBUGGEOMSKIN_H__
#define __DEBUGGEOMSKIN_H__

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		DebugGeomSkin.h
	@brief		検証用スキン
	@author		ta.Ishii
	@date		2012/11/4 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#if defined( _DEBUG )

#if false

#include "DebugGeomBase.h"

/*==========================================================================*/
/*!
	@class	CDebugSkin
	@brief	デバッグ用スキン
*/
/*==========================================================================*/
class CDebugGeomSkin : public CDebugGeomBase
{
public:
	CDebugGeomSkin();
	virtual ~CDebugGeomSkin();

	virtual void ShaderCalc( const CShaderManager::E_SHADERID eShaderID );	
	virtual void Calc( const f32 fDelta );

protected:
	//メッシュ作成関数
	virtual bool CreateMesh();	

private:
	//!< 描画モデル
	class CModel* m_pcSkinModel;
};

#endif

#endif	//	_DEBUG
#endif // __DEBUGGEOMSKIN_H__

//EOF