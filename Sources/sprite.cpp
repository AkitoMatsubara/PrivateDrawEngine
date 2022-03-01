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
Sprite::Sprite()
{
	ID3D11Device* device = FRAMEWORK->GetDevice();
	HRESULT hr{ S_OK };

	// ByteWidth,BindFlags,StructuerByteStride�͉Ϗ��A���̑����͂��܂�ω����邱�Ƃ͂Ȃ�
	D3D11_BUFFER_DESC buffer_desc{};					// �o�b�t�@�̎g������ݒ肷��\����
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

	if (!SpriteShader)	// ��񂾂�����
	{
		SpriteShader = std::make_unique<ShaderEx>();
		SpriteShader->CreateVS(L"Shaders\\sprite_vs");
		SpriteShader->CreatePS(L"Shaders\\sprite_ps");
	}

	param = std::make_unique<Object2d>();

	param->Pos =DirectX::SimpleMath::Vector2(0.0f, 0.0f);
	param->TexPos  =DirectX::SimpleMath::Vector2(0.0f, 0.0f);
	param->Angle   = 0.0f;
	param->Color   =DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
}

Sprite::~Sprite()
{
	for (auto& path : FontList)
	{
		// �V�X�e������t�H���g���폜����
		if (RemoveFontResourceEx(path, FR_PRIVATE, NULL) == 0)
		{
			_ASSERT_EXPR_A(false, "Delete Font Failed.");	// �t�H���g�폜���s
		}
	}
	FontList.clear();
}

