//-----------------------------------------------------------------------------
// File: WaveFile.h
//
// Copyright (c) Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef WAVEFILE_H
#define WAVEFILE_H

#include <windows.h>
#include <string>

// HRESULT translation for Direct3D10 and other APIs 
//#include <dxerr.h>

#if defined(DEBUG) | defined(_DEBUG)
#define DXTRACE_MSG(str)              DXTrace( __FILE__, (DWORD)__LINE__, 0, str, FALSE )
#define DXTRACE_ERR(str,hr)           DXTrace( __FILE__, (DWORD)__LINE__, hr, str, FALSE )
#define DXTRACE_ERR_MSGBOX(str,hr)    DXTrace( __FILE__, (DWORD)__LINE__, hr, str, TRUE )
#else
#define DXTRACE_MSG(str)              (0L)
#define DXTRACE_ERR(str,hr)           (hr)
#define DXTRACE_ERR_MSGBOX(str,hr)    (hr)
#endif

#include <mmsystem.h>

//-----------------------------------------------------------------------------
// Typing macros 
//-----------------------------------------------------------------------------
#define WAVEFILE_READ   1
#define WAVEFILE_WRITE  2


//-----------------------------------------------------------------------------
// Name: class CWaveFile
// Desc: Encapsulates reading or writing sound data to or from a wave file
//-----------------------------------------------------------------------------
class CWaveFile
{
public:
    WAVEFORMATEX* m_pwfx;        // Pointer to WAVEFORMATEX structure
    HMMIO m_hmmio;       // MM I/O handle for the WAVE
    MMCKINFO m_ck;          // Multimedia RIFF chunk
    MMCKINFO m_ckRiff;      // Use in opening a WAVE file
    DWORD m_dwSize;      // The size of the wave file
    BYTE* m_pbData;
    DWORD data_offset;  //dataチャンクまでのオフセット

protected:
    HRESULT ReadMMIO();

public:
            CWaveFile();
            ~CWaveFile();

			HRESULT Open(TCHAR* strwideFileName);
    HRESULT Close();

    HRESULT Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead );

    DWORD   GetSize();
    HRESULT ResetFile();
    WAVEFORMATEX* GetFormat()
    {
        return m_pwfx;
    };

    HRESULT Seek( int offset, int origin );
};


#endif // DXUTWAVEFILE_H
