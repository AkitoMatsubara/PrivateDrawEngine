#include <thread>
#include "SceneLoading.h"
#include "SceneGame.h"

bool SceneLoading::Initialize() {
	Microsoft::WRL::ComPtr<ID3D11Device> device = FRAMEWORK->GetDevice();	// framework����device���擾
	HRESULT hr = { S_OK };

	// �V�[���R���X�^���g�o�b�t�@�̐ݒ�
	{
		D3D11_BUFFER_DESC buffer_desc{};
		buffer_desc.ByteWidth = sizeof(SceneConstants);
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		hr = device->CreateBuffer(&buffer_desc, nullptr, ConstantBuffers[0].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	// Sampler�̐ݒ�
	DefaultSampleClamp = std::make_shared<Sampler>(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);

	// �e��N���X�ݒ�
	{

		// ���[�h�摜�̏�����
		LoadingImage = std::make_unique<Sprite>();
		LoadingImage->LoadImages(L".\\Resources\\Loading\\Loading.png");
	}

	 // �X���b�h�J�n
	std::thread threadTemp(LoadingThread, this);	// LoadingThread�֐������s�Athis�������ɐݒ�
	Thread.swap(threadTemp);

	// �X���b�h�̊Ǘ������
	Thread.detach();

	GpuParticle = std::make_unique<GPUParticle>();
	GpuParticle->Init();

	//�f�B�t�@�[�h �R���e�L�X�g
	device->CreateDeferredContext(NULL, deferredContext.GetAddressOf());
	return true;
}

void SceneLoading::Update() {
	imguiUpdate();

	{
		// ���̃V�[����
		if (NextScene->isReady())
		{
			SceneManager::getInstance().setLoadComplete(true);	// ���[�h����
			SceneManager::getInstance().ChangeScene(NextScene.release());
			return;
		}

	}

}

void SceneLoading::Render() {
	HRESULT hr{ S_OK };

	if (!NextScene->isReady())
	{
#if 1	// deferredContext ���C���X���b�h�ƃT�u�X���b�h�œ�����DeviceContext�ɃA�N�Z�X����ƃ������j�󂪋N����̂ŕʁX�ɕ�����
		static const Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context = FRAMEWORK->GetDeviceContext();	// �R�}���h�����p
		Microsoft::WRL::ComPtr<ID3D11CommandList> CommandList;
		CommandList.Reset();	// ���g�̍폜

		FRAMEWORK->Clear(ClearColor, deferredContext.Get());

		// �r���[�|�[�g�̐ݒ�
		FRAMEWORK->CreateViewPort(deferredContext.Get(), SCREEN_WIDTH, SCREEN_HEIGHT);

		// �T���v���[�X�e�[�g���o�C���h
		DefaultSampleClamp->Set(deferredContext.Get(), 0);

		deferredContext->OMSetBlendState(FRAMEWORK->GetBlendState(FRAMEWORK->BS_ALPHA), nullptr, 0xFFFFFFFF);	// �u�����h�C���^�[�t�F�[�X�̃|�C���^�A�u�����h�t�@�N�^�[�̔z��l�A�T���v���J�o���b�W(����̓f�t�H���g�w��)


		// �V�[���R���X�^���g�o�b�t�@�X�V
		SceneConstants data{};
		XMStoreFloat4x4(&data.view_projection, Camera::getInstance().GetView() * Camera::getInstance().GetProjection());	// Matrix����4x4�֕ϊ�
		data.light_direction = DirectX::SimpleMath::Vector4{ light_dir[0],light_dir[1],light_dir[2],0 };	// �V�F�[�_�ɓn�����C�g�̌���
		data.camera_position = DirectX::SimpleMath::Vector4{ Camera::getInstance().GetPos().x,Camera::getInstance().GetPos().y,Camera::getInstance().GetPos().z,0 };	// �V�F�[�_�ɓn���J�����̈ʒu
		data.view = Camera::getInstance().GetView();
		data.projection = Camera::getInstance().GetProjection();
		deferredContext->UpdateSubresource(ConstantBuffers[0].Get(), 0, 0, &data, 0, 0);
		deferredContext->VSSetConstantBuffers(1, 1, ConstantBuffers[0].GetAddressOf());
		deferredContext->PSSetConstantBuffers(1, 1, ConstantBuffers[0].GetAddressOf());

		// 2D�I�u�W�F�N�g�̕`��ݒ�
		{
			deferredContext->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(FRAMEWORK->DS_FALSE), 1);	// 3D�I�u�W�F�N�g�̌��ɏo�����߈�U
			LoadingImage->Render(deferredContext.Get(), LoadingImage->getTexture(), LoadingImage->getShader(), CommandList.GetAddressOf());
			deferredContext->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(FRAMEWORK->DS_TRUE_WRITE), 1);			// 2D�I�u�W�F�N�g�Ƃ̑O��֌W���������肷�邽�ߍĐ�

		}
		// 3D�I�u�W�F�N�g�̕`��ݒ�
		{
			//GpuParticle->Play(deferredContext.Get());
			//immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(DS_TRUE_WRITE), 1);			// 2D�I�u�W�F�N�g�Ƃ̑O��֌W���������肷�邽�ߍĐݒ�
		}

		//�f�B�t�@�[�h �R���e�L�X�g�ō쐬�����R�}���h���X�g�����s
		immediate_context->ExecuteCommandList(CommandList.Get(), FALSE);
#else
		static const Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context = FRAMEWORK->GetDeviceContext();
		Microsoft::WRL::ComPtr<ID3D11CommandList> CommandList;
		FRAMEWORK->Clear(ClearColor);	// ��U�N���A

		// �r���[�|�[�g�̐ݒ�
		FRAMEWORK->CreateViewPort(SCREEN_WIDTH, SCREEN_HEIGHT, immediate_context.Get());

		// �T���v���[�X�e�[�g���o�C���h
		DefaultSampleClamp->Set(immediate_context.Get(), 0);

		immediate_context->OMSetBlendState(FRAMEWORK->GetBlendState(FRAMEWORK->BS_ALPHA), nullptr, 0xFFFFFFFF);	// �u�����h�C���^�[�t�F�[�X�̃|�C���^�A�u�����h�t�@�N�^�[�̔z��l�A�T���v���J�o���b�W(����̓f�t�H���g�w��)

		// �V�[���R���X�^���g�o�b�t�@�X�V
		SceneConstants data{};
		XMStoreFloat4x4(&data.view_projection, Camera::getInstance().GetView() * Camera::getInstance().GetProjection());	// Matrix����4x4�֕ϊ�
		data.light_direction = DirectX::SimpleMath::Vector4{ light_dir[0],light_dir[1],light_dir[2],0 };	// �V�F�[�_�ɓn�����C�g�̌���
		data.camera_position = DirectX::SimpleMath::Vector4{ Camera::getInstance().GetPos().x,Camera::getInstance().GetPos().y,Camera::getInstance().GetPos().z,0 };	// �V�F�[�_�ɓn���J�����̈ʒu
		data.view = Camera::getInstance().GetView();
		data.projection = Camera::getInstance().GetProjection();
		immediate_context->UpdateSubresource(ConstantBuffers[0].Get(), 0, 0, &data, 0, 0);
		immediate_context->VSSetConstantBuffers(0, 1, ConstantBuffers[0].GetAddressOf());	// cBuffer�̓h���[�R�[���̂��тɏ��������̂œs�x�ݒ肷��K�v������
		immediate_context->PSSetConstantBuffers(0, 1, ConstantBuffers[0].GetAddressOf());

		// 2D�I�u�W�F�N�g�̕`��ݒ�
		{
			immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(FRAMEWORK->DS_FALSE), 1);	// 3D�I�u�W�F�N�g�̌��ɏo�����߈�U
			//LoadingImage->Render(immediate_context.Get(), LoadingImage->getTexture(), LoadingImage->getShader(), CommandList.GetAddressOf());
			LoadingImage->Render(immediate_context.Get(), LoadingImage->getTexture(), LoadingImage->getShader(), CommandList.GetAddressOf());
			immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(FRAMEWORK->DS_TRUE_WRITE), 1);			// 2D�I�u�W�F�N�g�Ƃ̑O��֌W���������肷�邽�ߍĐ�
		}
		// 3D�I�u�W�F�N�g�̕`��ݒ�
		{
			//GpuParticle->Play(deferredContext.Get());
			//immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(DS_TRUE_WRITE), 1);			// 2D�I�u�W�F�N�g�Ƃ̑O��֌W���������肷�邽�ߍĐݒ�
		}

		//�f�B�t�@�[�h �R���e�L�X�g�ō쐬�����R�}���h���X�g�����s
		//immediate_context->ExecuteCommandList(CommandList.Get(), FALSE);
		#endif
	}
#ifdef USE_IMGUI
	//	ImGui::Render();
	//	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#else
	FRAMEWORK->Flip();
#endif
}

void SceneLoading::LoadingThread(SceneLoading* scene) {
	HRESULT hr;
	//	COM�֘A�̏������ŃX���b�h���ɌĂԕK�v������
	hr = CoInitialize(nullptr);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// ���̃V�[���̏�����
	scene->NextScene->Initialize();

	// �X���b�h���I���O��COM�֘A�̏I����
	CoUninitialize();

	// �J�ڏ����ݒ�
	scene->NextScene->setReady(true);
}

void SceneLoading::imguiUpdate() {
#ifdef USE_IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	imguiSceneChanger();

	// ���C�g�������O���[�o���ݒ�
	ImGui::Begin("Light");
	ImGui::SliderFloat3("Light_Direction", light_dir, -10.0f, 10.0f);
	ImGui::Checkbox("focus Zero", &focus_zero);
	ImGui::End();
#endif
}