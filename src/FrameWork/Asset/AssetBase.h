#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		CAssetBase.h
	@brief		アセット基底
	@author		shiba
	@date		2012/9/11 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
class CAssetBase
{
public:
	CAssetBase() :
		m_unAssetUID(INVALID_U32) ,
		m_nAssetRefCount( 0 )
	{
		ZeroMemory(m_StrAssetName, CAssetBase::ASSET_NAME_LENGTH);
	}
	virtual ~CAssetBase(){}

	virtual bool Init() { return true;  };

	inline void SetAssetUID( const u32 unUID );
	inline const u32 GetAssetUID() const;

	inline void SetAssetName(const TCHAR* strName);
	inline TCHAR* GetAssetName();

	inline void AddRefCount();
	inline void DecRefCount();
	inline s32 GetRefCount() const;

	static const int ASSET_NAME_LENGTH = 256;

protected:
	u32 m_unAssetUID;							// アセットのID
	TCHAR m_StrAssetName[ASSET_NAME_LENGTH];	// アセット名称

	s32 m_nAssetRefCount;	//!< 削除用参照カウンタ
};

void CAssetBase::SetAssetUID(const u32 unUID)
{
	m_unAssetUID = unUID;
}

const u32 CAssetBase::GetAssetUID() const
{
	return m_unAssetUID;
}

void CAssetBase::SetAssetName(const TCHAR* strName)
{
	wcsncpy(m_StrAssetName, strName, wcslen(strName));
}

TCHAR* CAssetBase::GetAssetName()
{
	return m_StrAssetName;
}

void CAssetBase::AddRefCount()
{
	m_nAssetRefCount++;
}

void CAssetBase::DecRefCount()
{
	m_nAssetRefCount--;
}

s32 CAssetBase::GetRefCount() const
{
	return m_nAssetRefCount;
}

//EOF
