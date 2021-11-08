
#include <sstream>
#include <WICTextureLoader.h>

#include "framework.h"
#include "shader.h"
#include "Sprite_Batch.h"
#include "texture.h"
#include "misc.h"


// ���_�o�b�t�@�I�u�W�F�N�g�̐���
sprite_Batch::sprite_Batch(const wchar_t* filename, size_t max_sprites, const char* vs_cso_name, const char* ps_cso_name) :max_vertices(max_sprites * 6)
{
	ID3D11Device* device = FRAMEWORK->GetDevice();
	HRESULT hr{ S_OK };

	std::unique_ptr<Vertex[]> vertices{ std::make_unique<Vertex[]>(max_vertices) };

	// �e�N�X�`���̃��[�h
	load_texture_from_file(filename, shader_resource_view.GetAddressOf(), &texture2d_desc);

	// ByteWidth,BindFlags,StructuerByteStride�͉Ϗ��A���̑����͂��܂�ω����邱�Ƃ͂Ȃ�
	D3D11_BUFFER_DESC buffer_desc{};			                // �o�b�t�@�̎g������ݒ肷��\����
	buffer_desc.ByteWidth = sizeof(Vertex) * (UINT)max_vertices;// �o�b�t�@�̑傫��
	buffer_desc.Usage = D3D11_USAGE_DYNAMIC;			        // �o�b�t�@�ւ̊e���ڂł̃A�N�Z�X�����w�� ���݂�GPU�i�ǂݎ���p�j��CPU�i�������ݐ�p�j�̗�������A�N�Z�X�ł���ݒ�
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	        // �o�C���h���@ ���̐ݒ蒸�_�o�b�t�@��R���X�^���g�o�b�t�@�Ƃ��Ďg�p���邱�Ƃ����߂�
	buffer_desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;        // ���\�[�X�ɑ΂��Ă�CPU�A�N�Z�X���@ �O�ŃA�N�Z�X�s�v�̐ݒ�ɂȂ�B���݂͏������ݐ�p
	buffer_desc.MiscFlags = 0;							        // ���̑��̃I�v�V���� �s�v�Ȃ�O���w��  ���Ȃ݂�...misc�Fmiscellaneous�̗��́B�u�G���ȁv�Ƃ����Ӗ�
	buffer_desc.StructureByteStride = 0;				        // �\���̂̃T�C�Y �o�b�t�@���\���̂Ƃ��Ċm�ۂ���ꍇ�Ɏg�p �O�w��ł����삷��

	D3D11_SUBRESOURCE_DATA subresource_data{};			        // �쐬����o�b�t�@�̏������f�[�^��ۑ�����\����
	subresource_data.pSysMem = vertices.get();			        // �o�b�t�@������������f�[�^���w�� �ǂ̏��ŏ��������邩
	subresource_data.SysMemPitch = 0;					        // �������̃s�b�` 2D or 3D�e�N�X�`���̏ꍇ�Ɏg�p����
	subresource_data.SysMemSlicePitch = 0;				        //	�[�x���x��	 ����

	hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer.GetAddressOf());		// �쐬����o�b�t�@���A�쐬����o�b�t�@�̏��������A�쐬�����o�b�t�@��ۑ�����|�C���^
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));	// _ASSERT_EXPR�F��������������������Ȃ���Α������̃��b�Z�[�W��\������

	//	// ���̓��C�A�E�g�I�u�W�F�N�g�̐���
	//D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	//{
	//	{
	//	"POSITION",						// �Z�}���e�B�N�X��	HLSL���̃V�O�l�`��(�^��ϐ����̑g�ݍ��킹�̎��H)�̖��O�ƈ�v�����邱�Ƃő��M�������_������M���邱�Ƃ��ł���
	//	0,								// �Z�}���e�B�N�X�ԍ� �����ł����ʂł���悤�ɔԍ������蓖�Ă�B�ԍ���ύX���邱�Ƃ�HLSL�ŕʂ̏�񂾂ƔF���ł���
	//	DXGI_FORMAT_R32G32B32_FLOAT,	// �t�H�[�}�b�g	R23G23B23�͎���float3
	//	0,								// ���̓X���b�g�ԍ�	�������C�A�E�g���ǂ̓��̓X���b�g�ɑ΂��Ĕ��f����邩���w�肷��
	//	D3D11_APPEND_ALIGNED_ELEMENT,	// �v�f����擪�܂ł̃I�t�Z�b�g�l	�e�f�[�^�̔z��擪�����o�C�g����Ă��邩�B
	//									// D3D11_APPEND_ALIGNED_ELEMENT���w��ŃI�t�Z�b�g�l�������v�Z ��v�Z�Ȃ�t�H�[�}�b�g�T�C�Y�����Z���Ă���
	//	D3D11_INPUT_PER_VERTEX_DATA,	// ���̓f�[�^�̎��	���_�f�[�^�ƃC���X�^���X�f�[�^�̂Q���
	//	0								// �J��Ԃ���(���_�f�[�^�̎��͂O)	��L�ŃC���X�^���X�f�[�^��ݒ肵���ꍇ�ɈӖ�������
	//	},
	//	{"COLOR"   ,0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	//	{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT		,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	//};

	//// ���_�V�F�[�_�[�I�u�W�F�N�g�̐���
	//create_vs_from_cso(vs_cso_name, vertex_shader.GetAddressOf(), input_layout.GetAddressOf(), input_element_desc, _countof(input_element_desc));

	//// �s�N�Z���V�F�[�_�I�u�W�F�N�g�̐���
	//create_ps_from_cso(ps_cso_name, pixel_shader.GetAddressOf());

	param.Pos     = DirectX::XMFLOAT2(0.0f, 0.0f);
	param.Size    = DirectX::XMFLOAT2(texture2d_desc.Width, texture2d_desc.Height);
	param.TexPos  = DirectX::XMFLOAT2(0.0f, 0.0f);
	param.TexSize = DirectX::XMFLOAT2(texture2d_desc.Width, texture2d_desc.Height);
	param.Angle   = 0.0f;
	param.Color   = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
}

