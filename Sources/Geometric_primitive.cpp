#include "Geometric_primitive.h"
#include "shader.h"
#include "misc.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"


Geometric_primitive::Geometric_primitive(ID3D11Device* device, const char* vs_cso_name, const char* ps_cso_name) {

	Vertex vertices[24]{};	// ���_���z��ivertices�j�ɂ��ׂĒ��_�̈ʒu�E�@�������i�[����B
							// �T�C�Y��1.0�̐������̃f�[�^���쐬����i�d�S�����_�ɂ���j�B�������̂̃R���g���[���|�C���g���� 8 �A
							// 1�̃R���g���[���|�C���g�̈ʒu�ɂ͖@���̌���(x,y,z)���Ⴄ���_��3����̂Œ��_���̑����� 8���_x3��=24��

	uint32_t indeces[36]{};	// �`�揇�ɎO�p�`�̒��_���(vertices�̔z��ԍ�)������
							// �����̎Ⴂ������n�܂�A���v��肪�\�ʂɂȂ�悤�Ɋi�[

	/*     LEFT_TOP  *------*  RIGHT_TOP     */
	/*               |   �^ |                */
	/*               | �^   |                */
	/*  LEFT_BOTTOM  *------*  RIGHT_BOTTOM  */

	// ���
	int face = FACE::TOP_FACE;
	vertices[face * 4 + LEFT_TOP].position     = XMFLOAT3(LEFT , TOP, BACK);
	vertices[face * 4 + RIGHT_TOP].position    = XMFLOAT3(RIGHT, TOP, BACK);
	vertices[face * 4 + LEFT_BOTTOM].position  = XMFLOAT3(LEFT , TOP, FRONT);
	vertices[face * 4 + RIGHT_BOTTOM].position = XMFLOAT3(RIGHT, TOP, FRONT);
	vertices[face * 4 + LEFT_TOP].normal       = XMFLOAT3(0.0f , 1.0f, 0.0f);
	vertices[face * 4 + RIGHT_TOP].normal      = XMFLOAT3(0.0f , 1.0f, 0.0f);
	vertices[face * 4 + LEFT_BOTTOM].normal    = XMFLOAT3(0.0f , 1.0f, 0.0f);
	vertices[face * 4 + RIGHT_BOTTOM].normal   = XMFLOAT3(0.0f , 1.0f, 0.0f);

	indeces[face * 6 + 0] = face * 4 + LEFT_TOP;
	indeces[face * 6 + 1] = face * 4 + RIGHT_TOP;
	indeces[face * 6 + 2] = face * 4 + LEFT_BOTTOM;

	indeces[face * 6 + 3] = face * 4 + RIGHT_TOP;
	indeces[face * 6 + 4] = face * 4 + RIGHT_BOTTOM;
	indeces[face * 6 + 5] = face * 4 + LEFT_BOTTOM;

	// ����
	face = FACE::BOTTOM_FACE;
	vertices[face * 4 + LEFT_TOP].position     = XMFLOAT3(LEFT , BOTTOM, BACK);
	vertices[face * 4 + RIGHT_TOP].position    = XMFLOAT3(RIGHT, BOTTOM, BACK);
	vertices[face * 4 + LEFT_BOTTOM].position  = XMFLOAT3(LEFT , BOTTOM, FRONT);
	vertices[face * 4 + RIGHT_BOTTOM].position = XMFLOAT3(RIGHT, BOTTOM, FRONT);
	vertices[face * 4 + LEFT_TOP].normal       = XMFLOAT3(0.0f , -1.0f, 0.0f);
	vertices[face * 4 + RIGHT_TOP].normal      = XMFLOAT3(0.0f , -1.0f, 0.0f);
	vertices[face * 4 + LEFT_BOTTOM].normal    = XMFLOAT3(0.0f , -1.0f, 0.0f);
	vertices[face * 4 + RIGHT_BOTTOM].normal   = XMFLOAT3(0.0f , -1.0f, 0.0f);

	indeces[face * 6 + 0] = face * 4 + LEFT_TOP;
	indeces[face * 6 + 1] = face * 4 + LEFT_BOTTOM;
	indeces[face * 6 + 2] = face * 4 + RIGHT_TOP;

	indeces[face * 6 + 3] = face * 4 + RIGHT_TOP;
	indeces[face * 6 + 4] = face * 4 + LEFT_BOTTOM;
	indeces[face * 6 + 5] = face * 4 + RIGHT_BOTTOM;

	// �O��
	face = FACE::FRONT_FACE;
	vertices[face * 4 + LEFT_TOP].position     = XMFLOAT3(LEFT , TOP,	 FRONT);
	vertices[face * 4 + RIGHT_TOP].position    = XMFLOAT3(RIGHT, TOP,	 FRONT);
	vertices[face * 4 + LEFT_BOTTOM].position  = XMFLOAT3(LEFT , BOTTOM, FRONT);
	vertices[face * 4 + RIGHT_BOTTOM].position = XMFLOAT3(RIGHT, BOTTOM, FRONT);
	vertices[face * 4 + LEFT_TOP].normal       = XMFLOAT3(0.0f , 0.0f, -1.0f);
	vertices[face * 4 + RIGHT_TOP].normal      = XMFLOAT3(0.0f , 0.0f, -1.0f);
	vertices[face * 4 + LEFT_BOTTOM].normal    = XMFLOAT3(0.0f , 0.0f, -1.0f);
	vertices[face * 4 + RIGHT_BOTTOM].normal   = XMFLOAT3(0.0f , 0.0f, -1.0f);

	indeces[face * 6 + 0] = face * 4 + LEFT_TOP;
	indeces[face * 6 + 1] = face * 4 + RIGHT_TOP;
	indeces[face * 6 + 2] = face * 4 + LEFT_BOTTOM;

	indeces[face * 6 + 3] = face * 4 + RIGHT_TOP;
	indeces[face * 6 + 4] = face * 4 + RIGHT_BOTTOM;
	indeces[face * 6 + 5] = face * 4 + LEFT_BOTTOM;

	// ���
	face = FACE::BACK_FACE;
	vertices[face * 4 + LEFT_TOP].position     = XMFLOAT3(LEFT , TOP,	 BACK);
	vertices[face * 4 + RIGHT_TOP].position    = XMFLOAT3(RIGHT, TOP,	 BACK);
	vertices[face * 4 + LEFT_BOTTOM].position  = XMFLOAT3(LEFT , BOTTOM, BACK);
	vertices[face * 4 + RIGHT_BOTTOM].position = XMFLOAT3(RIGHT, BOTTOM, BACK);
	vertices[face * 4 + LEFT_TOP].normal     = XMFLOAT3(0.0f, 0.0f, 1.0f);
	vertices[face * 4 + RIGHT_TOP].normal    = XMFLOAT3(0.0f, 0.0f, 1.0f);
	vertices[face * 4 + LEFT_BOTTOM].normal  = XMFLOAT3(0.0f, 0.0f, 1.0f);
	vertices[face * 4 + RIGHT_BOTTOM].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);

	indeces[face * 6 + 0] = face * 4 + LEFT_TOP;
	indeces[face * 6 + 1] = face * 4 + LEFT_BOTTOM;
	indeces[face * 6 + 2] = face * 4 + RIGHT_TOP;

	indeces[face * 6 + 3] = face * 4 + RIGHT_TOP;
	indeces[face * 6 + 4] = face * 4 + LEFT_BOTTOM;
	indeces[face * 6 + 5] = face * 4 + RIGHT_BOTTOM;

	// �E��
	face = FACE::RIGHT_FACE;
	vertices[face * 4 + LEFT_TOP].position     = XMFLOAT3(RIGHT, TOP,	 FRONT);
	vertices[face * 4 + RIGHT_TOP].position    = XMFLOAT3(RIGHT, TOP,	 BACK);
	vertices[face * 4 + LEFT_BOTTOM].position  = XMFLOAT3(RIGHT, BOTTOM, FRONT);
	vertices[face * 4 + RIGHT_BOTTOM].position = XMFLOAT3(RIGHT, BOTTOM, BACK);
	vertices[face * 4 + LEFT_TOP].normal       = XMFLOAT3(1.0f, 0.0f, 0.0f);
	vertices[face * 4 + RIGHT_TOP].normal      = XMFLOAT3(1.0f, 0.0f, 0.0f);
	vertices[face * 4 + LEFT_BOTTOM].normal    = XMFLOAT3(1.0f, 0.0f, 0.0f);
	vertices[face * 4 + RIGHT_BOTTOM].normal   = XMFLOAT3(1.0f, 0.0f, 0.0f);

	indeces[face * 6 + 0] = face * 4 + LEFT_TOP;
	indeces[face * 6 + 1] = face * 4 + RIGHT_TOP;
	indeces[face * 6 + 2] = face * 4 + LEFT_BOTTOM;

	indeces[face * 6 + 3] = face * 4 + RIGHT_TOP;
	indeces[face * 6 + 4] = face * 4 + RIGHT_BOTTOM;
	indeces[face * 6 + 5] = face * 4 + LEFT_BOTTOM;

	// ����
	face = FACE::LEFT_FACE;
	vertices[face * 4 + LEFT_TOP].position     = XMFLOAT3(LEFT, TOP,    FRONT);
	vertices[face * 4 + RIGHT_TOP].position    = XMFLOAT3(LEFT, TOP,    BACK);
	vertices[face * 4 + LEFT_BOTTOM].position  = XMFLOAT3(LEFT, BOTTOM, FRONT);
	vertices[face * 4 + RIGHT_BOTTOM].position = XMFLOAT3(LEFT, BOTTOM, BACK);
	vertices[face * 4 + LEFT_TOP].normal       = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	vertices[face * 4 + RIGHT_TOP].normal      = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	vertices[face * 4 + LEFT_BOTTOM].normal    = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	vertices[face * 4 + RIGHT_BOTTOM].normal   = XMFLOAT3(-1.0f, 0.0f, 0.0f);

	indeces[face * 6 + 0] = face * 4 + LEFT_TOP;
	indeces[face * 6 + 1] = face * 4 + LEFT_BOTTOM;
	indeces[face * 6 + 2] = face * 4 + RIGHT_TOP;

	indeces[face * 6 + 3] = face * 4 + RIGHT_TOP;
	indeces[face * 6 + 4] = face * 4 + LEFT_BOTTOM;
	indeces[face * 6 + 5] = face * 4 + RIGHT_BOTTOM;



	Create_com_buffers(device, vertices, 24, indeces, 36);

	HRESULT hr{ S_OK };

	D3D11_INPUT_ELEMENT_DESC input_element_desc[]{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// �V�F�[�_�쐬
	create_vs_from_cso(device, vs_cso_name, vertex_shader.GetAddressOf(), input_layout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
	create_ps_from_cso(device, ps_cso_name, pixel_shader.GetAddressOf());

	// �R���X�^���g�o�b�t�@�쐬
	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(Constants);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// �e��p�����[�^�̏�����
	param.Pos   = XMFLOAT3(0.0f, 0.0f,0.0f);
	param.Size  = XMFLOAT3(1.0f,1.0f,1.0f);
	param.Angle = XMFLOAT3(0.0f, 0.0f, 0.0f);
	param.Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

}

void Geometric_primitive::Create_com_buffers(ID3D11Device* device, Vertex* vertices, size_t vertex_count, uint32_t* indices, size_t index_count){
	HRESULT hr{ S_OK };

	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * vertex_count);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subresource_data;
	subresource_data.pSysMem = vertices;	// �ǂ̏��ŏ��������邩
	subresource_data.SysMemPitch = 0;
	subresource_data.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * index_count);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	subresource_data.pSysMem = indices;		// �ǂ̏��ŏ��������邩

	hr = device->CreateBuffer(&buffer_desc, &subresource_data, index_buffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


}