void Sprite::LoadImages(const wchar_t* filename)
{
	HRESULT hr;
	// �e�N�X�`���̃��[�h
	hr = load_texture_from_file(filename, shader_resource_view.GetAddressOf(), &texture2d_desc);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));	// _ASSERT_EXPR�F��������������������Ȃ���Α������̃��b�Z�[�W��\������
	// �T�C�Y�Ȃǂ͉摜����擾���Ă��邽�߂����Őݒ肷��
	param->Size = DirectX::SimpleMath::Vector2(static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
	param->TexSize = DirectX::SimpleMath::Vector2(static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
}

void Sprite::CreateVertexData(Shader* shader,DirectX::SimpleMath::Vector2 pos,DirectX::SimpleMath::Vector2 size, float angle,
	DirectX::SimpleMath::Vector4 color,DirectX::SimpleMath::Vector2 TexPos,DirectX::SimpleMath::Vector2 TexSize) {
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

	DirectX::SimpleMath::Vector3 left_top    { pos.x         ,pos.y          ,1.0f };	// ����
	DirectX::SimpleMath::Vector3 right_top   { pos.x + size.x,pos.y          ,1.0f };	// �E��
	DirectX::SimpleMath::Vector3 left_bottom { pos.x         ,pos.y + size.y ,1.0f };	// ����
	DirectX::SimpleMath::Vector3 right_bottom{ pos.x + size.x,pos.y + size.y ,1.0f };	// �E��

	// ��]�̒��S����`�̒��S�_��
	DirectX::SimpleMath::Vector2 center{ 0,0 };
	center.x = pos.x + size.x * 0.5f;	// �ʒu-(�傫��/2)�Œ��_�ʒu���甼�T�C�Y������=�����ɂȂ�
	center.y = pos.y + size.y * 0.5f;
	// ���_��]
	SpriteMath::rotate(left_top    , center, angle);
	SpriteMath::rotate(left_bottom , center, angle);
	SpriteMath::rotate(right_top   , center, angle);
	SpriteMath::rotate(right_bottom, center, angle);

	// �X�N���[�����W�n����NDC(���K���f�o�C�X���W)�ւ̍��W�ϊ����s��
	left_top     = SpriteMath::ConvertToNDC(left_top    , viewport);	// ���_�ʒu�A�X�N���[���̑傫��
	left_bottom  = SpriteMath::ConvertToNDC(left_bottom , viewport);
	right_top    = SpriteMath::ConvertToNDC(right_top   , viewport);
	right_bottom = SpriteMath::ConvertToNDC(right_bottom, viewport);

	const DirectX::SimpleMath::Vector2 TexLeft_top    { (TexPos.x)             / texture2d_desc.Width , (TexPos.y)				/ texture2d_desc.Height };
	const DirectX::SimpleMath::Vector2 TexRight_top   { (TexPos.x + TexSize.x) / texture2d_desc.Width , (TexPos.y)				/ texture2d_desc.Height };
	const DirectX::SimpleMath::Vector2 TexLeft_bottom { (TexPos.x)             / texture2d_desc.Width , (TexPos.y + TexSize.y)	/ texture2d_desc.Height };
	const DirectX::SimpleMath::Vector2 TexRight_bottom{ (TexPos.x + TexSize.x) / texture2d_desc.Width , (TexPos.y + TexSize.y)	/ texture2d_desc.Height };


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
		vertices[0].normal =DirectX::SimpleMath::Vector3(0, 0, 1);
		vertices[1].normal =DirectX::SimpleMath::Vector3(0, 0, 1);
		vertices[2].normal =DirectX::SimpleMath::Vector3(0, 0, 1);
		vertices[3].normal =DirectX::SimpleMath::Vector3(0, 0, 1);

	}
	immediate_context->Unmap(vertex_buffer.Get(), 0);	// �}�b�s���O���� ���_�o�b�t�@���㏑��������K�����s�BMap&Unmap�̓Z�b�g�Ŏg�p����


	// �V�F�[�_�̗L����
	shader->Activate();

	// SRV�o�C���h
	immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());	// ���W�X�^�ԍ��A�V�F�[�_���\�[�X�̐��ASRV�̃|�C���^

	// ���_�o�b�t�@�̃o�C���h
	const UINT stride{ sizeof(Vertex) };
	const UINT offset{ 0 };
	immediate_context->IASetVertexBuffers(
		0,								// ���̓X���b�g�̊J�n�ԍ�
		1,								// ���_�o�b�t�@�̐�
		vertex_buffer.GetAddressOf(),	// ���_�o�b�t�@�̔z��
		&stride,						// �P���_�̃T�C�Y�̔z��
		&offset);				// ���_�o�b�t�@�̊J�n�ʒu�����炷�I�t�Z�b�g�̔z��

	//�v���~�e�B�u�^�C�v�y�уf�[�^�̏����Ɋւ�����̃o�C���h
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);	// �v���~�e�B�u�̌`����w��ł��� ����͘A���O�p�`�ɕύX

	// ���X�^���C�U�X�e�[�g�̐ݒ�
	immediate_context->RSSetState(FRAMEWORK->GetRasterizerState(FRAMEWORK->RS_SOLID_BACK));
	// �v���~�e�B�u�̕`��
	immediate_context->Draw(4, 0);	// ���_�̐��A�`��J�n���_�Ŏg�����_�o�b�t�@�̍ŏ��̃C���f�b�N�X
	// �V�F�[�_�̖�����
	shader->Inactivate();
}

//void Sprite::Render(Shader* shader,DirectX::SimpleMath::Vector2 pos,DirectX::SimpleMath::Vector2 size, float angle,DirectX::SimpleMath::Vector4 color,DirectX::SimpleMath::Vector2 TexPos,DirectX::SimpleMath::Vector2 TexSize) {
//	CreateVertexData(shader, pos, size, angle, color, TexPos, TexSize);
//}

void Sprite::Render(Shader* shader) {
	CreateVertexData(shader, param->Pos, param->Size, param->Angle, param->Color, param->TexPos, param->TexSize);
}

//void Sprite::Render(Shader* shader,DirectX::SimpleMath::Vector2 Pos,DirectX::SimpleMath::Vector2 Size) {
//	CreateVertexData(shader, Pos, Size, param->Angle, param->Color, param->TexPos, param->TexSize);
//}