sprite_Batch::~sprite_Batch() {
	//vertex_shader->Release();
	//pixel_shader->Release();
	//input_layout->Release();
	//vertex_buffer->Release();
	//shader_resource_view->Release();

	rerease_all_textures();
}

void sprite_Batch::begin() {
	ID3D11DeviceContext* immediate_context = FRAMEWORK->GetDeviceContext();

	vertices.clear();
	// �V�F�[�_�̃o�C���h
	immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
	immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
	// SRV�o�C���h
	immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());	// ���W�X�^�ԍ��A�V�F�[�_���\�[�X�̐��ASRV�̃|�C���^
}

void sprite_Batch::end() {
	ID3D11DeviceContext* immediate_context = FRAMEWORK->GetDeviceContext();

	// �e�N�X�`�����W�𒸓_�o�b�t�@�ɃZ�b�g����
	HRESULT hr = { S_OK };
	D3D11_MAPPED_SUBRESOURCE mapped_subrecource{};
	// mapped_subrecource��vertex_buffer�Ƀ}�b�s���O
	hr = immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subrecource);	// ���I�Ȓ萔�o�b�t�@�[�� Map ���ď������ނƂ��� D3D11_MAP_WRITE_DISCARD ���g�p����
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	size_t vertex_count = vertices.size();
	_ASSERT_EXPR(max_vertices >= vertex_count, "Buffer overflow");
	Vertex* data{ reinterpret_cast<Vertex*>(mapped_subrecource.pData) };	// reinterpret_cast�F���肦�Ȃ��悤�ȕϊ��̂Ƃ��Ɏg�p����H
	if (data != nullptr) {	// ���̏㏑��
		// vertices�̃f�[�^���R�s�[
		const Vertex* p = vertices.data();
		memcpy_s(data, max_vertices * sizeof(Vertex), p, vertex_count * sizeof(Vertex));	// memcpy_s�FBuffer�Ԃł̃o�C�g�̃R�s�[(�R�s�[��|�C���^�A�R�s�[��T�C�Y�A�R�s�[���|�C���^�A�R�s�[���T�C�Y)
	}
	immediate_context->Unmap(vertex_buffer.Get(), 0);	// �}�b�s���O���� ���_�o�b�t�@���㏑��������K�����s�BMap&Unmap�̓Z�b�g�Ŏg�p����

	// ���_�o�b�t�@�̃o�C���h
	UINT stride{ sizeof(Vertex) };
	UINT offset{ 0 };
	immediate_context->IASetVertexBuffers(
		0,				               // ���̓X���b�g�̊J�n�ԍ�
		1,				               // ���_�o�b�t�@�̐�
		vertex_buffer.GetAddressOf(),  // ���_�o�b�t�@�̔z��
		&stride,		               // �P���_�̃T�C�Y�̔z��
		&offset);		               // ���_�o�b�t�@�̊J�n�ʒu�����炷�I�t�Z�b�g�̔z��

	//�v���~�e�B�u�^�C�v�y�уf�[�^�̏����Ɋւ�����̃o�C���h
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// �v���~�e�B�u�̌`����w��ł���H
											// D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ��3���_�AD3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP�͎l���_ �A���O�p�`
	// ���̓��C�A�E�g�I�u�W�F�N�g�̃o�C���h
	immediate_context->IASetInputLayout(input_layout.Get());	// ���̓��C�A�E�g�̐ݒ�

	// ���X�^���C�U�X�e�[�g�̐ݒ�
	immediate_context->RSSetState(rasterizer_states[0].Get());

	// �v���~�e�B�u�̕`��
	immediate_context->Draw(static_cast<UINT>(vertex_count), 0);	// ���_�̐��A�`��J�n���_�Ŏg�����_�o�b�t�@�̍ŏ��̃C���f�b�N�X

}

