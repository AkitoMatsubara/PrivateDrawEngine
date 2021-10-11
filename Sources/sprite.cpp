#include "framework.h"
#include "texture.h"
#include "sprite.h"
#include "misc.h"
#include <sstream>
#include <WICTextureLoader.h>

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"


// ���_���̃Z�b�g
Vertex vertices[]
{
	{{-1.0,+1.0,0},{0,0,1},{0,0},{1,1,1,1}},	// ����,�@��,RGB�F,UV���W
	{{+1.0,+1.0,0},{0,0,1},{1,0},{1,1,1,1}},	// �E��,�@��,RGB�F,UV���W
	{{-1.0,-1.0,0},{0,0,1},{0,1},{1,1,1,1}},	// ����,�@��,RGB�F,UV���W
	{{+1.0,-1.0,0},{0,0,1},{1,1},{1,1,1,1}},	// �E��,�@��,RGB�F,UV���W
};


// ���_�o�b�t�@�I�u�W�F�N�g�̐���
Sprite::Sprite(const wchar_t* filename, const char* vs_cso_name ,const char* ps_cso_name )
{
	ID3D11Device* device = FRAMEWORK->GetDevice();
	HRESULT hr{ S_OK };

	//// �摜�t�@�C���̃��[�h��SRV�I�u�W�F�N�g�̐���
	//ID3D11Resource* resource{};
	//hr = CreateWICTextureFromFile(device, filename, &resource, &shader_resource_view);	// �������ǂݍ��܂��΃��\�[�X��SRV�����������
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//resource->Release();
	//// �e�N�X�`�����̎擾
	//ID3D11Texture2D* texture2d{};
	//hr = resource->QueryInterface<ID3D11Texture2D>(&texture2d);	// ����̃C���^�[�t�F�C�X���T�|�[�g���Ă��邩�𔻕ʂ���
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//texture2d->GetDesc(&texture2d_desc);
	//texture2d->Release();

	// �e�N�X�`���̃��[�h(��L���������W���[����)
	load_texture_from_file(filename, shader_resource_view.GetAddressOf(), &texture2d_desc);

	// ByteWidth,BindFlags,StructuerByteStride�͉Ϗ��A���̑����͂��܂�ω����邱�Ƃ͂Ȃ�
	D3D11_BUFFER_DESC buffer_desc{};			// �o�b�t�@�̎g������ݒ肷��\����
	buffer_desc.ByteWidth = sizeof(vertices);			// �o�b�t�@�̑傫��
	buffer_desc.Usage = D3D11_USAGE_DYNAMIC;			// �o�b�t�@�ւ̊e���ڂł̃A�N�Z�X�����w�� ���݂�GPU�i�ǂݎ���p�j��CPU�i�������ݐ�p�j�̗�������A�N�Z�X�ł���ݒ�
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// �o�C���h���@ ���̐ݒ蒸�_�o�b�t�@��R���X�^���g�o�b�t�@�Ƃ��Ďg�p���邱�Ƃ����߂�
	buffer_desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;// ���\�[�X�ɑ΂��Ă�CPU�A�N�Z�X���@ �O�ŃA�N�Z�X�s�v�̐ݒ�ɂȂ�B���݂͏������ݐ�p
	buffer_desc.MiscFlags = 0;							// ���̑��̃I�v�V���� �s�v�Ȃ�O���w��  ���Ȃ݂�...misc�Fmiscellaneous�̗��́B�u�G���ȁv�Ƃ����Ӗ�
	buffer_desc.StructureByteStride = 0;				// �\���̂̃T�C�Y �o�b�t�@���\���̂Ƃ��Ċm�ۂ���ꍇ�Ɏg�p �O�w��ł����삷��

	D3D11_SUBRESOURCE_DATA subresource_data{};			// �쐬����o�b�t�@�̏������f�[�^��ۑ�����\����
	subresource_data.pSysMem = vertices;				// �o�b�t�@������������f�[�^���w�� �ǂ̏��ŏ��������邩
	subresource_data.SysMemPitch = 0;					// �������̃s�b�` 2D or 3D�e�N�X�`���̏ꍇ�Ɏg�p����
	subresource_data.SysMemSlicePitch = 0;				// �[�x���x��	����

	hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer.GetAddressOf());		// �쐬����o�b�t�@���A�쐬����o�b�t�@�̏��������A�쐬�����o�b�t�@��ۑ�����|�C���^
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));	// _ASSERT_EXPR�F��������������������Ȃ���Α������̃��b�Z�[�W��\������


	//// ���̓��C�A�E�g�I�u�W�F�N�g�̐���
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

	// ���_�V�F�[�_�[�I�u�W�F�N�g�̐���
	//FILE* fp{};
	//fopen_s(&fp, vs_cso_name, "rb");	// �t�@�C���|�C���^�A�t�@�C�����Arb�F�ǂݎ���p�̃o�C�i�����[�h
	//_ASSERT_EXPR_A(fp, L"_VS.CSO File not found.");
	//fseek(fp, 0, SEEK_END);	// �t�@�C���|�C���^�A�ړ��o�C�g���A�t�@�C���̐擪(_SET)�A���݈ʒu(_CUR)�A�I�[(_END)
	//long vs_cso_sz{ ftell(fp) };	// �t�@�C���̓ǂݏ����ʒu���擾
	//fseek(fp, 0, SEEK_SET);
	//std::unique_ptr<unsigned char[]>vs_cso_data{ std::make_unique<unsigned char[]>(vs_cso_sz) };	// unique_ptr��make_unique�Ŏ��̐���
	//fread(vs_cso_data.get(), vs_cso_sz, 1, fp);	// �ǂݍ��݃f�[�^�̊i�[��A�ǂݍ��݃f�[�^�̃o�C�g�����A�ǂݍ��݃f�[�^�̐��A�t�@�C���|�C���^
	//fclose(fp);
	//hr = device->CreateVertexShader(vs_cso_data.get(), vs_cso_sz, nullptr, &vertex_shader);	// �V�F�[�_�̃|�C���^�[�A�V�F�[�_�[�T�C�Y�Adynamic linkage�Ŏg���|�C���^�A�쐬�����o�b�t�@��ۑ�����|�C���^
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	////	�����FIL(���̓��C�A�E�g)�̍\�����AIL�̗v�f���AVS�̃|�C���^�AVS�̃T�C�Y�A�쐬����IL��ۑ�����|�C���^
	//hr = device->CreateInputLayout(input_element_desc, _countof(input_element_desc), vs_cso_data.get(), vs_cso_sz, &input_layout);
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	//// ���_�V�F�[�_�[�I�u�W�F�N�g�̐���
	//create_vs_from_cso(vs_cso_name, &vertex_shader, &input_layout, input_element_desc, _countof(input_element_desc));

	// �s�N�Z���V�F�[�_�I�u�W�F�N�g�̐���
	//fopen_s(&fp, ps_cso_name, "rb");	// �t�@�C���|�C���^�A�t�@�C�����Arb�F�ǂݎ���p�̃o�C�i�����[�h
	//_ASSERT_EXPR_A(fp, L"_PS.CSO File not found.");
	//fseek(fp, 0, SEEK_END);	// �t�@�C���|�C���^�A�ړ��o�C�g���A�t�@�C���̐擪(_SET)�A���݈ʒu(_CUR)�A�I�[(_END)
	//long ps_cso_sz{ ftell(fp) };	// �t�@�C���̓ǂݏ����ʒu���擾
	//fseek(fp, 0, SEEK_SET);
	//std::unique_ptr<unsigned char[]>ps_cso_data{ std::make_unique<unsigned char[]>(ps_cso_sz) };	// unique_ptr��make_unique�Ŏ��̐���
	//fread(ps_cso_data.get(), ps_cso_sz, 1, fp);	// �ǂݍ��݃f�[�^�̊i�[��A�ǂݍ��݃f�[�^�̃o�C�g�����A�ǂݍ��݃f�[�^�̐��A�t�@�C���|�C���^
	//fclose(fp);
	//hr = device->CreatePixelShader(ps_cso_data.get(), ps_cso_sz, nullptr, &pixel_shader);	// �V�F�[�_�̃|�C���^�[�A�V�F�[�_�[�T�C�Y�Adynamic linkage�Ŏg���|�C���^�A�쐬�����o�b�t�@��ۑ�����|�C���^
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//// �s�N�Z���V�F�[�_�I�u�W�F�N�g�̐���
	//create_ps_from_cso(ps_cso_name, &pixel_shader);

	// ���X�^���C�U�I�u�W�F�N�g�̐���
	D3D11_RASTERIZER_DESC rasterizer_desc{};
	/*-----�h��Ԃ� �O�ʕ`��-----*/
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;	// �����_�����O�Ɏg���h��Ԃ����[�h D3D11_FILL_SOLID|D3D11_FILL_WIREFRAME
	rasterizer_desc.CullMode = D3D11_CULL_BACK;	// �`�悷��@������ D3D11_CULL_NONE(���ʕ`��)|D3D11_CULL_FRONT(��ʕ`��)|D3D11_CULL_BACK(�O�ʕ`��)
	rasterizer_desc.FrontCounterClockwise = FALSE;	// �O�p�`���O�ʂ��w�ʂ������肷�� TRUE�̎��A���_�����Ύ��肾�ƑO�����Ƃ݂Ȃ����
	rasterizer_desc.DepthBias = 0;					// �[�x�o�C�A�X ����[�x��ɕ\������Ƃ��ɗD��x�����߂�̂Ɏg�p�����肷��
	rasterizer_desc.DepthBiasClamp = 0;			// ��L���l     �s�N�Z���̍ő�[�x�o�C�A�X
	rasterizer_desc.SlopeScaledDepthBias = 0;		// ��L���l     ����̃s�N�Z���̌X���̃X�J���[
	rasterizer_desc.DepthClipEnable = TRUE;		// �����Ɋ�Â��ăN���b�s���O��L���ɂ��邩
	rasterizer_desc.ScissorEnable = FALSE;			// �V�U�[��`�J�����O���g�p���邩 �V�U�[��`�F�`��̈�̎w��ɂ悭�g����
	rasterizer_desc.MultisampleEnable = FALSE;		// �}���`�T���v�����O�A���`�G�C���A�X(MSAA)��RTV���g�p���Ă��鎞�Ature�Ŏl�ӌ`���C���A���`�G�C���A�X�Afalse�ŃA���t�@���C���A���`�G�C���A�X���g�p
													// MSAA���g�p����ɂ̓��\�[�X��������DX11_SAMPLE_DESC::Count��1����̒l��ݒ肷��K�v������
	rasterizer_desc.AntialiasedLineEnable = FALSE;	// MSAA��RTV���g�p���Ă��鎞�A�����`���MultisampleEnable��false�̎��ɃA���`�G�C���A�X��L���ɂ���
	hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	/*-----���C���[�t���[�� �O�ʕ`��-----*/
	rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	rasterizer_desc.AntialiasedLineEnable = TRUE;
	hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states[1].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	/*-----���C���[�t���[�� ���ʕ`��-----*/
	rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizer_desc.CullMode = D3D11_CULL_NONE;
	rasterizer_desc.AntialiasedLineEnable = TRUE;
	hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states[2].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	param.Pos = XMFLOAT2(0.0f, 0.0f);
	param.Size    = XMFLOAT2(texture2d_desc.Width, texture2d_desc.Height);
	param.TexPos  = XMFLOAT2(0.0f, 0.0f);
	param.TexSize = XMFLOAT2(texture2d_desc.Width, texture2d_desc.Height);
	param.Angle   = 0.0f;
	param.Color   = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
}