void Sprite::ImguiWindow() {
	static float pos[2]     { param->Pos.x    ,param->Pos.y };
	static float size[2]    { param->Size.x   ,param->Size.y };
	static float angle      { param->Angle};
	static float TexPos[2]  { param->TexPos.x ,param->TexPos .y };
	static float TexSize[2] { param->TexSize.x,param->TexSize.y };
	static float Color[4] = { param->Color.x  ,param->Color.y,param->Color.z,param->Color.w };

	ImGui::Begin("Sprite_param");

	ImGui::SliderFloat2("Position", pos, -1280, 720);
	ImGui::SliderFloat2("Size", size, 0, 1960);
	ImGui::SliderFloat2("TexPos", TexPos, 0, 1960);
	ImGui::SliderFloat2("TexSize", TexSize, 0, 1960);
	ImGui::SliderFloat("angle", &angle, 0, 360);
	ImGui::ColorEdit4("Color", (float*)&Color);

	ImGui::End();

	setPos    (DirectX::SimpleMath::Vector2(pos[0], pos[1]));
	setSize   (DirectX::SimpleMath::Vector2(size[0], size[1]));
	setRotate(angle);
	setTexPos (DirectX::SimpleMath::Vector2(TexPos[0], TexPos[1]));
	setTexSize(DirectX::SimpleMath::Vector2(TexSize[0], TexSize[1]));
	setColor  (DirectX::SimpleMath::Vector4(Color[0], Color[1], Color[2], Color[3]));

}


DirectX::SimpleMath::Vector2 Sprite::Division(DirectX::SimpleMath::Vector2 val1,DirectX::SimpleMath::Vector2 val2) {
	DirectX::SimpleMath::Vector2 valOut;
	valOut.x = val1.x / val2.x;
	valOut.y = val1.y / val2.y;
	return valOut;
}


void FontRender(float x, float y, std::string string, int size, DirectX::XMFLOAT4 color)
{
	/* �^�ϊ� */
	std::wstring w_str;
	size_t wcs_buff_size = string.length() + 1;	// �����񒷎Z�o
	wchar_t* wcs = new wchar_t[wcs_buff_size];	// �����񒷕���char���m��

	// �o�͐�A�����񒷁A������
	mbstowcs_s(nullptr, wcs, wcs_buff_size, string.c_str(), _TRUNCATE);

	w_str = wcs;	// �ۑ�
	delete[] wcs;	// ����Ȃ��̂ō폜

	// �`��E��������
	float rx = x;
	float ry = y;
	int rw = 0, rh = 0;
	int before_char_rw = 0, y_cnt = 0;
	std::wstring now_char;	// �`�悷�镶��

	for (int i = 0; i < w_str.length(); ++i)
	{
		// �����������[�ł���Ύ��s���Ȃ�
		if (w_str[i] == '\0') return;

		if (w_str[i] == '\n')	// ���s�R�[�h�ł����
		{
			before_char_rw = 0;
			++y_cnt;

			rx = 0.0f;
			ry = y + (y_cnt * rh);
			continue;
		}

		//�����X�v���C�g���擾
		now_char = w_str[i];	// ���`��Ώۂ̕���
		//// ������̉摜��ێ������N���X���Ăяo��
		//�X�v���C�gclass* spr = Get_FontData(now_char.c_str(), size);	// �`�敶���A�T�C�Y

		////�`��T�C�Y����
		//float exrate = size / static_cast<float>(font_data_[now_char].font_size_);	// �t�H���g�T�C�Y�ɂ���Ē���

		////�`�敝�A����
		//rw = static_cast<int>(spr->TextureWidth() * exrate);	// �Y���C��
		//rh = static_cast<int>(spr->TextureHeight() * exrate);

		////�`��ʒu
		//rx += before_char_rw; // �������m�ۂ������W

		//spr->�`��(rx, ry, rw, rh, color);	// �����`��

		//before_char_rw = rw; //�ύX
	}
}