#include "SceneTest_2.h"
#include "SceneTitle.h"

#include "Enemy.h"
#include "Stages.h"

#include "UseConputeShader.h"

#include <SimpleMath.h>
bool SceneTest_2::Initialize() {
	Microsoft::WRL::ComPtr<ID3D11Device> device = FRAMEWORK->GetDevice();	// framework����device���擾
	HRESULT hr = { S_OK };

	// �V�[���R���X�^���g�o�b�t�@�̐ݒ�
	CreateConstantBuffer(constant_buffer[0].GetAddressOf(), sizeof(scene_constants));

	// Sampler�̐ݒ�
	sampleClamp = std::make_shared<Sampler>(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);

	// �e��N���X�ݒ�
	{
		camera = std::make_unique<Camera>();

		// sprite�I�u�W�F�N�g�𐶐�(����͐擪�̂P�����𐶐�����)
		sprites = std::make_unique<Sprite>(L".\\Resources\\screenshot.jpg");	// �V�F�[�_�[�̓R���X�g���N�^���Ŏw�肵�Ă��邽�߁A�ʂ��g���ɂ͉��ǂ��K�v
		sprites->setSize(1280, 720);
		SpriteShader = std::make_unique<ShaderEx>();
		SpriteShader->Create(L"Shaders\\sprite_vs", L"Shaders\\sprite_ps");

		// Geometric_primitive�I�u�W�F�N�g�̐���
		{
			grid = std::make_unique<Geometric_Cube>();
			grid->setPos(DirectX::SimpleMath::Vector3(0, -1, 0));
			grid->setSize(DirectX::SimpleMath::Vector3(10, 0.1f, 10));
		}

		player = std::make_unique<Player>();
		player->Initialize();

		EnemyManager::getInstance().Initialize();

		StageManager::getInstance().Initialize();

		SkinnedShader = std::make_unique<ShaderEx>();
		SkinnedShader->Create(L"Shaders\\skinned_mesh_vs", L"Shaders\\skinned_mesh_ps");
	}

	// Compute Shader�Z�b�e�B���O
	{
		// CS�p�R���X�^���g�o�b�t�@�̐ݒ�
		CreateConstantBuffer(constant_buffer[1].GetAddressOf(), sizeof(cs_constants));

		ComputeShader = std::make_unique<ShaderEx>();
		ComputeShader->Create(L"Shaders\\ComputeShader_cs");

		// ���͗p�o�b�t�@�[�ɏ����l��ݒ肷��
		for (int i = 0; i < NUM_ELEMENTS; i++)
		{
			vBufInArray[i].i = i;
			vBufInArray[i].f = static_cast<float>(NUM_ELEMENTS - 1 - i);
		}

		// �R���s���[�g�V�F�[�_�[�ւ̓��͎��Ɏg�p����SRV���쐬����
		UseConputeShader::CreateSRVForStructuredBuffer(sizeof(BUFIN_TYPE), NUM_ELEMENTS, &vBufInArray[0], pBufInput.GetAddressOf(), pBufInputSRV.GetAddressOf());

		// �R���s���[�g�V�F�[�_�[����̏o�͎��Ɏg�p����UAV���쐬����
		UseConputeShader::CreateUAVForStructuredBuffer(sizeof(BUFOUT_TYPE), NUM_ELEMENTS, NULL, pBufResult.GetAddressOf(), pBufResultUAV.GetAddressOf());
	}

	camera->SetProjection(DirectX::XMConvertToRadians(30), camera->GetWidth() / camera->GetHeight(), camera->GetNear(), camera->GetFar());

	return true;
}