void Geometric_primitive::Render(ID3D11DeviceContext* immediate_context,  const XMFLOAT4X4& world, const XMFLOAT4& material_color) {
	uint32_t stride{ sizeof(Vertex) };
	uint32_t offset{ 0 };

	// ���_�o�b�t�@�̃o�C���h
	immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);

	// �C���f�b�N�X�o�b�t�@�̃o�C���h
	immediate_context->IASetIndexBuffer(index_buffer.Get(),		// �C���f�b�N�X���i�[�����I�u�W�F�N�g�̃|�C���^
		DXGI_FORMAT_R32_UINT,		// �C���f�b�N�X�o�b�t�@���̃f�[�^�̃t�H�[�}�b�g(16bit��32bit�̂ǂ��炩)
		0);							// �I�t�Z�b�g

	//�v���~�e�B�u�^�C�v�y�уf�[�^�̏����Ɋւ�����̃o�C���h
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ���̓��C�A�E�g�I�u�W�F�N�g�̃o�C���h
	immediate_context->IASetInputLayout(input_layout.Get());

	// �V�F�[�_�̃o�C���h
	immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
	immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

	Constants data{ world,material_color };
	// ����������}�b�v�s�\�ȃ������ɍ쐬���ꂽ�T�u���\�[�X�Ƀf�[�^���R�s�[
	immediate_context->UpdateSubresource(constant_buffer.Get(),	// ���惊�\�[�X�ւ̃|�C���^
		0,	// ����T�u���\�[�X�����ʂ���C���f�b�N�X
		0, &data, 0, 0);

	// �萔�o�b�t�@�̐ݒ�
	immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

	D3D11_BUFFER_DESC buffer_desc{};
	index_buffer->GetDesc(&buffer_desc);
	immediate_context->DrawIndexed(buffer_desc.ByteWidth / sizeof(uint32_t), 0, 0);	// �`�悷��C���f�b�N�X�̐�,�ŏ��̃C���f�b�N�X�̏ꏊ,���_�o�b�t�@����ǂݎ��O�ɒǉ�����l
}

