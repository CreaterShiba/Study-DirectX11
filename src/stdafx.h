#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		StdAfx.h
	@brief		プリコンパイル済みヘッダー
	@author		shiba
	@date		2013/2/11 create this file.
*/
/*==========================================================================*/

//-------------------------------------------------------------- 
//  windows
//-------------------------------------------------------------- 
//#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーから使用されていない部分を除外します。
#include <stdio.h> 
#include <assert.h> 
#include <tchar.h> 
#include <windows.h>
#include <math.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>

#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

//-------------------------------------------------------------- 
//  std
//-------------------------------------------------------------- 
#include <string> 
#include <vector> 
#include <map>

//--------------------------------------------------------------------
// Engine
//--------------------------------------------------------------------
#include "FrameWork/Utility/UtilDefine.h"
#include "FrameWork/Utility/UtilMacro.h"
#include "FrameWork/Utility/UtilColor.h"

#include "FrameWork/Math/vector2.h"
#include "FrameWork/Math/vector3.h"
#include "FrameWork/Math/vector4.h"
#include "FrameWork/Math/matrix.h"

#include "FrameWork/DXUtil/UtilDX11.h"
#include "FrameWork/DXUtil/DXMng.h"

#include "FrameWork/Utility/UtilHash.h"
#include "FrameWork/Utility/UtilMath.h"


//--------------------------------------------------------------------
// Engine(Debug)
//--------------------------------------------------------------------
#include "FrameWork/Debugger/DebugConsole.h"
#if defined( _DEBUG )
#include "FrameWork/Debugger/DebugFps.h"
#include "FrameWork/Debugger/DebugGeometry.h"
#include "FrameWork/Debugger/DebugProfile.h"
#endif

//--------------------------------------------------------------------
// Project
//--------------------------------------------------------------------
#include "GameSystem/ProjectDefine.h"
#include "GameSystem/ProjectParam.h"

//EOF
