#pragma once

#include <d3d11.h>
#include <memory>
#include <wrl.h>
#include <windows.h>
#include <tchar.h>
#include <sstream>
#include "misc.h"
#include "high_resolution_timer.h"

#include "Sampler.h"

#ifdef USE_IMGUI
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern ImWchar glyphRangesJapanese[];
#endif

CONST LONG SCREEN_WIDTH{ 1920 };
CONST LONG SCREEN_HEIGHT{ 1080 };
CONST BOOL FULLSCREEN{ FALSE };
CONST LPCWSTR APPLICATION_NAME{ L"MyGame" };	// �E�B���h�E��ɕ\�������e�L�X�g �E�B���h�E��

class framework
{
	// �ϐ�
private:
	static framework* instance;

	high_resolution_timer tictoc;
	uint32_t frames{ 0 };
	float elapsed_time{ 0.0f };
	void calculate_frame_stats();

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11Debug> debug;

	inline static Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context;	// �`��R�}���h�̒ǉ��⑗�M�Ȃǂ̏����������Ă���BCPU���Œǉ����ꂽ�`��R�}���h��GPU���ɑ��M����B
																	// Immediate�͐��������R�}���h�𑦎����s���邱�Ƃ�\���B���΂�Deferred�Ƃ������̂����݂���B

	Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		render_target_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		depth_stencil_view;


	static const int DEPTH_STENCIL_TYPE = 9;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>		depth_stencil_state[DEPTH_STENCIL_TYPE];

	static const int RATERIZER_TYPE = 6;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizer_state[RATERIZER_TYPE];

	static const int BLEND_TYPE = 9;
	Microsoft::WRL::ComPtr<ID3D11BlendState> bd_states[BLEND_TYPE];

	Microsoft::WRL::ComPtr<ID3D11SamplerState>	sampler_states[3];
	std::shared_ptr<Sampler> sample;

public:
	CONST HWND hwnd;

	//DepthStencilState
	enum { DS_FALSE, DS_TRUE, DS_FALSE_WRITE, DS_TRUE_WRITE, DS_END };
	// DlendState
	enum { BS_NONE, BS_ALPHA, BS_ADD, BS_SUBTRACT, BS_REPLACE, BS_MULTIPLY, BS_LIGHTEN, BS_DARKEN, BS_SCREEN, BS_END };
	// RasterizerState
	enum { RS_SOLID_NONE, RS_WIRE_NONE, RS_SOLID_BACK, RS_WIRE_BACK, RS_SOLID_BACK_CCW, RS_END};

	// �֐�
private:
	bool initialize();
	bool uninitialize();

	bool CreateDeviceAndSwapCain();		// Device��SwapChain�쐬
	bool CreateRenderTargetView();		// RenderTargetView�̍쐬
	bool CreateDepthStencileView();		// DepthStencilView�̍쐬
	bool CreateDepthStencileState();	// DepthStencilState�̍쐬
	bool CreateRasterizerState();		// RasterizerState�̍쐬
	bool CreateBlendState();			// BlendState�̍쐬

public:
	framework(HWND hwnd);
	~framework();

	framework(const framework&) = delete;
	framework& operator=(const framework&) = delete;
	framework(framework&&) noexcept = delete;
	framework& operator=(framework&&) noexcept = delete;

	void Clear(FLOAT color[4], ID3D11DeviceContext* device_context);
	void Flip(int n = 0);

	int run();	// �Q�[�����[�v
	LRESULT CALLBACK handle_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	static framework* getInstance() { return instance; }

	bool CreateViewPort(ID3D11DeviceContext* device_context, float width = SCREEN_WIDTH, float height = SCREEN_HEIGHT);	// ViewPort�̍쐬

	// �Q�b�^�[
	ID3D11Device* GetDevice() { return device.Get(); }
	ID3D11DeviceContext* GetDeviceContext() { return immediate_context.Get(); }
	ID3D11BlendState* GetBlendState(int state) { return bd_states[state].Get(); }
	ID3D11DepthStencilState* GetDepthStencileState(int state) { return depth_stencil_state[state].Get(); }
	ID3D11DepthStencilView* GetDepthStencileView() { return depth_stencil_view.Get(); }
	ID3D11RasterizerState* GetRasterizerState(int state) { return rasterizer_state[state].Get(); }

	const float GetElapsedTime() { return tictoc.time_interval(); }
};

#define FRAMEWORK framework::getInstance()