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

	// �V�[���R���X�^���g�o�b�t�@�̐ݒ�
	CreateConstantBuffer(ConstantBuffers[0].GetAddressOf(), sizeof(SceneConstants));

	// Sampler�̐ݒ�
	DefaultSampleClamp = std::make_shared<Sampler>(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);

	// �e��N���X�ݒ�
	{
		// sprite�I�u�W�F�N�g�𐶐�
		Sprites = std::make_unique<Sprite>();
		Sprites->LoadImages(L".\\Resources\\Loading\\TempImage.jpg");
		Sprites->setSize(SCREEN_WIDTH, SCREEN_HEIGHT);

		// Geometric_primitive�I�u�W�F�N�g�̐���
		{
			Grid = std::make_unique<Geometric_Cube>();
			Grid->setPos(DirectX::SimpleMath::Vector3(0, -1, 0));
			Grid->setSize(DirectX::SimpleMath::Vector3(10, 0.1f, 10));
		}

		player = std::make_unique<Player>();
		player->Initialize();

		EnemyManager::getInstance().Initialize();

		StageManager::getInstance().Initialize();
	}

	Camera::getInstance().SetProjection(DirectX::XMConvertToRadians(30), Camera::getInstance().GetWidth() / Camera::getInstance().GetHeight(), Camera::getInstance().GetNear(), Camera::getInstance().GetFar());

	GpuParticle = std::make_unique<GPUParticle>();
	GpuParticle->Init();

	Skybox = std::make_unique<SkyBox>(L".\\Resources\\SkyBox.png");

	//font = std::make_unique<Font>();
	//font->LoadFont(L".\\Resources\\fonts\\APJapanesefontF.ttf", L"���񂸂����X");
	//font->CreateFontTexture(L"D");

	// �V���h�E�}�b�v
	ShadowMapDrawer= std::make_unique<Sprite>();	// �V���h�E�}�b�v�m�F�p
	for (int i = 0;i < 2;i++) {
		ShadowMapTexture[i] = std::make_unique<Texture>();	// �����_�[�^�[�Q�b�g�p�e�N�X�`��
		ShadowMapTexture[i]->Create(static_cast<UINT>(SHADOW_SIZE), static_cast<UINT>(SHADOW_SIZE), DXGI_FORMAT_R32G32_FLOAT);
	}
	GaussShadowMapTexture = std::make_unique<Texture>();	// �����_�[�^�[�Q�b�g�p�e�N�X�`��
	GaussShadowMapTexture->Create(static_cast<UINT>(SHADOW_SIZE), static_cast<UINT>(SHADOW_SIZE), DXGI_FORMAT_R32G32_FLOAT);

	ShadowDepth = std::make_unique<Texture>();	// �����_�[�^�[�Q�b�g�p�e�N�X�`��
	ShadowDepth->CreateDepth(static_cast<UINT>(SHADOW_SIZE), static_cast<UINT>(SHADOW_SIZE));

	ShadowShader= std::make_unique<ShaderEx>();
	ShadowShader->CreateVS(L"Shaders/DepthShadow_vs");	// �e�N�X�`���ɏ������ރV�F�[�_
	ShadowShader->CreatePS(L"Shaders/DepthShadow_ps");
	RenderShadowShader = std::make_unique<ShaderEx>();
	RenderShadowShader->CreateVS(L"Shaders/RenderShadow_vs");	// �e�N�X�`���ɏ������ރV�F�[�_
	RenderShadowShader->CreatePS(L"Shaders/RenderShadow_ps");
	CreateConstantBuffer(ConstantBuffers[3].GetAddressOf(), sizeof(ShadowConstant));
	GaussianBlur = std::make_unique<ShaderEx>();
	GaussianBlur->CreateVS(L"Shaders/GaussianBlur_vs");
	GaussianBlur->CreatePS(L"Shaders/GaussianBlur_ps");
	CreateConstantBuffer(ConstantBuffers[2].GetAddressOf(), sizeof(GaussianBlurConstants));

	return true;
}

