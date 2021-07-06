#include "Sprite_Batch.h"
#include "misc.h"
#include <sstream>
#include <WICTextureLoader.h>


// ���_���̃Z�b�g
//vFormat_t vertices[]
//{
//	{{-1.0,+1.0,0},{1,1,1,1},{0,0}},	// ����,RGB�F,UV���W
//	{{+1.0,+1.0,0},{1,1,1,1},{1,0}},	// �E��,RGB�F,UV���W
//	{{-1.0,-1.0,0},{1,1,1,1},{0,1}},	// ����,RGB�F,UV���W
//	{{+1.0,-1.0,0},{1,1,1,1},{1,1}},	// �E��,RGB�F,UV���W
//};


// ���_�o�b�t�@�I�u�W�F�N�g�̐���
sprite_Batch::sprite_Batch(ID3D11Device* device, const wchar_t* filename, size_t max_sprites) :max_vertices(max_sprites * 6)
{
	HRESULT hr{ S_OK };

	std::unique_ptr<vFormat_t[]> vertices{ std::make_unique<vFormat_t[]>(max_vertices) };

	// �摜�t�@�C���̃��[�h��SRV�I�u�W�F�N�g�̐���
	ID3D11Resource* resource{};
	hr = DirectX::CreateWICTextureFromFile(device, filename, &resource, &shader_resource_view);	// �������ǂݍ��܂��΃��\�[�X��SRV�����������
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	resource->Release();

	// �e�N�X�`�����̎擾
	ID3D11Texture2D* texture2d{};
	hr = resource->QueryInterface<ID3D11Texture2D>(&texture2d);	// ����̃C���^�[�t�F�C�X���T�|�[�g���Ă��邩�𔻕ʂ���
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	texture2d->GetDesc(&texture2d_desc);
	texture2d->Release();

	// ByteWidth,BindFlags,StructuerByteStride�͉Ϗ��A���̑����͂��܂�ω����邱�Ƃ͂Ȃ�
	D3D11_BUFFER_DESC buffer_desc{};			                // �o�b�t�@�̎g������ݒ肷��\����
	buffer_desc.ByteWidth = sizeof(vFormat_t) * max_vertices;	// �o�b�t�@�̑傫��
	buffer_desc.Usage = D3D11_USAGE_DYNAMIC;			        // �o�b�t�@�ւ̊e���ڂł̃A�N�Z�X�����w�� ���݂�GPU�i�ǂݎ���p�j��CPU�i�������ݐ�p�j�̗�������A�N�Z�X�ł���ݒ�
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	        // �o�C���h���@ ���̐ݒ蒸�_�o�b�t�@��R���X�^���g�o�b�t�@�Ƃ��Ďg�p���邱�Ƃ����߂�
	buffer_desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;        // ���\�[�X�ɑ΂��Ă�CPU�A�N�Z�X���@ �O�ŃA�N�Z�X�s�v�̐ݒ�ɂȂ�B���݂͏������ݐ�p
	buffer_desc.MiscFlags = 0;							        // ���̑��̃I�v�V���� �s�v�Ȃ�O���w��  ���Ȃ݂�...misc�Fmiscellaneous�̗��́B�u�G���ȁv�Ƃ����Ӗ�
	buffer_desc.StructureByteStride = 0;				        // �\���̂̃T�C�Y �o�b�t�@���\���̂Ƃ��Ċm�ۂ���ꍇ�Ɏg�p �O�w��ł����삷��

	D3D11_SUBRESOURCE_DATA subresource_data{};			        // �쐬����o�b�t�@�̏������f�[�^��ۑ�����\����
	subresource_data.pSysMem = vertices.get();			        // �o�b�t�@������������f�[�^���w�� �ǂ̏��ŏ��������邩
	subresource_data.SysMemPitch = 0;					        // �������̃s�b�` 2D or 3D�e�N�X�`���̏ꍇ�Ɏg�p����
	subresource_data.SysMemSlicePitch = 0;				        //	�[�x���x��	 ����

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
		D3D11_APPEND_ALIGNED_ELEMENT,	// �v�f����擪�܂ł̃I�t�Z�b�g�l	�e�f�[�^�̔z��擪�����o�C�g����Ă��邩�B
										// D3D11_APPEND_ALIGNED_ELEMENT���w��ŃI�t�Z�b�g�l�������v�Z ��v�Z�Ȃ�t�H�[�}�b�g�T�C�Y�����Z���Ă���
		D3D11_INPUT_PER_VERTEX_DATA,	// ���̓f�[�^�̎��	���_�f�[�^�ƃC���X�^���X�f�[�^�̂Q���
		0								// �J��Ԃ���(���_�f�[�^�̎��͂O)	��L�ŃC���X�^���X�f�[�^��ݒ肵���ꍇ�ɈӖ�������
		},
		{"COLOR"   ,0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT		,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
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

	Status.Pos = DirectX::XMFLOAT2(0.0f, 0.0f);
	Status.Size = DirectX::XMFLOAT2(texture2d_desc.Width, texture2d_desc.Height);
	Status.TexPos = DirectX::XMFLOAT2(0.0f, 0.0f);
	Status.TexSize = DirectX::XMFLOAT2(texture2d_desc.Width, texture2d_desc.Height);
	Status.Angle = 0.0f;
	Status.Color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
}

