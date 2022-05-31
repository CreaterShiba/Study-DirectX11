#pragma once
/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		Loader.h
	@brief		ファイルローダー
	@author		shiba
	@date		2012/7/24 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#include "FrameWork/Utility/UtilDefine.h"
#include "FrameWork/Task/Task.h"
#include "FrameWork/Utility/UtilInterface.h"

//==========================================================================*/
/*!
	@class	Loader
	@brief	ローダークラス
*/
//==========================================================================*/
class CLoader : public NSUnCopyable
{
protected:
	enum E_LOADSEQ
	{
		eLOADSEQ_INIT,		//ロード初期化中
		eLOADSEQ_LOAD,		//ロード開始中	
		eLOADSEQ_LOADEND,	//ロード終了
	};

public:
	CLoader();
	virtual ~CLoader();

public:
	virtual	bool	Init();											//初期化
	virtual void	Update(const f32 fDelta) { NS_UNUSED(fDelta); }	//更新
	virtual void	Destroy() {}									//破棄

	//@brief ロード開始(非同期読み込み) @todo まだ
	virtual bool	LoadStartAsync() { return true; }
	//@brief ロード完了した？
	inline bool		IsLoadEnd();

private:
	void Term();	//破棄処理

protected:
	E_LOADSEQ m_eLoadSeq;
};

//--------------------------------------------------------------------
// inline
//--------------------------------------------------------------------
bool CLoader::IsLoadEnd()
{
	return m_eLoadSeq == eLOADSEQ_LOADEND;
}

//EOF
