#include "GPUParticle.h"
#include "framework.h"
#include "UseComputeShader.h"


bool GPUParticle::Init()
{
	Microsoft::WRL::ComPtr<ID3D11Device> device = FRAMEWORK->GetDevice();

	// Compute Shader�Z�b�e�B���O
	{
		// �V�[���R���X�^���g�o�b�t�@�̍쐬�ƌ^�̐ݒ�
		CreateConstantBuffer(ConstantBuffer.GetAddressOf(), sizeof(scene_constants));
		// �I�u�W�F�N�g�R���X�^���g�o�b�t�@�̍쐬
		CreateConstantBuffer(obj_ConstantBuffer.GetAddressOf(), sizeof(obj_constants));
		// CS�R���X�^���g�o�b�t�@�̍쐬�ƌ^�̐ݒ�
		CreateConstantBuffer(cs_ConstantBuffer.GetAddressOf(), sizeof(cs_constants));


		// �V�F�[�_�̍쐬
		ParticleShader = std::make_unique<ShaderEx>();
		ParticleShader->CreateCS(L"Shaders\\GPUPaticleCS");	// �t�@�C���w��
		ParticleShader->CreateVS(L"Shaders\\GPUParticle_vs");
		ParticleShader->CreatePS(L"Shaders\\GPUParticle_ps");

		// ���͗p�o�b�t�@�[�ɏ����l��ݒ肷��
		for (int i = 0; i < NUM_ELEMENTS; i++)
		{
			vBufInArray[i].Position = DirectX::SimpleMath::Vector3{ 0.0f, 0.0f, 0.0f };
		}

		// �R���s���[�g�V�F�[�_�[�ւ̓��͎��Ɏg�p����SRV���쐬����
		UseComputeShader::CreateSRVForStructuredBuffer(sizeof(BUFIN_TYPE), NUM_ELEMENTS, &vBufInArray[0], pBufInput.GetAddressOf(), pBufInputSRV.GetAddressOf());

		// �R���s���[�g�V�F�[�_�[����̏o�͎��Ɏg�p����UAV���쐬����
		UseComputeShader::CreateUAVForStructuredBuffer(sizeof(BUFOUT_TYPE), NUM_ELEMENTS, NULL, pBufResult.GetAddressOf(), pBufResultUAV.GetAddressOf());
	}
	obj = std::make_unique<Skinned_Mesh>(".\\Resources\\Shots\\Shot.fbx");
	sprite[0] = std::make_unique<sprite_Batch>(L".\\Resources\\particle.png",2048);
	texture = std::make_unique<Texture>();
	texture->Load(L".\\Resources\\particle.png");
	return true;
}

void GPUParticle::Update()
{
	// �R���s���[�g�V�F�[�_�[�����s����
	{
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context = FRAMEWORK->GetDeviceContext();
		Microsoft::WRL::ComPtr<ID3D11Device> device = FRAMEWORK->GetDevice();
		HRESULT hr = { S_OK };

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		static float theta = 0.0f;
		theta = (theta <= 1.0f) ? theta + 0.01f : 0.0f;	// �`�J�`�J�����`�I(�F��)
#if 0
		D3D11_MAPPED_SUBRESOURCE subRes;	// �ʂ̍X�V���@ �̂͂��B������
		immediate_context->Map(pBufInput.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subRes);
		BUFIN_TYPE* pBufType = (BUFIN_TYPE*)subRes.pData;
		if (GetAsyncKeyState(VK_SPACE) & 1) { pBufType->Position.x += 1.0f; }	// TODO : �����߂�
https://cafe.kiite.jp/pc		//memcpy(�R�s�[��, pBufType, sizeof(BUFIN_TYPE) * NUM_ELEMENTS);	// ���͑O�̃f�[�^�ێ��H
		immediate_context->Unmap(pBufInput.Get(), 0);
#else
		// �V�[���R���X�^���g�o�b�t�@�X�V
		immediate_context->PSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
		immediate_context->VSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
		// �I�u�W�F�N�g�R���X�^���g�o�b�t�@�X�V
		{
			DirectX::SimpleMath::Matrix world;
			DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(sprite[0]->getSize().x,sprite[0]->getSize().y,1.0f) };	// �g�k
			DirectX::XMMATRIX R = DirectX::XMMatrixRotationAxis({ 0.0f,0.0f,1.0f }, sprite[0]->getAngle());
			DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(sprite[0]->getPos().x,sprite[0]->getPos().y,0.0f) };	// ���s�ړ�

			XMStoreFloat4x4(&world, S * R * T);	// ���[���h�ϊ��s��쐬
			obj_constants objData{};
			objData.world = world;

			immediate_context->UpdateSubresource(obj_ConstantBuffer.Get(), 0, 0, &objData, 0, 0);
			immediate_context->PSSetConstantBuffers(1, 1, obj_ConstantBuffer.GetAddressOf());
			immediate_context->VSSetConstantBuffers(1, 1, obj_ConstantBuffer.GetAddressOf());

		}// CS�R���X�^���g�o�b�t�@�X�V
		static cs_constants csData{};
		if (GetAsyncKeyState(VK_SPACE) & 1)
		{
			csData.Size.x += 1.0f;
			csData.Size.y += 1.0f;
			csData.Size.z += 1.0f;
		}	// TODO : �����߂�
		if (GetAsyncKeyState(VK_SHIFT) & 1)
		{
			csData.Size.x -= 1.0f;
			csData.Size.y -= 1.0f;
			csData.Size.z -= 1.0f;
		}	// TODO : �����߂�
		immediate_context->UpdateSubresource(cs_ConstantBuffer.Get(), 0, 0, &csData, 0, 0);
		immediate_context->CSSetConstantBuffers(3, 1, cs_ConstantBuffer.GetAddressOf());
