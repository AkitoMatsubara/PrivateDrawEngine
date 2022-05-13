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
		LoadingImage->LoadImages(L".\\Resources\\black-metal-texture.jpg");
	}

	 // �X���b�h�J�n
	std::thread thread(LoadingThread, this);	// LoadingThread�֐������s�Athis�������ɐݒ�
	// �X���b�h�̊Ǘ������
	thread.detach();

	GpuParticle = std::make_unique<GPUParticle>();
	GpuParticle->Init();

	return true;
}

void SceneLoading::Update() {
	imguiUpdate();

	{
		// ���̃V�[����
		if (NextScene->isReady())
		{
			SceneManager::getInstance().ChangeScene(NextScene.release());
			//SceneManager::getInstance().ChangeScene(std::move(nextScene));
			return;
		}

	}

}

void SceneLoading::Render() {
	HRESULT hr{ S_OK };

	ID3D11DeviceContext* immediate_context = FRAMEWORK->GetDeviceContext();	// DevCon�擾

	FRAMEWORK->Clear(ClearColor);	// ��U�N���A

	// �r���[�|�[�g�̐ݒ�
	FRAMEWORK->CreateViewPort();

	// �T���v���[�X�e�[�g���o�C���h
	DefaultSampleClamp->Set(0);

	immediate_context->OMSetBlendState(FRAMEWORK->GetBlendState(FRAMEWORK->BS_ALPHA), nullptr, 0xFFFFFFFF);	// �u�����h�C���^�[�t�F�[�X�̃|�C���^�A�u�����h�t�@�N�^�[�̔z��l�A�T���v���J�o���b�W(����̓f�t�H���g�w��)

	// 2D�I�u�W�F�N�g�̕`��ݒ�
	{
		immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(FRAMEWORK->DS_TRUE), 1);	// 3D�I�u�W�F�N�g�̌��ɏo�����߈�U
		LoadingImage->Render();
	}
	// 3D�I�u�W�F�N�g�̕`��ݒ�
	{
		GpuParticle->Play();
		//D3D11_VIEWPORT viewport;
		//UINT num_viewports{ 1 };
		//immediate_context->RSGetViewports(&num_viewports, &viewport);	// ���X�^���C�U�X�e�[�W�Ƀo�C���h���ꂽviewport�̔z����擾

		//float aspect_ratio{ viewport.Width / viewport.Height };	// �A�X�y�N�g��
		//// �������e�s��̍쐬
		//DirectX::XMMATRIX P{DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(30),aspect_ratio,0.1f,100.0f) };	// ����p,�c����,�߂���Z,������Z

		//DirectX::XMVECTOR eye{DirectX::XMVectorSet(EyePos.x,EyePos.y,EyePos.z,1.0f) };
		//DirectX::XMVECTOR focus;
		////if (!focus_zero) {
		////	//focus = { XMVectorSet(EyePos.x,EyePos.y,EyePos.z + 1,1.0f) };	// �J�����ʒu�̑O
		////	focus = { XMVectorSet(skinned_mesh->getPos().x,skinned_mesh->getPos().y,skinned_mesh->getPos().z,1.0f) };	// �J�����ʒu�̑O
		////}
		////else {
		//	focus = {DirectX::XMVectorSet(0.0f,0.0f,0.0f,1.0f) };
		////}
		//DirectX::XMVECTOR up{DirectX::XMVectorSet(0.0f,1.0f,0.0f,0.0f) };
		//// ViewMatrix�̍쐬(LH = LeftHand(������W�n))
		//DirectX::XMMATRIX V{DirectX::XMMatrixLookAtLH(eye, focus, up) };	// �J�������W�A�œ_�A�J�����̏����

		//// �R���X�^���g�o�b�t�@�X�V
		//scene_constants data{};
		//XMStoreFloat4x4(&data.view_projection, V * P);	// Matrix����4x4�֕ϊ�
		//data.light_direction = DirectX::SimpleMath::Vector4{ light_dir[0],light_dir[1],light_dir[2],0 };	// �V�F�[�_�ɓn�����C�g�̌���
		//data.camera_position = DirectX::SimpleMath::Vector4{ EyePos.x,EyePos.y,EyePos.z,0 };				// �V�F�[�_�ɓn���J�����̈ʒu
		//immediate_context->UpdateSubresource(ConstantBuffers[0].Get(), 0, 0, &data, 0, 0);
		//immediate_context->VSSetConstantBuffers(1, 1, ConstantBuffers[0].GetAddressOf());	// cBuffer�̓h���[�R�[���̂��тɏ��������̂œs�x�ݒ肷��K�v������
		//immediate_context->PSSetConstantBuffers(1, 1, ConstantBuffers[0].GetAddressOf());

		//immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(DS_TRUE_WRITE), 1);			// 2D�I�u�W�F�N�g�Ƃ̑O��֌W���������肷�邽�ߍĐݒ�
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