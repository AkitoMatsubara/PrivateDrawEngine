#include "SceneTitle.h"
#include "SceneGame.h"
#include "SceneLoading.h"

bool SceneTitle::Initialize() {
	Microsoft::WRL::ComPtr<ID3D11Device> device = FRAMEWORK->GetDevice();	// framework����device���擾
// �V�[���R���X�^���g�o�b�t�@�̐ݒ�
	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(SceneConstants);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	HRESULT hr = device->CreateBuffer(&buffer_desc, nullptr, ConstantBuffers[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// Blender�̐ݒ�
	//blender.setBlend(device.Get());

	// Sampler�̐ݒ�
	DefaultSampleClamp = std::make_shared<Sampler>(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);

	// �e��N���X�ݒ�
	{
		// sprite�I�u�W�F�N�g�𐶐�(����͐擪�̂P�����𐶐�����)
		TitleImage = std::make_unique<Sprite>();
		TitleImage->LoadImages(L".\\Resources\\Title\\Title.png");

		// Geometric_primitive�I�u�W�F�N�g�̐���
		{
			Grid = std::make_unique<Geometric_Cube>();
			Grid->setPos(DirectX::SimpleMath::Vector3(0, -1, 0));
			Grid->setSize(DirectX::SimpleMath::Vector3(10, 0.1f, 10));
		}

		GpuParticle = std::make_unique<GPUParticle>();
		GpuParticle->Init();

		skinned_mesh = std::make_unique<Skinned_Mesh>(".\\Resources\\Player\\Player.fbx");

		Camera::getInstance().SetProjection(DirectX::XMConvertToRadians(30), Camera::getInstance().GetWidth() / Camera::getInstance().GetHeight(), Camera::getInstance().GetNear(), Camera::getInstance().GetFar());
	}
	return true;
}

void SceneTitle::Update() {
	imguiUpdate();
	const float elapsed_time = FRAMEWORK->GetElapsedTime();

	// �V�[���؂�ւ�
	//if (GetKeyState('G') < 0) {
	//	setScene(std::make_unique<SceneLoading>(std::make_unique<SceneGame>()));
	//}
	if (GetAsyncKeyState(VK_RETURN) & 1)
	{
		setScene(std::make_unique<SceneLoading>(std::make_unique<SceneGame>()));
	}
	//if (GetAsyncKeyState('G') & 1) setScene(std::make_unique<SceneGame>());
	// �J��������
	// �J�����ݒ�
	Camera::getInstance().Set(Camera::getInstance().GetPos(), DirectX::SimpleMath::Vector3(0, 0, 0), DirectX::XMFLOAT3(0, 1, 0));
	// �J��������
	Camera::getInstance().Operate();
	Camera::getInstance().Activate();
}

void SceneTitle::Render() {
	HRESULT hr{ S_OK };

	static const Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context = FRAMEWORK->GetDeviceContext();

	FRAMEWORK->Clear(ClearColor,immediate_context.Get());	// ��U�N���A

		// �r���[�|�[�g�̐ݒ�
	FRAMEWORK->CreateViewPort(immediate_context.Get());

	// �T���v���[�X�e�[�g���o�C���h
	DefaultSampleClamp->Set(immediate_context.Get(), 0);

	immediate_context->OMSetBlendState(FRAMEWORK->GetBlendState(FRAMEWORK->BS_ALPHA), nullptr, 0xFFFFFFFF);	// �u�����h�C���^�[�t�F�[�X�̃|�C���^�A�u�����h�t�@�N�^�[�̔z��l�A�T���v���J�o���b�W(����̓f�t�H���g�w��)

	// 2D�I�u�W�F�N�g�̕`��ݒ�
	{
		immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(FRAMEWORK->DS_FALSE), 1);	// 3D�I�u�W�F�N�g�̌��ɏo�����߈�U
		TitleImage->Render(immediate_context.Get());
		immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(FRAMEWORK->DS_TRUE_WRITE), 1);			// 2D�I�u�W�F�N�g�Ƃ̑O��֌W���������肷�邽�ߍĐݒ�
	}
	// 3D�I�u�W�F�N�g�̕`��ݒ�
	{
		// �R���X�^���g�o�b�t�@�X�V
		SceneConstants data{};
		XMStoreFloat4x4(&data.view_projection, Camera::getInstance().GetView() * Camera::getInstance().GetProjection());	// Matrix����4x4�֕ϊ�
		data.light_direction = DirectX::SimpleMath::Vector4{ light_dir[0],light_dir[1],light_dir[2],0 };	// �V�F�[�_�ɓn�����C�g�̌���
		data.camera_position = DirectX::SimpleMath::Vector4{ eyePos.x,eyePos.y,eyePos.z,0 };				// �V�F�[�_�ɓn���J�����̈ʒu
		data.view = Camera::getInstance().GetView();
		data.projection = Camera::getInstance().GetProjection();
		immediate_context->UpdateSubresource(ConstantBuffers[0].Get(), 0, 0, &data, 0, 0);
		immediate_context->VSSetConstantBuffers(1, 1, ConstantBuffers[0].GetAddressOf());	// cBuffer�̓h���[�R�[���̂��тɏ��������̂œs�x�ݒ肷��K�v������
		immediate_context->PSSetConstantBuffers(1, 1, ConstantBuffers[0].GetAddressOf());
		{
			// 3D�I�u�W�F�N�gRender���ɈڐA ���󂱂��ł���K�v�Ȃ��H
			Grid->Render(true);
			//skinned_mesh->Render(immediate_context.Get());
			//GpuParticle->Play(immediate_context.Get());
		}
	}

#ifdef USE_IMGUI
	//	ImGui::Render();
	//	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#else
	FRAMEWORK->Flip();
#endif
}

void SceneTitle::imguiUpdate() {
#ifdef USE_IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	imguiSceneChanger();
	// 2D�p �����֐��Ŋ��������Ă�
	TitleImage->ImguiWindow();
	// 3D�p�p�����[�^
	//skinned_mesh->imguiWindow("fbx");

	// ���C�g�������O���[�o���ݒ�
	ImGui::Begin("Light");
	ImGui::SliderFloat3("Light_Direction", light_dir, -10.0f, 10.0f);
	ImGui::Checkbox("focus Zero", &focus_zero);
	ImGui::End();
#endif
}