#endif
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ���낢��ݒ�ADispatch�����w�肵��CS�����s
		UseComputeShader::RunComputeShader(ParticleShader->GetCS(), pBufInputSRV.Get(), 1, pBufResultUAV.Get(), 1, 1, 1, 1);

		// �A���I�[�_�[�h�A�N�Z�X�r���[�̃o�b�t�@�̓��e�� CPU ����ǂݍ��݉\�ȃo�b�t�@�փR�s�[����
		UseComputeShader::CreateAndCopyToBuffer(pBufResult.Get(), ReadBackBuffer.GetAddressOf());

		D3D11_MAPPED_SUBRESOURCE MappedResource = { 0 };
		hr = immediate_context->Map(ReadBackBuffer.Get(), 0, D3D11_MAP_READ, 0, &MappedResource);	// �ǂݎ���p�Ń}�b�v
		{	// �}�b�s���O���̏���
			BUFOUT_TYPE* p;	// �󂯎��^�̕ϐ���p�ӂ���
			// "p,�z��v�f��"�ƃE�H�b�`���ɓ��͂���ƒl������� ����֗�
			p = (BUFOUT_TYPE*)MappedResource.pData;	// �^�w�肵�đ��
			obj->setScale(p->Size);
		}
		immediate_context->Unmap(ReadBackBuffer.Get(), 0);	// �}�b�v����
		ReadBackBuffer.Reset();	// ��� ���������[�N����̂���

	}
}

void GPUParticle::Draw()
{
	obj->Render();
	{
		sprite[0]->begin();
		for (int i = 0; i < 1092; i++)
		{
			sprite[0]->setPos((i * sprite[0]->getTexSize().x), (i - ((i / sprite[0]->getTexSize().y))));
			sprite[0]->Render();
		}
		sprite[0]->end(ParticleShader.get());
	}
	//ParticleShader->Activate();
	//texture->Set(0);
	//FRAMEWORK->GetDeviceContext()->Draw(4, 0);
	//ParticleShader->Inactivate();

}

void GPUParticle::SetSceneConstantBuffer(const ID3D11Buffer* cbBuf)
{
	memcpy(ConstantBuffer.Get(), cbBuf, sizeof(ID3D11Buffer));
}

void GPUParticle::CreateConstantBuffer(ID3D11Buffer** dstBuf, size_t size)
{
	Microsoft::WRL::ComPtr<ID3D11Device> device = FRAMEWORK->GetDevice();

	D3D11_BUFFER_DESC buffer_desc{};
	ZeroMemory(&buffer_desc, sizeof(D3D11_BUFFER_DESC));	// ��U������
	buffer_desc.ByteWidth = size;	// �^�w��
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	HRESULT hr = device->CreateBuffer(&buffer_desc, nullptr, dstBuf);	// �o�b�t�@�쐬
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

}
