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
	wcex.cbSize = sizeof(WNDCLASSEX);	// �\���̂̃o�C�g�P�ʂ̃T�C�Y�̎w��
	wcex.style = CS_HREDRAW | CS_VREDRAW;	// Horizontal&Vertical ReDraw �ʒu��T�C�Y���ύX���ꂽ�Ƃ��ɍĕ`�悷��
	wcex.lpfnWndProc = window_procedure;	// ���̃E�B���h�E�Ɋ֘A�t����ꂽ�E�B���h�E�v���V�[�W��(���炩�̃A�N�V�������N�������ۂɔ������郁�b�Z�[�W��������֐�)�̃|�C���^��ݒ�
	wcex.cbClsExtra = 0;	// �E�B���h�E�N���X�\���̂ɑ����Ċ��蓖�Ă�ǉ��̃o�C�g�� �s��
	wcex.cbWndExtra = 0;	// �E�B���h�E�C���X�^���X�ɑ����Ċ��蓖�Ă�ǉ��̃o�C�g�� �s��
	wcex.hInstance = instance;
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor(NULL, IDC_CROSS);	// �}�E�X�J�[�\���̐ݒ�H
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);	// �w�i�̐ݒ�H
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = APPLICATION_NAME;	// �E�B���h�E�ɕ\�����閼�O��ݒ肵�Ă���炵���H
	wcex.hIconSm = 0;	// �E�B���h�E�̃A�C�R����ݒ�ł���炵��
	RegisterClassExW(&wcex);

	RECT rc{ 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);	// �E�B���h�E�����`�ɕK�v�ȃT�C�Y���v�Z����
	// �E�B���h�E�����̃}�N��
	HWND hwnd = CreateWindowExW(0, APPLICATION_NAME, L"", WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
		rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, instance, NULL);
	ShowWindow(hwnd, cmd_show);	// �E�B���h�E�̕\����Ԃ�ݒ肷��

	framework framework(hwnd);
	SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&framework));
	return framework.run();
}
