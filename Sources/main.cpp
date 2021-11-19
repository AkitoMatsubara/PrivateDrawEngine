#include <time.h>

#include "framework.h"


LRESULT CALLBACK window_procedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	framework* p{ reinterpret_cast<framework*>(GetWindowLongPtr(hwnd, GWLP_USERDATA)) };
	return p ? p->handle_message(hwnd, msg, wparam, lparam) : DefWindowProc(hwnd, msg, wparam, lparam);
}

int WINAPI WinMain(_In_ HINSTANCE instance, _In_opt_  HINSTANCE prev_instance, _In_ LPSTR cmd_line, _In_ int cmd_show)
{
	srand(static_cast<unsigned int>(time(nullptr)));

#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(####);
#endif

	WNDCLASSEXW wcex{};
	wcex.cbSize = sizeof(WNDCLASSEX);	// 構造体のバイト単位のサイズの指定
	wcex.style = CS_HREDRAW | CS_VREDRAW;	// Horizontal&Vertical ReDraw 位置やサイズが変更されたときに再描画する
	wcex.lpfnWndProc = window_procedure;	// このウィンドウに関連付けられたウィンドウプロシージャ(何らかのアクションを起こした際に発生するメッセージが送られる関数)のポインタを設定
	wcex.cbClsExtra = 0;	// ウィンドウクラス構造体に続いて割り当てる追加のバイト数 不明
	wcex.cbWndExtra = 0;	// ウィンドウインスタンスに続いて割り当てる追加のバイト数 不明
	wcex.hInstance = instance;
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor(NULL, IDC_CROSS);	// マウスカーソルの設定？
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);	// 背景の設定？
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = APPLICATION_NAME;	// ウィンドウに表示する名前を設定しているらしい？
	wcex.hIconSm = 0;	// ウィンドウのアイコンを設定できるらしい
	RegisterClassExW(&wcex);

	RECT rc{ 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);	// ウィンドウ長方形に必要なサイズを計算する
	// ウィンドウ生成のマクロ
	HWND hwnd = CreateWindowExW(0, APPLICATION_NAME, L"", WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
		rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, instance, NULL);
	ShowWindow(hwnd, cmd_show);	// ウィンドウの表示状態を設定する

	framework framework(hwnd);
	SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&framework));
	return framework.run();
}
