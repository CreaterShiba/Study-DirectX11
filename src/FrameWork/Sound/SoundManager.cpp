/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		SoundManager.cpp
	@brief		サウンド管理
	@author		shiba
	@date		2012/9/29 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "SoundManager.h"

//#define CUT_XAUDIO
#if !defined(CUT_XAUDIO)
//Windows7で動かない
#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
#if defined(_MSC_VER) && (_MSC_VER < 1700)
#error DirectX Tool Kit for Audio does not support VS 2010 without the DirectX SDK 
#endif
#include <xaudio2.h>
#include <xaudio2fx.h>
#include <x3daudio.h>
#include <xapofx.h>
#pragma comment(lib,"xaudio2.lib")
#else
// Using XAudio 2.7 requires the DirectX SDK
#include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\comdecl.h>
#include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\xaudio2.h>
#include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\xaudio2fx.h>
#include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\xapofx.h>
#pragma warning(push)
#pragma warning( disable : 4005 )
#include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\x3daudio.h>
#pragma warning(pop)
#pragma comment(lib,"x3daudio.lib")
#pragma comment(lib,"xapofx.lib")
#endif
#endif
//#pragma comment(lib, "X3daudio.lib")

#include "FrameWork/Utility/UtilCsv.h"

using namespace DirectX;
using namespace std;
NS_SINGLETON_INSTANCE_DEFINE(CSoundManager);

//@todo 3D再生対応

//Voice終了時コールバック
bool CSoundManager::SoundData::polling()
{
	// 呼ばれるまで待機させる
	DWORD tEventState;
	tEventState = WaitForSingleObject(mVoiceCallback.hBufferEndEvent, 0);
	if (tEventState == WAIT_OBJECT_0) {
		//static s32 nAddCount = 0;
		//NS_Report(_T("Sound StopCallback:%d\n"), nAddCount);
		//nAddCount++;
		return true;
	}

	return false;
}

//巻き戻し再生(ループ対応)
void CSoundManager::SoundData::rewind()
{
	if (m_pWavData != nullptr)
	{
		// dataチャンクの先頭に戻す
		m_pWavData->mWav.ResetFile();
		// dataチャンクの先頭からloop_pointまでシーク
		m_pWavData->mWav.Seek(m_unLoopTime, SEEK_CUR);
#if !defined(CUT_XAUDIO)

		// SourceVoiceにデータを送信
		XAUDIO2_BUFFER submit = { 0 };
		submit.AudioBytes = m_pWavData->mWav.GetSize();	// バッファのバイト数
		submit.pAudioData = &m_pWavData->mData[0];		// バッファの先頭アドレス
		submit.Flags = XAUDIO2_END_OF_STREAM;			// バッファが終了するときに呼ぶ

		m_pSoundSource->SubmitSourceBuffer(&submit);
		m_pSoundSource->Start();
#endif
	}
}

void CSoundManager::SoundData::Stop()
{
#if !defined(CUT_XAUDIO)
	Destroy();	//AuidioSource開放
#endif
	m_bUse = false;	//未使用とする。

	//データ参照を解除
	if (m_pWavData != nullptr)
	{
		//@note
		//mWavを一回消して２回メつかおうとするとリークを起こす。
		//ひとまず一回作ったwavファイルは残して使いまわす
#if 0
		m_pWavData->m_nRefCount--;
		if (m_pWavData->m_nRefCount <= 0)
		{
			NS_ASSERT(m_pWavData->m_nRefCount == 0);
			m_pWavData->m_eSoundID = E_SOUND_ID::eSOUNDID_NONE;
			m_pWavData->Destroy();
			NS_Report(_T("WavData RefCount is 0\n"));
		}
#endif
		m_pWavData = nullptr;
	}
}


CSoundManager::CSoundManager() 
	:
#if !defined(CUT_XAUDIO)
	m_pIXAudio(nullptr),
	m_pMasterVoice(nullptr),
	m_pSubMixVoice(nullptr),
#endif
	m_fSEVolume(1.0f),
	m_fBGMVolume(1.0f),
	m_fSoundVolume(1.0f)
{
	m_vListenerPos.SetZero();

	m_paWavData.clear();
	m_paWavData.reserve(WAV_MAX);

	for (s32 i = 0; i < WAV_MAX; ++i)
	{
		WavData* pWavData;
		CLASS_NEW(pWavData, WavData);

		m_paWavData.push_back(pWavData);
	}

	m_paSoundData.clear();
	m_paSoundData.reserve(VOICE_MAX);
	for (s32 i = 0; i < VOICE_MAX; ++i)
	{
		SoundData* pSoundData;
		CLASS_NEW(pSoundData, SoundData);

		m_paSoundData.push_back(pSoundData);
	}


#if !defined(CUT_XAUDIO)

	//// 初期化をキレイに行うためのテンポラリ
	mListener.pCone = nullptr;
#endif

}

