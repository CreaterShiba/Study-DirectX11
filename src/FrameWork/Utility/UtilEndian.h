#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		UtilEndian.h
	@brief		エンディアン操作
	@author		shiba
	@date		2012/9/12 create this file.
	@note		http://projectkuto.web.fc2.com/program/endian.html
*/
/*==========================================================================*/
class CUtilEndian
{
public:
	/**
	 * エンディアン変換　16ビット用
	 * @param value         値
	 */
	template <class T>
	static void swap_endian16( T& value)
	{
		value = ((value & 0xFF) << 8) | ((value >> 8) & 0xFF);
	}

	/**
	 * エンディアン変換　32ビット用
	 * @param value         値
	 */
	template <class T>
	static void swap_endian32( T& value)
	{
		value = (value >> 24) | (value << 24) | ((value >> 8) & 0xFF00) | ((value << 8) & 0xFF0000);
	}
};

//EOF
