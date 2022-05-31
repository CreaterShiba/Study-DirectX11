/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		FontTexture.cpp
	@brief		フォント用テクスチャクラス
	@author		shiba
	@date		2013/1/19 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#include "AssetFontTexture.h"
#include "FrameWork/Font/FontFile.h"
#include "FrameWork/Rendering/RenderingTexture.h"
#include "FrameWork/DXUtil/UtilDX11.h"
#include "FrameWork/Asset/AssetManager.h"

//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CAssetFontTexture::CAssetFontTexture() 
: m_pTexture( nullptr )
{
}

//---------------------------------------------------------------------------
/*!
	@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CAssetFontTexture::~CAssetFontTexture()
{
	SAFE_RELEASE( m_pTexture );
}

//---------------------------------------------------------------------------
/*!
	@brief	初期化
*/
//---------------------------------------------------------------------------
bool CAssetFontTexture::Init()
{
	m_pcaFontData.reserve( FONTTEXTURE_INFONTNUM );

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	フォントテクスチャロード
*/
//---------------------------------------------------------------------------
bool CAssetFontTexture::LoadFontTexture()
{
	NS_ASSERT( !m_pTextureReVi );

	//--------------------------------------------------------------------
	// テクスチャの作成
	//--------------------------------------------------------------------
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory( &texDesc, sizeof( texDesc ) );
	texDesc.Width = FONT_TEXTURE_SIZE;					//幅
	texDesc.Height = FONT_TEXTURE_SIZE;					//高さ
	texDesc.MipLevels = 1;								//ミップマップレベル
	texDesc.ArraySize = 1;								//テクスチャ配列サイズ
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;		//フォーマット
	texDesc.SampleDesc.Count = 1;						//サンプラー数
	texDesc.Usage = D3D11_USAGE_DEFAULT;				//
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;		//シェーダバインド可能
	texDesc.CPUAccessFlags = 0;							//

	DXMng::GetInstance()->GetDevice().CreateTexture2D( &texDesc, nullptr, &m_pTexture );

	// 2Dテクスチャにアクセスするシェーダ リソース ビューの設定
	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
	ZeroMemory( &srDesc, sizeof( srDesc ) );
	srDesc.Format        = texDesc.Format;					// フォーマット
	srDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;		// 2Dテクスチャ
	srDesc.Texture2D.MostDetailedMip = 0;					// 最初のミップマップ レベル
	srDesc.Texture2D.MipLevels       = 1;					// すべてのミップマップ レベル

	//シェーダリソースビューの作成
	if( FAILED(DXMng::GetInstance()->GetDevice().CreateShaderResourceView( m_pTexture, &srDesc, &m_pTextureReVi) ) )
	{
		NS_Report( _T("ShaderResourceViewLoadFailed") ); 
		return FALSE;
	}

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	満タン？
*/
//---------------------------------------------------------------------------
const bool CAssetFontTexture::IsFullFont() const
{
	return m_pcaFontData.size() >= FONTTEXTURE_INFONTNUM;
}

//---------------------------------------------------------------------------
/*!
	@brief		フォントデータの取得
	@param[in]	文字コード
	@return		データがなければnullptrを返します。
*/
//---------------------------------------------------------------------------
const CFontOneData* CAssetFontTexture::GetFontData( const u16 ucCode ) const
{
	for( u32 i = 0; i < m_pcaFontData.size(); i++)
	{
		if( m_pcaFontData[ i ].ucCode == ucCode )
		{
			return &m_pcaFontData[ i ];
		}
	}

	return nullptr;
}

//---------------------------------------------------------------------------
/*!
	@brief		フォントデータの書き込み
	@param[in]	書き込み文字コード
	@param[in]	フォントデータ
*/
//---------------------------------------------------------------------------
const CFontOneData& CAssetFontTexture::LoadGryphToTexture( const u16 unCode, const CFontFile* const pcFont )
{
	// 空いているフォントテクスチャの座標を取り出す

	//--------------------------------------------------------------------
	// // 取り出した座標のテクスチャ位置にラスタライズ
	//--------------------------------------------------------------------
	GLYPHMETRICS GM;	//グリフ情報
	CONST MAT2 mat = {{0,1},{0,0},{0,0},{0,1}};

	//必要なバッファサイズをまず返してもらう。
	//文字コードはShift-jis
	u32 size = GetGlyphOutline( pcFont->m_hHdc, (u32)unCode, pcFont->m_nGradFlag, &GM, 0, nullptr, &mat);
	NS_ASSERT(size != GDI_ERROR);
	NS_ASSERT(size > 0);

	//バッファの作成。格納してもらう
	u8* pucFontBmp = new u8[ size ]; 
	u32* punFontFix = new u32[ size ];
	GetGlyphOutline( pcFont->m_hHdc, unCode, GGO_GRAY4_BITMAP, &GM, size, pucFontBmp, &mat);

	//書き込むフォントの縦横サイズ。
	s32 fontWidth = ( (GM.gmBlackBoxX + 3) / 4 * 4 );	// 4アライン
	s32 fontHeight = GM.gmBlackBoxY;

	s32 fontSetcount = m_pcaFontData.size();
	s32 fontnum = FONT_TEXTURE_SIZE / FONT_SIZE;
	s32 row = fontSetcount / fontnum;		//縦
	s32 col = fontSetcount - row * fontnum;	//横

	for( s32 h = 0; h < fontHeight; h++ )
	{
		for( s32 w = 0; w < fontWidth; w++ )
		{
			u32 unArrayIdx = h * fontWidth + w;

			//ビットマップフォント情報の取得。
			//アンチエイリアス階層で分割
			u32 unAlpha = pucFontBmp[ unArrayIdx ] * 255 / pcFont->m_nGrad;
			//帰ってきているのはあくまでアルファ値なので、ビットシフトして
			//アルファ格納＆色格納
			punFontFix[ unArrayIdx ] = unAlpha << 24 | 0x00ffffff;

			//NS_Report(_T("DrawAccessBuf :: Idx:%d :: Color:%d\n"), unArrayIdx, punFontFix[ h * fontWidth + w ] );
		}
	}
	SAFE_DELETEA( pucFontBmp );	//用済み
	
	D3D11_BOX box;
	box.top = row * FONT_SIZE;
	box.left = col * FONT_SIZE;
	box.right = box.left + fontWidth; 
	box.bottom = box.top + fontHeight;
	box.front = 0; box.back = 1;

	//テクスチャへ部分書き込み
	CUtilDX11::GetConDevice().UpdateSubresource(
		m_pTexture,
		0,
		&box,
		punFontFix,
		sizeof( u32 ) * fontWidth ,				// コピー元フォーマットの1要素分のバイト数 * 横の幅
		sizeof( u32 ) * fontWidth * fontHeight	// コピー元フォーマットの1要素分のバイト数 * 横の幅 * 縦の幅
		);

	s32 fontStartUVY = row * FONT_SIZE;
	s32 fontStartUVX = col * FONT_SIZE;

	SAFE_DELETEA( punFontFix );

	// UV獲得。	
	CFontOneData cData;
	cData.ucCode = unCode;
	cData.unTexID = 0;

	cData.u = (f32)fontStartUVX / (f32)FONT_TEXTURE_SIZE;
	cData.v = (f32)fontStartUVY / (f32)FONT_TEXTURE_SIZE;
	cData.scalex = fontWidth;
	cData.scaley = fontHeight;
	cData.u2 = ((f32)fontWidth+(f32)fontStartUVX) / (f32)FONT_TEXTURE_SIZE;
	cData.v2 = ((f32)fontHeight+(f32)fontStartUVY) / (f32)FONT_TEXTURE_SIZE;

	m_pcaFontData.push_back( cData );	

	s32 nIdx = m_pcaFontData.size() - 1;
	return m_pcaFontData[ nIdx ];
}

//EOF