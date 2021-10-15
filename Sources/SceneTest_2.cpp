#include "SceneTest_2.h"
#include "SceneTitle.h"

bool SceneTest_2::Initialize() {
	ComPtr<ID3D11Device> device = FRAMEWORK->GetDevice();	// framework����device���擾
	HRESULT hr = { S_OK };

	// �V�[���R���X�^���g�o�b�t�@�̐ݒ�
	{
		D3D11_BUFFER_DESC buffer_desc{};
		buffer_desc.ByteWidth = sizeof(scene_constants);
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer[0].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	// Sampler�̐ݒ�
	sampleClamp = std::make_shared<Sampler>(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);

	// �e��N���X�ݒ�
	{
		// sprite�I�u�W�F�N�g�𐶐�(����͐擪�̂P�����𐶐�����)
		sprites = make_unique<Sprite>(L".\\Resources\\screenshot.jpg");	// �V�F�[�_�[�̓R���X�g���N�^���Ŏw�肵�Ă��邽�߁A�ʂ��g���ɂ͉��ǂ��K�v
		sprites->setSize(1280, 720);
		SpriteShader = std::make_unique<ShaderEx>();
		SpriteShader->Create(L"Shaders\\sprite_vs", L"Shaders\\sprite_ps");

		// Geometric_primitive�I�u�W�F�N�g�̐���
		{
			grid = make_unique<Geometric_Cube>();
			grid->setPos(XMFLOAT3(0, -1, 0));
			grid->setSize(XMFLOAT3(10, 0.1f, 10));
			GeomtricShader = std::make_unique<ShaderEx>();
			GeomtricShader->Create(L"Shaders\\geometric_primitive_vs", L"Shaders\\geometric_primitive_ps");
		}

		//skinned_mesh = make_unique<Skinned_Mesh>(device.Get(), ".\\Resources\\cube.000.fbx");		// �e�N�X�`���A�}�e���A������
		//skinned_mesh = make_unique<Skinned_Mesh>(device.Get(), ".\\Resources\\cube.001.0.fbx");	// �e�N�X�`���g�p
		//skinned_mesh = make_unique<Skinned_Mesh>(device.Get(), ".\\Resources\\cube.001.1.fbx");	// ���ߍ��݃e�N�X�`��
		//skinned_mesh = make_unique<Skinned_Mesh>(device.Get(), ".\\Resources\\cube.002.0.fbx");	// 3��e�N�X�`���g�p
		//skinned_mesh = make_unique<Skinned_Mesh>(device.Get(), ".\\Resources\\cube.002.1.fbx");	// �e�N�X�`���L�薳���A�}�e���A���L�薳������
		skinned_mesh = make_unique<Skinned_Mesh>(".\\Resources\\cube.003.0.fbx");	// �������b�V�� �L���[�u�Ɖ�
		//skinned_mesh = make_unique<Skinned_Mesh>(".\\Resources\\cube.003.1.fbx", Skinned_Mesh::CST_RIGHT_Z, true);	// 3�p�`������Ă��Ȃ��������b�V�� �L���[�u

		player = make_unique<Player>();
		player->Initialize();

		SkinnedShader = std::make_unique<ShaderEx>();
		SkinnedShader->Create(L"Shaders\\skinned_mesh_vs", L"Shaders\\skinned_mesh_ps");
	}

	// Compute Shader�Z�b�e�B���O
	{
		// �V�[���R���X�^���g�o�b�t�@�̐ݒ�
		{

			D3D11_BUFFER_DESC buffer_desc{};
			buffer_desc.ByteWidth = sizeof(cs_constants);
			buffer_desc.Usage = D3D11_USAGE_DEFAULT;
			buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			buffer_desc.CPUAccessFlags = 0;
			buffer_desc.MiscFlags = 0;
			buffer_desc.StructureByteStride = 0;
			hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer[1].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

		ComputeShader = make_unique<ShaderEx>();
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
	if (GetAsyncKeyState('G') < 0) setScene(std::make_unique<SceneTitle>());

	player->Update();

	// �J��������
	static float cameraSpeed = 7.0f;
	if (GetKeyState(VK_RIGHT) < 0)  eyePos.x += cameraSpeed * elapsed_time;	// �E��
	if (GetKeyState(VK_LEFT) < 0)  eyePos.x -= cameraSpeed * elapsed_time;	// ����
	if (GetKeyState(VK_UP) < 0)  eyePos.z += cameraSpeed * elapsed_time;	// �O��
	if (GetKeyState(VK_DOWN) < 0)  eyePos.z -= cameraSpeed * elapsed_time;	// ���
	if (GetKeyState(VK_SPACE) < 0)  eyePos.y += cameraSpeed * elapsed_time;	// ���
	if (GetKeyState(VK_SHIFT) < 0)  eyePos.y -= cameraSpeed * elapsed_time;	// ����

	{
		//// �R���s���[�g�V�F�[�_�[�����s����
		//RunComputeShader(pD3DDeviceContext, pComputeShader, pBufSRV, pBufResultUAV, NUM_ELEMENTS / 2, 1, 1);
		ComPtr<ID3D11DeviceContext> immediate_context= FRAMEWORK->GetDeviceContext();
		ComPtr<ID3D11Device> device = FRAMEWORK->GetDevice();
		HRESULT hr = { S_OK };

		immediate_context->CSSetShader(ComputeShader->GetCS(), NULL, 0);
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		static float theta = 0;
		theta = (theta <= 1.0f) ? theta + 0.001 : 0.0f;
		//D3D11_MAPPED_SUBRESOURCE subRes;
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


		// SRV��CS�ɐݒ�
		immediate_context->CSSetShaderResources(0, 1, pBufInputSRV.GetAddressOf());
		// UAV��CS�ɐݒ�
		immediate_context->CSSetUnorderedAccessViews(0, 1, pBufResultUAV.GetAddressOf(),NULL);
		// CS�̎��s
		immediate_context->Dispatch(/*NUM_ELEMENTS / 2*/3, 1, 1);
		// CS�̐ݒ����
		immediate_context->CSSetShader(NULL, NULL, 0);

		ID3D11UnorderedAccessView* ppUAViewNULL[1] = { NULL };
		immediate_context->CSSetUnorderedAccessViews(0, 1, ppUAViewNULL, NULL);

		ID3D11ShaderResourceView* ppSRVNULL[2] = { NULL, NULL };
		immediate_context->CSSetShaderResources(0, 2, ppSRVNULL);

		ID3D11Buffer* ppCBNULL[1] = { NULL };
		immediate_context->CSSetConstantBuffers(0, 1, ppCBNULL);


		// �A���I�[�_�[�h�A�N�Z�X�r���[�̃o�b�t�@�̓��e�� CPU ����ǂݍ��݉\�ȃo�b�t�@�փR�s�[����
		ID3D11Buffer* debugbuf = CreateAndCopyToBuffer(device.Get(), immediate_context.Get(), pBufResult.Get());

		D3D11_MAPPED_SUBRESOURCE MappedResource = { 0 };
		hr = immediate_context->Map(debugbuf, 0, D3D11_MAP_READ, 0, &MappedResource);	// �ǂݎ���p�Ń}�b�v
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		BUFOUT_TYPE* p;
		ZeroMemory(&p, sizeof(BUFOUT_TYPE));
		p = reinterpret_cast<BUFOUT_TYPE*>(MappedResource.pData);	// �^�ϊ����đ��
		immediate_context->Unmap(debugbuf, 0);	// �}�b�v����

		// �R���s���[�g�V�F�[�_�[�̌��ʂ��o��
		for (int i = 0; i < NUM_ELEMENTS; i++)
		{
			printf("%d + %d = %d\n", i, NUM_ELEMENTS - 1 - i, p[i]);
		}

		skinned_mesh->setColor(p[0].i,p[1].i,p[2].i,1.0f);

		hr = S_OK;
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
		immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(DS_TRUE), 1);	// 3D�I�u�W�F�N�g�̌��ɏo�����߈�U
		sprites->Render(SpriteShader.get());
	}
	// 3D�I�u�W�F�N�g�̕`��ݒ�
	{
		D3D11_VIEWPORT viewport;
		UINT num_viewports{ 1 };
		immediate_context->RSGetViewports(&num_viewports, &viewport);	// ���X�^���C�U�X�e�[�W�Ƀo�C���h���ꂽviewport�̔z����擾

		float aspect_ratio{ viewport.Width / viewport.Height };	// �A�X�y�N�g��
		// �������e�s��̍쐬
		XMMATRIX P{ XMMatrixPerspectiveFovLH(XMConvertToRadians(30),aspect_ratio,0.1f,100.0f) };	// ����p,�c����,�߂���Z,������Z

		XMVECTOR eye{ XMVectorSet(eyePos.x,eyePos.y,eyePos.z,1.0f) };
		XMVECTOR focus;
		if (!focus_zero) {
			//focus = { XMVectorSet(eyePos.x,eyePos.y,eyePos.z + 1,1.0f) };	// �J�����ʒu�̑O
			focus = { XMVectorSet(skinned_mesh->getPos().x,skinned_mesh->getPos().y,skinned_mesh->getPos().z,1.0f) };	// �J�����ʒu�̑O
		}
		else {
			focus = { XMVectorSet(0.0f,0.0f,0.0f,1.0f) };
		}
		XMVECTOR up{ XMVectorSet(0.0f,1.0f,0.0f,0.0f) };
		// ViewMatrix�̍쐬(LH = LeftHand(������W�n))
		XMMATRIX V{ XMMatrixLookAtLH(eye, focus, up) };	// �J�������W�A�œ_�A�J�����̏����

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
			grid->wireframe = true;
			grid->Render(GeomtricShader.get());
			//skinned_mesh->Render(SkinnedShader.get());
			player->Render();
		}
	}

#ifdef USE_IMGUI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif
	FRAMEWORK->Flip();
}

// �R���s���[�g�V�F�[�_�[�ւ̓��͎��Ɏg�p����V�F�[�_�[���\�[�X�r���[���쐬����
HRESULT SceneTest_2::CreateSRVForStructuredBuffer(UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBuf, ID3D11ShaderResourceView** ppSRVOut) {
	ID3D11Device* pD3DDevice = FRAMEWORK->GetDevice();
	HRESULT hr = E_FAIL;
	*ppBuf = NULL;
	*ppSRVOut = NULL;

	// �\�����o�b�t�@�[���쐬����

	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	BufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS |		// �A���I�[�_�[�h �A�N�Z�X ���\�[�X���o�C���h����
		D3D11_BIND_SHADER_RESOURCE;								// �o�b�t�@�[���V�F�[�_�[ �X�e�[�W�Ƀo�C���h����
	BufferDesc.ByteWidth = uElementSize * uCount;					// �o�b�t�@�T�C�Y
	BufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;	// �\�����o�b�t�@�[�Ƃ��ă��\�[�X���쐬����
	BufferDesc.StructureByteStride = uElementSize;					// �\�����o�b�t�@�[�̃T�C�Y (�o�C�g�P��)

	// �����l��ݒ�
	if (pInitData)
	{
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = pInitData;
		hr = pD3DDevice->CreateBuffer(&BufferDesc, &InitData, ppBuf);
		if (FAILED(hr))
			goto EXIT;
	}
	// �����l�Ȃ��ŗ̈�̂݊m�ۂ���
	else
	{
		hr = pD3DDevice->CreateBuffer(&BufferDesc, NULL, ppBuf);
		if (FAILED(hr))
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

	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	BufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS |          // �A���I�[�_�[�h �A�N�Z�X ���\�[�X���o�C���h����
		D3D11_BIND_SHADER_RESOURCE;            // �o�b�t�@�[���V�F�[�_�[ �X�e�[�W�Ƀo�C���h����
	BufferDesc.ByteWidth = uElementSize * uCount;                  // �o�b�t�@�T�C�Y
	BufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;  // �\�����o�b�t�@�[�Ƃ��ă��\�[�X���쐬����
	BufferDesc.StructureByteStride = uElementSize;                 // �\�����o�b�t�@�[�̃T�C�Y (�o�C�g�P��)

	// �����l��ݒ�
	if (pInitData)
	{
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = pInitData;
		hr = pD3DDevice->CreateBuffer(&BufferDesc, &InitData, ppBuf);
		if (FAILED(hr))
			goto EXIT;
	}
	// �����l�Ȃ��ŗ̈�̂݊m�ۂ���
	else
	{
		hr = pD3DDevice->CreateBuffer(&BufferDesc, NULL, ppBuf);
		if (FAILED(hr))
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

void SceneTest_2::imguiUpdate() {
#ifdef USE_IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	imguiSceneChanger();
	// 2D�p �����֐��Ŋ��������Ă�
	sprites->ImguiWindow();
	// 3D�p�p�����[�^
	skinned_mesh->imguiWindow("fbx");

	// ���C�g�������O���[�o���ݒ�
	ImGui::Begin("Light");
	ImGui::SliderFloat3("Light_Direction", light_dir, -10.0f, 10.0f);
	ImGui::Checkbox("focus Zero", &focus_zero);
	ImGui::End();
#endif
}