void SceneTest_2::Update() {
	const float elapsed_time = FRAMEWORK->GetElapsedTime();
	// �V�[���؂�ւ�
	if (GetAsyncKeyState('G') & 1) setScene(std::make_unique<SceneTitle>());

	player->Update();
	StageManager::getInstance().Update();


	// �������e�s��̍쐬
	//camera->Set((camera->GetPos() + player->Parameters->Position), player->Parameters->Position, DirectX::XMFLOAT3(0, 1, 0));
	camera->Set(camera->GetPos(), player->Parameters->Position, DirectX::XMFLOAT3(0, 1, 0));
	//DirectX::SimpleMath::Vector3 pos = camera->GetPos();
	//pos.y = 0.0f;
	//camera->SetPos(pos);

		// �J��������
	camera->Operate();

	if (GetAsyncKeyState(VK_RBUTTON) &1) {
		EnemyManager::getInstance().newSet(player->Parameters.get());	// �������E�N���b�N�œG�𐶐�
	}
	{
		for (auto it = EnemyManager::getInstance().getEnemys()->begin(); it != EnemyManager::getInstance().getEnemys()->end(); ++it)
		{
			// �G�̕`��
			it->get()->setTarget(*player->Parameters);
		}

	}
	EnemyManager::getInstance().Update();

	{
		// �R���s���[�g�V�F�[�_�[�����s����
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context = FRAMEWORK->GetDeviceContext();
		Microsoft::WRL::ComPtr<ID3D11Device> device = FRAMEWORK->GetDevice();
		HRESULT hr = { S_OK };

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		static float theta = 0.0f;
		theta = (theta <= 1.0f) ? theta + 0.01f : 0.0f;	// �`�J�`�J�����`�I(�F��)
		//D3D11_MAPPED_SUBRESOURCE subRes;	// �ʂ̍X�V���@ �̂͂��B������
		//immediate_context->Map(pBufInput.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subRes);
		//BUFIN_TYPE* pBufType = (BUFIN_TYPE*)subRes.pData;
		//pBufType->f += 5;
		////memcpy(subRes.pData, vBufInArray, sizeof(BUFIN_TYPE) * NUM_ELEMENTS);
		//immediate_context->Unmap(pBufInput.Get(), 0);

		// �R���X�^���g�o�b�t�@�X�V
		cs_constants csData{};
		csData.Theta = theta;
		immediate_context->UpdateSubresource(constant_buffer[1].Get(), 0, 0, &csData, 0, 0);
		immediate_context->CSSetConstantBuffers(2, 1, constant_buffer[1].GetAddressOf());
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		UseConputeShader::RunComputeShader(ComputeShader->GetCS(), pBufInputSRV.Get(), pBufResultUAV.Get(), 3, 1, 1);

		// �A���I�[�_�[�h�A�N�Z�X�r���[�̃o�b�t�@�̓��e�� CPU ����ǂݍ��݉\�ȃo�b�t�@�փR�s�[����
		ID3D11Buffer* debugbuf = UseConputeShader::CreateAndCopyToBuffer(device.Get(), immediate_context.Get(), pBufResult.Get());

		D3D11_MAPPED_SUBRESOURCE MappedResource = { 0 };
		hr = immediate_context->Map(debugbuf, 0, D3D11_MAP_READ, 0, &MappedResource);	// �ǂݎ���p�Ń}�b�v
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		BUFOUT_TYPE* p;
		ZeroMemory(&p, sizeof(BUFOUT_TYPE));
		p = reinterpret_cast<BUFOUT_TYPE*>(MappedResource.pData);	// �^�ϊ����đ��
		immediate_context->Unmap(debugbuf, 0);	// �}�b�v����
		debugbuf->Release();	// CS�󂯎��|�C���^�����

		player->Parameters->Color = DirectX::SimpleMath::Vector4{ p[1].i, p[0].i, p[2].i, 1.0f };
	}

	imguiUpdate();
}

void SceneTest_2::Render() {
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
		immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(DS_TRUE), 1);		// 3D�I�u�W�F�N�g�̌��ɏo�����߈�U
		sprites->Render(SpriteShader.get());
		immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(DS_TRUE_WRITE), 1);	// 2D�I�u�W�F�N�g�Ƃ̑O��֌W���������肷�邽�ߍĐݒ�
	}
	// 3D�I�u�W�F�N�g�̕`��ݒ�
	{
		camera->Activate();

		// �R���X�^���g�o�b�t�@�X�V
		scene_constants data{};
		XMStoreFloat4x4(&data.view_projection, camera->GetView() * camera->GetProjection());	// Matrix����4x4�֕ϊ�
		data.light_direction = DirectX::SimpleMath::Vector4{ light_dir[0],light_dir[1],light_dir[2],0 };	// �V�F�[�_�ɓn�����C�g�̌���
		data.camera_position = DirectX::SimpleMath::Vector4{ camera->GetPos().x,camera->GetPos().y,camera->GetPos().z,0 };				// �V�F�[�_�ɓn���J�����̈ʒu
		immediate_context->UpdateSubresource(constant_buffer[0].Get(), 0, 0, &data, 0, 0);
		immediate_context->VSSetConstantBuffers(1, 1, constant_buffer[0].GetAddressOf());	// cBuffer�̓h���[�R�[���̂��тɏ��������̂œs�x�ݒ肷��K�v������
		immediate_context->PSSetConstantBuffers(1, 1, constant_buffer[0].GetAddressOf());

		{
			grid->Render(true);
			StageManager::getInstance().Render();
			player->Render();
			EnemyManager::getInstance().Render();
		}
	}

#ifdef USE_IMGUI
	//	ImGui::Render();
	//	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#else
	FRAMEWORK->Flip();
#endif
}

void SceneTest_2::imguiUpdate() {
#ifdef USE_IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.0f, 0.6f, 0.1f, 1.0f));	// �����ĂԂ�ImGui::PopStyleColor�������Ȃ��Ƃ����Ȃ��炵��
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.0f, 0.2f, 0.0f, 1.0f));

	//imguiSceneChanger();
	// 2D�p �����֐��Ŋ��������Ă�
	sprites->ImguiWindow();
	// 3D�p�p�����[�^
	player->ImguiPlayer();
	// ���C�g�������O���[�o���ݒ�
	ImGui::Begin("SceneImGui");
	ImGui::SliderFloat3("Light_Direction", light_dir, -10.0f, 10.0f);
	ImGui::Separator();
	ImGui::Text("PlayerShots: %d", player->getShotManager()->getSize());
	ImGui::Text("EnemyShots: %d", EnemyManager::getInstance().getShotManager()->getSize());
	ImGui::Text("Enemys: %d", EnemyManager::getInstance().getSize());
	ImGui::Text("StageParts: %d", StageManager::getInstance().getSize());
	ImGui::Separator();	// ������
	ImGui::Text("Total Objects: %d", StageManager::getInstance().getSize() + EnemyManager::getInstance().getEnemys()->size()
										+ player->getShotManager()->getSize() + EnemyManager::getInstance().getShotManager()->getSize());

	ImGui::PopStyleColor(2);	// ImGui::PushStyleColor��ɂ��������������ۂ�
	ImGui::End();
#endif
}