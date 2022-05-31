//-----------------------------------------------------------------------------
// File: SDKWaveFile.cpp
//
// Desc: Classes for reading and writing wav files. Feel free to use this class
//       as a starting point for adding extra functionality.
//
// XNA Developer Connection
//
// Copyright (c) Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
//#define STRICT
//#include "../Header/DXUT.h"
#include "WaveFile.h"
#include "tchar.h"

#undef min // use __min instead
#undef max // use __max instead

#pragma comment( lib, "winmm.lib" )


//-----------------------------------------------------------------------------
// Name: CWaveFile::CWaveFile()
// Desc: Constructs the class.  Call Open() to open a wave file for reading.
//       Then call Read() as needed.  Calling the destructor or Close()
//       will close the file.
//-----------------------------------------------------------------------------
CWaveFile::CWaveFile()
    : data_offset( 0 )
{
    m_pwfx = NULL;
    m_hmmio = NULL;
    m_dwSize = 0;
}


//-----------------------------------------------------------------------------
// Name: CWaveFile::~CWaveFile()
// Desc: Destructs the class
//-----------------------------------------------------------------------------
CWaveFile::~CWaveFile()
{
    Close();

    if( m_pwfx != 0 )
    {
        delete[] m_pwfx;
        m_pwfx = 0;
    }
}


//-----------------------------------------------------------------------------
// Name: CWaveFile::Open()
// Desc: Opens a wave file for reading
//-----------------------------------------------------------------------------
HRESULT CWaveFile::Open(TCHAR* strwideFileName)
{
    HRESULT hr = S_OK;

	////WCHARへの変換

	//TCHAR strwideFileName[256] = { 0 };
	//mbstowcs(strwideFileName, filename.c_str(), strlen(filename.c_str()));

    m_hmmio = mmioOpen(strwideFileName, NULL, MMIO_ALLOCBUF | MMIO_READ );

    if( NULL == m_hmmio )
    {
		return E_FAIL;
        //return DXTRACE_ERR( _T("mmioOpen"), E_FAIL );
    }

    if( FAILED( hr = ReadMMIO() ) )
    {
        // ReadMMIO will fail if its an not a wave file
        mmioClose( m_hmmio, 0 );
		return hr;
        //return DXTRACE_ERR( _T("ReadMMIO"), hr );
    }

	if (FAILED(hr = ResetFile()))
		return hr;
        //return DXTRACE_ERR( _T("ResetFile"), hr );

    // After the reset, the size of the wav file is m_ck.cksize so store it now
    m_dwSize = m_ck.cksize;

    //dataチャンクまでのオフセットを取得
    data_offset = mmioSeek( m_hmmio, 0, SEEK_CUR );

    return hr;
}