void Geometric_primitive::Render(ID3D11DeviceContext* immediate_context, const XMFLOAT4& material_color) {
	XMMATRIX S{ XMMatrixScaling(param.Size.x,param.Size.y,param.Size.z) };				// �g�k
	XMMATRIX R{ XMMatrixRotationRollPitchYaw(param.Angle.x,param.Angle.y,param.Angle.z) };	// ��]
	XMMATRIX T{ XMMatrixTranslation(param.Pos.x,param.Pos.y,param.Pos.z) };			// ���s�ړ�

	XMFLOAT4X4 world;
	XMStoreFloat4x4(&world, S * R * T);	// ���[���h�ϊ��s��쐬

	uint32_t stride{ sizeof(Vertex) };
	uint32_t offset{ 0 };

	// ���_�o�b�t�@�̃o�C���h
	immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);

	// �C���f�b�N�X�o�b�t�@�̃o�C���h
	immediate_context->IASetIndexBuffer(index_buffer.Get(),		// �C���f�b�N�X���i�[�����I�u�W�F�N�g�̃|�C���^
		DXGI_FORMAT_R32_UINT,		// �C���f�b�N�X�o�b�t�@���̃f�[�^�̃t�H�[�}�b�g(16bit��32bit�̂ǂ��炩)
		0);							// �I�t�Z�b�g

	//�v���~�e�B�u�^�C�v�y�уf�[�^�̏����Ɋւ�����̃o�C���h
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ���̓��C�A�E�g�I�u�W�F�N�g�̃o�C���h
	immediate_context->IASetInputLayout(input_layout.Get());

	// �V�F�[�_�̃o�C���h
	immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
	immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

	Constants data{ world,material_color };
	// ����������}�b�v�s�\�ȃ������ɍ쐬���ꂽ�T�u���\�[�X�Ƀf�[�^���R�s�[
	immediate_context->UpdateSubresource(constant_buffer.Get(),	// ���惊�\�[�X�ւ̃|�C���^
		0,	// ����T�u���\�[�X�����ʂ���C���f�b�N�X
		0, &data, 0, 0);

	// �萔�o�b�t�@�̐ݒ�
	immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

	D3D11_BUFFER_DESC buffer_desc{};
	index_buffer->GetDesc(&buffer_desc);
	immediate_context->DrawIndexed(buffer_desc.ByteWidth / sizeof(uint32_t), 0, 0);	// �`�悷��C���f�b�N�X�̐�,�ŏ��̃C���f�b�N�X�̏ꏊ,���_�o�b�t�@����ǂݎ��O�ɒǉ�����l
}

void Geometric_primitive::imguiWindow() {

	static float pos[3]{ param.Pos.x ,param.Pos.y ,param.Pos.z };
	static float size[3]{ param.Size.x ,param.Size.y ,param.Size.z };
	static float angle[3]{ param.Angle.x,param.Angle.y,param.Angle.z };
	static float Color[4]{ param.Color.x ,param.Color.y,param.Color.z,param.Color.w };

	ImGui::Begin("Geometric Primitive");

	//ImGui::Begin("Sprite_param");
	ImGui::SliderFloat3("Position", pos, -5, 5);
	ImGui::SliderFloat3("Size", size, 0, 5);
	ImGui::SliderFloat3("angle", angle, -360, 360);
	ImGui::ColorEdit4(u8"Color", (float*)&Color);

	ImGui::End();
	setPos(DirectX::XMFLOAT3(pos[0], pos[1], pos[2]));
	setSize(DirectX::XMFLOAT3(size[0], size[1], size[2]));
	setAngle(DirectX::XMFLOAT3(XMConvertToRadians(angle[0]), XMConvertToRadians(angle[1]), XMConvertToRadians(angle[2])));
	setColor(DirectX::XMFLOAT4(Color[0], Color[1], Color[2], Color[3]));
}