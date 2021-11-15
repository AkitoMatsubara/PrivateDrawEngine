//#include "SceneManager.h"
#include "SceneTitle.h"
#include "SceneTest_2.h"
#include "SceneLoading.h"

bool SceneTitle::Initialize() {
	Microsoft::WRL::ComPtr<ID3D11Device> device = FRAMEWORK->GetDevice();	// framework����device���擾
// �V�[���R���X�^���g�o�b�t�@�̐ݒ�
	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(scene_constants);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	HRESULT hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// Blender�̐ݒ�
	//blender.setBlend(device.Get());

	// Sampler�̐ݒ�
	sampleClamp = std::make_shared<Sampler>(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);

	// �e��N���X�ݒ�
	{
		// sprite�I�u�W�F�N�g�𐶐�(����͐擪�̂P�����𐶐�����)
		TitleImage = std::make_unique<Sprite>(L".\\Resources\\screenshot.jpg");	// �V�F�[�_�[�̓R���X�g���N�^���Ŏw�肵�Ă��邽�߁A�ʂ��g���ɂ͉��ǂ��K�v
		TitleImage->setSize(1280, 720);
		SpriteShader = std::make_unique<ShaderEx>();
		SpriteShader->Create(L"Shaders\\sprite_vs", L"Shaders\\sprite_ps");

		// Geometric_primitive�I�u�W�F�N�g�̐���
		{
			grid = std::make_unique<Geometric_Cube>();
			grid->setPos(DirectX::SimpleMath::Vector3(0, -1, 0));
			grid->setSize(DirectX::SimpleMath::Vector3(10, 0.1f, 10));
			GeomtricShader = std::make_unique<ShaderEx>();
			GeomtricShader->Create(L"Shaders\\geometric_primitive_vs", L"Shaders\\geometric_primitive_ps");
		}

		//skinned_mesh = make_unique<Skinned_Mesh>(".\\Resources\\cube.000.fbx");		// �e�N�X�`���A�}�e���A������
		skinned_mesh = std::make_unique<Skinned_Mesh>(".\\Resources\\cube.001.0.fbx");	// �e�N�X�`���g�p
		//skinned_mesh = make_unique<Skinned_Mesh>(".\\Resources\\cube.001.1.fbx");	// ���ߍ��݃e�N�X�`��
		//skinned_mesh = make_unique<Skinned_Mesh>(".\\Resources\\cube.002.0.fbx");	// 3��e�N�X�`���g�p
		//skinned_mesh = make_unique<Skinned_Mesh>(".\\Resources\\cube.002.1.fbx");	// �e�N�X�`���L�薳���A�}�e���A���L�薳������
		//skinned_mesh = make_unique<Skinned_Mesh>(".\\Resources\\cube.003.0.fbx");	// �������b�V�� �L���[�u�Ɖ�
		//skinned_mesh = make_unique<Skinned_Mesh>(".\\Resources\\cube.003.1.fbx", Skinned_Mesh::CST_RIGHT_Z, true);	// 3�p�`������Ă��Ȃ��������b�V�� �L���[�u
		SkinnedShader = std::make_unique<ShaderEx>();
		SkinnedShader->Create(L"Shaders\\skinned_mesh_vs", L"Shaders\\skinned_mesh_ps");
}
	return true;
}

void SceneTitle::Update() {
	imguiUpdate();
	const float elapsed_time = FRAMEWORK->GetElapsedTime();

	// �V�[���؂�ւ�
	//if (GetKeyState('G') < 0) {
	//	setScene(std::make_unique<SceneLoading>(std::make_unique<SceneTest_2>()));
	//}
	if (GetAsyncKeyState('G') & 1) setScene(std::make_unique<SceneTest_2>());
	// �J��������
	static float speed = 7.0f;
	if (GetKeyState('D') < 0)  eyePos.x += speed * elapsed_time;	// �E��
	if (GetKeyState('A') < 0)  eyePos.x -= speed * elapsed_time;	// ����
	if (GetKeyState('W') < 0)  eyePos.z += speed * elapsed_time;	// �O��
	if (GetKeyState('S') < 0)  eyePos.z -= speed * elapsed_time;	// ����
	if (GetKeyState(VK_SPACE) < 0)  eyePos.y += speed * elapsed_time;	// ���
	if (GetKeyState(VK_SHIFT) < 0)  eyePos.y -= speed * elapsed_time;	// ����
}

