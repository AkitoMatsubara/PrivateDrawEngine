#include "GPUParticle.h"
#include "framework.h"
#include "UseComputeShader.h"

// TODO �K�� �������݉\�\�����o�b�t�@���쐬����
HRESULT CreateWriteStructuredBuffer(UINT uElementSize, UINT uCount, ID3D11Buffer** ppBuf)
{
	ID3D11Device* pD3DDevice = FRAMEWORK->GetDevice();
	HRESULT hr = E_FAIL;
	*ppBuf = nullptr;

	// �\�����o�b�t�@�[���쐬����
	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));

	BufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	BufferDesc.ByteWidth = uElementSize * uCount;					// �o�b�t�@�T�C�Y
	BufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	BufferDesc.StructureByteStride = uElementSize;					// �\�����o�b�t�@�[�̃T�C�Y (�o�C�g�P��)
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = pD3DDevice->CreateBuffer(&BufferDesc, nullptr, ppBuf);

	if (FAILED(hr))_ASSERT_EXPR_A(false, "FAILED CreateStructuredBuffer");

	return hr;
}

bool GPUParticle::Init()
{
	Microsoft::WRL::ComPtr<ID3D11Device> device = FRAMEWORK->GetDevice();

	{
		//�R���s���[�g�v�Z�̃��[�N���Z�b�g

		DispathNo = 500;  // �f�B�X�p�b�`��
		const int CSnumthreadNo = 500;	// CS�̎��s��
		ParticleAmount = DispathNo * CSnumthreadNo;        // �p�[�e�B�N���̐� (DispatchNo * CSnumthread)
		HRESULT hr = { S_OK };

		// �`��p���\�[�X�E�r���[
		{
			VerticesBuffer = nullptr;	// �o�b�t�@�����̂Œ��g�������Ă͑�ρA�����܂�

			// ���_�o�b�t�@�̒�`�@�i�������݂ɂ��Ή��j
			D3D11_BUFFER_DESC vBufferDesc;
			vBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			vBufferDesc.ByteWidth = sizeof(VBuffer) * ParticleAmount;
			vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // ���_�o�b�t�@�ƃX�g���[���o�͂Ƃ��Ďg��
			vBufferDesc.CPUAccessFlags = 0;
			vBufferDesc.MiscFlags = 0;
			vBufferDesc.StructureByteStride = 0;

			// ���_�o�b�t�@�̃T�u���\�[�X���w�肷�邽�߂̃|�C���^ ��������
			VBuffer* vBuffer = new VBuffer[ParticleAmount];

			// ���_�o�b�t�@�̃T�u���\�[�X�̒�`(�o�b�t�@�[���\�[�X�ł���������Ă�j
			D3D11_SUBRESOURCE_DATA vSubData;//�������p�f�[�^���쐬
			vSubData.pSysMem = vBuffer;  // �o�b�t�@�E�f�[�^�̏����l
			vSubData.SysMemPitch = 0;
			vSubData.SysMemSlicePitch = 0;
			// �`��p���_���̓o�b�t�@�̍쐬
			device->CreateBuffer(&vBufferDesc, &vSubData, VerticesBuffer.GetAddressOf());
			delete[] vBuffer;	// ���Еt��

			// �����l�̐ݒ�
			for (int i = 0; i < ParticleAmount; ++i) {
				VBuffer data;
				// �����ʒu�̐ݒ�
				data.Position.x = ((i % DispathNo) - (DispathNo / 2.0f)) / (float)DispathNo * 0.005f;
				data.Position.z = ((i / DispathNo) - (DispathNo / 2.0f)) / (float)DispathNo * 0.005f;
				data.Position.y = 0.1f;

				// �������x�̐ݒ�
				data.Velocity.x = ((rand() % 2000) - 1000) * 0.00005f;
				data.Velocity.y = ((rand() % 2000) - 1000) * 0.00005f;
				data.Velocity.z = ((rand() % 2000) - 1000) * 0.00005f;

				data.Force = DirectX::XMFLOAT3(0, -0.0005f, 0);//�����x

				// ���̑�
				data.Color = DirectX::XMFLOAT4(testColor[0], testColor[1], testColor[2], testColor[3]);
				data.Active = true;
				testLife = 50;
				data.Life = testLife * (rand() % 10 + 1);

				vVecBuf.emplace_back(data);	// �i�[
			}
			// ���I�Ȓ萔�o�b�t�@�̒�`
			CreateConstantBuffer(DynamicCBuffer.GetAddressOf(), sizeof(cbCBuffer), true);

			// ���\�[�X�̐ݒ�
			// ���̓��\�[�X
			// SRV�̍쐬
			UseComputeShader::CreateWritableStructuredBufferAndSRV(sizeof(VBuffer), ParticleAmount, vVecBuf.data(), InputBuffer.GetAddressOf(), g_pSRV.GetAddressOf());

			// �o�̓��\�[�X
			// UAV�̍쐬
			UseComputeShader::CreateStructuredBufferAndUAV(sizeof(VBuffer), ParticleAmount, nullptr,OutputBuffer.GetAddressOf(), g_pUAV.GetAddressOf());

			// ���[�h�o�b�N�p�o�b�t�@ ���\�[�X�̍쐬
			UseComputeShader::CreateAndCopyToBuffer(OutputBuffer.Get(), CPUReadBackBuffer.GetAddressOf());
		}
	}
	// Compute Shader�Z�b�e�B���O
	{
		// �V�[���R���X�^���g�o�b�t�@�̍쐬�ƌ^�̐ݒ�
		CreateConstantBuffer(ConstantBuffer.GetAddressOf(), sizeof(scene_constants));
		// Sampler�̐ݒ�
		if (sample == nullptr)
		{
			sample = std::make_shared<Sampler>(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);
		}
		//�`��p�V�F�[�_
		if (ParticleShader == nullptr)
		{
			// ���͗v�f
			D3D11_INPUT_ELEMENT_DESC layout[] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
					D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
					D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "FORCE",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
					D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
					D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "ACTIVE",    0, DXGI_FORMAT_R32_UINT, 0,
					D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "LIFE",    0, DXGI_FORMAT_R32_FLOAT, 0,
					D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
			static constexpr UINT IL_NUM = ARRAYSIZE(layout);	// IL�̔z��T�C�Y�擾
			// �V�F�[�_�̍쐬
			ParticleShader = std::make_unique<ShaderEx>();
			ParticleShader->CreateCS(L"Shaders\\GPUParticle_cs");	// �t�@�C���w��
			ParticleShader->CreateVS(L"Shaders\\GPUParticle_vs", IL_NUM, layout);
			ParticleShader->CreateGS(L"Shaders\\GPUParticle_gs");
			ParticleShader->CreatePS(L"Shaders\\GPUParticle_ps");
		}
	}
	if (texture == nullptr)
	{
		texture = std::make_shared<Texture>();
		texture->Load(L".\\Resources\\particle_W.png");
	}
	return true;
}