Sprite::~Sprite() {
	//vertex_shader->Release();
	//pixel_shader->Release();
	//input_layout->Release();
	//vertex_buffer->Release();
	//shader_resource_view->Release();

	rerease_all_textures();
}


void Sprite::CreateVertexData(Shader* shader,XMFLOAT2 pos, XMFLOAT2 size, float angle, XMFLOAT4 color
	, XMFLOAT2 TexPos, XMFLOAT2 TexSize) {
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

	XMFLOAT3 left_top    { pos.x         ,pos.y          ,0 };	// ����
	XMFLOAT3 right_top   { pos.x + size.x,pos.y          ,0 };	// �E��
	XMFLOAT3 left_bottom { pos.x         ,pos.y + size.y ,0 };	// ����
	XMFLOAT3 right_bottom{ pos.x + size.x,pos.y + size.y ,0 };	// �E��

	//// ��]������ �ȒP�Ɋ֐�������������@�A�����_���Ƃ����炵��
	//auto rotate = [](XMFLOAT3& pos, XMFLOAT2 center, float angle) {
	//	pos.x -= center.x;	// ��x���S�_�����炷
	//	pos.y -= center.y;

	//	float cos{ cosf(XMConvertToRadians(angle)) };	// Degree�Ȃ̂�Radian�ɕϊ�
	//	float sin{ sinf(XMConvertToRadians(angle)) };
	//	float tx{ pos.x };	// ��]�O�̒��_���W
	//	float ty{ pos.y };
	//	pos.x = tx * cos - sin * ty;	// ��]�̌���
	//	pos.y = tx * sin + cos * ty;

	//	pos.x += center.x;	// ���炵�����߂�
	//	pos.y += center.y;
	//};

	// ��]�̒��S����`�̒��S�_��
	XMFLOAT2 center{ 0,0 };
	center.x = pos.x + size.x * 0.5f;	// �ʒu-(�傫��/2)�Œ��_�ʒu���甼�T�C�Y������=�����ɂȂ�
	center.y = pos.y + size.y * 0.5f;
	rotate(left_top, center, angle);
	rotate(left_bottom, center, angle);
	rotate(right_top, center, angle);
	rotate(right_bottom, center, angle);

	// �X�N���[�����W�n����NDC(���K���f�o�C�X���W)�ւ̍��W�ϊ����s��
	left_top     = ConvertToNDC(left_top    , viewport);	// ���_�ʒu�A�X�N���[���̑傫��
	left_bottom  = ConvertToNDC(left_bottom , viewport);
	right_top    = ConvertToNDC(right_top   , viewport);
	right_bottom = ConvertToNDC(right_bottom, viewport);

	XMFLOAT2 TexLeft_top    { (TexPos.x)             / texture2d_desc.Width , (TexPos.y)				/ texture2d_desc.Height };
	XMFLOAT2 TexRight_top   { (TexPos.x + TexSize.x) / texture2d_desc.Width , (TexPos.y)				/ texture2d_desc.Height };
	XMFLOAT2 TexLeft_bottom { (TexPos.x)             / texture2d_desc.Width , (TexPos.y + TexSize.y)	/ texture2d_desc.Height };
	XMFLOAT2 TexRight_bottom{ (TexPos.x + TexSize.x) / texture2d_desc.Width , (TexPos.y + TexSize.y)	/ texture2d_desc.Height };


	// �v�Z���ʂŒ��_�o�b�t�@�I�u�W�F�N�g���X�V����
	// �e�N�X�`�����W�𒸓_�o�b�t�@�ɃZ�b�g����
	HRESULT hr = { S_OK };
	D3D11_MAPPED_SUBRESOURCE mapped_subrecource{};
	// mapped_subrecource��vertex_buffer�Ƀ}�b�s���O
	hr = immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subrecource);	// ���I�Ȓ萔�o�b�t�@�[�� Map ���ď������ނƂ��� D3D11_MAP_WRITE_DISCARD ���g�p����
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	Vertex* vertices{ reinterpret_cast<Vertex*>(mapped_subrecource.pData) };	// reinterpret_cast�F���肦�Ȃ��悤�ȕϊ��̂Ƃ��Ɏg�p����H
	if (vertices != nullptr) {	// ���̏㏑��
		vertices[0].position = left_top;
		vertices[1].position = right_top;
		vertices[2].position = left_bottom;
		vertices[3].position = right_bottom;

		for (int i = 0; i < 4; i++) {
			vertices[i].color = color;
		}

		// UV���W�𒸓_�o�b�t�@�ɃZ�b�g
		vertices[0].texcoord = TexLeft_top;
		vertices[1].texcoord = TexRight_top;
		vertices[2].texcoord = TexLeft_bottom;
		vertices[3].texcoord = TexRight_bottom;

		// �@������ݒ�
		vertices[0].normal = XMFLOAT3(0, 0, 1);
		vertices[1].normal = XMFLOAT3(0, 0, 1);
		vertices[2].normal = XMFLOAT3(0, 0, 1);
		vertices[3].normal = XMFLOAT3(0, 0, 1);

	}
	immediate_context->Unmap(vertex_buffer.Get(), 0);	// �}�b�s���O���� ���_�o�b�t�@���㏑��������K�����s�BMap&Unmap�̓Z�b�g�Ŏg�p����

	// SRV�o�C���h
	immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());	// ���W�X�^�ԍ��A�V�F�[�_���\�[�X�̐��ASRV�̃|�C���^

	// ���_�o�b�t�@�̃o�C���h
	UINT stride{ sizeof(Vertex) };
	UINT offset{ 0 };
	immediate_context->IASetVertexBuffers(
		0,								// ���̓X���b�g�̊J�n�ԍ�
		1,								// ���_�o�b�t�@�̐�
		vertex_buffer.GetAddressOf(),	// ���_�o�b�t�@�̔z��
		&stride,						// �P���_�̃T�C�Y�̔z��
		&offset);						// ���_�o�b�t�@�̊J�n�ʒu�����炷�I�t�Z�b�g�̔z��

	//�v���~�e�B�u�^�C�v�y�уf�[�^�̏����Ɋւ�����̃o�C���h
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);	// �v���~�e�B�u�̌`����w��ł���H ����͘A���O�p�`�ɕύX

	// �V�F�[�_�̗L����
	shader->Activate();
	//// ���̓��C�A�E�g�I�u�W�F�N�g�̃o�C���h
	//immediate_context->IASetInputLayout(input_layout.Get());	// ���̓��C�A�E�g�̐ݒ�

	//// �V�F�[�_�̃o�C���h
	//immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
	//immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

	// ���X�^���C�U�X�e�[�g�̐ݒ�
	immediate_context->RSSetState(rasterizer_states[0].Get());
	// �v���~�e�B�u�̕`��
	immediate_context->Draw(4, 0);	// ���_�̐��A�`��J�n���_�Ŏg�����_�o�b�t�@�̍ŏ��̃C���f�b�N�X
	// �V�F�[�_�̖�����
	shader->Inactivate();
}

