#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		SoundManager.h
	@brief		サウンド管理
	@author		shiba
	@date		2012/9/29 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/

//#define CUT_XAUDIO

#if !defined(CUT_XAUDIO)
#include <xaudio2.h>
#include <X3daudio.h>
#endif

#include "WaveFile.h" //WindowSDK
#include "SoundID.h"

//コールバック対応クラス
#if !defined(CUT_XAUDIO)
class VoiceCallback : public IXAudio2VoiceCallback {
#else
class VoiceCallback{
#endif
public:
	HANDLE hBufferEndEvent;
	VoiceCallback() : hBufferEndEvent(CreateEvent(NULL, FALSE, FALSE, NULL)) {}
	~VoiceCallback() {
		//WaitForSingleObject( hBufferEndEvent, INFINITE );
		CloseHandle(hBufferEndEvent);
	}

	// voiceの終了時に呼ばれる関数
	STDMETHOD_(void, OnStreamEnd()) {
		SetEvent(hBufferEndEvent);
	}

	// 純粋仮想関数なので使わないものも定義する
	STDMETHOD_(void, OnVoiceProcessingPassEnd()) {}
	STDMETHOD_(void, OnVoiceProcessingPassStart(UINT32 SamplesRequired)) { NS_UNUSED(SamplesRequired); }
	STDMETHOD_(void, OnBufferEnd(void * pBufferContext)) { NS_UNUSED(pBufferContext); }
	STDMETHOD_(void, OnBufferStart(void * pBufferContext)) { NS_UNUSED(pBufferContext); }
	STDMETHOD_(void, OnLoopEnd(void * pBufferContext)) { NS_UNUSED(pBufferContext); }
	STDMETHOD_(void, OnVoiceError(void * pBufferContext, HRESULT Error)) {
		NS_UNUSED(pBufferContext); NS_UNUSED(Error);
	}
};

/*==========================================================================*/
/*!
	@class	CSoundManager
	@brief	サウンド管理
*/
/*==========================================================================*/
class CSoundManager
{
	//シングルトン
	NS_SINGLETON(CSoundManager);
	CSoundManager();
	~CSoundManager();

private:
	const static s32 WAV_MAX = 32;			//!< 最大32まで、同時にWavデータを持つ
	const static s32 VOICE_MAX = 64;		//!< 同時発音数
	const static s32 FILE_NAME_MAX = 256;

private:

	//サウンドデータ
	class SoundFileData
	{
	public:
		bool Init()
		{
			ZeroMemory(m_strSoundFileName, CSoundManager::FILE_NAME_MAX);
			return true;
		}
		void Destroy() {}

	public:
		E_SOUND_ID m_eSoundID;
		TCHAR m_strSoundFileName[CSoundManager::FILE_NAME_MAX];
	};

	//Wavデータ
	class WavData
	{
	public:
		WavData() :
			m_eSoundID(E_SOUND_ID::eSOUNDID_NONE),
			m_nRefCount(0),
			m_bOpenWav(false)
		{
			mData.clear();
		}
		~WavData()
		{
			Destroy();
		}
		bool Init() { return true; }

		bool OpenWav(TCHAR *str)
		{
			NS_ASSERT(!m_bOpenWav);
			m_bOpenWav = SUCCEEDED(mWav.Open(str));
			return m_bOpenWav;
		}

		void Destroy()
		{
			CloseWav();
			mData.clear();
		}
	private:
		bool CloseWav()
		{
			//NS_ASSERT(m_bOpenWav);
			m_bOpenWav = SUCCEEDED(mWav.Close());
			if (m_bOpenWav)
				m_bOpenWav = false;
			return !m_bOpenWav;
		}

	private:

	public:
		std::vector< BYTE > mData;				//!< 読み込み済みWavデータ
		CWaveFile mWav;							//!< Wavデータ読み込みユーティリティ
		E_SOUND_ID m_eSoundID;					//!< ID
		int m_nRefCount;						//!< 参照カウンタ
		bool m_bOpenWav;
	};

