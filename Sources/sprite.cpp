#include "sprite.h"
#include "misc.h"
#include <sstream>

// ���_���̃Z�b�g

vFormat_t vertices[]
{
	{{-1.0, +1.0, 0},{1,1,1,1}},	// ���� , RGB�F
	{{+1.0, +1.0, 0},{1,0,0,1}},	// �E�� , RGB�F
	{{-1.0, -1.0, 0},{0,1,0,1}},	// ���� , RGB�F
	{{+1.0, -1.0, 0},{0,0,1,1}},	// �E�� , RGB�F
};


// ���_�o�b�t�@�I�u�W�F�N�g�̐���
Sprite::Sprite(ID3D11Device* device) {
	HRESULT hr{ S_OK };

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
	subresource_data.SysMemSlicePitch = 0;				//	�[�x���x��	 ����

	hr = device->CreateBuffer(&buffer_desc, &subresource_data, &vertex_buffer);		// �쐬����o�b�t�@���A�쐬����o�b�t�@�̏��������A�쐬�����o�b�t�@��ۑ�����|�C���^
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));	// _ASSERT_EXPR�F��������������������Ȃ���Α������̃��b�Z�[�W��\������

	// ���_�V�F�[�_�[�I�u�W�F�N�g�̐���
	const char* vs_cso_name{ "Shaders\\sprite_vs.cso" };

	FILE* fp{};
	fopen_s(&fp, vs_cso_name, "rb");	// �t�@�C���|�C���^�A�t�@�C�����Arb�F�ǂݎ���p�̃o�C�i�����[�h
	_ASSERT_EXPR_A(fp, "_PS.CSO File not found.");

	fseek(fp, 0, SEEK_END);	// �t�@�C���|�C���^�A�ړ��o�C�g���A�t�@�C���̐擪(_SET)�A���݈ʒu(_CUR)�A�I�[(_END)
	long vs_cso_sz{ ftell(fp) };	// �t�@�C���̓ǂݏ����ʒu���擾
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]>vs_cso_data{ std::make_unique<unsigned char[]>(vs_cso_sz) };	// unique_ptr��make_unique�Ŏ��̐���
	fread(vs_cso_data.get(), vs_cso_sz, 1, fp);	// �ǂݍ��݃f�[�^�̊i�[��A�ǂݍ��݃f�[�^�̃o�C�g�����A�ǂݍ��݃f�[�^�̐��A�t�@�C���|�C���^
	fclose(fp);

	hr = device->CreateVertexShader(vs_cso_data.get(), vs_cso_sz, nullptr, &vertex_shader);	// �V�F�[�_�̃|�C���^�[�A�V�F�[�_�[�T�C�Y�Adynamic linkage�Ŏg���|�C���^�A�쐬�����o�b�t�@��ۑ�����|�C���^
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// ���̓��C�A�E�g�I�u�W�F�N�g�̐���
	D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	{
		{
		"POSITION",						// �Z�}���e�B�N�X��	HLSL���̃V�O�l�`��(�^��ϐ����̑g�ݍ��킹�̎��H)�̖��O�ƈ�v�����邱�Ƃő��M�������_������M���邱�Ƃ��ł���
		0,								// �Z�}���e�B�N�X�ԍ� �����ł����ʂł���悤�ɔԍ������蓖�Ă�B�ԍ���ύX���邱�Ƃ�HLSL�ŕʂ̏�񂾂ƔF���ł���
		DXGI_FORMAT_R32G32B32_FLOAT,	// �t�H�[�}�b�g	R23G23B23�͎���float3
		0,								// ���̓X���b�g�ԍ�	�������C�A�E�g���ǂ̓��̓X���b�g�ɑ΂��Ĕ��f����邩���w�肷��
		D3D11_APPEND_ALIGNED_ELEMENT,	// �v�f����擪�܂ł̃I�t�Z�b�g�l	�e�f�[�^�̔z��擪�����{�Ɨ���Ă��邩�BD3D11_APPEND_ALIGNED_ELEMENT���w��ŃI�t�Z�b�g�l��ݒ肷��Ɠ��`
		D3D11_INPUT_PER_VERTEX_DATA,	// ���̓f�[�^�̎��	���_�f�[�^�ƃC���X�^���X�f�[�^�̂Q���
		0								// �J��Ԃ���(���_�f�[�^�̎��͂O)	��L�ŃC���X�^���X�f�[�^��ݒ肵���ꍇ�ɈӖ�������
		},
		{"COLOR",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	};

	//	�����FIL(���̓��C�A�E�g)�̍\�����AIL�̗v�f���AVS�̃|�C���^�AVS�̃T�C�Y�A�쐬����IL��ۑ�����|�C���^
	hr = device->CreateInputLayout(input_element_desc, _countof(input_element_desc), vs_cso_data.get(), vs_cso_sz, &input_layout);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// �s�N�Z���V�F�[�_�I�u�W�F�N�g�̐���
	const char* ps_cso_name{ "Shaders\\sprite_ps.cso" };

	fopen_s(&fp, ps_cso_name, "rb");	// �t�@�C���|�C���^�A�t�@�C�����Arb�F�ǂݎ���p�̃o�C�i�����[�h
	_ASSERT_EXPR_A(fp, "_VS.CSO File not found.");

	fseek(fp, 0, SEEK_END);	// �t�@�C���|�C���^�A�ړ��o�C�g���A�t�@�C���̐擪(_SET)�A���݈ʒu(_CUR)�A�I�[(_END)
	long ps_cso_sz{ ftell(fp) };	// �t�@�C���̓ǂݏ����ʒu���擾
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]>ps_cso_data{ std::make_unique<unsigned char[]>(ps_cso_sz) };	// unique_ptr��make_unique�Ŏ��̐���
	fread(ps_cso_data.get(), ps_cso_sz, 1, fp);	// �ǂݍ��݃f�[�^�̊i�[��A�ǂݍ��݃f�[�^�̃o�C�g�����A�ǂݍ��݃f�[�^�̐��A�t�@�C���|�C���^
	fclose(fp);

	hr = device->CreatePixelShader(ps_cso_data.get(), ps_cso_sz, nullptr, &pixel_shader);	// �V�F�[�_�̃|�C���^�[�A�V�F�[�_�[�T�C�Y�Adynamic linkage�Ŏg���|�C���^�A�쐬�����o�b�t�@��ۑ�����|�C���^
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

}