sprite_Batch::~sprite_Batch() {
	vertex_shader->Release();
	pixel_shader->Release();
	input_layout->Release();
	vertex_buffer->Release();
	shader_resource_view->Release();
}

void sprite_Batch::begin(ID3D11DeviceContext* immediate_context) {
	vertices.clear();
	// �V�F�[�_�̃o�C���h
	immediate_context->VSSetShader(vertex_shader, nullptr, 0);
	immediate_context->PSSetShader(pixel_shader, nullptr, 0);
	// SRV�o�C���h
	immediate_context->PSSetShaderResources(0, 1, &shader_resource_view);	// ���W�X�^�ԍ��A�V�F�[�_���\�[�X�̐��ASRV�̃|�C���^
}

void sprite_Batch::end(ID3D11DeviceContext* immediate_context) {

	// �e�N�X�`�����W�𒸓_�o�b�t�@�ɃZ�b�g����
	HRESULT hr = { S_OK };
	D3D11_MAPPED_SUBRESOURCE mapped_subrecource{};
	// mapped_subrecource��vertex_buffer�Ƀ}�b�s���O
	hr = immediate_context->Map(vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subrecource);	// ���I�Ȓ萔�o�b�t�@�[�� Map ���ď������ނƂ��� D3D11_MAP_WRITE_DISCARD ���g�p����
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	size_t vertex_count = vertices.size();
	_ASSERT_EXPR(max_vertices >= vertex_count, "Buffer overflow");
	vFormat_t* data{ reinterpret_cast<vFormat_t*>(mapped_subrecource.pData) };	// reinterpret_cast�F���肦�Ȃ��悤�ȕϊ��̂Ƃ��Ɏg�p����H
	if (data != nullptr) {	// ���̏㏑��
		// vertices�̃f�[�^���R�s�[
		const vFormat_t* p = vertices.data();
		memcpy_s(data, max_vertices * sizeof(vFormat_t), p, vertex_count * sizeof(vFormat_t));	// memcpy_s�FBuffer�Ԃł̃o�C�g�̃R�s�[(�R�s�[��|�C���^�A�R�s�[��T�C�Y�A�R�s�[���|�C���^�A�R�s�[���T�C�Y)
		//memcpy_s(data, max_vertices * sizeof(vFormat_t), p, vertex_count * sizeof(vFormat_t));	// memcpy_s�FBuffer�Ԃł̃o�C�g�̃R�s�[(�R�s�[��|�C���^�A�R�s�[��T�C�Y�A�R�s�[���|�C���^�A�R�s�[���T�C�Y)
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
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// �v���~�e�B�u�̌`����w��ł���H
											// D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ��3���_�AD3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP�͎l���_ �A���O�p�`
	// ���̓��C�A�E�g�I�u�W�F�N�g�̃o�C���h
	immediate_context->IASetInputLayout(input_layout);	// ���̓��C�A�E�g�̐ݒ�

	// �v���~�e�B�u�̕`��
	immediate_context->Draw(static_cast<UINT>(vertex_count), 0);	// ���_�̐��A�`��J�n���_�Ŏg�����_�o�b�t�@�̍ŏ��̃C���f�b�N�X

}

void sprite_Batch::render(ID3D11DeviceContext* immediate_context, DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size, float angle, DirectX::XMFLOAT4 color, DirectX::XMFLOAT2 sPos, DirectX::XMFLOAT2 sSize) {
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
	rotate(left_top, center, angle);
	rotate(left_bottom, center, angle);
	rotate(right_top, center, angle);
	rotate(right_bottom, center, angle);

	// �X�N���[�����W�n����NDC(���K���f�o�C�X���W)�ւ̍��W�ϊ����s��
	left_top     = ConvertToNDC(left_top, viewport);	// ���_�ʒu�A�X�N���[���̑傫��
	left_bottom  = ConvertToNDC(left_bottom, viewport);
	right_top    = ConvertToNDC(right_top, viewport);
	right_bottom = ConvertToNDC(right_bottom, viewport);

	DirectX::XMFLOAT2 TexLeft_top    { (sPos.x)           / texture2d_desc.Width , (sPos.y)			  / texture2d_desc.Height };
	DirectX::XMFLOAT2 TexRight_top   { (sPos.x + sSize.x) / texture2d_desc.Width , (sPos.y)			  / texture2d_desc.Height };
	DirectX::XMFLOAT2 TexLeft_bottom { (sPos.x)           / texture2d_desc.Width , (sPos.y + sSize.y) / texture2d_desc.Height };
	DirectX::XMFLOAT2 TexRight_bottom{ (sPos.x + sSize.x) / texture2d_desc.Width , (sPos.y + sSize.y) / texture2d_desc.Height };


	// �v�Z���ʂŒ��_�o�b�t�@�I�u�W�F�N�g���X�V����
	//				�O�p�`��2���				//
	/*				0	*----*1	 *4				*/
	/*					|   /   /|				*/
	/*					|  /   / |				*/
	/*					| /   /  |				*/
	/*				2	*	3*---*5				*/

	vertices.push_back({ { left_top.x    ,left_top.y,	 0 }, { color.x,color.y,color.z,color.w }, { TexLeft_top.x,		TexLeft_top.y	 } });	// ����
	vertices.push_back({ { right_top.x   ,right_top.y,	 0 }, { color.x,color.y,color.z,color.w }, { TexRight_top.x,	TexRight_top.y	 } });	// �E��
	vertices.push_back({ { left_bottom.x ,left_bottom.y, 0 }, { color.x,color.y,color.z,color.w }, { TexLeft_top.x,		TexLeft_bottom.y } });	// ����
	vertices.push_back({ { left_bottom.x ,left_bottom.y, 0 }, { color.x,color.y,color.z,color.w }, { TexLeft_top.x,		TexLeft_bottom.y } });	// ����
	vertices.push_back({ { right_top.x   ,right_top.y,	 0 }, { color.x,color.y,color.z,color.w }, { TexRight_top.x,	TexRight_top.y	 } });	// �E��
	vertices.push_back({ { right_bottom.x,right_bottom.y,0 }, { color.x,color.y,color.z,color.w }, { TexRight_bottom.x,	TexRight_bottom.y} });	// �E��



}

void sprite_Batch::render(ID3D11DeviceContext* immediate_context) {
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

	DirectX::XMFLOAT3 left_top{ Status.Pos.x                ,Status.Pos.y          ,0 };	// ����
	DirectX::XMFLOAT3 right_top{ Status.Pos.x + Status.Size.x,Status.Pos.y          ,0 };	// �E��
	DirectX::XMFLOAT3 left_bottom{ Status.Pos.x                ,Status.Pos.y + Status.Size.y ,0 };	// ����
	DirectX::XMFLOAT3 right_bottom{ Status.Pos.x + Status.Size.x,Status.Pos.y + Status.Size.y ,0 };	// �E��

	// ��]�̒��S����`�̒��S�_��
	DirectX::XMFLOAT2 center{ 0,0 };
	center.x = Status.Pos.x + Status.Size.x * 0.5f;	// �ʒu-(�傫��/2)�Œ��_�ʒu���甼�T�C�Y������=�����ɂȂ�
	center.y = Status.Pos.y + Status.Size.y * 0.5f;
	// ���_��]�֐�
	rotate(left_top, center, Status.Angle);
	rotate(left_bottom, center, Status.Angle);
	rotate(right_top, center, Status.Angle);
	rotate(right_bottom, center, Status.Angle);

	// �X�N���[�����W�n����NDC(���K���f�o�C�X���W)�ւ̍��W�ϊ����s��
	left_top     = ConvertToNDC(left_top    , viewport);	// ���_�ʒu�A�X�N���[���̑傫��
	left_bottom  = ConvertToNDC(left_bottom , viewport);
	right_top    = ConvertToNDC(right_top   , viewport);
	right_bottom = ConvertToNDC(right_bottom, viewport);

	// �e�N�X�`�����W
	DirectX::XMFLOAT2 TexLeft_top    { (Status.TexPos.x)                    / texture2d_desc.Width ,(Status.TexPos.y)					 / texture2d_desc.Height };
	DirectX::XMFLOAT2 TexRight_top   { (Status.TexPos.x + Status.TexSize.x) / texture2d_desc.Width ,(Status.TexPos.y)					 / texture2d_desc.Height };
	DirectX::XMFLOAT2 TexLeft_bottom { (Status.TexPos.x)                    / texture2d_desc.Width ,(Status.TexPos.y + Status.TexSize.y) / texture2d_desc.Height };
	DirectX::XMFLOAT2 TexRight_bottom{ (Status.TexPos.x + Status.TexSize.x) / texture2d_desc.Width ,(Status.TexPos.y + Status.TexSize.y) / texture2d_desc.Height };


	// �v�Z���ʂŒ��_�o�b�t�@�I�u�W�F�N�g���X�V����
	vertices.push_back({ { left_top.x    ,left_top.y,	 0 }, { Status.Color.x,Status.Color.y,Status.Color.z,Status.Color.w }, { TexLeft_top.x,		TexLeft_top.y	 } });
	vertices.push_back({ { right_top.x   ,right_top.y,	 0 }, { Status.Color.x,Status.Color.y,Status.Color.z,Status.Color.w }, { TexRight_top.x,	TexRight_top.y	 } });
	vertices.push_back({ { left_bottom.x ,left_bottom.y, 0 }, { Status.Color.x,Status.Color.y,Status.Color.z,Status.Color.w }, { TexLeft_top.x,		TexLeft_bottom.y } });
	vertices.push_back({ { left_bottom.x ,left_bottom.y, 0 }, { Status.Color.x,Status.Color.y,Status.Color.z,Status.Color.w }, { TexLeft_top.x,		TexLeft_bottom.y } });
	vertices.push_back({ { right_top.x   ,right_top.y,	 0 }, { Status.Color.x,Status.Color.y,Status.Color.z,Status.Color.w }, { TexRight_top.x,	TexRight_top.y	 } });
	vertices.push_back({ { right_bottom.x,right_bottom.y,0 }, { Status.Color.x,Status.Color.y,Status.Color.z,Status.Color.w }, { TexRight_bottom.x,	TexRight_bottom.y} });
}

DirectX::XMFLOAT3 sprite_Batch::ConvertToNDC(DirectX::XMFLOAT3 pos, D3D11_VIEWPORT viewport) {
	pos.x = (pos.x * 2 / viewport.Width) - 1.0f;	// x�l���Q�{�A���̌�X�N���[���T�C�Y�Ŋ����ĂP�������Ɛ��K�������
	pos.y = 1.0f - (pos.y * 2.0f / viewport.Height);	// y�l���Q�{�A�X�N���[���T�C�Y�Ŋ��������̂łP�������Ɛ��K��	x�ƈႤ�̂͂����炭����E����W�n�̊֌W
	// �����sprite(�摜)�Ȃ̂�z�l�͕ύX����K�v�Ȃ�
	return pos;
}

void sprite_Batch::imguiWindow() {
}

DirectX::XMFLOAT2 sprite_Batch::division(DirectX::XMFLOAT2 val1, DirectX::XMFLOAT2 val2) {
	DirectX::XMFLOAT2 valOut;
	valOut.x = val1.x / val2.x;
	valOut.y = val1.y / val2.y;
	return valOut;
}


inline void sprite_Batch::rotate(DirectX::XMFLOAT3& pos, DirectX::XMFLOAT2 center, float angle) {
	pos.x -= center.x;	// ��x���S�_�����炷
	pos.y -= center.y;

	float cos{ cosf(DirectX::XMConvertToRadians(angle)) };	// Degree�Ȃ̂�Radian�ɕϊ�
	float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
	float tx{ pos.x };	// ��]�O�̒��_���W
	float ty{ pos.y };
	pos.x = tx * cos - sin * ty;	// ��]�̌���
	pos.y = tx * sin + cos * ty;

	pos.x += center.x;	// ���炵�����߂�
	pos.y += center.y;

}