void sprite_Batch::CreateVertexData(DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size, float angle, DirectX::XMFLOAT4 color, DirectX::XMFLOAT2 TexPos, DirectX::XMFLOAT2 TexSize) {
	ID3D11DeviceContext* immediate_context = FRAMEWORK->GetDeviceContext();

	// �X�N���[��(�r���[�|�[�g)�̃T�C�Y���擾����
	D3D11_VIEWPORT viewport{};
	UINT num_viewports{ 1 };
	immediate_context->RSGetViewports(&num_viewports, &viewport);

	// ���������`�̊e���_�̈ʒu���v�Z����
	/*		left_top	*----*	right_top			*/
	/*					|   /|						*/
	/*					|  / |						*/
	/*					| /  |						*/
	/*		left_bottom	*----*	right_bottom		*/

	DirectX::XMFLOAT3 left_top    { pos.x         ,pos.y          ,0 };	// ����
	DirectX::XMFLOAT3 right_top   { pos.x + size.x,pos.y          ,0 };	// �E��
	DirectX::XMFLOAT3 left_bottom { pos.x         ,pos.y + size.y ,0 };	// ����
	DirectX::XMFLOAT3 right_bottom{ pos.x + size.x,pos.y + size.y ,0 };	// �E��

	// ��]�̒��S����`�̒��S�_��
	DirectX::XMFLOAT2 center{ 0,0 };
	center.x = pos.x + size.x * 0.5f;	// �ʒu-(�傫��/2)�Œ��_�ʒu���甼�T�C�Y������=�����ɂȂ�
	center.y = pos.y + size.y * 0.5f;
	// ���_��]
	rotate(left_top    , center, angle);
	rotate(left_bottom , center, angle);
	rotate(right_top   , center, angle);
	rotate(right_bottom, center, angle);

	// �X�N���[�����W�n����NDC(���K���f�o�C�X���W)�ւ̍��W�ϊ����s��
	left_top     = ConvertToNDC(left_top    , viewport);	// ���_�ʒu�A�X�N���[���̑傫��
	left_bottom  = ConvertToNDC(left_bottom , viewport);
	right_top    = ConvertToNDC(right_top   , viewport);
	right_bottom = ConvertToNDC(right_bottom, viewport);

	DirectX::XMFLOAT2 TexLeft_top    { (TexPos.x)             / texture2d_desc.Width , (TexPos.y)             / texture2d_desc.Height };
	DirectX::XMFLOAT2 TexRight_top   { (TexPos.x + TexSize.x) / texture2d_desc.Width , (TexPos.y)             / texture2d_desc.Height };
	DirectX::XMFLOAT2 TexLeft_bottom { (TexPos.x)             / texture2d_desc.Width , (TexPos.y + TexSize.y) / texture2d_desc.Height };
	DirectX::XMFLOAT2 TexRight_bottom{ (TexPos.x + TexSize.x) / texture2d_desc.Width , (TexPos.y + TexSize.y) / texture2d_desc.Height };


	// �v�Z���ʂŒ��_�o�b�t�@�I�u�W�F�N�g���X�V����
	//				�O�p�`��2���				//
	/*				0	*----*1	 *4				*/
	/*					||||/   /|				*/
	/*					|||/   /||				*/
	/*					||/   /|||				*/
	/*				2	*	3*---*5				*/

	vertices.push_back({ { left_top.x    ,left_top.y,	 0 }, {0,0,1}, { TexLeft_top.x,		TexLeft_top.y	 }, { color.x,color.y,color.z,color.w } });	// ����
	vertices.push_back({ { right_top.x   ,right_top.y,	 0 }, {0,0,1}, { TexRight_top.x,	TexRight_top.y	 }, { color.x,color.y,color.z,color.w } });	// �E��
	vertices.push_back({ { left_bottom.x ,left_bottom.y, 0 }, {0,0,1}, { TexLeft_top.x,		TexLeft_bottom.y }, { color.x,color.y,color.z,color.w } });	// ����
	vertices.push_back({ { left_bottom.x ,left_bottom.y, 0 }, {0,0,1}, { TexLeft_top.x,		TexLeft_bottom.y }, { color.x,color.y,color.z,color.w } });	// ����
	vertices.push_back({ { right_top.x   ,right_top.y,	 0 }, {0,0,1}, { TexRight_top.x,	TexRight_top.y	 }, { color.x,color.y,color.z,color.w } });	// �E��
	vertices.push_back({ { right_bottom.x,right_bottom.y,0 }, {0,0,1}, { TexRight_bottom.x,	TexRight_bottom.y}, { color.x,color.y,color.z,color.w } });	// �E��
}

