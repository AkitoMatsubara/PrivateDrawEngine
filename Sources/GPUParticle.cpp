#include "GPUParticle.h"
#include "framework.h"
#include "UseComputeShader.h"


bool GPUParticle::Init()
{
	Microsoft::WRL::ComPtr<ID3D11Device> device = FRAMEWORK->GetDevice();

	{
		//�R���s���[�g�v�Z�̃��[�N���Z�b�g

		DispathNo = 10;  /*�f�B�X�p�b�`��*/
		PerticleAmount = 2000;        // �p�[�e�B�N���̐�
		chainA = 0;
		chainB = 1;
		HRESULT hr = { S_OK };

		// �`��p���\�[�X�E�r���[
		{
			// ���_�o�b�t�@�̒�`�@�i�������݂ɂ��Ή��j
			D3D11_BUFFER_DESC vBufferDesc;
			vBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			vBufferDesc.ByteWidth = sizeof(VBuffer) * PerticleAmount;
			vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // ���_�o�b�t�@�ƃX�g���[���o�͂Ƃ��Ďg��
			vBufferDesc.CPUAccessFlags = 0;
			vBufferDesc.MiscFlags = 0;
			vBufferDesc.StructureByteStride = 0;

			// ���_�o�b�t�@�̃T�u���\�[�X�̏����l(���_���W)
			struct VBuffer* vBuf = new VBuffer[PerticleAmount];

			// ���_�o�b�t�@�̃T�u���\�[�X�̒�`(�o�b�t�@�[���\�[�X�ł���������Ă�j
			D3D11_SUBRESOURCE_DATA vSubData;//�������p�f�[�^���쐬
			vSubData.pSysMem = vBuf;  // �o�b�t�@�E�f�[�^�̏����l
			vSubData.SysMemPitch = 0;
			vSubData.SysMemSlicePitch = 0;

			// �`��p���_���̓o�b�t�@�̍쐬�@�@�������p�f�[�^�𑗂��Ă���
			device->CreateBuffer(&vBufferDesc, &vSubData, VerticesBuffer.GetAddressOf());
			//delete[] vBuf;

			// ���I�Ȓ萔�o�b�t�@�̒�`
			CreateConstantBuffer(DynamicCBuffer.GetAddressOf(), sizeof(cbCBuffer), true);

			//�������p�f�[�^
			//struct VBuffer* vBuf = new VBuffer[PerticleAmount];
			float fx = 0;
			float fy = 0;
			float fz = 0;
			float theta = 0;
			float x = 0;
			float y = 0;
			for (int i = 0; i < PerticleAmount; ++i) {

				fx = ((i % DispathNo) - (DispathNo / 2.0f))/* / (float)DispathNo * 0.005f*/;
				fz = ((i / DispathNo) - (DispathNo / 2.0f))/* / (float)DispathNo * 0.005f*/;
				fy = 0.1f;
				vBuf[i].Position = DirectX::XMFLOAT3(fx, fy, fz);


				theta = 0.1f;
 				x = fx * cosf(theta) - fy * sinf(theta);
				y = fx * sinf(theta) + fy * cosf(theta);
				//x = fx * cosf(theta) - fy * sinf(theta) + (((float)rand() / RAND_MAX) - 0.5)*20;
				//y = fx * sinf(theta) + fy * cosf(theta) + (((float)rand() / RAND_MAX) - 0.5)*20;
				vBuf[i].Velocity = DirectX::XMFLOAT3(x / 10000.0f, y / 10000.0f, 0);//���x
				vBuf[i].Force = DirectX::XMFLOAT3(0, 0, 0);//�����x
			}

			// ���\�[�X�̐ݒ�
			// �ŏ��̓��̓��\�[�X
			UseComputeShader::CreateStructuredBuffer(sizeof(VBuffer), PerticleAmount, vBuf, InBuffer.GetAddressOf());
			// SRV�̍쐬
			UseComputeShader::CreateSRVfromStructuredBuffer(InBuffer.Get(), g_pSRV[0].GetAddressOf());
			// UAV�̍쐬
			UseComputeShader::CreateUAVfromStructuredBuffer(InBuffer.Get(), g_pUAV[0].GetAddressOf());


			// �ŏ��̏o�̓��\�[�X
			UseComputeShader::CreateStructuredBuffer(sizeof(VBuffer), PerticleAmount, nullptr, OutBuffer.GetAddressOf());
			// SRV�̍쐬
			UseComputeShader::CreateSRVfromStructuredBuffer(OutBuffer.Get(), g_pSRV[1].GetAddressOf());
			// UAV�̍쐬
			UseComputeShader::CreateUAVfromStructuredBuffer(OutBuffer.Get(), g_pUAV[1].GetAddressOf());

			delete[] vBuf;

			// CPU�ւ̃f�[�^�������ݗp�o�b�t�@��CPU�]���pUAV�̐ݒ�
			UseComputeShader::CreateStructuredBufferAndUAV(sizeof(ReturnBuffer), 1, nullptr, ToCpuBuffer.GetAddressOf(), ToCpuUAV.GetAddressOf());


			// ���[�h�o�b�N�p�o�b�t�@ ���\�[�X�̍쐬
			UseComputeShader::CreateAndCopyToBuffer(ToCpuBuffer.Get(), CPUReadBackBuffer.GetAddressOf());
		}
		//�A���t�@�u�����h�p�u�����h�X�e�[�g�쐬
		//png�t�@�C�����ɃA���t�@��񂪂���B�A���t�@�ɂ�蓧�߂���悤�w�肵�Ă���
		D3D11_BLEND_DESC blend_desc;
		ZeroMemory(&blend_desc, sizeof(D3D11_BLEND_DESC));
		blend_desc.IndependentBlendEnable = false;
		blend_desc.AlphaToCoverageEnable = false;
		blend_desc.RenderTarget[0].BlendEnable = true;
		blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;


		//hr = device->CreateBlendState(&bd, &pBlendState.p);
		hr = device->CreateBlendState(&blend_desc, bd_states.GetAddressOf());



	}
	// Compute Shader�Z�b�e�B���O
	{
		// �V�[���R���X�^���g�o�b�t�@�̍쐬�ƌ^�̐ݒ�
		CreateConstantBuffer(ConstantBuffer.GetAddressOf(), sizeof(scene_constants));
		// �I�u�W�F�N�g�R���X�^���g�o�b�t�@�̍쐬
		CreateConstantBuffer(obj_ConstantBuffer.GetAddressOf(), sizeof(obj_constants));
		// CS�R���X�^���g�o�b�t�@�̍쐬�ƌ^�̐ݒ�
		CreateConstantBuffer(cs_ConstantBuffer.GetAddressOf(), sizeof(cs_constants));
		// Sampler�̐ݒ�
		sample = std::make_shared<Sampler>(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);

		//�`��p�V�F�[�_
		// ���͗v�f
		static constexpr UINT IL_NUM = 3;
		D3D11_INPUT_ELEMENT_DESC layout[IL_NUM] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "FORCE",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// �V�F�[�_�̍쐬
		ParticleShader = std::make_unique<ShaderEx>();
		ParticleShader->CreateCS(L"Shaders\\GPUParticle_cs");	// �t�@�C���w��
		ParticleShader->CreateVS(L"Shaders\\GPUParticle_vs", IL_NUM, layout);
		ParticleShader->CreateGS(L"Shaders\\GPUParticle_gs");
		ParticleShader->CreatePS(L"Shaders\\GPUParticle_ps");

		// ���͗p�o�b�t�@�[�ɏ����l��ݒ肷��
		for (int i = 0; i < NUM_ELEMENTS; i++)
		{
			vBufInArray[i].Position = DirectX::SimpleMath::Vector3{ 0.0f, 0.0f, 0.0f };
		}

		// �R���s���[�g�V�F�[�_�[�ւ̓��͎��Ɏg�p����SRV���쐬����
		UseComputeShader::CreateStructuredBufferAndSRV(sizeof(BUFIN_TYPE), NUM_ELEMENTS, &vBufInArray[0], pBufInput.GetAddressOf(), pBufInputSRV.GetAddressOf());

		// �R���s���[�g�V�F�[�_�[����̏o�͎��Ɏg�p����UAV���쐬����
		UseComputeShader::CreateStructuredBufferAndUAV(sizeof(BUFOUT_TYPE), NUM_ELEMENTS, NULL, pBufResult.GetAddressOf(), pBufResultUAV.GetAddressOf());
	}
	texture = std::make_shared<Texture>();
	texture->Load(L".\\Resources\\particle.png");
	return true;
}