	//サウンド毎クラス
	class SoundData {
	public:
		WavData* m_pWavData;					//!< 対応Wavデータのポインタ(参照するだけ)
#if !defined(CUT_XAUDIO)
		IXAudio2SourceVoice* m_pSoundSource;	//!< サウンドソース
#endif
		bool m_bLoopFlag;						//!< ループフラグ
		u32 m_unLoopTime;						//!< ループする場合のイントロ指定
		bool m_bExclusiveFlag;					//!< 排他制御フラグ @todo 同時発音数的なのに切り替わることになる
		VoiceCallback mVoiceCallback;			//!< コールバック
		bool m_b3DPlayFlag;						//!< 3D再生フラグ
#if !defined(CUT_XAUDIO)
		X3DAUDIO_EMITTER* m_pEmitter;			//!< 3Dオーディオ用データ
#endif
		bool m_bUse;							//!< 使用中か否か
	public:
		SoundData() :
			m_pWavData(nullptr),
#if !defined(CUT_XAUDIO)
			m_pSoundSource(nullptr),
#endif
			m_bLoopFlag(false),
			m_unLoopTime(0),
			m_bExclusiveFlag(false),
#if !defined(CUT_XAUDIO)
			m_pEmitter(nullptr),
#endif
			m_bUse(false)
		{
		}

		~SoundData() {
			Destroy();
		}

		bool Init()
		{
			return true;
		}
		void Destroy()
		{
#if !defined(CUT_XAUDIO)
			if (m_pSoundSource != nullptr)
			{
				m_pSoundSource->Stop();
				m_pSoundSource->DestroyVoice();
				m_pSoundSource = nullptr;
			}
#endif
		}

		bool polling();	//再生チェック
		void rewind();	//巻き戻し
		void Stop();
	};


public:
	bool Init();
	void Update(const f32 fDelta);

	s32 PlaySE(const E_SOUND_ID eSoundID);
	s32 Play3DSE(const NSVector3& vPos, const E_SOUND_ID eSoundID);
	bool Set3DPos(const NSVector3& vPos, const s32 nSoundCueID);

	s32 PlayBGM(const E_SOUND_ID eSoundID);
	void StopBGM(const s32 nSoundCueID);

	void SetListenerPos(const NSVector3& vListenerPos)
	{
		m_vListenerPos = vListenerPos;
	}
	void SetSEVolume(const f32 fVolume)
	{
		m_fSEVolume = fVolume;
		m_fSEVolume = UtilMath::Clamp(m_fSEVolume, 0.0f, 1.0f);
	}
	void SetBGMVolume(const f32 fVolume)
	{
		m_fBGMVolume = fVolume;
		m_fBGMVolume = UtilMath::Clamp(m_fSEVolume, 0.0f, 1.0f);
	}
	void SetVolume(const f32 fVolume)
	{
		m_fSoundVolume = fVolume;
		m_fSoundVolume = UtilMath::Clamp(m_fSEVolume, 0.0f, 1.0f);
	}

private:
	void Destroy();
	s32 PlayWavCore(const s32 nWavDataID);
	s32 GetEmptySoundIndex();
	s32 LoadWavData(const E_SOUND_ID eSoundID);
	bool LoadSoundFileData();

private:
#if !defined(CUT_XAUDIO)
	IXAudio2* m_pIXAudio;
	IXAudio2MasteringVoice* m_pMasterVoice;
	IXAudio2SubmixVoice* m_pSubMixVoice;
#endif
	f32 m_fSEVolume;				//!< ボリューム
	f32 m_fBGMVolume;
	f32 m_fSoundVolume;

	std::vector<WavData*> m_paWavData;		//!< Wavデータ
	std::vector<SoundData*> m_paSoundData;	//<! 再生データ

	std::vector<SoundFileData*> m_paSoundFileData;	//!< ファイルデータ

	//3Dデータ----------------------
	NSVector3 m_vListenerPos;

#if !defined(CUT_XAUDIO)
	X3DAUDIO_LISTENER mListener;		//!< 3Dオーディオ用リスナー
	X3DAUDIO_HANDLE m_3dInstance;		//!< 3Dインスタンス
	X3DAUDIO_DSP_SETTINGS m_dspSetting;	//!< DSP
#endif
										// チャンネル関連
	WORD mSourceChannels;
	u32 mDistinationChannels;

	DWORD dwChannelMask;
	UINT32 nChannels;
	//------------------------------
};

//EOF