void Sprite::Render(Shader* shader,XMFLOAT2 pos, XMFLOAT2 size, float angle, XMFLOAT4 color, XMFLOAT2 TexPos, XMFLOAT2 TexSize) {
	CreateVertexData(shader, pos, size, angle, color, TexPos, TexSize);
}

void Sprite::Render(Shader* shader) {
	CreateVertexData(shader, param.Pos, param.Size, param.Angle, param.Color, param.TexPos, param.TexSize);
}

void Sprite::Render(Shader* shader, XMFLOAT2 Pos, XMFLOAT2 Size) {
	CreateVertexData(shader, Pos, Size, param.Angle, param.Color, param.TexPos, param.TexSize);
}

//void Sprite::Text_Out(ID3D11DeviceContext* immediate_context, std::string s, XMFLOAT2 pos, XMFLOAT2 size, XMFLOAT4 color) {
//	XMFLOAT2 TexPos(static_cast<float>(texture2d_desc.Width / 16), static_cast<float>(texture2d_desc.Height / 16));
//	float carriage = 0;
//	for (const char c : s) {
//		Render(immediate_context, XMFLOAT2(pos.x + carriage, pos.y), size, 0, color,
//			XMFLOAT2(TexPos.x * (c & 0x0F), TexPos.y * (c >> 4)), TexPos);
//		carriage += size.x;
//	}
//}

