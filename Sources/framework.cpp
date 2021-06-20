#include "framework.h"

framework::framework(HWND hwnd) : hwnd(hwnd)
{
}

bool framework::initialize()
{
	// �f�o�C�X�E�f�o�C�X�R���e�L�X�g�E�X���b�v�`�F�[���̍쐬
	HRESULT hr{ S_OK };

	UINT creat_device_flags{ 0 };
#ifdef _DEBUG
	creat_device_flags |= D3D11_CREATE_DEVICE_DEBUG;	// �f�o�b�O���C���[���T�|�[�g����f�o�C�X���쐬
#endif

	D3D_FEATURE_LEVEL feature_levels{ D3D_FEATURE_LEVEL_11_0 };	// ������B�����DirectX11�̊�����S�ɖ�������GPU�œ��삷�郌�x��

	DXGI_SWAP_CHAIN_DESC swap_chain_desc{};
	swap_chain_desc.BufferCount = 1;	// SwapChain�̃o�b�t�@��
	swap_chain_desc.BufferDesc.Width = SCREEN_WIDTH;	// �o�b�t�@�̉���
	swap_chain_desc.BufferDesc.Height = SCREEN_HEIGHT;	// �o�b�t�@�̏c��
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// �o�b�t�@�̃t�H�[�}�b�g�w�� UNORM��Unsigned Normalized�̗��A�����Ȃ����K�������炵���� https://docs.microsoft.com/ja-jp/windows/uwp/graphics-concepts/data-type-conversion
	swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;	        // ���t���b�V�����[�g�̐ݒ� ����
	swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;	        // ���t���b�V�����[�g�̐ݒ� ���q	�܂�1/60
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// �o�b�t�@�̎g�p���@�̎w�� �V�F�[�_�̏o�͂Ɏg����悤�ɂ���炵���� http://marupeke296.com/DX10_No1_Init.html
	swap_chain_desc.OutputWindow = hwnd;	// �o�̓E�B���h�E�n���h��
	swap_chain_desc.SampleDesc.Count = 1;	// 1�s�N�Z���̐F�����߂�T���v�����O��	���g�p��1
	swap_chain_desc.SampleDesc.Quality = 0;	// �T���v�����O�̕i��(���x)			���g�p��0
	swap_chain_desc.Windowed = !FULLSCREEN;	// �E�B���h�E���[�h
	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, creat_device_flags,
		&feature_levels, 1, D3D11_SDK_VERSION, &swap_chain_desc, &swap_chain, &device, NULL, &immediate_context);	// Device��SwapChain�̐ݒ�𓯎��ɍs�� �Q�l�� https://yttm-work.jp/directx/directx_0012.html
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));	// _ASSERT_EXPR�͂����炭���b�Z�[�W�\�����\��assert�BSUCCEEDED�Ő�������Ahr_trace�͂����炭�G���[���b�Z�[�W�̕\���H

	// �����_�[�^�[�Q�b�g�r���[�̐ݒ�
	ID3D11Texture2D* back_buffer{};
	hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&back_buffer));	// swap_chain�̂��o�b�t�@�ō쐬�������̂�Get����
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = device->CreateRenderTargetView(back_buffer, NULL, &render_target_view);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	back_buffer->Release();	// Target�̎擾���I������̂Ńo�b�t�@������B�Q�ƃJ�E���^���P���Z����Ă��邽�߉�����Ȃ��ƃ��������[�N�̌����ƂȂ�

	// �[�x�X�e���V���r���[�̍쐬
	ID3D11Texture2D* depth_stencil_buffer{};
	D3D11_TEXTURE2D_DESC textuer2d_desc{};
	textuer2d_desc.Width = SCREEN_WIDTH;	// �o�b�t�@�̉���
	textuer2d_desc.Height = SCREEN_HEIGHT;	// �o�b�t�@�̏c��
	textuer2d_desc.MipLevels = 1;	// �~�j�}�b�v�̃��x���w��
	textuer2d_desc.ArraySize = 1;	// �e�N�X�`���z��̃T�C�Y�w��
	textuer2d_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// �e�N�X�`���̃t�H�[�}�b�g�BDXGI_FORMAT_D24_UNORM_S8_UINT���g����Depth24bit�AStencil8bit�ƂȂ�
	textuer2d_desc.SampleDesc.Count = 1;
	textuer2d_desc.SampleDesc.Quality = 0;
	textuer2d_desc.Usage = D3D11_USAGE_DEFAULT;				// �e�N�X�`���̎g�p���@
	textuer2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;	// �o�C���h�ݒ�
	textuer2d_desc.CPUAccessFlags = 0;	// ���\�[�X�ւ�CPU�̃A�N�Z�X�����ݒ� ��{�O�ł����炵��
	textuer2d_desc.MiscFlags = 0;		// ���\�[�X�I�v�V�����̃t���O �s�K�v�Ȃ̂łO
	hr = device->CreateTexture2D(&textuer2d_desc, NULL, &depth_stencil_buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
	depth_stencil_view_desc.Format = textuer2d_desc.Format;
	depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depth_stencil_view_desc.Texture2D.MipSlice = 0;	// �ŏ��Ɏg�p����~�b�v�}�b�v�̃��x�����w��
	hr = device->CreateDepthStencilView(depth_stencil_buffer, &depth_stencil_view_desc, &depth_stensil_view);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	depth_stencil_buffer->Release();

	// �T���v���[�X�e�[�g�̐���
	D3D11_SAMPLER_DESC sampler_desc;
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;	// �g�k���̐F�̎擾���@ https://msdn.microsoft.com/ja-jp/library/ee416129(v=vs.85).aspx
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;		// UV���W��0�`1�͈̔͊O�̏ꍇ�̐F�̎擾���@
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MipLODBias = 0;							// �v�Z���ꂽ�~�b�v�}�b�v ���x������̃o�C�A
	sampler_desc.MaxAnisotropy = 16;						// �T���v�����O�Ɉٕ�����Ԃ��g�p���Ă���ꍇ�̌��E�l�B�L���Ȓl�� 1 �` 16
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;	// ��r�I�v�V����
	sampler_desc.BorderColor[0] = 0;
	sampler_desc.BorderColor[1] = 0;
	sampler_desc.BorderColor[2] = 0;
	sampler_desc.BorderColor[3] = 0;
	sampler_desc.MinLOD = 0;								// �A�N�Z�X�\�ȃ~�j�}�b�v�̉����l
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;				// �A�N�Z�X�\�ȃ~�j�}�b�v�̏���l
	hr = device->CreateSamplerState(&sampler_desc, &sampler_states[0]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	hr = device->CreateSamplerState(&sampler_desc, &sampler_states[1]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;
	hr = device->CreateSamplerState(&sampler_desc, &sampler_states[2]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// sprite�I�u�W�F�N�g�𐶐�(����͐擪�̂P�����𐶐�����)
	sprites[0] = new Sprite(device,L".\\resources\\box.png");	// �V�F�[�_�[�̓R���X�g���N�^���Ŏw�肵�Ă��邽�߁A�ʂ��g���ɂ͉��ǂ��K�v

	// �r���[�|�[�g�̐ݒ�
	D3D11_VIEWPORT viewport{};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(SCREEN_WIDTH);
	viewport.Height = static_cast<float>(SCREEN_HEIGHT);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	immediate_context->RSSetViewports(1, &viewport);

	return true;
}

void framework::update(float elapsed_time/*Elapsed seconds from last frame*/)
{
#ifdef USE_IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif


#ifdef USE_IMGUI
	ImGui::Begin("ImGUI");
		ImGui::SliderFloat("Pos.x", &spritePos.x, -SCREEN_WIDTH, SCREEN_WIDTH);
		ImGui::SliderFloat("Pos.y", &spritePos.y, -SCREEN_HEIGHT, SCREEN_HEIGHT);
		ImGui::SliderFloat("angle", &angle, 0, 360);
	ImGui::End();
#endif
}
void framework::render(float elapsed_time/*Elapsed seconds from last frame*/)
{
	HRESULT hr{ S_OK };

	FLOAT color[]{ 0.2f,0.2f,0.2f,1.0f };	// �w�i�F
	immediate_context->ClearRenderTargetView(render_target_view, color);	// �N���A�Ώۂ�View�A�N���A����F
	immediate_context->ClearDepthStencilView(depth_stensil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	immediate_context->OMSetRenderTargets(1, &render_target_view, depth_stensil_view);

	// �T���v���[�X�e�[�g���o�C���h
	immediate_context->PSSetSamplers(0, 1, &sampler_states[0]);
	immediate_context->PSSetSamplers(1, 1, &sampler_states[1]);
	immediate_context->PSSetSamplers(2, 1, &sampler_states[2]);

	// sprites�̕`��	(��`)
	// �|�C���^�A��`����̕`��ʒu�A��`�̑傫���A�F
	//sprites[0]->render(immediate_context, spritePos.x,spritePos.y, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, DirectX::XMFLOAT4(0, 0.5f, 0, 1));
	sprites[0]->render(immediate_context, spritePos, DirectX::XMFLOAT2(SCREEN_WIDTH/2, SCREEN_HEIGHT/2), angle, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

#ifdef USE_IMGUI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif


	UINT sync_interval{ 0 };	// sync_interval �� 1 ���Z�b�g����� 60FPS �̌Œ�t���[�����[�g�œ��삷��B�O�ŉσt���[�����[�g�ł̓����O��ɍs��
	swap_chain->Present(sync_interval, 0);	// �o�b�N�o�b�t�@���t�����g�o�b�t�@�ɑ��M����

}

bool framework::uninitialize()
{
	device->Release();
	immediate_context->Release();
	swap_chain->Release();
	render_target_view->Release();
	depth_stensil_view->Release();

	// sprites�I�u�W�F�N�g�̉��
	for (Sprite* p : sprites) {
		delete p;
	}
	//// sprites�I�u�W�F�N�g�̉��
	//for (ID3D11SamplerState* s : sampler_states) {
	//	delete s;
	//}

	return true;
}

framework::~framework()
{

}