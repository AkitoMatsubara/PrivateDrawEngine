#include "SceneTest_2.h"
#include "SceneTitle.h"
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

		stage = std::make_unique<Stage>();
		stage->Initialize();

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
		CreateSRVForStructuredBuffer(sizeof(BUFIN_TYPE), NUM_ELEMENTS, &vBufInArray[0], pBufInput.GetAddressOf(), pBufInputSRV.GetAddressOf());

		// �R���s���[�g�V�F�[�_�[����̏o�͎��Ɏg�p����UAV���쐬����
		CreateUAVForStructuredBuffer(sizeof(BUFOUT_TYPE), NUM_ELEMENTS, NULL, pBufResult.GetAddressOf(), pBufResultUAV.GetAddressOf());
	}

	return true;
}

void SceneTest_2::Update() {
	imguiUpdate();
	const float elapsed_time = FRAMEWORK->GetElapsedTime();
	// �V�[���؂�ւ�
	if (GetAsyncKeyState('G') & 1) setScene(std::make_unique<SceneTitle>());

	player->Update();
	stage->Update();

	// �J��������
	camera->Operate();

	// �������e�s��̍쐬
	camera->Set((camera->GetPos() + player->Parameters->Position), player->Parameters->Position, DirectX::XMFLOAT3(0, 1, 0));
	camera->SetProjection(DirectX::XMConvertToRadians(30), camera->GetWidth() / camera->GetHeight(), camera->GetNear(), camera->GetFar());

	if (GetAsyncKeyState(VK_RBUTTON) &1) {
		EnemyManager::getInstance().newSet(player->Parameters.get());	// �������E�N���b�N�œG�𐶐�
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
		RunComputeShader(ComputeShader->GetCS(), pBufInputSRV.Get(), pBufResultUAV.Get(), 3, 1, 1);

		// �A���I�[�_�[�h�A�N�Z�X�r���[�̃o�b�t�@�̓��e�� CPU ����ǂݍ��݉\�ȃo�b�t�@�փR�s�[����
		ID3D11Buffer* debugbuf = CreateAndCopyToBuffer(device.Get(), immediate_context.Get(), pBufResult.Get());

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
			stage->Render();
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

	imguiSceneChanger();
	// 2D�p �����֐��Ŋ��������Ă�
	sprites->ImguiWindow();
	// 3D�p�p�����[�^
	player->ImguiPlayer();
	// ���C�g�������O���[�o���ݒ�
	ImGui::Begin("Light");
	ImGui::SliderFloat3("Light_Direction", light_dir, -10.0f, 10.0f);
	ImGui::Checkbox("focus Zero", &focus_zero);
	ImGui::Text("Enemys: %d", EnemyManager::getInstance().getEnemys()->size());
	ImGui::Text("Total Objects: %d", EnemyManager::getInstance().getEnemys()->size() + player->getShotManager()->getSize());

	ImGui::PopStyleColor(2);	// ImGui::PushStyleColor��ɂ���Ăяo�����ۂ�

	ImGui::End();
#endif
}

// �\�����o�b�t�@���쐬����
HRESULT SceneTest_2::CreateStructuredBuffer(UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBuf)
{
	ID3D11Device* pD3DDevice = FRAMEWORK->GetDevice();
	HRESULT hr = E_FAIL;

	// �\�����o�b�t�@�[���쐬����

	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	BufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS |			// �A���I�[�_�[�h �A�N�Z�X ���\�[�X���o�C���h����
		D3D11_BIND_SHADER_RESOURCE;				// �o�b�t�@�[���V�F�[�_�[ �X�e�[�W�Ƀo�C���h����
	BufferDesc.ByteWidth = uElementSize * uCount;					// �o�b�t�@�T�C�Y
	BufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;	// �\�����o�b�t�@�[�Ƃ��ă��\�[�X���쐬����
	BufferDesc.StructureByteStride = uElementSize;					// �\�����o�b�t�@�[�̃T�C�Y (�o�C�g�P��)

	// �����l��ݒ�
	if (pInitData)
	{
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = pInitData;
		hr = pD3DDevice->CreateBuffer(&BufferDesc, &InitData, ppBuf);
	}
	// �����l�Ȃ��ŗ̈�̂݊m�ۂ���
	else
	{
		hr = pD3DDevice->CreateBuffer(&BufferDesc, NULL, ppBuf);
	}
	return hr;
}

// �R���s���[�g�V�F�[�_�[�ւ̓��͎��Ɏg�p����V�F�[�_�[���\�[�X�r���[���쐬����
HRESULT SceneTest_2::CreateSRVForStructuredBuffer(UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBuf, ID3D11ShaderResourceView** ppSRVOut) {
	ID3D11Device* pD3DDevice = FRAMEWORK->GetDevice();
	HRESULT hr = E_FAIL;
	*ppBuf = NULL;
	*ppSRVOut = NULL;

	// �\�����o�b�t�@�[���쐬����
	hr = CreateStructuredBuffer(uElementSize, uCount, pInitData, ppBuf);
	if (FAILED(hr))
	{
		goto EXIT;
	}

	// �\�����o�b�t�@�[����V�F�[�_�[���\�[�X�r���[���쐬����

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;   // �g�����ꂽ�o�b�t�@�[�ł��邱�Ƃ��w�肷��
	SRVDesc.BufferEx.FirstElement = 0;
	SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	SRVDesc.BufferEx.NumElements = uCount;                  // ���\�[�X���̗v�f�̐�

	// �\�����o�b�t�@�[�����ƂɃV�F�[�_�[���\�[�X�r���[���쐬����
	hr = pD3DDevice->CreateShaderResourceView(*ppBuf, &SRVDesc, ppSRVOut);
	if (FAILED(hr))
		goto EXIT;

	hr = S_OK;
EXIT:
	return hr;
}

// �R���s���[�g�V�F�[�_�[����̏o�͎��Ɏg�p����A���I�[�_�[�h�A�N�Z�X�r���[���쐬����
HRESULT SceneTest_2::CreateUAVForStructuredBuffer(UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBuf, ID3D11UnorderedAccessView** ppUAVOut) {
	ID3D11Device* pD3DDevice = FRAMEWORK->GetDevice();
	HRESULT hr = E_FAIL;
	*ppBuf = NULL;
	*ppUAVOut = NULL;

	// �\�����o�b�t�@�[���쐬����

	hr = CreateStructuredBuffer(uElementSize, uCount, pInitData, ppBuf);
	if (FAILED(hr))
	{
		goto EXIT;
	}

	// �\�����o�b�t�@�[����A���I�[�_�[�h�A�N�Z�X�r���[���쐬����
	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
	ZeroMemory(&UAVDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;   // �o�b�t�@�[�ł��邱�Ƃ��w�肷��
	UAVDesc.Buffer.FirstElement = 0;
	UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	UAVDesc.Buffer.NumElements = uCount;                  // ���\�[�X���̗v�f�̐�

	// �\�����o�b�t�@�[�����ƂɃA���I�[�_�[�h �A�N�Z�X �r���[���쐬����
	hr = pD3DDevice->CreateUnorderedAccessView(*ppBuf, &UAVDesc, ppUAVOut);
	if (FAILED(hr))
		goto EXIT;

	hr = S_OK;
EXIT:
	return hr;
}

// �A���I�[�_�[�h�A�N�Z�X�r���[�̃o�b�t�@�̓��e��CPU����ǂݍ��݉\�ȃo�b�t�@���쐬���ăR�s�[����
ID3D11Buffer* SceneTest_2::CreateAndCopyToBuffer(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DDeviceContext, ID3D11Buffer* pBuffer)
{
	ID3D11Buffer* debugbuf = NULL;

	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	pBuffer->GetDesc(&BufferDesc);
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;  // CPU ����ǂݍ��݂ł���悤�ɐݒ肷��
	BufferDesc.Usage = D3D11_USAGE_STAGING;             // GPU ���� CPU �ւ̃f�[�^�]�� (�R�s�[) ���T�|�[�g���郊�\�[�X
	BufferDesc.BindFlags = 0;
	BufferDesc.MiscFlags = 0;
	if (FAILED(pD3DDevice->CreateBuffer(&BufferDesc, NULL, &debugbuf))) return debugbuf;

	pD3DDeviceContext->CopyResource(debugbuf, pBuffer);

	return debugbuf;
}

void SceneTest_2::RunComputeShader(ID3D11ComputeShader* pComputeShader, ID3D11ShaderResourceView* pSrcSRV, ID3D11UnorderedAccessView* pDstUAV, UINT X, UINT Y, UINT Z) {
	ID3D11DeviceContext* immediate_context = FRAMEWORK->GetDeviceContext();

	immediate_context->CSSetShader(pComputeShader, NULL, 0);

	// �V�F�[�_�[���\�[�X�r���[���R���s���[�g�V�F�[�_�[�ɐݒ�
	immediate_context->CSSetShaderResources(0, 1, &pSrcSRV);

	// �A���I�[�_�[�h�A�N�Z�X�r���[���R���s���[�g�V�F�[�_�[�ɐݒ�
	immediate_context->CSSetUnorderedAccessViews(0, 1, &pDstUAV, NULL);

	// �R���s���[�g�V�F�[�_�[�����s����B�X���b�h���Ƃ��悤�킩���
	immediate_context->Dispatch(X, Y, Z);

	// �R���s���[�g�V�F�[�_�̐ݒ����
	immediate_context->CSSetShader(NULL, NULL, 0);

	ID3D11UnorderedAccessView* ppUAViewNULL[1] = { NULL };
	immediate_context->CSSetUnorderedAccessViews(0, 1, ppUAViewNULL, NULL);

	ID3D11ShaderResourceView* ppSRVNULL[2] = { NULL, NULL };
	immediate_context->CSSetShaderResources(0, 2, ppSRVNULL);

	ID3D11Buffer* ppCBNULL[1] = { NULL };
	immediate_context->CSSetConstantBuffers(0, 1, ppCBNULL);
}