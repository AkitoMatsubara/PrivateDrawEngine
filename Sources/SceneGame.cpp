#include "SceneGame.h"
#include "SceneTitle.h"

#include "Enemy.h"
#include "Stages.h"

#include "UseComputeShader.h"

#include <SimpleMath.h>

#include "SceneClear.h"
#include "SceneLoading.h"

bool SceneGame::Initialize() {
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
		sprites = std::make_unique<Sprite>();
		sprites->LoadImages(L".\\Resources\\screenshot.jpg");
		sprites->setSize(SCREEN_WIDTH, SCREEN_HEIGHT);

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
	}

	// Compute Shader�Z�b�e�B���O
	{
		// CS�p�R���X�^���g�o�b�t�@�̐ݒ�
		CreateConstantBuffer(constant_buffer[1].GetAddressOf(), sizeof(cs_constants));

		ComputeShader = std::make_unique<ShaderEx>();
		ComputeShader->CreateCS(L"Shaders\\ComputeShader_cs");

		// ���͗p�o�b�t�@�[�ɏ����l��ݒ肷��
		for (int i = 0; i < NUM_ELEMENTS; i++)
		{
			vBufInArray[i].i = i;
			vBufInArray[i].f = static_cast<float>(NUM_ELEMENTS - 1 - i);
		}

		// �R���s���[�g�V�F�[�_�[�ւ̓��͎��Ɏg�p����SRV���쐬����
		UseComputeShader::CreateStructuredBufferAndSRV(sizeof(BUFIN_TYPE), NUM_ELEMENTS, &vBufInArray[0], pBufInput.GetAddressOf(), pBufInputSRV.GetAddressOf());

		// �R���s���[�g�V�F�[�_�[����̏o�͎��Ɏg�p����UAV���쐬����
		UseComputeShader::CreateStructuredBufferAndUAV(sizeof(BUFOUT_TYPE), NUM_ELEMENTS, NULL, pBufResult.GetAddressOf(), pBufResultUAV.GetAddressOf());
	}

	camera->SetProjection(DirectX::XMConvertToRadians(30), camera->GetWidth() / camera->GetHeight(), camera->GetNear(), camera->GetFar());

	GpuParticle = std::make_unique<GPUParticle>();
	GpuParticle->Init();

	skybox = std::make_unique<SkyBox>(L".\\Resources\\SkyBox.png");

	test = std::make_unique<Font>();
	test->LoadFont(L".\\Resources\\fonts\\APJapanesefontF.ttf", L"���񂸂����X");
	test->CreateFontTexture(L"D");
	return true;
}

void SceneGame::Update() {
	const float elapsed_time = FRAMEWORK->GetElapsedTime();
	// �V�[���؂�ւ�
	if (GetAsyncKeyState('L') & 1)
	{
		setScene(std::make_unique<SceneLoading>(std::make_unique<SceneTitle>()));
	}
	//if (GetAsyncKeyState('C') & 1)
	//{
	//	setScene(std::make_unique<SceneLoading>(std::make_unique<SceneClear>()));
	//}

	player->Update();
	StageManager::getInstance().Update();

	// �J�����ݒ�
	camera->Set(camera->GetPos(), player->Parameters->Position, DirectX::XMFLOAT3(0, 1, 0));
	// �J��������
	camera->Operate();

	GpuParticle->Update(camera.get());

	// �G�֘A
	{
		// �������E�N���b�N�Ń����_���ʒu�ɓG�𐶐�
		// TODO �f�o�b�O�p
		if (GetAsyncKeyState(VK_RBUTTON) < 0)
		{
			Object3d obj3d_desc;
			obj3d_desc.CopyParam(player->Parameters.get());
			obj3d_desc.Position = DirectX::SimpleMath::Vector3((rand() % 25) - 12.0f, 0.0f, (rand() % 25) - 12.0f);
			obj3d_desc.Orientation *= DirectX::SimpleMath::Quaternion::CreateFromAxisAngle({ 0.0f,1.0f,0.0f }, DirectX::XMConvertToRadians(static_cast<float>(rand() % 180)));
			EnemyManager::getInstance().newSet(&obj3d_desc);
			//EnemyManager::getInstance().newSet(player->Parameters.get());
		}
		for (auto it = EnemyManager::getInstance().getEnemys()->begin(); it != EnemyManager::getInstance().getEnemys()->end(); ++it)
		{
			// ���ׂĂ̓G�Ƀ^�[�Q�b�g�ݒ�
			it->get()->setTarget(*player->Parameters);
		}

		EnemyManager::getInstance().Update();	// �G�X�V
	}

	// �R���s���[�g�V�F�[�_�[�����s����
	{
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
		UseComputeShader::RunComputeShader(ComputeShader->GetCS(), pBufInputSRV.Get(), 0, pBufResultUAV.Get(), 0, 3, 1, 1);

		// �A���I�[�_�[�h�A�N�Z�X�r���[�̃o�b�t�@�̓��e�� CPU ����ǂݍ��݉\�ȃo�b�t�@�փR�s�[����
		ID3D11Buffer* debugbuf = nullptr;
		UseComputeShader::CreateAndCopyToBuffer(pBufResult.Get(), &debugbuf);

		D3D11_MAPPED_SUBRESOURCE MappedResource = { 0 };
		hr = immediate_context->Map(debugbuf, 0, D3D11_MAP_READ, 0, &MappedResource);	// �ǂݎ���p�Ń}�b�v
		{
			BUFOUT_TYPE* p;	// �󂯎��^�̕ϐ���p�ӂ���
			// "p,�z��v�f��"�ƃE�H�b�`���ɓ��͂���ƒl������� ����֗�
			p = (BUFOUT_TYPE*)MappedResource.pData;	// �^�ϊ����đ��
			player->Parameters->Color = DirectX::SimpleMath::Vector4{ p[1].i, p[0].i, p[2].i, 1.0f };
		}
		immediate_context->Unmap(debugbuf, 0);	// �}�b�v����
		debugbuf->Release();	// CS�󂯎��|�C���^�����
	}

	imguiUpdate();
}