Sprite::~Sprite() {
	vertex_shader->Release();
	pixel_shader->Release();
	input_layout->Release();
	vertex_buffer->Release();
}

void Sprite::render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh, DirectX::XMFLOAT4 color) {
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

	DirectX::XMFLOAT3 left_top     = { dx     ,dy      ,0};	// ����
	DirectX::XMFLOAT3 left_bottom  = { dx     ,dy + dh ,0};	// ����
	DirectX::XMFLOAT3 right_top    = { dx + dw,dy      ,0};	// �E��
	DirectX::XMFLOAT3 right_bottom = { dx + dw,dy + dh ,0};	// �E��

	// �X�N���[�����W�n����NDC(���K���f�o�C�X���W)�ւ̍��W�ϊ����s��
	left_top     = Convert_Screen_to_NDC(left_top    , viewport);	// ���_�ʒu�A�X�N���[���̑傫��
	left_bottom  = Convert_Screen_to_NDC(left_bottom , viewport);
	right_top    = Convert_Screen_to_NDC(right_top   , viewport);
	right_bottom = Convert_Screen_to_NDC(right_bottom, viewport);

	// �v�Z���ʂŒ��_�o�b�t�@�I�u�W�F�N�g���X�V����
	HRESULT hr = { S_OK };
	D3D11_MAPPED_SUBRESOURCE mapped_subrecource{};
	// mapped_subrecource��vertex_buffer�Ƀ}�b�s���O
	hr = immediate_context->Map(vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subrecource);	// ���I�Ȓ萔�o�b�t�@�[�� Map ���ď������ނƂ��� D3D11_MAP_WRITE_DISCARD ���g�p����
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	vFormat_t* vertices{ reinterpret_cast<vFormat_t*>(mapped_subrecource.pData) };	// reinterpret_cast�F���肦�Ȃ��悤�ȕϊ��̂Ƃ��Ɏg�p����H
	if (vertices != nullptr) {	// ���̏㏑��
		vertices[0].position = left_top;
		vertices[1].position = right_top;
		vertices[2].position = left_bottom;
		vertices[3].position = right_bottom;

		for (int i = 0; i < 4; i++) {
			vertices[i].color = color;
		}
	}
	immediate_context->Unmap(vertex_buffer, 0);	// �}�b�s���O���� ���_�o�b�t�@���㏑��������K�����s�BMap&Unmap�̓Z�b�g�Ŏg�p����

	// ���_�o�b�t�@�̃o�C���h
	UINT stride{ sizeof(vFormat_t) };
	UINT offset{ 0 };
	immediate_context->IASetVertexBuffers(
		0,				// ���̓X���b�g�̊J�n�ԍ�
		1,				// ���_�o�b�t�@�̐�
		&vertex_buffer,	// ���_�o�b�t�@�̔z��
		&stride,		// �P���_�̃T�C�Y�̔z��
		&offset);		// ���_�o�b�t�@�̊J�n�ʒu�����炷�I�t�Z�b�g�̔z��

	//�v���~�e�B�u�^�C�v�y�уf�[�^�̏����Ɋւ�����̃o�C���h
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);	// �v���~�e�B�u�̌`����w��ł���H ����͘A���O�p�`�ɕύX

	// ���̓��C�A�E�g�I�u�W�F�N�g�̃o�C���h
	immediate_context->IASetInputLayout(input_layout);	// ���̓��C�A�E�g�̐ݒ�

	// �V�F�[�_�̃o�C���h
	immediate_context->VSSetShader(vertex_shader, nullptr, 0);
	immediate_context->PSSetShader(pixel_shader, nullptr, 0);

	// �v���~�e�B�u�̕`��
	immediate_context->Draw(4, 0);	// ���_�̐��A�`��J�n���_�Ŏg�����_�o�b�t�@�̍ŏ��̃C���f�b�N�X



}

DirectX::XMFLOAT3 Sprite::Convert_Screen_to_NDC(DirectX::XMFLOAT3 pos, D3D11_VIEWPORT viewport) {
	pos.x = (pos.x * 2 / viewport.Width) - 1.0f;	// x�l���Q�{�A���̌�X�N���[���T�C�Y�Ŋ����ĂP�������Ɛ��K�������
	pos.y = 1.0f - (pos.y * 2.0f / viewport.Height);	// y�l���Q�{�A�X�N���[���T�C�Y�Ŋ��������̂łP�������Ɛ��K��	x�ƈႤ�̂͂����炭����E����W�n�̊֌W
	// �����sprite(�摜)�Ȃ̂�z�l�͕ύX����K�v�Ȃ�
	return pos;
}