void sprite_Batch::Render(DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size, float angle, DirectX::XMFLOAT4 color, DirectX::XMFLOAT2 sPos, DirectX::XMFLOAT2 sSize) {
	CreateVertexData(pos, size, angle, color, sPos, sSize);
}

void sprite_Batch::Render() {
	CreateVertexData(param.Pos, param.Size, param.Angle, param.Color, param.TexPos, param.TexSize);
}

void sprite_Batch::Render(DirectX::XMFLOAT2 Pos, DirectX::XMFLOAT2 Size) {
	CreateVertexData(Pos, Size, 0, param.Color, param.TexPos, param.TexSize);
}

DirectX::XMFLOAT3 sprite_Batch::ConvertToNDC(DirectX::XMFLOAT3 pos, D3D11_VIEWPORT viewport) {
	pos.x = (pos.x * 2 / viewport.Width) - 1.0f;	// x�l���Q�{�A���̌�X�N���[���T�C�Y�Ŋ����ĂP�������Ɛ��K�������
	pos.y = 1.0f - (pos.y * 2.0f / viewport.Height);	// y�l���Q�{�A�X�N���[���T�C�Y�Ŋ��������̂łP�������Ɛ��K��	x�ƈႤ�̂͂����炭����E����W�n�̊֌W
	// �����sprite(�摜)�Ȃ̂�z�l�͕ύX����K�v�Ȃ�
	return pos;
}