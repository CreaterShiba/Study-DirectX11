/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		FileIOWin.cpp
	@brief		ファイル読み込み(Windows)
	@author		shiba
	@date		2013/2/2 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#include "FileIOWin.h"

//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CFileIOWin::CFileIOWin()
: m_hFileHandle(  )
, m_hEventHandle(  )
, m_sOverLap()
, m_bAsync( false )
{

}

//---------------------------------------------------------------------------
/*!
	@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CFileIOWin::~CFileIOWin()
{
	Close();
}

//---------------------------------------------------------------------------
/*!
	@brief	初期化
*/
//---------------------------------------------------------------------------
bool CFileIOWin::Init()
{
	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	Win32Genericタイプへ
*/
//---------------------------------------------------------------------------
u64 convGeneric( const CFileIOBase::E_ACCESS_TYPE eType )
{
	u64 accessType = 0;

	switch( eType )
	{
	case CFileIOBase::eFile_Access_Write:
		accessType = GENERIC_WRITE;
		break;
	case CFileIOBase::eFile_Access_Read:
		accessType = GENERIC_READ;
		break;
	default:
		break;
	}

	return accessType;
}

//---------------------------------------------------------------------------
/*!
	@brief	ファイルオープン
*/
//---------------------------------------------------------------------------
bool CFileIOWin::Open( const TCHAR* pFileName, const E_ACCESS_TYPE eType )
{
	if( m_hFileHandle )
	{
		NS_Report(_T("FileAlreadyOpen[%s]\n"), m_strFileName );
		return false;
	}

	u64 accessType = convGeneric( eType );

	m_hFileHandle = CreateFile( 
		pFileName, 
		accessType, 
		FILE_SHARE_READ, 
		nullptr,
		OPEN_ALWAYS/*OPEN_EXISTING*/, 
		0, 
		nullptr);

	if( m_hFileHandle == INVALID_HANDLE_VALUE )
	{
		NS_Report(_T("FileOpenError[%s]\n"), pFileName );
		return false;
	}

	m_FileSize = GetFileSize( m_hFileHandle, 0 );
	if( m_FileSize == -1 )
	{
		if( GetLastError() == NO_ERROR )
		{
			//リアルにサイズがマイナス１
			NS_Report(_T("GetFileSizeError[%s]\n"), pFileName );
			Close();
			return false;
		}
		else
		{
			NS_Report(_T("GetFileSizeError[%s]\n"), pFileName );
			Close();
			return false;
		}
	}

	memcpy_s( m_strFileName, sizeof( m_strFileName ), pFileName, sizeof( m_strFileName ) );
	m_eAccessType = eType;

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	ファイルオープン（非同期）
*/
//---------------------------------------------------------------------------
bool CFileIOWin::OpenAsync( const TCHAR* pFileName, const E_ACCESS_TYPE eType )
{
	if( m_hFileHandle )
	{
		NS_Report(_T("FileAlreadyOpen[%s]\n"), m_strFileName );
		return false;
	}

	u64 accessType = convGeneric( eType );

	m_hFileHandle = CreateFile( 
		pFileName, 
		accessType, 
		FILE_SHARE_READ, 
		nullptr,
		OPEN_ALWAYS/*OPEN_EXISTING*/, 
		FILE_FLAG_OVERLAPPED, 
		nullptr);

	if( m_hFileHandle == INVALID_HANDLE_VALUE )
	{
		NS_Report(_T("FileOpenError[%s]\n"), pFileName );
		return false;
	}

	m_FileSize = GetFileSize( m_hFileHandle, 0 );
	if( m_FileSize == -1 )
	{
		if( GetLastError() == NO_ERROR )
		{
			//リアルにサイズがマイナス１
			NS_Report(_T("GetFileSizeError[%s]\n"), pFileName );
			Close();
			return false;
		}
		else
		{
			NS_Report(_T("GetFileSizeError[%s]\n"), pFileName );
			Close();
			return false;
		}
	}

	m_hEventHandle = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	if( !m_hEventHandle )
	{
		Close();
		return false;
	}

	ZeroMemory( &m_sOverLap, sizeof( m_sOverLap ) );
	m_sOverLap.Offset = 0;
	m_sOverLap.OffsetHigh = 0;
	m_sOverLap.hEvent = m_hEventHandle;

	m_bAsync = true;
	memcpy_s( m_strFileName, sizeof( m_strFileName ), pFileName, sizeof( m_strFileName ) );
	m_eAccessType = eType;

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	ファイルクローズ
*/
//---------------------------------------------------------------------------
bool CFileIOWin::Close()
{
	bool bRet = false;

	if( m_hEventHandle )
	{
		CloseHandle( m_hEventHandle );
		m_hEventHandle = nullptr;
	}

	if( m_hFileHandle )
	{
		if( CloseHandle( m_hFileHandle ) ) 
		{
			bRet = true;
			m_hFileHandle = nullptr;
		}
		else
		{
			bRet = false;
		}
	}
	else
	{
		bRet = false;
	}
	return bRet;
}

//---------------------------------------------------------------------------
/*!
	@brief		ファイルリード
	@param[in]	ロードバッファ	
	@param[in]	読み込みサイズ
	@return		実際に読み込んだサイズ
*/
//---------------------------------------------------------------------------
u32 CFileIOWin::Read( void* pBuf, const u32 Size )
{
	if( m_eAccessType == eFile_Access_Read )
	{
		if( m_hFileHandle )
		{
			u64 readsize = 0;
			if( m_bAsync )
			{
				//非同期読み込み
				ReadFile( m_hFileHandle, pBuf, Size, &readsize, &m_sOverLap );
			}
			else
			{
				//通常読み込み
				ReadFile( m_hFileHandle, pBuf, Size, &readsize, nullptr);
			}

			return static_cast< u32 >( readsize );
		}
		else
		{
			return 0;
		}
	}
	else
	{
		NS_Report(_T("FileAccessTypeError... m_eAccessType is Write\n"));
		return 0;
	}
}

//---------------------------------------------------------------------------
/*!
	@brief		ファイル書き込み
	@param[in]	書き込むバッファ	
	@param[in]	書き込みサイズ
	@return		実際に書き込んだサイズ
*/
//---------------------------------------------------------------------------
u32 CFileIOWin::Write( const void* const pBuf, const u32 Size )
{
	if( m_eAccessType == CFileIOBase::eFile_Access_Write )
	{
		if( m_hFileHandle )
		{
			u64 writesize = 0;

			WriteFile( m_hFileHandle, pBuf, Size, &writesize, nullptr);

			FlushFileBuffers( m_hFileHandle );

			return static_cast< u32 >( writesize );
		}
		else
		{
			return 0;
		}
	}
	else
	{
		NS_Report(_T("FileAccessTypeError... m_eAccessType is Read\n"));
		return 0;
	}
}

//---------------------------------------------------------------------------
/*!
	@brief	
*/
//---------------------------------------------------------------------------
bool CFileIOWin::IsExecuting()
{
	if( m_bAsync )
	{
		if( WaitForSingleObject( m_hEventHandle, INFINITE ) == WAIT_OBJECT_0 )
		{
			return true;
		}
		else
		{
			return false;
		}
		//return HasOverlappedIoCompleted( &m_sOverLap );
	}
	else
	{
		return true;
	}
}

//EOF
