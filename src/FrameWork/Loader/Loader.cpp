﻿/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		Loader.cpp
	@brief		ファイルローダー
	@author		shiba
	@date		2012/7/24 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#include "Loader.h"
#include "LoaderManager.h"

//----------------------------------------------------------------------------
/*	@brief	コンストラクタ
*/
//----------------------------------------------------------------------------
CLoader::CLoader() :
m_eLoadSeq( E_LOADSEQ::eLOADSEQ_INIT )
{
}

//----------------------------------------------------------------------------
/*	@brief	デストラクタ
*/
//----------------------------------------------------------------------------
CLoader::~CLoader()
{
	Term();
}

//----------------------------------------------------------------------------
/*	@brief	初期化
*/
//----------------------------------------------------------------------------
bool CLoader::Init()
{	
	return true;
}

void CLoader::Term()
{
}

//EOF