CSoundManager::~CSoundManager()
{
	Destroy();
}

void CSoundManager::Destroy()
{
#if !defined(CUT_XAUDIO)
	SAFE_DELETEA(m_dspSetting.pMatrixCoefficients);
	SAFE_DELETEA(m_dspSetting.pDelayTimes);
	//	SAFE_DELETE_ARRAY( mEmitter.pChannelAzimuths );
	SAFE_DELETE(mListener.pCone);
#endif

	for (s32 i = 0; i < WAV_MAX; ++i)
	{
		CLASS_DESTROY(m_paWavData[i]);
	}
	m_paWavData.clear();

	for (s32 i = 0; i < VOICE_MAX; ++i)
	{
		CLASS_DESTROY(m_paSoundData[i]);
	}
	m_paSoundData.clear();

	for (s32 i = 0; i < (s32)E_SOUND_ID::eSOUNDID_MAX; ++i)
	{
		CLASS_DESTROY(m_paSoundFileData[i]);
	}
	m_paSoundFileData.clear();

#if !defined(CUT_XAUDIO)
	if (m_pMasterVoice != nullptr) 
	{
		m_pMasterVoice->DestroyVoice();
		m_pMasterVoice = nullptr;
	}

	if (m_pSubMixVoice != nullptr)
	{
		m_pSubMixVoice->DestroyVoice();
		m_pSubMixVoice = nullptr;
	}

	if (m_pMasterVoice != nullptr) {
		m_pMasterVoice->DestroyVoice();
		m_pMasterVoice = nullptr;
	}

	SAFE_RELEASE(m_pIXAudio);
#endif
	CoUninitialize();
}

bool CSoundManager::Init()
{
	//COMショキカ
	HRESULT hr;
	if (FAILED(hr = CoInitializeEx(NULL, COINIT_MULTITHREADED))) 
	{
		NS_Report(_T("Error! CoInitializeEx"));
		return false;
	}
#if !defined(CUT_XAUDIO)

	//XAudio生成
	UINT32 flags = 0;
//#if defined( _DEBUG )
//	flags |= XAUDIO2_DEBUG_ENGINE;
//#endif
	if (FAILED(hr = XAudio2Create(&m_pIXAudio, flags)))
	{
		NS_Report(_T("Error! XAudio2Create"));
		return false;
	}

	// マスタリングボイスの作成
	if (FAILED(hr = m_pIXAudio->CreateMasteringVoice(&m_pMasterVoice, XAUDIO2_DEFAULT_CHANNELS, XAUDIO2_DEFAULT_SAMPLERATE, 0, 0, NULL)))
	{
		NS_Report(_T("Error! CreateMasteringVoice"));
		return false;
	}

	//3D周り初期化
#if false
	XAUDIO2_DEVICE_DETAILS details;
	if (FAILED(hr = xAudio->GetDeviceDetails(0, &details))) {
		assert(FALSE);
	}
	if (FAILED(hr = m_pIXAudio->CreateSubmixVoice(&m_pSubMixVoice, 1, details.OutputFormat.Format.nSamplesPerSec))) {
		assert(FALSE);
	}
#else
	XAUDIO2_VOICE_DETAILS vdetails;
	m_pMasterVoice->GetVoiceDetails(&vdetails);

	if (FAILED(hr = m_pIXAudio->CreateSubmixVoice(&m_pSubMixVoice, 1, vdetails.InputSampleRate))) {
		assert(FALSE);
	}
#endif


	mSourceChannels = 2;
	mDistinationChannels = vdetails.InputChannels;

	//// ワールド座標の定数の設定
	//const INT           XMIN = -10;
	//const INT           XMAX = 10;
	//const INT           ZMIN = -10;
	//const INT           ZMAX = 10;

	// DSP の設定
	m_dspSetting.SrcChannelCount = mSourceChannels;
	m_dspSetting.DstChannelCount = mDistinationChannels;	// OUTPUT_CHANNELS;

	m_dspSetting.pMatrixCoefficients = new FLOAT32[mSourceChannels * mDistinationChannels];
	m_dspSetting.pDelayTimes = new FLOAT32[mSourceChannels];

	//mDSPSettings.pDelayTimes = NULL;
	//mDSPSettings.pMatrixCoefficients = NULL;

	// 初期化は一回だけ行いたい
	X3DAudioInitialize(SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER |
		SPEAKER_LOW_FREQUENCY | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT,
		X3DAUDIO_SPEED_OF_SOUND, m_3dInstance);

	//   mDSPSettings.SrcChannelCount = mSourceChannels;
	//   mDSPSettings.DstChannelCount = mDistinationChannels;
	//   mDSPSettings.pMatrixCoefficients = matrixCoefficients;
#endif

	LoadSoundFileData();

	return true;
}

