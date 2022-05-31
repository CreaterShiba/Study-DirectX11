#pragma once

#define INVALID_U32 0xFFFFFFFF
#define INVALID_U8 0xFF

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		UtilDefine.h
	@brief		ユーティリティ定義
	@author		shiba
	@date		2012/3/1 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
typedef char s8;
typedef unsigned char u8;

typedef short s16;
typedef unsigned short u16;

typedef int s32;
typedef unsigned int u32;

typedef long s64;
typedef unsigned long u64;

typedef float f32;

typedef unsigned char	UTF8;	///< UTF-8文字形式(8bit)
typedef unsigned short	UTF16;	///< UTF-16文字形式(16bit)

typedef DirectX::XMMATRIX NSMatrix;
typedef DirectX::XMVECTOR NSVector4;
typedef DirectX::XMVECTOR NSQuat;
//typedef DirectX::XMFLOAT4 NSVector4;

#define SHADER_PRE_COMPILE

const int WINDOW_WIDTH = 1600;
const int WINDOW_HEIGHT = 900;

const DWORD FPS = 60; // FPS設定

//EOF

