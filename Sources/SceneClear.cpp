#include "SceneClear.h"
#include "SceneTitle.h"

#include "Enemy.h"
#include "Stages.h"

#include "UseComputeShader.h"

#include <SimpleMath.h>
bool SceneClear::Initialize() {
	Microsoft::WRL::ComPtr<ID3D11Device> device = FRAMEWORK->GetDevice();	// framework����device���擾
	HRESULT hr = { S_OK };

	// �V�[���R���X�^���g�o�b�t�@�̐ݒ�
	CreateConstantBuffer(ConstantBuffers[0].GetAddressOf(), sizeof(SceneConstants));

	// Sampler�̐ݒ�
	DefaultSampleClamp = std::make_shared<Sampler>(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);

	// �e��N���X�ݒ�
	{
		camera = std::make_unique<Camera>();

		// sprite�I�u�W�F�N�g�𐶐�(����͐擪�̂P�����𐶐�����)
		Sprites = std::make_unique<Sprite>();
		Sprites->LoadImages(L".\\Resources\\Clear\\Clear.png");
		Sprites->setSize(SCREEN_WIDTH, SCREEN_HEIGHT);


		player = std::make_unique<Player>();
		player->Initialize();

		EnemyManager::getInstance().Initialize();

		StageManager::getInstance().Initialize();
	}


	camera->SetProjection(DirectX::XMConvertToRadians(30), camera->GetWidth() / camera->GetHeight(), camera->GetNear(), camera->GetFar());
	return true;
}

void SceneClear::Update() {
	const float elapsed_time = FRAMEWORK->GetElapsedTime();
	// �V�[���؂�ւ�
	if (GetAsyncKeyState('T') & 1)
	{
		setScene(std::make_unique<SceneTitle>());
	}

	player->Update();
	StageManager::getInstance().Update();

	// �J�����ݒ�
	camera->Set(camera->GetPos(), player->Parameters->Position, DirectX::XMFLOAT3(0, 1, 0));
	// �J��������
	camera->Operate();

	// �G�֘A
	{
		// �������E�N���b�N�Ń����_���ʒu�ɓG�𐶐�
		// TODO �f�o�b�O�p
		if (GetAsyncKeyState(VK_RBUTTON) < 0)
		{
			Object3d desc;
			desc.CopyParam(player->Parameters.get());
			desc.Position = DirectX::SimpleMath::Vector3((rand() % 25) - 12.0f, 0.0f, (rand() % 25) - 12.0f);
			desc.Orientation *= DirectX::SimpleMath::Quaternion::CreateFromAxisAngle({ 0.0f,1.0f,0.0f }, DirectX::XMConvertToRadians(static_cast<float>(rand() % 180)));
			EnemyManager::getInstance().newSet(&desc);
			//EnemyManager::getInstance().newSet(player->Parameters.get());
		}
		for (auto it = EnemyManager::getInstance().getEnemys()->begin(); it != EnemyManager::getInstance().getEnemys()->end(); ++it)
		{
			// ���ׂĂ̓G�Ƀ^�[�Q�b�g�ݒ�
			it->get()->setTarget(*player->Parameters);
		}

		EnemyManager::getInstance().Update();	// �G�X�V
	}

	imguiUpdate();
}

void SceneClear::Render() {
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
		immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(FRAMEWORK->DS_TRUE), 1);		// 3D�I�u�W�F�N�g�̌��ɏo�����߈�U
		Sprites->Render(immediate_context.Get());
		immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(FRAMEWORK->DS_TRUE_WRITE), 1);	// 2D�I�u�W�F�N�g�Ƃ̑O��֌W���������肷�邽�ߍĐݒ�
	}
	// 3D�I�u�W�F�N�g�̕`��ݒ�
	{
		camera->Activate();

		// �R���X�^���g�o�b�t�@�X�V
		SceneConstants data{};
		XMStoreFloat4x4(&data.view_projection, camera->GetView() * camera->GetProjection());	// Matrix����4x4�֕ϊ�
		data.light_direction = DirectX::SimpleMath::Vector4{ light_dir[0],light_dir[1],light_dir[2],0 };	// �V�F�[�_�ɓn�����C�g�̌���
		data.camera_position = DirectX::SimpleMath::Vector4{ camera->GetPos().x,camera->GetPos().y,camera->GetPos().z,0 };				// �V�F�[�_�ɓn���J�����̈ʒu
		immediate_context->UpdateSubresource(ConstantBuffers[0].Get(), 0, 0, &data, 0, 0);
		immediate_context->VSSetConstantBuffers(1, 1, ConstantBuffers[0].GetAddressOf());	// cBuffer�̓h���[�R�[���̂��тɏ��������̂œs�x�ݒ肷��K�v������
		immediate_context->PSSetConstantBuffers(1, 1, ConstantBuffers[0].GetAddressOf());

		{
			StageManager::getInstance().Render(immediate_context.Get());
			player->Render(immediate_context.Get());
			EnemyManager::getInstance().Render(immediate_context.Get());
		}
	}

#ifdef USE_IMGUI
	//	ImGui::Render();
	//	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#else
	FRAMEWORK->Flip();
#endif
}

void SceneClear::imguiUpdate() {
#ifdef USE_IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.0f, 0.6f, 0.1f, 1.0f));	// �����ĂԂ�ImGui::PopStyleColor�������Ȃ��Ƃ����Ȃ��炵��
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.0f, 0.2f, 0.0f, 1.0f));

	//imguiSceneChanger();
	// 2D�p �����֐��Ŋ��������Ă�
	//sprites->ImguiWindow();
	// 3D�p�p�����[�^
	player->ImguiPlayer();
	// ���C�g�������O���[�o���ݒ�
	ImGui::Begin("SceneImGui Object Counts");
	//ImGui::SliderFloat3("Light_Direction", light_dir, -10.0f, 10.0f);
	ImGui::Separator();
	ImGui::Text("PlayerShots: %d", player->getShotManager()->getSize());
	ImGui::Text("EnemyShots: %d", EnemyManager::getInstance().getShotManager()->getSize());
	ImGui::Text("Enemys: %d", EnemyManager::getInstance().getSize());
	ImGui::Text("StageParts: %d", StageManager::getInstance().getSize());
	ImGui::Separator();	// ������
	ImGui::Text("Total Objects: %d", StageManager::getInstance().getSize() + EnemyManager::getInstance().getEnemys()->size()
		+ player->getShotManager()->getSize() + EnemyManager::getInstance().getShotManager()->getSize());
	if (ImGui::Button("Scene Initialize")) { SceneClear::Initialize(); }

	ImGui::PopStyleColor(2);	// ImGui::PushStyleColor��ɂ��������������ۂ�
	ImGui::End();
#endif
}