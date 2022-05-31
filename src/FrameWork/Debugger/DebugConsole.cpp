/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		DebugConsole.cpp
	@brief		コンソールデバッグ周り
	@author		shiba
	@date		2012/9/29 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#include "DebugConsole.h"
#include "Framework/Utility/UtilDefine.h"

CConsoleDebug::E_OUTLEVEL CConsoleDebug::m_eVerbosity = CConsoleDebug::E_OUTLEVEL_LOW;	//この値以下のものは出ない。

//----------------------------------------------------------------------------
/*	@brief		出力ウィンドウに文字を出力
	@param[in]	pBuff	文字列
	@param[in]	...		可変引数(pBuff)
*/	
//----------------------------------------------------------------------------
void CConsoleDebug::Output(  const TCHAR* pBuff, ...)
{
#if defined(_DEBUG)
	//@todo ２つ以上の引数が指定されたときうまくいかない？
	if( !pBuff )
	{
		//第一引数が設定されていない
		//冗長性レベルが高いときのみ、出力
		return;
	}

	TCHAR	strTemp[ 1024 ];
	s32		nLength = 0;
	ZeroMemory(strTemp, 1024);

	va_list argptr;
	va_start( argptr, pBuff );	// argptrを引数のアドレスに指定する。
	nLength = _vstprintf_s( strTemp, 1024, pBuff, argptr );
	va_end( argptr );			// 後始末

	if( nLength > 0 )
	{
		OutputDebugString( strTemp );
	}
#endif
}

//----------------------------------------------------------------------------
/*	@brief		出力ウィンドウに文字を出力
	@param[in]	eVerbosity	出力レベル
	@param[in]	pBuff	文字列
	@param[in]	...		可変引数(pBuff)
*/	
//----------------------------------------------------------------------------
void CConsoleDebug::Output_LV( const CConsoleDebug::E_OUTLEVEL eVerbosity, const TCHAR* pBuff, ... )
{
#if defined(_DEBUG)
	if( !pBuff && m_eVerbosity > eVerbosity )
	{
		//第一引数が設定されていない
		//冗長性レベルが高いときのみ、出力
		return;
	}

	TCHAR	strTemp[ 1024 ];
	s32		nLength = 0;

	va_list argptr;
	va_start(argptr, pBuff);	// argptrを引数のアドレスに指定する。
	nLength = _vstprintf_s(strTemp, 1024, pBuff, argptr);
	va_end( argptr );			// 後始末

	if( nLength > 0 )
	{
		OutputDebugString( strTemp );
	}
#endif
}

//EOF