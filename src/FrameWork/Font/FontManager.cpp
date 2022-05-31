/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		FontManager.cpp
	@brief		フォントマネージャー
	@author		shiba
	@date		2013/1/18 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#include "FontManager.h"
#include "FontFile.h"

#include "FrameWork/Asset/AssetFontTexture.h"
#include "FrameWork/Asset/AssetManager.h"

#include "FrameWork/Rendering/Sprite.h"

//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CFontSprite::CFontSprite() 
: m_bReserved( false )
{

}

//---------------------------------------------------------------------------
/*!
	@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CFontSprite::~CFontSprite()
{
}

NS_SINGLETON_INSTANCE_DEFINE( CFontManager );

//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CFontManager::CFontManager() :
	m_eRenderStage(CRenderingManager::E_RENDER_STAGE_SPRITE)
{
}

//---------------------------------------------------------------------------
/*!
	@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CFontManager::~CFontManager()
{
	//フォントデータ削除
	for( u32 i = 0; i < m_caFontData.GetSize(); ++i )
	{
		SAFE_DELETE( m_caFontData.Get( i ) ) ;
	}
	for( u32 i = 0; i < m_pcaSprite.GetSize(); ++i )
	{
		SAFE_DELETE_TASK( m_pcaSprite.Get( i ) ) ;
	}
}

//---------------------------------------------------------------------------
/*!
	@brief	初期化
*/
//---------------------------------------------------------------------------
bool CFontManager::Init()
{
	CAssetManager* pcAssetM = CAssetManager::GetInstance();
	NS_ASSERT(pcAssetM);

#if true
	//フォントテクスチャ生成、初期化。
	for( u32 i = 0; i < m_pcaFontTexture.GetSize(); ++i )
	{
		//フォント名を渡すカンジでいいかもしれないけど
		TCHAR str[ 64 ] = { 0 };
		wsprintf( str, _T("AssetFontTexture:%d\n"), i );

		m_pcaFontTexture.Set( i, pcAssetM->LoadFontTexture( str ) );
	}
#endif

#if true
	//スプライトの生成
	CRenderingContext cContext;
	cContext.SetShader(CShaderManager::eSHADERID_SPRITE);

	for (u32 i = 0; i < m_pcaSprite.GetSize(); i++)
	{
		CFontSprite* pcSprite = nullptr;
		CLASS_NEW(pcSprite, CFontSprite());

		pcSprite->SetRenderContext(cContext);
		pcSprite->SetRenderPriority(eREN_PRI_TEXT);

		m_pcaSprite.Set(i, pcSprite);
	}
#endif

#if true
	//フォントデータ作成
	for( u32 i = 0; i < m_caFontData.GetSize(); ++i )
	{
		CFontFile* pcFontFile = nullptr;
		CLASS_NEW( pcFontFile, CFontFile() );
		m_caFontData.Set( i, pcFontFile );
	}

	bool bRet = false;
	//MSゴシック
	bRet = m_caFontData.Get(eFONTDATA_FILE_Ms)->LoadSystemFont(_T("MS ゴシック"), FONT_SIZE, FONT_SIZE);
	if (!bRet)
	{
		return false;
	}

#endif
	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	更新
*/
//---------------------------------------------------------------------------
bool CFontManager::Update()
{
	//いったんすべてリセットする
	for (u32 i = 0; i < m_pcaSprite.GetSize(); i++)
	{
		CFontSprite* pcFontSprite = m_pcaSprite.Get(i);
		if (pcFontSprite)
		{
			//@todo いちいちリストから外してるので負荷に注意、調査
			pcFontSprite->m_bReserved = false;
			pcFontSprite->SetDrawEnable(false, m_eRenderStage);
		}
	}

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	フォント描画
	@param[in]	str	描画文字列
	@param[in]	vPos	描画座標
	@param[in]	eFontDataType	フォントタイプ
	@param[in]	scale	描画スケール(デフォルト:1)
*/
//---------------------------------------------------------------------------
void CFontManager::DrawText2D( const TCHAR* str, const NSVector2& vPos, const E_FONTDATA_FILE eFontDataType, const f32 scale /* = 1.0f */ )
{
	u32 unFontSize = wcslen( str );
	f32 fWidthOffSet = 0.0f;							//スプライトの配置オフセット
	const CFontOneData* pPreviousFontData = nullptr;	//一つのフォントを配置する一個前のフォントデータ

	for( u32 i = 0; i < unFontSize; i++ )
	{
		//--------------------------------------------------------------------
		// 空いているスプライトを取り出す
		//--------------------------------------------------------------------
		const s32 nIdx = GetIndexFreeSprite();
		NS_ASSERT(nIdx >= 0);	// スプライトが空いていない
		CFontSprite* const pcSprite = m_pcaSprite.Get( nIdx );

		// スプライトを描画登録
		pcSprite->m_bReserved = true;
		pcSprite->SetDrawEnable(true, m_eRenderStage);

		//--------------------------------------------------------------------
		// グリフ情報取り出し
		//--------------------------------------------------------------------
		u16 uccode = str[i];						//文字コードID
		const CFontOneData* pcFontData = nullptr;	//参照グリフデータ
		s32 nFontTextureIndex = -1;					//参照するテクスチャID

		for (s32 j = 0; j < FONTTEXTURE_EACH_FONTDATA; ++j)
		{
			const CAssetFontTexture* pcFontTexture = m_pcaFontTexture.Get(eFontDataType * FONTTEXTURE_EACH_FONTDATA + j);
			if (pcFontTexture && !pcFontTexture->IsFullFont())
			{
				nFontTextureIndex = j;

				pcFontData = pcFontTexture->GetFontData(uccode);
				if (pcFontData)
				{
					//グリフ情報があったので参照テクスチャIDは確定。
					break;
				}
			}
		}

		//if (nFontTextureIndex < 0)
		//{
		//	// テクスチャがグリフデータで満タン
		//	//@todo グリフデータのリセット、再構築
		//}

		CAssetFontTexture* pcFontTexture = m_pcaFontTexture.Get(nFontTextureIndex);
		if (pcFontTexture != nullptr)
		{
			pcSprite->SetTexture(pcFontTexture);
			if (pPreviousFontData != nullptr)
			{
				//fWidthOffSet += pPreviousFontData->scalex * scale;
				fWidthOffSet += FONT_SIZE * scale;
			}
			pcSprite->SetPos(NSVector3(vPos.x + fWidthOffSet, vPos.y, 0.0f));	//グリフごとのサイズにしないと

			if (pcFontData == nullptr)
			{
				//グリフ情報が見つからなかったので、新しく作成
				pcFontData = &pcFontTexture->LoadGryphToTexture(uccode, m_caFontData.Get(eFontDataType));
			}

			if (pcFontData)
			{
				//@todo 毎フレ生成走るので考える
				pcSprite->SetScale(NSVector3((f32)pcFontData->scalex * scale, (f32)pcFontData->scaley * scale, 1.0f));
				pcSprite->SetUV(pcFontData->u, pcFontData->v, pcFontData->u2, pcFontData->v2);
			}
			pPreviousFontData = pcFontData;
		}
	}
}

//---------------------------------------------------------------------------
/*!
	@brief	空きスプライト取り出し
*/
//---------------------------------------------------------------------------
const s32 CFontManager::GetIndexFreeSprite()
{
	for( u32 i = 0; i < m_pcaSprite.GetSize(); i++)
	{
		if(m_pcaSprite.Get(i) != nullptr && !m_pcaSprite.Get( i )->m_bReserved )
		{
			//使ってないよ。
			return i;
		}
	}

	return -1;
}

#if defined( _DEBUG )

#include <atlconv.h>
#include <atlstr.h>
#include <atlbase.h>

//---------------------------------------------------------------------------
/*!
	@brief	コールバック
*/
//---------------------------------------------------------------------------
int CALLBACK MyEnumFontFamExProc(ENUMLOGFONTEX *lpelfe, 
								 NEWTEXTMETRICEX *lpntme, DWORD FontType, LPARAM lParam)
{
	NS_UNUSED( lParam );
	NS_UNUSED( lpntme );

	CT2A	faceName(lpelfe->elfLogFont.lfFaceName);
	//ATLTRACE("%d %s\n", FontType, faceName);

	return 1;
}

//---------------------------------------------------------------------------
/*!
	@brief	フォントデータ列挙
*/
//---------------------------------------------------------------------------
void CFontManager::EnumFontFamilies()
{
	HDC	hDC = GetDC(nullptr);
	LOGFONT	lf = {0};
	lf.lfCharSet = SHIFTJIS_CHARSET;
	EnumFontFamiliesEx(hDC, &lf, (FONTENUMPROC)MyEnumFontFamExProc, 0, 0);	
}

#endif

//EOF