void SceneTitle::Render() {
	HRESULT hr{ S_OK };

	ID3D11DeviceContext* immediate_context = FRAMEWORK->GetDeviceContext();	// DevCon�擾

	FRAMEWORK->Clear(ClearColor);	// ��U�N���A

		// �r���[�|�[�g�̐ݒ�
	FRAMEWORK->CreateViewPort();

	// �T���v���[�X�e�[�g���o�C���h
	sampleClamp->Set(0);

	immediate_context->OMSetBlendState(FRAMEWORK->GetBlendState(FRAMEWORK->BS_ALPHA), nullptr, 0xFFFFFFFF);	// �u�����h�C���^�[�t�F�[�X�̃|�C���^�A�u�����h�t�@�N�^�[�̔z��l�A�T���v���J�o���b�W(����̓f�t�H���g�w��)

	// 2D�I�u�W�F�N�g�̕`��ݒ�
	{
		immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(DS_TRUE), 1);	// 3D�I�u�W�F�N�g�̌��ɏo�����߈�U
		TitleImage->Render(SpriteShader.get());
	}
	// 3D�I�u�W�F�N�g�̕`��ݒ�
	{
		D3D11_VIEWPORT viewport;
		UINT num_viewports{ 1 };
		immediate_context->RSGetViewports(&num_viewports, &viewport);	// ���X�^���C�U�X�e�[�W�Ƀo�C���h���ꂽviewport�̔z����擾

		float aspect_ratio{ viewport.Width / viewport.Height };	// �A�X�y�N�g��
		// �������e�s��̍쐬
		DirectX::XMMATRIX P{DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(30),aspect_ratio,0.1f,100.0f) };	// ����p,�c����,�߂���Z,������Z

		DirectX::XMVECTOR eye{DirectX::XMVectorSet(eyePos.x,eyePos.y,eyePos.z,1.0f) };
		DirectX::XMVECTOR focus;
		if (!focus_zero) {
			focus = {DirectX::XMVectorSet(eyePos.x,eyePos.y,eyePos.z + 1,1.0f) };	// �J�����ʒu�̑O
		}
		else {
			focus = {DirectX::XMVectorSet(0.0f,0.0f,0.0f,1.0f) };
		}
		DirectX::XMVECTOR up{DirectX::XMVectorSet(0.0f,1.0f,0.0f,0.0f) };
		// ViewMatrix�̍쐬(LH = LeftHand(������W�n))
		DirectX::XMMATRIX V{DirectX::XMMatrixLookAtLH(eye, focus, up) };	// �J�������W�A�œ_�A�J�����̏����

		// �R���X�^���g�o�b�t�@�X�V
		scene_constants data{};
		XMStoreFloat4x4(&data.view_projection, V * P);	// Matrix����4x4�֕ϊ�
		data.light_direction = { light_dir[0],light_dir[1],light_dir[2],0 };	// �V�F�[�_�ɓn�����C�g�̌���
		data.camera_position = { eyePos.x,eyePos.y,eyePos.z,0 };				// �V�F�[�_�ɓn���J�����̈ʒu
		immediate_context->UpdateSubresource(constant_buffer[0].Get(), 0, 0, &data, 0, 0);
		immediate_context->VSSetConstantBuffers(1, 1, constant_buffer[0].GetAddressOf());	// cBuffer�̓h���[�R�[���̂��тɏ��������̂œs�x�ݒ肷��K�v������
		immediate_context->PSSetConstantBuffers(1, 1, constant_buffer[0].GetAddressOf());

		immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(DS_TRUE_WRITE), 1);			// 2D�I�u�W�F�N�g�Ƃ̑O��֌W���������肷�邽�ߍĐݒ�

		{
			// 3D�I�u�W�F�N�gRender���ɈڐA ���󂱂��ł���K�v�Ȃ��H
			//XMMATRIX S{ XMMatrixScaling(geometric_primitive[0]->getSize().x,geometric_primitive[0]->getSize().y,geometric_primitive[0]->getSize().z) };				// �g�k
			//XMMATRIX R{ XMMatrixRotationRollPitchYaw(geometric_primitive[0]->getAngle().x,geometric_primitive[0]->getAngle().y,geometric_primitive[0]->getAngle().z) };	// ��]
			//XMMATRIX T{ XMMatrixTranslation(geometric_primitive[0]->getPos().x,geometric_primitive[0]->getPos().y,geometric_primitive[0]->getPos().z) };			// ���s�ړ�
			//XMFLOAT4X4 world;
			//XMStoreFloat4x4(&world, S * R * T);	// ���[���h�ϊ��s��쐬
			grid->Render(true);
			//obj_1->Render(immediate_context.Get());
			//obj_2->Render(immediate_context.Get());
			//static_mesh->Render(immediate_context.Get());
			skinned_mesh->Render(SkinnedShader.get());
		}
	}

#ifdef USE_IMGUI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif
	FRAMEWORK->Flip();
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
	skinned_mesh->imguiWindow("fbx");

	// ���C�g�������O���[�o���ݒ�
	ImGui::Begin("Light");
	ImGui::SliderFloat3("Light_Direction", light_dir, -10.0f, 10.0f);
	ImGui::Checkbox("focus Zero", &focus_zero);
	ImGui::End();
#endif
}