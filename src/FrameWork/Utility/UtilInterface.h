#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		UtilInterface.h
	@brief		ユーティリティインターフェイス
	@author		shiba
	@date		2012/9/11 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

//コピー禁止
class NSUnCopyable
{
protected:
	NSUnCopyable(){}
	virtual ~NSUnCopyable(){}

private:
	NSUnCopyable( const NSUnCopyable& );
	NSUnCopyable& operator =( const NSUnCopyable& );
};

//EOF