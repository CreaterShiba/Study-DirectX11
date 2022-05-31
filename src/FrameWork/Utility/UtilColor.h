#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		UtilColor.h
	@brief		カラーユーティリティー
	@author		shiba
	@date		2012/9/9 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

typedef NSVector4 SColor;

//==========================================================================*/
/*!
	@class	CUtilColor
	@brief	カラーユーティリティー
*/
//==========================================================================*/
class CUtilColor
{
public:
	CUtilColor()
	{
		m_vColor = s_cWhite;
	};
	virtual ~CUtilColor(){};

public:
	/*
	enum E_COLORTYPE
	{
		eCOLORTYPE_RED,				// 赤
		eCOLORTYPE_BLUE,			// 青
		eCOLORTYPE_GREEN,			// 緑
		eCOLORTYPE_YELLOW,			// 黄色
		eCOLORTYPE_CYAN,			// 水色
		eCOLORTYPE_MAGENTA,			// 紫色
		eCOLORTYPE_BLACK,			// 黒
		eCOLORTYPE_ORANGE,			// 橙
		eCOLORTYPE_WHITE,			// 白
		
		eCOLORTYPE_SUM,
	};
	*/

	static	const SColor	s_cRed;			//!< 赤
	static	const SColor	s_cBlue;		//!< 青
	static	const SColor	s_cGreen;		//!< 緑
	static	const SColor	s_cYellow;		//!< 黄
	static	const SColor	s_cCyan;		//!< 水
	static	const SColor	s_cPurple;		//!< 紫
	static	const SColor	s_cBlack;		//!< 黒
	static	const SColor	s_cOrange;		//!< 橙
	static	const SColor	s_cWhite;		//!< 白

protected:
	SColor m_vColor;	//色情報

public:
	void SetColor( const SColor& vCol )
	{
		m_vColor = vCol;
	}
	const CUtilColor::SColor& GetColor()
	{
		return m_vColor;
	}
};

//EOF