void CSoundManager::Update(const f32 fDelta)
{
	NS_UNUSED(fDelta);

	for (s32 i = 0; i < VOICE_MAX; ++i)
	{
		if (m_paSoundData[i] != nullptr && m_paSoundData[i]->m_bUse)
		{
			//再生終了チェック
			if (m_paSoundData[i]->polling())
			{
				if (m_paSoundData[i]->m_bLoopFlag)
				{
					//ループ再生
					m_paSoundData[i]->rewind();
				}
				else
				{
					m_paSoundData[i]->Stop();
				}
			}
		}
	}
}

s32 CSoundManager::PlaySE(const E_SOUND_ID eSoundID)
{
	s32 nWavID = LoadWavData(eSoundID);
	s32 nCueID = PlayWavCore(nWavID);

	if (nCueID < 0)
	{
		//PlayWavCoreが失敗。
		//Wavデータも消さないと
	}

	return nCueID;
}

s32 CSoundManager::Play3DSE(const NSVector3& vPos, const E_SOUND_ID eSoundID)
{
	s32 nWavID = LoadWavData(eSoundID);
	s32 nCueID = PlayWavCore(nWavID);

	NS_UNUSED(vPos);

	if (nCueID < 0)
	{
		//PlayWavCoreが失敗。
		//Wavデータも消さないと
	}

	return nCueID;
}

s32 CSoundManager::PlayBGM(const E_SOUND_ID eSoundID)
{
	s32 nWavID = LoadWavData(eSoundID);
	s32 nCueID = PlayWavCore(nWavID);

	if (nCueID < 0)
	{
		//PlayWavCoreが失敗。
		//Wavデータも消さないと
	}

	return nCueID;
}

void CSoundManager::StopBGM(const s32 nSoundCueID)
{
	//再生中の物を停止させる
	int nIndex = nSoundCueID;
	nIndex = UtilMath::Clamp(nSoundCueID, (s32)E_SOUND_ID::eSOUNDID_NONE, (s32)E_SOUND_ID::eSOUNDID_MAX);

	if (m_paSoundData[nIndex]->m_bUse)
	{
		m_paSoundData[nIndex]->Stop();
	}
}

s32 CSoundManager::PlayWavCore(const s32 nWavDataID)
{
	s32 nSoundIndex = -1;

	if (nWavDataID < 0)
	{
		//nWavDataID無効
		return -1;
	}
#if !defined(CUT_XAUDIO)

	//Wavデータを用いてサウンドを再生する
	if (m_pIXAudio != nullptr)
	{
		//使用してないサウンドデータを用いる
		s32 nEmptyIndex = GetEmptySoundIndex();
		if (nEmptyIndex >= 0)
		{
			SoundData* pcSoundaData = m_paSoundData[nEmptyIndex];
			if (pcSoundaData != nullptr)
			{
				m_pIXAudio->CreateSourceVoice(
					&pcSoundaData->m_pSoundSource,
					m_paWavData[nWavDataID]->mWav.GetFormat(), 0,
					XAUDIO2_MAX_FREQ_RATIO, &pcSoundaData->mVoiceCallback/*, &sendList*/);

				//参照。カウンタを増やす
				NS_ASSERT(pcSoundaData->m_pWavData == nullptr);

				pcSoundaData->m_pWavData = m_paWavData[nWavDataID];
				pcSoundaData->m_pWavData->m_nRefCount++;

				//  SourceVoiceにデータを送信
				XAUDIO2_BUFFER submit = { 0 };
				submit.AudioBytes = pcSoundaData->m_pWavData->mWav.GetSize();	// バッファのバイト数
				submit.pAudioData = &pcSoundaData->m_pWavData->mData[0];		// バッファの先頭アドレス
				submit.Flags = XAUDIO2_END_OF_STREAM;							// バッファが終了するときに呼ぶ

				if (pcSoundaData->m_bLoopFlag) {
					//これなんのためにある？調べる
					submit.LoopCount = XAUDIO2_LOOP_INFINITE;
				}

				if (!SUCCEEDED(pcSoundaData->m_pSoundSource->SubmitSourceBuffer(&submit)))
				{
					NS_Report(_T("Sound Error! SubmitSourceBuffer\n"));
				}

				//@todo SEとBGMわけないと
				if (!SUCCEEDED(pcSoundaData->m_pSoundSource->SetVolume(m_fSoundVolume)))
				{
					NS_Report(_T("Sound Error! SetVolume\n"));
				}

				if (!SUCCEEDED(pcSoundaData->m_pSoundSource->Start()))
				{
					NS_Report(_T("Sound Error! Star\nt"));
				}

				pcSoundaData->m_bUse = true;	//使用中とする。

				nSoundIndex = nEmptyIndex;
			}
		}
	}
#endif

	return nSoundIndex;
}