XMFLOAT3 Sprite::ConvertToNDC(XMFLOAT3 pos, D3D11_VIEWPORT viewport) {
	pos.x = (pos.x * 2 / viewport.Width) - 1.0f;	// x�l���Q�{�A���̌�X�N���[���T�C�Y�Ŋ����ĂP�������Ɛ��K�������
	pos.y = 1.0f - (pos.y * 2.0f / viewport.Height);	// y�l���Q�{�A�X�N���[���T�C�Y�Ŋ��������̂łP�������Ɛ��K��	x�ƈႤ�̂͂����炭����E����W�n�̊֌W
	// �����sprite(�摜)�Ȃ̂�z�l�͕ύX����K�v�Ȃ�
	return pos;
}

void Sprite::ImguiWindow() {
	static float pos[2]     { param.Pos.x    ,param.Pos.y };
	static float size[2]    { param.Size.x   ,param.Size.y };
	static float angle      { param.Angle};
	static float TexPos[2]  { param.TexPos.x ,param.TexPos .y };
	static float TexSize[2] { param.TexSize.x,param.TexSize.y };
	static float Color[4] = { param.Color.x  ,param.Color.y,param.Color.z,param.Color.w };

	ImGui::Begin("Sprite_param");

	ImGui::SliderFloat2("Position", pos, -1280, 720);
	ImGui::SliderFloat2("Size", size, 0, 1960);
	ImGui::SliderFloat2("TexPos", TexPos, 0, 1960);
	ImGui::SliderFloat2("TexSize", TexSize, 0, 1960);
	ImGui::SliderFloat("angle", &angle, 0, 360);
	ImGui::ColorEdit4(u8"Color", (float*)&Color);

	ImGui::End();

	setPos(DirectX::XMFLOAT2(pos[0], pos[1]));
	setSize(DirectX::XMFLOAT2(size[0], size[1]));
	setAngle(angle);
	setTexPos(DirectX::XMFLOAT2(TexPos[0], TexPos[1]));
	setTexSize(DirectX::XMFLOAT2(TexSize[0], TexSize[1]));
	setColor(DirectX::XMFLOAT4(Color[0], Color[1], Color[2], Color[3]));

}


XMFLOAT2 Sprite::Division(XMFLOAT2 val1, XMFLOAT2 val2) {
	XMFLOAT2 valOut;
	valOut.x = val1.x / val2.x;
	valOut.y = val1.y / val2.y;
	return valOut;
}