void GPUParticle::Update()
{
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context = FRAMEWORK->GetDeviceContext();
	Microsoft::WRL::ComPtr<ID3D11Device> device = FRAMEWORK->GetDevice();
	HRESULT hr = { S_OK };
	//	// �R���s���[�g�V�F�[�_�[�����s����
	//	{
	//
	//		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//		static float theta = 0.0f;
	//		theta = (theta <= 1.0f) ? theta + 0.01f : 0.0f;	// �`�J�`�J�����`�I(�F��)
	//#if 0
	//		D3D11_MAPPED_SUBRESOURCE subRes;	// �ʂ̍X�V���@ �̂͂��B������
	//		immediate_context->Map(pBufInput.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subRes);
	//		BUFIN_TYPE* pBufType = (BUFIN_TYPE*)subRes.pData;
	//		if (GetAsyncKeyState(VK_SPACE) & 1) { pBufType->Position.x += 1.0f; }	// TODO : �����߂�
	//		//memcpy(�R�s�[��, pBufType, sizeof(BUFIN_TYPE) * NUM_ELEMENTS);	// ���͑O�̃f�[�^�ێ��H
	//		immediate_context->Unmap(pBufInput.Get(), 0);
	//#else
	//		// �V�[���R���X�^���g�o�b�t�@�X�V
	//		immediate_context->PSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
	//		immediate_context->VSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
	//		// �I�u�W�F�N�g�R���X�^���g�o�b�t�@�X�V
	//		{
	//			DirectX::SimpleMath::Matrix world;
	//			DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(sprite[0]->getSize().x,sprite[0]->getSize().y,1.0f) };	// �g�k
	//			DirectX::XMMATRIX R = DirectX::XMMatrixRotationAxis({ 0.0f,0.0f,1.0f }, sprite[0]->getAngle());
	//			DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(sprite[0]->getPos().x,sprite[0]->getPos().y,0.0f) };	// ���s�ړ�
	//
	//			XMStoreFloat4x4(&world, S * R * T);	// ���[���h�ϊ��s��쐬
	//			obj_constants objData{};
	//			objData.world = world;
	//
	//			immediate_context->UpdateSubresource(obj_ConstantBuffer.Get(), 0, 0, &objData, 0, 0);
	//			immediate_context->PSSetConstantBuffers(1, 1, obj_ConstantBuffer.GetAddressOf());
	//			immediate_context->VSSetConstantBuffers(1, 1, obj_ConstantBuffer.GetAddressOf());
	//
	//		}// CS�R���X�^���g�o�b�t�@�X�V
	//		static cs_constants csData{};
	//		//if (GetAsyncKeyState(VK_SPACE) & 1)
	//		//{
	//		//	csData.Size.x += 1.0f;
	//		//	csData.Size.y += 1.0f;
	//		//	csData.Size.z += 1.0f;
	//		//}	// TODO : �����߂�
	//		//if (GetAsyncKeyState(VK_SHIFT) & 1)
	//		//{
	//		//	csData.Size.x -= 1.0f;
	//		//	csData.Size.y -= 1.0f;
	//		//	csData.Size.z -= 1.0f;
	//		//}	// TODO : �����߂�
	//		immediate_context->UpdateSubresource(cs_ConstantBuffer.Get(), 0, 0, &csData, 0, 0);
	//		immediate_context->CSSetConstantBuffers(3, 1, cs_ConstantBuffer.GetAddressOf());
	//#endif
	//		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//
	//		// ���낢��ݒ�ADispatch�����w�肵��CS�����s
	//		UseComputeShader::RunComputeShader(ParticleShader->GetCS(), pBufInputSRV.Get(), 1, pBufResultUAV.Get(), 1, 1, 1, 1);
	//
	//		// �A���I�[�_�[�h�A�N�Z�X�r���[�̃o�b�t�@�̓��e�� CPU ����ǂݍ��݉\�ȃo�b�t�@�փR�s�[����
	//		UseComputeShader::CreateAndCopyToBuffer(pBufResult.Get(), ReadBackBuffer.GetAddressOf());
	//
	//		D3D11_MAPPED_SUBRESOURCE MappedResource = { 0 };
	//		hr = immediate_context->Map(ReadBackBuffer.Get(), 0, D3D11_MAP_READ, 0, &MappedResource);	// �ǂݎ���p�Ń}�b�v
	//		{	// �}�b�s���O���̏���
	//			BUFOUT_TYPE* p;	// �󂯎��^�̕ϐ���p�ӂ���
	//			// "p,�z��v�f��"�ƃE�H�b�`���ɓ��͂���ƒl������� ����֗�
	//			p = (BUFOUT_TYPE*)MappedResource.pData;	// �^�w�肵�đ��
	//			obj->setScale(p->Size);
	//		}
	//		immediate_context->Unmap(ReadBackBuffer.Get(), 0);	// �}�b�v����
	//		ReadBackBuffer.Reset();	// ��� ���������[�N����̂���
	//
	//	}

	// �T���v���R�s�[-------------------------------------
	// **********************************************************
	// �R���s���[�g�E�V�F�[�_���g�������Z
	// **********************************************************
	// �V�F�[�_��ݒ�
	g_cbCBuffer.No = DispathNo;

	// ***************************************
	// �萔�o�b�t�@�̃}�b�v�擾

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	immediate_context->Map(DynamicCBuffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	 // ���I�R���X�^���g�o�b�t�@�̒l�ɃR�s�[����
	CopyMemory(MappedResource.pData, &g_cbCBuffer, sizeof(cbCBuffer));
	// �}�b�v����
	immediate_context->Unmap(DynamicCBuffer.Get(), NULL);

	// PS�ɒ萔�o�b�t�@��ݒ�
	immediate_context->CSSetConstantBuffers(0, 1, DynamicCBuffer.GetAddressOf());
	immediate_context->CSSetShader(ParticleShader->GetCS(), 0, 0);

	//// ���݂�SRV������
	//ID3D11ShaderResourceView* SRVNULL = NULL;
	//immediate_context->CSSetShaderResources(0, 1, &SRVNULL);
	// SRV�̐ݒ�
	immediate_context->CSSetShaderResources(0, 1, g_pSRV[chainA].GetAddressOf());

	// UAV�̐ݒ�
	immediate_context->CSSetUnorderedAccessViews(0, 1, g_pUAV[chainB].GetAddressOf(), 0);
	immediate_context->CSSetUnorderedAccessViews(1, 1, ToCpuUAV.GetAddressOf(), 0);
	// CS�̎��s
	immediate_context->Dispatch(DispathNo, 1, 1);

	immediate_context->CopyResource(VerticesBuffer.Get(), InBuffer.Get());
	immediate_context->CopyResource(CPUReadBackBuffer.Get(), ToCpuBuffer.Get());

	// ���ʂ�CPU����ǂݍ���
	hr = immediate_context->Map(CPUReadBackBuffer.Get(), 0, D3D11_MAP_READ, 0, &MappedResource);
	ReturnBuffer* pt = (ReturnBuffer*)MappedResource.pData;
	// ���o�����f�[�^���R�s�[
	memcpy(&CpuGpuBuffer, pt, sizeof(ReturnBuffer));
	// �}�b�v����
	immediate_context->Unmap(CPUReadBackBuffer.Get(), 0);

	chainA = chainA ? 0 : 1; // �o�b�t�@�[�̐؂�ւ�
	chainB = chainB ? 0 : 1; // �o�b�t�@�[�̐؂�ւ�
	//----------------------------------------------------
}

void GPUParticle::Draw(Camera* camera)
{
	//ParticleShader->Activate();
	//texture->Set(0);
	//FRAMEWORK->GetDeviceContext()->Draw(4, 0);
	//ParticleShader->Inactivate();

	//----------------------------------------------------
	ID3D11DeviceContext* immadiate_context = FRAMEWORK->GetDeviceContext();


	g_cbCBuffer.Projection = DirectX::XMMatrixTranspose(camera->GetProjection());
	g_cbCBuffer.View = DirectX::XMMatrixTranspose(camera->GetView());	// �s����V�F�[�_�ɓn���ɂ͓]�u�s��ɂ���
	g_cbCBuffer.ParticleSize.x = 0.08f;
	g_cbCBuffer.ParticleSize.y = 0.08f;
	g_cbCBuffer.EyePos = DirectX::SimpleMath::Vector4(camera->GetPos().x, camera->GetPos().y, camera->GetPos().z, 1.0f);
	//g_cbCBuffer.FogNear = 10.0f;	//���̎n�܂�ʒu
	//g_cbCBuffer.FogFar = 60.0f;		//���̏I���ʒu
	g_cbCBuffer.FogColor = DirectX::SimpleMath::Vector4{ 0.0f,0.0f,0.0f,1.0f };	//���̏I���ʒu

	immadiate_context->UpdateSubresource(DynamicCBuffer.Get(), 0, 0, &g_cbCBuffer, 0, 0);
	D3D11_BUFFER_DESC desc;
	DynamicCBuffer->GetDesc(&desc);
	// ***************************************
	// VS�ɒ萔�o�b�t�@��ݒ�
	immadiate_context->VSSetConstantBuffers(0, 1, DynamicCBuffer.GetAddressOf());
	// PS�ɒ萔�o�b�t�@��ݒ�
	immadiate_context->PSSetConstantBuffers(0, 1, DynamicCBuffer.GetAddressOf());
	immadiate_context->GSSetConstantBuffers(0, 1, DynamicCBuffer.GetAddressOf());

	// ***************************************

	// IA�ɒ��_�o�b�t�@��ݒ�
	UINT strides[1] = { sizeof(VBuffer) };
	UINT offsets[1] = { 0 };
	immadiate_context->IASetVertexBuffers(0, 1, VerticesBuffer.GetAddressOf(), strides, offsets);
	// IA�Ƀv���~�e�B�u�̎�ނ�ݒ�
	//immadiate_context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	// �e�V�F�[�_�̃Z�b�g
	ParticleShader->Activate();

	texture->Set(0);
	sample->Set(0);

	UINT mask = 0xffffffff;
	immadiate_context->OMSetBlendState(FRAMEWORK->GetBlendState(FRAMEWORK->BS_ALPHA), nullptr, mask);
	immadiate_context->RSSetState(FRAMEWORK->GetRasterizerState(FRAMEWORK->RS_SOLID_NONE));

	// ***************************************
	// �`�悷��
	immadiate_context->Draw(PerticleAmount, 0);
	// �V�F�[�_�̖�����
	ParticleShader->Inactivate();
	//----------------------------------------------------
}

void GPUParticle::SetSceneConstantBuffer(const ID3D11Buffer* cbBuf)
{
	memcpy(ConstantBuffer.Get(), cbBuf, sizeof(ID3D11Buffer));
}

void GPUParticle::CreateConstantBuffer(ID3D11Buffer** dstBuf, size_t size, bool dynamicFlg)
{
	Microsoft::WRL::ComPtr<ID3D11Device> device = FRAMEWORK->GetDevice();

	D3D11_BUFFER_DESC buffer_desc{};
	ZeroMemory(&buffer_desc, sizeof(D3D11_BUFFER_DESC));	// ��U������
	buffer_desc.ByteWidth = static_cast<UINT>(size);	// �^�w��
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	if (dynamicFlg)
	{
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		buffer_desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	}
	else
	{
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.CPUAccessFlags = 0;
	}
	HRESULT hr = device->CreateBuffer(&buffer_desc, nullptr, dstBuf);	// �o�b�t�@�쐬
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

}
