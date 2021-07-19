#pragma once

#include <windows.h>
#include <tchar.h>
#include <sstream>

#include "misc.h"
#include "high_resolution_timer.h"
#include "sprite.h"
#include "sprite_Batch.h"
#include "Geometric_Primitive.h"
#include "Static_Mesh.h"
#include "Blender.h"
#include <d3d11.h>
#include <wrl.h>

using namespace Microsoft::WRL;
using namespace std;

#ifdef USE_IMGUI
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern ImWchar glyphRangesJapanese[];
#endif

CONST LONG SCREEN_WIDTH{ 1280 };
CONST LONG SCREEN_HEIGHT{ 720 };
CONST BOOL FULLSCREEN{ FALSE };
CONST LPCWSTR APPLICATION_NAME{ L"X3DGP" };

class framework
{
public:
	CONST HWND hwnd;


	framework(HWND hwnd);
	~framework();

	framework(const framework&) = delete;
	framework& operator=(const framework&) = delete;
	framework(framework&&) noexcept = delete;
	framework& operator=(framework&&) noexcept = delete;

	static framework* instance;

	ComPtr<ID3D11Device> device;

	ComPtr<ID3D11DeviceContext> immediate_context;	// �`��R�}���h�̒ǉ��⑗�M�Ȃǂ̏����������Ă���BCPU���Œǉ����ꂽ�`��R�}���h��GPU���ɑ��M����B
													// Immediate�͐��������R�}���h�𑦎����s���邱�Ƃ�\���B���΂�Deferred�Ƃ������̂����݂���B�v����

	ComPtr<IDXGISwapChain> swap_chain;
	ComPtr<ID3D11RenderTargetView>		render_target_view;
	ComPtr<ID3D11DepthStencilView>		depth_stensil_view;
	ComPtr<ID3D11SamplerState>			sampler_states[3];
	ComPtr<ID3D11DepthStencilState>		depth_stencil_state[4];
	Blender blender;

	// Sprite�^ �摜�`��p
	unique_ptr<Sprite> sprites;
	unique_ptr<sprite_Batch> sprite_batches[8];
	unique_ptr<Sprite> sprite_text;	// �����\���I�Ȃ��

	// Geometric_primitive�p
	struct scene_constants {	// �V�[���萔�o�b�t�@
		XMFLOAT4X4 view_projection;	// VP�ϊ��s��
		XMFLOAT4 light_direction;	// ���C�g�̌���
	};
	ComPtr<ID3D11Buffer> constant_buffer[8];

	// Geometric_primitive�̕ϐ����
	unique_ptr< Geometric_Cube> grid;	// �O���b�h�����ǂ�
	unique_ptr< Geometric_Capsule> obj_1;
	unique_ptr< Geometric_Capsule> obj_2;

	// Static_Mesh�p
	unique_ptr<Static_Mesh> static_mesh;


	// �l ImGui�Ő��l��ҏW�A�i�[���Ċ֐��ɓn���ϐ�
	float light_dir[3] { 0.0f,0.0f,1.0f };
	XMFLOAT3 eyePos = XMFLOAT3(0.0f, 0.0f, -10.0f);

	static framework* getInstance() {
		return instance;
	}

	int run()
	{
		MSG msg{};

		if (!initialize())
		{
			return 0;
		}

#ifdef USE_IMGUI
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\consola.ttf", 14.0f, nullptr, glyphRangesJapanese);
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX11_Init(device.Get(), immediate_context.Get());
		ImGui::StyleColorsDark();
#endif

		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				tictoc.tick();
				calculate_frame_stats();
				update(tictoc.time_interval());
				render(tictoc.time_interval());
			}
		}

#ifdef USE_IMGUI
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
#endif

#if 1
		BOOL fullscreen = 0;
		swap_chain->GetFullscreenState(&fullscreen, 0);
		if (fullscreen)
		{
			swap_chain->SetFullscreenState(FALSE, 0);
		}
#endif

		return uninitialize() ? static_cast<int>(msg.wParam) : 0;
	}

	LRESULT CALLBACK handle_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
#ifdef USE_IMGUI
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) { return true; }
#endif
		switch (msg)
		{
		case WM_PAINT:
		{
			PAINTSTRUCT ps{};
			BeginPaint(hwnd, &ps);

			EndPaint(hwnd, &ps);
		}
		break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CREATE:
			break;
		case WM_KEYDOWN:
			if (wparam == VK_ESCAPE)
			{
				PostMessage(hwnd, WM_CLOSE, 0, 0);
			}
			break;
		case WM_ENTERSIZEMOVE:
			tictoc.stop();
			break;
		case WM_EXITSIZEMOVE:
			tictoc.start();
			break;
		default:
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}
		return 0;
	}

private:
	bool initialize();
	void update(float elapsed_time/*Elapsed seconds from last frame*/);
	void render(float elapsed_time/*Elapsed seconds from last frame*/);
	bool uninitialize();

private:
	high_resolution_timer tictoc;
	uint32_t frames{ 0 };
	float elapsed_time{ 0.0f };
	void calculate_frame_stats()
	{
		if (++frames, (tictoc.time_stamp() - elapsed_time) >= 1.0f)
		{
			float fps = static_cast<float>(frames);
			std::wostringstream outs;
			outs.precision(6);
			outs << APPLICATION_NAME << L" : FPS : " << fps << L" / " << L"Frame Time : " << 1000.0f / fps << L" (ms)";
			SetWindowTextW(hwnd, outs.str().c_str());

			frames = 0;
			elapsed_time += 1.0f;
		}
	}
};

#define FRAMEWORK framework::getInstance()