//-----------------------------------------------------------------------------
// Name: CWaveFile::ResetFile()
// Desc: Resets the internal m_ck pointer so reading starts from the
//       beginning of the file again
//-----------------------------------------------------------------------------
HRESULT CWaveFile::ResetFile()
{
    if( m_hmmio == NULL )
        return CO_E_NOTINITIALIZED;

    // Seek to the data
	if (-1 == mmioSeek(m_hmmio, m_ckRiff.dwDataOffset + sizeof(FOURCC),
		SEEK_SET))
		return E_FAIL;
        //return DXTRACE_ERR( _T("mmioSeek"), E_FAIL );

    // Search the input file for the 'data' chunk.
    m_ck.ckid = mmioFOURCC( 'd', 'a', 't', 'a' );
	if (0 != mmioDescend(m_hmmio, &m_ck, &m_ckRiff, MMIO_FINDCHUNK))
		return E_FAIL;
        //return DXTRACE_ERR( _T("mmioDescend"), E_FAIL );

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CWaveFile::ReadMMIO()
// Desc: Support function for reading from a multimedia I/O stream.
//       m_hmmio must be valid before calling.  This function uses it to
//       update m_ckRiff, and m_pwfx.
//-----------------------------------------------------------------------------
HRESULT CWaveFile::ReadMMIO()
{
    MMCKINFO ckIn;           // chunk info. for general use.
    PCMWAVEFORMAT pcmWaveFormat;  // Temp PCM structure to load in.

    m_pwfx = NULL;

	if ((0 != mmioDescend(m_hmmio, &m_ckRiff, NULL, 0)))
		return E_FAIL;
        //return DXTRACE_ERR( _T("mmioDescend"), E_FAIL );

    // Check to make sure this is a valid wave file
    if( ( m_ckRiff.ckid != FOURCC_RIFF ) ||
		(m_ckRiff.fccType != mmioFOURCC('W', 'A', 'V', 'E')))
		return E_FAIL;
        //return DXTRACE_ERR( _T("mmioFOURCC"), E_FAIL );

    // Search the input file for for the 'fmt ' chunk.
    ckIn.ckid = mmioFOURCC( 'f', 'm', 't', ' ' );
	if (0 != mmioDescend(m_hmmio, &ckIn, &m_ckRiff, MMIO_FINDCHUNK))
		return E_FAIL;
        //return DXTRACE_ERR( _T("mmioDescend"), E_FAIL );

    // Expect the 'fmt' chunk to be at least as large as <PCMWAVEFORMAT>;
    // if there are extra parameters at the end, we'll ignore them
		if (ckIn.cksize < (LONG)sizeof(PCMWAVEFORMAT))
			return E_FAIL;
        //return DXTRACE_ERR( _T("sizeof(PCMWAVEFORMAT)"), E_FAIL );

    // Read the 'fmt ' chunk into <pcmWaveFormat>.
    if( mmioRead( m_hmmio, ( HPSTR )&pcmWaveFormat,
		sizeof(pcmWaveFormat)) != sizeof(pcmWaveFormat))
		return E_FAIL;
        //return DXTRACE_ERR( _T("mmioRead"), E_FAIL );

    // Allocate the waveformatex, but if its not pcm format, read the next
    // word, and thats how many extra bytes to allocate.
    if( pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM )
    {
        m_pwfx = ( WAVEFORMATEX* )new CHAR[ sizeof( WAVEFORMATEX ) ];
		if (NULL == m_pwfx)
			return E_FAIL;
            //return DXTRACE_ERR( _T("m_pwfx"), E_FAIL );

        // Copy the bytes from the pcm structure to the waveformatex structure
        memcpy( m_pwfx, &pcmWaveFormat, sizeof( pcmWaveFormat ) );
        m_pwfx->cbSize = 0;
    }
    else
    {
        // Read in length of extra bytes.
        WORD cbExtraBytes = 0L;
		if (mmioRead(m_hmmio, (CHAR*)&cbExtraBytes, sizeof(WORD)) != sizeof(WORD))
			return E_FAIL;
            //return DXTRACE_ERR( _T("mmioRead"), E_FAIL );

        m_pwfx = ( WAVEFORMATEX* )new CHAR[ sizeof( WAVEFORMATEX ) + cbExtraBytes ];
		if (NULL == m_pwfx)
			return E_FAIL;
            //return DXTRACE_ERR( _T("new"), E_FAIL );

        // Copy the bytes from the pcm structure to the waveformatex structure
        memcpy( m_pwfx, &pcmWaveFormat, sizeof( pcmWaveFormat ) );
        m_pwfx->cbSize = cbExtraBytes;

        // Now, read those extra bytes into the structure, if cbExtraAlloc != 0.
        if( mmioRead( m_hmmio, ( CHAR* )( ( ( BYTE* )&( m_pwfx->cbSize ) ) + sizeof( WORD ) ),
                      cbExtraBytes ) != cbExtraBytes )
		{
			return E_FAIL;
            //return DXTRACE_ERR( _T("mmioRead"), E_FAIL );
        }
    }

    // Ascend the input file out of the 'fmt ' chunk.
    if( 0 != mmioAscend( m_hmmio, &ckIn, 0 ) )
	{
		return E_FAIL;
        //return DXTRACE_ERR( _T("mmioAscend"), E_FAIL );
    }

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: CWaveFile::GetSize()
// Desc: Retuns the size of the read access wave file
//-----------------------------------------------------------------------------
DWORD CWaveFile::GetSize()
{
    return m_dwSize;
}




//-----------------------------------------------------------------------------
// Name: CWaveFile::Read()
// Desc: Reads section of data from a wave file into pBuffer and returns
//       how much read in pdwSizeRead, reading not more than dwSizeToRead.
//       This uses m_ck to determine where to start reading from.  So
//       subsequent calls will be continue where the last left off unless
//       Reset() is called.
//-----------------------------------------------------------------------------
HRESULT CWaveFile::Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead )
{
    MMIOINFO mmioinfoIn; // current status of m_hmmio

    if( m_hmmio == NULL )
        return CO_E_NOTINITIALIZED;
    if( pBuffer == NULL || pdwSizeRead == NULL )
        return E_INVALIDARG;

    *pdwSizeRead = 0;

	if (0 != mmioGetInfo(m_hmmio, &mmioinfoIn, 0))
		return E_FAIL;
        //return DXTRACE_ERR( _T("mmioGetInfo"), E_FAIL );

    UINT cbDataIn = dwSizeToRead;
    if( cbDataIn > m_ck.cksize )
        cbDataIn = m_ck.cksize;

    m_ck.cksize -= cbDataIn;

    for( DWORD cT = 0; cT < cbDataIn; cT++ )
    {
        // Copy the bytes from the io to the buffer.
        if( mmioinfoIn.pchNext == mmioinfoIn.pchEndRead )
        {
			if (0 != mmioAdvance(m_hmmio, &mmioinfoIn, MMIO_READ))
				return E_FAIL;
                //return DXTRACE_ERR( _T("mmioAdvance"), E_FAIL );

			if (mmioinfoIn.pchNext == mmioinfoIn.pchEndRead)
				return E_FAIL;
				//return DXTRACE_ERR(_T("mmioinfoIn.pchNext"), E_FAIL);
        }

        // Actual copy.
        *( ( BYTE* )pBuffer + cT ) = *( ( BYTE* )mmioinfoIn.pchNext );
        mmioinfoIn.pchNext++;
    }

	if (0 != mmioSetInfo(m_hmmio, &mmioinfoIn, 0))
		return E_FAIL;
        //return DXTRACE_ERR( _T("mmioSetInfo"), E_FAIL );

    *pdwSizeRead = cbDataIn;

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: CWaveFile::Close()
// Desc: Closes the wave file
//-----------------------------------------------------------------------------
HRESULT CWaveFile::Close()
{
    if( m_hmmio != 0 )
    {
        mmioClose( m_hmmio, 0 );
        m_hmmio = NULL;
    }

    return S_OK;
}


HRESULT CWaveFile::Seek( int offset, int origin )
{
    // Seek to the data
	if (-1 == mmioSeek(m_hmmio, offset, origin))
		return E_FAIL;
        //return DXTRACE_ERR( _T("mmioSeek"), E_FAIL );

    m_ck.cksize -= offset;

    return S_OK;
}