void SceneGame::Update() {
	const float elapsed_time = FRAMEWORK->GetElapsedTime();
	// �V�[���؂�ւ�
	if (GetAsyncKeyState('T') & 1)
	{
		setScene(std::make_unique<SceneLoading>(std::make_unique<SceneTitle>()));
	}
	if (GetAsyncKeyState('C') & 1)
	{
		setScene(std::make_unique<SceneLoading>(std::make_unique<SceneClear>()));
	}

	player->Update();
	StageManager::getInstance().Update();

	// �J�����ݒ�
	Camera::getInstance().Set(Camera::getInstance().GetPos(), player->Parameters->Position, DirectX::XMFLOAT3(0, 1, 0));
	// �J��������
	Camera::getInstance().Operate();
	Camera::getInstance().Activate();

	// �G�֘A
	{
		// �������E�N���b�N�Ń����_���ʒu�ɓG�𐶐�
		// TODO �f�o�b�O�p
		if (GetAsyncKeyState(VK_RBUTTON) < 0)
		{
			Object3d obj3d_desc;
			obj3d_desc.CopyParam(player->Parameters.get());
			obj3d_desc.Position		= DirectX::SimpleMath::Vector3((rand() % 25) - 12.0f, 0.0f, (rand() % 25) - 12.0f);
			obj3d_desc.Orientation *= DirectX::SimpleMath::Quaternion::CreateFromAxisAngle({ 0.0f,1.0f,0.0f }, DirectX::XMConvertToRadians(static_cast<float>(rand() % 180)));
			EnemyManager::getInstance().newSet(&obj3d_desc);
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

void SceneGame::GaussianFilter(DirectX::SimpleMath::Vector4* array, int karnel_size, float sigma)
{
	float sum = 0.0f;	// �S�d�݂̍��v
	int id = 0;
	int x, y;
	// �����̃}�C�i�X����n�߂邱�ƂŒ��S�����xy�I�t�Z�b�g�l���擾���Ă���
	for (y = -karnel_size / 2; y < karnel_size / 2; y++) {
		for (x = -karnel_size / 2; x < karnel_size / 2; x++) {
			array[id].x = static_cast<float>(x);
			array[id].y = static_cast<float>(y);
			// exp:�w���v�Z
			array[id].z = exp(-(x * x + y * y) / (2 * sigma * sigma)) / (2 * PI * sigma * sigma);	// �K�E�X�֐� �ǂ�����Ă������ł��񂪂����炵��
			sum += array[id].z;
			id++;
		}
	}
	// ���K��
	for (int i = 0; i < karnel_size * karnel_size; i++) {
		array[i].z /= sum;
	}
}

void SceneGame::DepthShadowMapping() {
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context = FRAMEWORK->GetDeviceContext();	// DevCon�擾

	// ShadowTextuere���^�[�Q�b�g�ɐݒ�
	ID3D11RenderTargetView* rtv[2] = {ShadowMapTexture[0]->GetRenderTargetView(),ShadowMapTexture[1]->GetRenderTargetView()};
	ID3D11DepthStencilView* dsv = ShadowDepth->GetDepthStencilView();
	// ���X�N���A
	immediate_context->OMSetRenderTargets(1, &rtv[0], dsv);	// �^�����̃e�N�X�`�����^�[�Q�b�g(�o�͐�)��
	float clearColor[4] = { 1.0f,1.0f,1.0f,1.0f };
	immediate_context->ClearRenderTargetView(rtv[0], clearColor);
	immediate_context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// �r���[�|�[�g�̐ݒ�
	FRAMEWORK->CreateViewPort(immediate_context.Get(), ShadowMapTexture[0]->GetWidth(), ShadowMapTexture[0]->GetHeight());

	// 3D�I�u�W�F�N�g�̕`��ݒ�
	{
		// TODO : Debug�p
		// ���C�g���J�������甭��
		if (lightForCamera) {
			light_dir[0] = -(Camera::getInstance().GetTarget().x - Camera::getInstance().GetPos().x);
			light_dir[1] = -(Camera::getInstance().GetTarget().y - Camera::getInstance().GetPos().y);
			light_dir[2] = -(Camera::getInstance().GetTarget().z - Camera::getInstance().GetPos().z);
		}

		// ���C�g����̃r���[���쐬
		Camera shadowView;
		DirectX::SimpleMath::Vector3 shadowPos, shadowTarget, shadowUp;
		// �����Ă�������̔��Α��Ɉʒu��ݒ�
		shadowPos.x = 0.0f + light_dir[0];
		shadowPos.y = 0.0f + light_dir[1] - 10;
		shadowPos.z = 0.0f + light_dir[2];
		shadowTarget = DirectX::SimpleMath::Vector3(0, 0, 0);	// ���_����
		shadowUp = DirectX::SimpleMath::Vector3(0, 1, 0);	// y�������A�b�v
		shadowView.Set(shadowPos, shadowTarget, shadowUp);
		shadowView.SetOrtho(SHADOW_SIZE / ortho, SHADOW_SIZE / ortho, 1.0f, 50.0f);
		shadowView.Activate();	// �s��v�Z
		// �R���X�^���g�o�b�t�@�̒l��ݒ�
		ShadowConstant.light_view_projection = shadowView.GetView() * shadowView.GetProjection();
		ShadowConstant.light_position = DirectX::SimpleMath::Vector4{ shadowPos.x,shadowPos.y,shadowPos.z,0.0f };
		ShadowConstant.shadow_color = DirectX::SimpleMath::Vector4(0.3f, 0.3f, 0.3f, 1.0f);

		// �R���X�^���g�o�b�t�@�X�V
		SceneConstants data{};
		XMStoreFloat4x4(&data.view_projection, ShadowConstant.light_view_projection);	// Matrix����4x4�֕ϊ�
		data.light_direction = DirectX::SimpleMath::Vector4{ light_dir[0],light_dir[1],light_dir[2],0 };	// �V�F�[�_�ɓn�����C�g�̌���
		data.camera_position = DirectX::SimpleMath::Vector4{ shadowView.GetPos().x,shadowView.GetPos().y,shadowView.GetPos().z,0 };				// �V�F�[�_�ɓn���J�����̈ʒu
		immediate_context->UpdateSubresource(ConstantBuffers[0].Get(), 0, 0, &data, 0, 0);
		immediate_context->VSSetConstantBuffers(1, 1, ConstantBuffers[0].GetAddressOf());	// cBuffer�̓h���[�R�[���̂��тɏ��������̂œs�x�ݒ肷��K�v������
		immediate_context->PSSetConstantBuffers(1, 1, ConstantBuffers[0].GetAddressOf());

		immediate_context->OMSetBlendState(FRAMEWORK->GetBlendState(FRAMEWORK->BS_ALPHA), nullptr, 0xFFFFFFFF);
		immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(FRAMEWORK->DS_TRUE_WRITE), 1);
		// �T���v���[�X�e�[�g���o�C���h
		DefaultSampleClamp->Set(immediate_context.Get(), 0);

		{
			// �V���h�E�}�b�v�ɐ[�x�������o�����߂ɃI�u�W�F�N�g��`�悷��
			player->Render(immediate_context.Get(), ShadowShader.get());
			EnemyManager::getInstance().Render(immediate_context.Get(), ShadowShader.get());
			//GpuParticle->Play();
		}

		// �ڂ�������

		// �ڂ���������̃e�N�X�`���̏o�͐�ɐݒ�
		immediate_context->OMSetRenderTargets(1, &rtv[1], nullptr);
		immediate_context->ClearRenderTargetView(rtv[1], clearColor);
		// �r���[�|�[�g�̐ݒ�
		immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(FRAMEWORK->DS_FALSE), 1);

		DirectX::SimpleMath::Vector4 weight[BUFFER_SIZE];
		GaussianFilter(weight, KARNEL_SIZE, 10);	// �T���v�����O�̈ʒu�I�t�Z�b�g�ƃE�F�C�g���v�Z����
		// �R���X�^���g�o�b�t�@�X�V
		GaussianBlurConstants cb{};
		memcpy(cb.weight, weight, sizeof(DirectX::SimpleMath::Vector4) * BUFFER_SIZE);
		cb.karnel_size = KARNEL_SIZE;
		cb.texcel.x = 1.0f / static_cast<float>(ShadowMapTexture[0]->GetWidth());
		cb.texcel.y = 1.0f / static_cast<float>(ShadowMapTexture[0]->GetHeight());
		immediate_context->UpdateSubresource(ConstantBuffers[2].Get(), 0, 0, &cb, 0, 0);
		immediate_context->VSSetConstantBuffers(3, 1, ConstantBuffers[2].GetAddressOf());
		immediate_context->PSSetConstantBuffers(3, 1, ConstantBuffers[2].GetAddressOf());

		DirectX::XMFLOAT2 size = { static_cast<float>(ShadowDepth->GetWidth()), static_cast<float>(ShadowDepth->GetHeight()) };
		ShadowMapDrawer->setTexSize(size.x,size.y);
		ShadowMapDrawer->setPos(0,0);
		ShadowMapDrawer->setSize(size.x,size.y);
		ShadowMapDrawer->Render(immediate_context.Get(), ShadowMapTexture[0].get(), GaussianBlur.get());	// �ڂ��������̎��s�A���ʂ������_�[�^�[�Q�b�g�̃e�N�X�`���ɏo�͂����

		// ���Еt��
		constexpr static ID3D11RenderTargetView* nullView[1] = {nullptr};
		immediate_context->OMSetRenderTargets(0, nullView, nullptr);

		// �V���h�E�R���X�^���g�o�b�t�@�X�V
		immediate_context->UpdateSubresource(ConstantBuffers[3].Get(), 0, 0, &ShadowConstant, 0, 0);
		immediate_context->VSSetConstantBuffers(3, 1, ConstantBuffers[3].GetAddressOf());
		immediate_context->PSSetConstantBuffers(3, 1, ConstantBuffers[3].GetAddressOf());

	}
}

void SceneGame::Render() {

	DepthShadowMapping();	// �V���h�E�}�b�v����

	static const Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context = FRAMEWORK->GetDeviceContext();

	FRAMEWORK->Clear(ClearColor,immediate_context.Get());	// RTV�̍Đݒ�

	// �r���[�|�[�g�̐ݒ�
	FRAMEWORK->CreateViewPort(immediate_context.Get());

	immediate_context->OMSetBlendState(FRAMEWORK->GetBlendState(FRAMEWORK->BS_ALPHA), nullptr, 0xFFFFFFFF);	// �u�����h�C���^�[�t�F�[�X�̃|�C���^�A�u�����h�t�@�N�^�[�̔z��l�A�T���v���J�o���b�W(����̓f�t�H���g�w��)

	// 2D�w�i�I�u�W�F�N�g�̕`��ݒ�
	{
		immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(FRAMEWORK->DS_TRUE), 1);		// 3D�I�u�W�F�N�g�̌��ɏo�����߈�U
		Skybox->Render(&Camera::getInstance());	// �X�J�C�{�b�N�X�`��
		immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(FRAMEWORK->DS_TRUE_WRITE), 1);	// 2D�I�u�W�F�N�g�Ƃ̑O��֌W���������肷�邽�ߍĐݒ�
	}
	// 3D�I�u�W�F�N�g�̕`��ݒ�
	{
		// �V�[���R���X�^���g�o�b�t�@�X�V
		SceneConstants data{};
		XMStoreFloat4x4(&data.view_projection, Camera::getInstance().GetView() * Camera::getInstance().GetProjection());	// Matrix����4x4�֕ϊ�
		data.light_direction = DirectX::SimpleMath::Vector4{ light_dir[0],light_dir[1],light_dir[2],0 };	// �V�F�[�_�ɓn�����C�g�̌���
		data.camera_position = DirectX::SimpleMath::Vector4{ Camera::getInstance().GetPos().x,Camera::getInstance().GetPos().y,Camera::getInstance().GetPos().z,0 };	// �V�F�[�_�ɓn���J�����̈ʒu
		data.view = Camera::getInstance().GetView();
		data.projection = Camera::getInstance().GetProjection();
		immediate_context->UpdateSubresource(ConstantBuffers[0].Get(), 0, 0, &data, 0, 0);
		immediate_context->VSSetConstantBuffers(1, 1, ConstantBuffers[0].GetAddressOf());	// cBuffer�̓h���[�R�[���̂��тɏ��������̂œs�x�ݒ肷��K�v������
		immediate_context->PSSetConstantBuffers(1, 1, ConstantBuffers[0].GetAddressOf());


		// �T���v���[�X�e�[�g���o�C���h
		DefaultSampleClamp->Set(immediate_context.Get(), 0);		// �T���v���[ (s0)
		ShadowMapTexture[1]->Set(immediate_context.Get() ,3);	// �ڂ������V���h�E�}�b�v���Z�b�g	(t3)
		DefaultSampleClamp->Set(immediate_context.Get(),3);		// �V���h�E�}�b�v�p�T���v���[ (s3)
		{
			// 3D�I�u�W�F�N�g�`��
			Grid->Render(true);
			StageManager::getInstance().Render(immediate_context.Get(), RenderShadowShader.get());
			/*if(lightForCamera)*/player->Render(immediate_context.Get(), RenderShadowShader.get());
			EnemyManager::getInstance().Render(immediate_context.Get());
			GpuParticle->Play(immediate_context.Get());

			// 2D�I�u�W�F�N�g�`��
			{
				immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(FRAMEWORK->DS_FALSE), 1);		// 3D�I�u�W�F�N�g�̑O�ɏo������
				immediate_context->OMSetBlendState(FRAMEWORK->GetBlendState(FRAMEWORK->BS_ALPHA), nullptr, 0xFFFFFFFF);	// �u�����h�C���^�[�t�F�[�X�̃|�C���^�A�u�����h�t�@�N�^�[�̔z��l�A�T���v���J�o���b�W(����̓f�t�H���g�w��)
				if (renderShadowMap) {
					ShadowMapDrawer->setTexSize(ShadowDepth->GetWidth(), ShadowDepth->GetHeight());
					DirectX::XMFLOAT2 size = { static_cast<float>(ShadowDepth->GetWidth() / 3), static_cast<float>(ShadowDepth->GetHeight() / 3) };
					ShadowMapDrawer->setPos(0,0);
					ShadowMapDrawer->setSize(size.x*0.5f, size.y*0.5f);
					ShadowMapDrawer->Render(immediate_context.Get(), ShadowMapTexture[shadowNo].get());
				}
			}
		}
		ShadowMapTexture[1]->Set(immediate_context.Get(), 3, false);	// 3�ԃX���b�g�̃e�N�X�`���ݒ�𔍂���
		ShadowDepth->Set(immediate_context.Get(), 0, false);			// 0�ԃX���b�g�̃e�N�X�`���ݒ�𔍂���
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

	// 3D�p�p�����[�^
	player->ImguiPlayer();
	GpuParticle->ImguiParticles();
	// ���C�g�������O���[�o���ݒ�
	ImGui::Begin("SceneImGui");
	{
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
		if (ImGui::CollapsingHeader("Initializes"))
		{
			if (ImGui::Button("Stage Initialize")) { StageManager::getInstance().Initialize(); }
			if (ImGui::Button("Player Initialize")) { player->Initialize(); }
			if (ImGui::Button("Scene Initialize")) { Initialize(); }
			if (ImGui::Button("Particle Set")) { GpuParticle->SetParticle(); }
		}
		ImGui::PopStyleColor(2);	// ImGui::PushStyleColor��ɂ��������������ۂ�
	}
	ImGui::End();

	ImGui::Begin("LightDirection");
	{
		ImGui::SliderFloat3("Light_Direction", light_dir, -50.0f, 50.0f);
		ImGui::Checkbox("LightFromCamera", &lightForCamera);
		if (ImGui::TreeNode("RenderMap"))
		{
			ImGui::Checkbox("ShadowMap", &renderShadowMap);
			if (ImGui::RadioButton("Depth", shadowNo == 0)) { shadowNo = 0; };ImGui::SameLine();
			if (ImGui::RadioButton("Gaussian", shadowNo == 1)) { shadowNo = 1; };
			ImGui::SliderFloat("Ortho", &ortho, 0.0f, 500.0f);
			ImGui::TreePop();
		}
	}
	ImGui::End();

#endif
}