void SceneGame::Render() {
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
		immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(FRAMEWORK->DS_TRUE), 1);		// 3D�I�u�W�F�N�g�̌��ɏo�����߈�U
		sprites->Render();
		skybox->Render(camera.get());
		test->Render();
		immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(FRAMEWORK->DS_TRUE_WRITE), 1);	// 2D�I�u�W�F�N�g�Ƃ̑O��֌W���������肷�邽�ߍĐݒ�
	}
	// 3D�I�u�W�F�N�g�̕`��ݒ�
	{
		camera->Activate();

		// �R���X�^���g�o�b�t�@�X�V
		scene_constants data{};
		XMStoreFloat4x4(&data.view_projection, camera->GetView() * camera->GetProjection());	// Matrix����4x4�֕ϊ�
		data.light_direction = DirectX::SimpleMath::Vector4{ light_dir[0],light_dir[1],light_dir[2],0 };	// �V�F�[�_�ɓn�����C�g�̌���
		data.camera_position = DirectX::SimpleMath::Vector4{ camera->GetPos().x,camera->GetPos().y,camera->GetPos().z,0 };				// �V�F�[�_�ɓn���J�����̈ʒu
		data.View = camera->GetView();
		data.Projection = camera->GetProjection();
		data.ParticleSize = DirectX::SimpleMath::Vector2{ 1.0f,1.0f };
		immediate_context->UpdateSubresource(constant_buffer[0].Get(), 0, 0, &data, 0, 0);
		immediate_context->VSSetConstantBuffers(1, 1, constant_buffer[0].GetAddressOf());	// cBuffer�̓h���[�R�[���̂��тɏ��������̂œs�x�ݒ肷��K�v������
		immediate_context->PSSetConstantBuffers(1, 1, constant_buffer[0].GetAddressOf());

		{
			grid->Render(true);
			//StageManager::getInstance().Render();
			//player->Render();
			//EnemyManager::getInstance().Render();
			GpuParticle->SetSceneConstantBuffer(constant_buffer[0].Get());
			GpuParticle->Draw();
		}
	}

#ifdef USE_IMGUI
	//	ImGui::Render();
	//	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#else
	FRAMEWORK->Flip();
#endif
}

void SceneGame::imguiUpdate() {
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
	GpuParticle->ImguiParticles();
	// ���C�g�������O���[�o���ݒ�
	ImGui::Begin("SceneImGui");
	{
		//ImGui::SliderFloat3("Light_Direction", light_dir, -10.0f, 10.0f);
		if (ImGui::CollapsingHeader("Object Counts"))
		{
			ImGui::Text("PlayerShots: %d", player->getShotManager()->getSize());
			ImGui::Text("EnemyShots: %d", EnemyManager::getInstance().getShotManager()->getSize());
			ImGui::Text("Enemys: %d", EnemyManager::getInstance().getSize());
			ImGui::Text("StageParts: %d", StageManager::getInstance().getSize());
			ImGui::Separator();	// ������
			ImGui::Text("Total Objects: %d", StageManager::getInstance().getSize() + EnemyManager::getInstance().getEnemys()->size()
				+ player->getShotManager()->getSize() + EnemyManager::getInstance().getShotManager()->getSize());
		}
		if (ImGui::CollapsingHeader("Initializes"),true)
		{
			if (ImGui::Button("Stage Initialize")) { StageManager::getInstance().Initialize(); }
			if (ImGui::Button("Player Initialize")) { player->Initialize(); }
			if (ImGui::Button("Scene Initialize")) { Initialize(); }
			//if (ImGui::Button("Particle Initialize")) { GpuParticle->Init(); }	// TODO ���������[�N �z�肵�Ă��Ȃ��g�����Ȃ̂ł��܂�C�ɂ��Ȃ��Ă��c�H
			if (ImGui::Button("Particle Set")) { GpuParticle->SetParticle(); }
		}
		ImGui::PopStyleColor(2);	// ImGui::PushStyleColor��ɂ��������������ۂ�
		ImGui::End();
	}
#endif
}