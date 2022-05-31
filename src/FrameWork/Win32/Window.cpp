/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		Window.cpp
	@brief		ウィンドウクラス
	@author		shiba
	@date		2012/2/29 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#include "Window.h"
#include "Tchar.h"


NS_SINGLETON_INSTANCE_DEFINE( CWindow );
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

CWindow::CWindow():
m_Hwnd( nullptr )
, m_bWindowEnd(false)
{
}

CWindow::~CWindow()
{
}

bool CWindow::Init()
{
	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	ウィンドウ作成
*/
//---------------------------------------------------------------------------
bool CWindow::CreateGameWindow(const int nSizeX, const int nSizeY, const TCHAR* strTitle)
{
	bool bSuccess = false;

	m_nWindowSizeX = nSizeX;
	m_nWindowSizeY = nSizeY;

	WNDCLASSEX wcex;											//ウィンドウ構造体宣言
	HINSTANCE hInstance = GetModuleHandle(nullptr);

	// ウインドウクラス構造体の設定
	wcex.cbSize = sizeof(WNDCLASSEX); 						// ウィンドウクラス構造体のサイズ
	wcex.style = CS_HREDRAW | CS_VREDRAW;						// 縦にも横にもサイズ変更したら再描画
	wcex.lpfnWndProc = (WNDPROC)WndProc;						// ウィンドウプロシージャのアドレス
	wcex.cbClsExtra = 0;										// ウィンドウクラス構造体で補助データを使用する場合のサイズ
	wcex.cbWndExtra = 0;										// 同上
	wcex.hInstance = hInstance;									// WinMainから渡されるhInstance
	wcex.hIcon = LoadIcon(nullptr, IDI_APPLICATION);			// アイコン
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);				// カーソル
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);	// クライアント領域のバックカラー
	wcex.lpszMenuName = nullptr;									// メニューバー
	wcex.lpszClassName = strTitle;							// ウィンドウクラスの名称
	wcex.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);			// 小さいアイコン

	RegisterClassEx(&wcex);									// ウインドウクラス構造体をセット

															// ウインドウの作成
	m_Hwnd = CreateWindowW(wcex.lpszClassName, strTitle,		// クラス名とタイトルバーのキャプション		
		WS_OVERLAPPEDWINDOW, 0, 0, m_nWindowSizeX,				// ウインドウのスタイル,水平位置,垂直位置,幅,高さ
		m_nWindowSizeY,
		nullptr, nullptr, hInstance, nullptr);

	//ウィンドウを中央にセット
	HWND hDeskWnd;
	RECT deskrc, rc;
	int x, y;

	hDeskWnd = GetDesktopWindow();						//デスクトップのハンドルを取得
	GetWindowRect(hDeskWnd, (LPRECT)&deskrc);			//デスクトップの解像度（大きさを取得）
	GetWindowRect(m_Hwnd, (LPRECT)&rc);				//ゲームの大きさを取得
	x = (deskrc.right - (rc.right - rc.left)) / 2;		//デスクトップの大きさの半分
	y = (deskrc.bottom - (rc.bottom - rc.top)) / 2;		//デスクトップの大きさの半分
	SetWindowPos(m_Hwnd, HWND_TOP, x, y, m_nWindowSizeX, m_nWindowSizeY, SWP_SHOWWINDOW); //ウィンドウをセット

																						  // ウインドウを表示
	ShowWindow(m_Hwnd, SW_SHOW);
	UpdateWindow(m_Hwnd);

	return bSuccess;
}

LRESULT	CALLBACK WndProc( HWND hWnd, u32 unMessage,	WPARAM wParam, LPARAM lParam )
{
	switch( unMessage )
	{
	case WM_ACTIVATEAPP:
	{
		break;
	}
	case  WM_CREATE:
		{
		}
		break;
	case WM_KEYDOWN:
		if( wParam == VK_ESCAPE )
		{
			PostQuitMessage( 0 );
		}
		break;

	case WM_DESTROY:
	{
		CWindow* pcWindow = CWindow::GetInstance();
		pcWindow->m_bWindowEnd = true;

		PostQuitMessage(0);
	}
		break;
		
	default:
		break;
	}

	return DefWindowProc( hWnd, unMessage, wParam, lParam );
}

//EOF