void GPUParticle::Update(Camera* camera)
{
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context = FRAMEWORK->GetDeviceContext();
	Microsoft::WRL::ComPtr<ID3D11Device> device = FRAMEWORK->GetDevice();
	HRESULT hr = { S_OK };

	D3D11_MAPPED_SUBRESOURCE subRes;
	immediate_context->Map(InputBuffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &subRes);	// subRes��InputBuffer���}�b�v
	{
		memcpy(subRes.pData, vVecBuf.data(), sizeof(VBuffer) * ParticleAmount);	// SRV�̃o�b�t�@�ɏ����������R�s�[
	}
	immediate_context->Unmap(InputBuffer.Get(), 0);

	// **********************************************************
	// �R���s���[�g�E�V�F�[�_���g�������Z
	// **********************************************************
	// �V�F�[�_��ݒ�
	g_cbCBuffer.No = DispathNo;

	g_cbCBuffer.Projection	= DirectX::XMMatrixTranspose(camera->GetProjection());
	g_cbCBuffer.View		= DirectX::XMMatrixTranspose(camera->GetView());	// �s����V�F�[�_�ɓn���ɂ͓]�u�s��ɂ���
	g_cbCBuffer.ParticleSize.x = 0.02f;
	g_cbCBuffer.ParticleSize.y = 0.02f;
	g_cbCBuffer.EyePos = DirectX::SimpleMath::Vector4(camera->GetPos().x, camera->GetPos().y, camera->GetPos().z, 1.0f);

	// ***************************************
	// �萔�o�b�t�@�̃}�b�v�擾

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	// D3D11_MAP_WRITE_DISCARD : ���Ƃ̃f�[�^�𖳌��ɂ��ď�������
	immediate_context->Map(DynamicCBuffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	{
		// ���I�R���X�^���g�o�b�t�@�̒l�ɃR�s�[����
		CopyMemory(MappedResource.pData, &g_cbCBuffer, sizeof(cbCBuffer));
	}
	// �}�b�v����
	immediate_context->Unmap(DynamicCBuffer.Get(), NULL);

	// PS�ɒ萔�o�b�t�@��ݒ�
	immediate_context->CSSetConstantBuffers(0, 1, DynamicCBuffer.GetAddressOf());
	immediate_context->CSSetShader(ParticleShader->GetCS(), 0, 0);

	// CPU������������s�����߁A�Q�̃o�b�t�@�ňʒu�������݂ɏo�����ꂵ�Ă���
	// 0�̏������ƂɌv�Z�A���ʂ�1�� �� 1�̏������ƂɌv�Z�A���ʂ�0�Ɂc  �Ƃ����悤��

	// SRV�̐ݒ�
	immediate_context->CSSetShaderResources(0, 1, g_pSRV.GetAddressOf());

	// UAV�̐ݒ�
	immediate_context->CSSetUnorderedAccessViews(0, 1, g_pUAV.GetAddressOf(), 0);
	// CS�̎��s
	if(runCS) immediate_context->Dispatch(DispathNo, 1, 1);

	immediate_context->CopyResource(VerticesBuffer.Get(), InputBuffer.Get());	// �`��O�Ɍv�Z�����l�𒸓_�o�b�t�@�ɓ����

	// ���ʂ�CPU����ǂݍ���
	immediate_context->CopyResource(CPUReadBackBuffer.Get(), OutputBuffer.Get());
	// ���ʂ𒸓_�o�b�t�@���փR�s�[����
	hr = immediate_context->Map(CPUReadBackBuffer.Get(), 0, D3D11_MAP_READ, 0, &MappedResource);
	{
		VBuffer* pt = static_cast<VBuffer*>(MappedResource.pData);
		memcpy(vVecBuf.data(), pt, sizeof(VBuffer) * ParticleAmount);
	}
	immediate_context->Unmap(CPUReadBackBuffer.Get(), 0);
	//----------------------------------------------------
}

void GPUParticle::Draw()
{
	ID3D11DeviceContext* immediate_context = FRAMEWORK->GetDeviceContext();

	// ***************************************
	// VS�ɒ萔�o�b�t�@��ݒ�
	immediate_context->VSSetConstantBuffers(0, 1, DynamicCBuffer.GetAddressOf());
	// PS�ɒ萔�o�b�t�@��ݒ�
	immediate_context->PSSetConstantBuffers(0, 1, DynamicCBuffer.GetAddressOf());
	immediate_context->GSSetConstantBuffers(0, 1, DynamicCBuffer.GetAddressOf());

	// ***************************************

	// IA�ɒ��_�o�b�t�@��ݒ�
	UINT strides[1] = { sizeof(VBuffer) };
	UINT offsets[1] = { 0 };
	immediate_context->IASetVertexBuffers(0, 1, VerticesBuffer.GetAddressOf(), strides, offsets);

	// IA�Ƀv���~�e�B�u�̎�ނ�ݒ�
	immediate_context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	// �e�V�F�[�_�̃Z�b�g
	ParticleShader->Activate();

	texture->Set(0);
	sample->Set(0);

	const UINT MASK = 0xffffffff;
	immediate_context->OMSetBlendState(FRAMEWORK->GetBlendState(FRAMEWORK->BS_ALPHA), nullptr, MASK);
	immediate_context->RSSetState(FRAMEWORK->GetRasterizerState(FRAMEWORK->RS_SOLID_NONE));
	sample->Set(0);
	// ***************************************
	// �`�悷��
	immediate_context->Draw(ParticleAmount, 0);
	// �V�F�[�_�̖�����
	ParticleShader->Inactivate();
	//----------------------------------------------------
}

void GPUParticle::SetParticle()
{

	//// for�ŉ񂵂�Active�łȂ���Ώ��������c�Ȃ񂾂��ǂ��܂�ɂ�for���������ē��삵�Ȃ�
	//for (std::vector<VBuffer>::iterator itr = vVecBuf.begin(); itr != vVecBuf.end();)
	//{
	//	if(itr->Active)
	//	{
	//		++itr;
	//	}
	//	else
	//	{
	//		itr = vVecBuf.erase(itr);
	//	}
	//}

	// TODO �p�[�e�B�N���ꊇ�폜�B���}���u�Ȃ���
	vVecBuf.clear();

	for (int i = 0; i < ParticleAmount; ++i) {
		VBuffer data;
		// �����ʒu�̐ݒ�
		data.Position.x = ((i % DispathNo) - (DispathNo / 2.0f)) / (float)DispathNo * 0.005f;
		data.Position.z = ((i / DispathNo) - (DispathNo / 2.0f)) / (float)DispathNo * 0.005f;
		data.Position.y = 0.1f;

		// �������x�̐ݒ�
		data.Velocity.x = ((rand() % 2000) - 1000) * 0.00005f;
		data.Velocity.y = ((rand() % 2000) - 1000) * 0.00005f;
		data.Velocity.z = ((rand() % 2000) - 1000) * 0.00005f;

		data.Force = DirectX::XMFLOAT3(0, -0.0005f, 0);//�����x

		// ���̑�
		data.Color = DirectX::XMFLOAT4(testColor[0], testColor[1], testColor[2], testColor[3]);
		data.Active = true;
		testLife = 50;
		data.Life = testLife * (rand() % 10 + 1);
		vVecBuf.emplace_back(data);	// �i�[
	}
}


void GPUParticle::SetSceneConstantBuffer(const ID3D11Buffer* cbBuf)
{
	memcpy(ConstantBuffer.Get(), cbBuf, sizeof(ID3D11Buffer));
}

// GPUParticle�Ɍ���Ȃ��Ǝv���̂ō���̓����o�֐��Ƃ��Ē�`
void GPUParticle::CreateConstantBuffer(ID3D11Buffer** dstBuf, size_t size, bool dynamicFlg)
{
	Microsoft::WRL::ComPtr<ID3D11Device> device = FRAMEWORK->GetDevice();

	*dstBuf = nullptr;
	D3D11_BUFFER_DESC buffer_desc{};
	ZeroMemory(&buffer_desc, sizeof(D3D11_BUFFER_DESC));	// ��U������
	buffer_desc.ByteWidth = static_cast<UINT>(size);	// �^�w��
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	if (dynamicFlg)
	{
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.CPUAccessFlags = 0;
	}
	HRESULT hr = device->CreateBuffer(&buffer_desc, nullptr, dstBuf);	// �o�b�t�@�쐬
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

}

void GPUParticle::ImguiParticles()
{
#ifdef USE_IMGUI
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.0f, 0.6f, 0.1f, 1.0f));	// �����ĂԂ�ImGui::PopStyleColor�������Ȃ��Ƃ����Ȃ��炵��
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.0f, 0.2f, 0.0f, 1.0f));

	// ���C�g�������O���[�o���ݒ�
	ImGui::Begin("Particles");

	ImGui::SliderFloat("Life", &testLife, 0.0f, 1.0f);
	ImGui::ColorEdit4("Color", (float*)testColor);
	ImGui::Checkbox("RunGPUParticle", &runCS);
	if (ImGui::Button("Particle Set")) { SetParticle(); }

	ImGui::Separator();
	ImGui::Text("DispatchNo: %d", DispathNo);
	ImGui::Text("ParticleAmount: %d", ParticleAmount);

	ImGui::PopStyleColor(2);

	ImGui::End();
#endif

}