s32 CSoundManager::LoadWavData(const E_SOUND_ID eSoundID)
{
	if (eSoundID == E_SOUND_ID::eSOUNDID_NONE)
	{
		return -1;
	}

	s32 nEmptyIndex = GetEmptySoundIndex();
	if (nEmptyIndex < 0)
	{
		//空きがないと再生できないので返す
		return -1;
	}

#if true
	//Wavデータが読み込み済みではないかチェック
	for (s32 i = 0; i < WAV_MAX; ++i)
	{
		if (m_paWavData[i] != nullptr && m_paWavData[i]->m_eSoundID == eSoundID)
		{
			//すでに読み込まれているWavデータをみつけたのでこれを使わせる
			//同一データを使ってるときは先頭に戻すSEEKがいるか？
			s32 nMinusSize = m_paWavData[i]->mWav.GetSize();
			m_paWavData[i]->mWav.Seek(-nMinusSize, SEEK_END);

			//NS_Report(_T("FindExistSound\n"));

			return i;
		}
	}
#endif

	//すでに読みこれてるデータはないので使用できる空きデータをチェック
	s32 nWavDataIndex = -1;
	for (s32 i = 0; i < WAV_MAX; ++i)
	{
		//あきWavデータチェック
		if (m_paWavData[i] != nullptr && m_paWavData[i]->m_nRefCount <= 0)
		{
			nWavDataIndex = i;
			break;
		}
	}
	if (nWavDataIndex >= 0)
	{
		//新しいWavデータ読み込み
		WavData* pWavData = m_paWavData[nWavDataIndex];
		if (pWavData != nullptr)
		{
			//Wavデータは一回開放しておく
			pWavData->Destroy();

			//@todo サウンドIDから文字列をだす
			TCHAR* str = nullptr;
			u32 nFilePathIndex = static_cast<u32>(eSoundID);
			if (nFilePathIndex >= 0 && nFilePathIndex < m_paSoundFileData.size())
			{
				str = m_paSoundFileData[nFilePathIndex]->m_strSoundFileName;
			}
			
			if (pWavData->OpenWav(str))
			{
				//  WAVファイルのサイズ分のバッファを作成し、データを書き込む
				pWavData->m_eSoundID = eSoundID;	//サウンドIDとWavデータを関連付け

				DWORD sizeNum = pWavData->mWav.GetSize();
				pWavData->mData.resize(sizeNum);

				//データを取り込む
				//これが一回でいいのかも
				m_paWavData[nWavDataIndex]->mWav.Read(&pWavData->mData[0], sizeNum, &sizeNum);
			}
			else
			{
				nWavDataIndex = -1;	//失敗扱い
			}

		}
	}

	return nWavDataIndex;
}

s32 CSoundManager::GetEmptySoundIndex()
{
	s32 nIndex = -1;

	for (s32 i = 0; i < VOICE_MAX; ++i)
	{
		if (m_paSoundData[i] != nullptr && !m_paSoundData[i]->m_bUse)
		{
			//未使用インデックス
			nIndex = i;
			break;
		}
	}

	return nIndex;
}

bool CSoundManager::LoadSoundFileData()
{
	vector<vector<string>> values;
	CUtilCSV::LoadCsvData(_T("resource/data/SoundFileName.csv"), values);

	//CSVファイルからデータを読み込んでセットしておく
	m_paSoundFileData.clear();
	m_paSoundFileData.reserve((s32)E_SOUND_ID::eSOUNDID_MAX);

	string filep = "resource/sound/";
	string wav = ".wav";

	for (s32 i = 0; i < (s32)E_SOUND_ID::eSOUNDID_MAX; ++i)
	{
		SoundFileData* pSoundData;
		CLASS_NEW(pSoundData, SoundFileData);

		//IDは順番で
		pSoundData->m_eSoundID = (E_SOUND_ID)i;

		//ファイル名をコピー
		string& filename = values[i][2];
		if (!filename.empty())
		{
			//リソース場所を連結
			//resource/sound/とWavをつける
			string finishStr = filep + filename + wav;

			mbstowcs(pSoundData->m_strSoundFileName, finishStr.c_str(), strlen(finishStr.c_str()));
		}
		m_paSoundFileData.push_back(pSoundData);
	}

	return true;

}

//EOF
