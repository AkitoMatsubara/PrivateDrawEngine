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

	hr = device->CreateBuffer(&buffer_desc, &subresource_data, VertexBuffer.GetAddressOf());		// �쐬����o�b�t�@���A�쐬����o�b�t�@�̏��������A�쐬�����o�b�t�@��ۑ�����|�C���^
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));	// _ASSERT_EXPR�F��������������������Ȃ���Α������̃��b�Z�[�W��\������

	if (!SpriteShader)	// ��񂾂�����
	{
		SpriteShader = std::make_unique<ShaderEx>();
		SpriteShader->CreateVS(L"Shaders\\sprite_vs");
		SpriteShader->CreatePS(L"Shaders\\sprite_ps");
	}
	SpriteTexture = std::make_unique<Texture>();

	Parameter = std::make_unique<Object2d>();

	Parameter->Pos =DirectX::SimpleMath::Vector2(0.0f, 0.0f);
	Parameter->TexPos  =DirectX::SimpleMath::Vector2(0.0f, 0.0f);
	Parameter->Angle   = 0.0f;
	Parameter->Color   =DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
}

Sprite::~Sprite() {}

void Sprite::LoadImages(const wchar_t* filename)
{
	// �e�N�X�`���̃��[�h
	SpriteTexture->Load(filename);

	//// �T�C�Y�Ȃǂ͉摜����擾���Ă��邽�߂����Őݒ肷��
	Parameter->Size = DirectX::SimpleMath::Vector2(static_cast<float>(SpriteTexture->GetWidth()), static_cast<float>(SpriteTexture->GetHeight()));
	Parameter->TexSize = DirectX::SimpleMath::Vector2(static_cast<float>(SpriteTexture->GetWidth()), static_cast<float>(SpriteTexture->GetHeight()));
	//hr = load_texture_from_file(filename, shader_resource_view.GetAddressOf(), &texture2d_desc);
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));	// _ASSERT_EXPR�F��������������������Ȃ���Α������̃��b�Z�[�W��\������
	//Parameter->Size = DirectX::SimpleMath::Vector2(static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
	//Parameter->TexSize = DirectX::SimpleMath::Vector2(static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
}

void Sprite::CreateVertexData(ID3D11DeviceContext* device_context, Texture* texture, Shader* shader, ID3D11CommandList** comand_list) {
	// �X�N���[��(�r���[�|�[�g)�̃T�C�Y���擾����
	D3D11_VIEWPORT viewport{};
	UINT num_viewports{ 1 };
	device_context->RSGetViewports(&num_viewports, &viewport);

	// ���������`�̊e���_�̈ʒu���v�Z����
	/*		left_top	*----*	right_top			*/
	/*					|   /|						*/
	/*					|  / |						*/
	/*					| /  |						*/
	/*		left_bottom	*----*	right_bottom		*/

	DirectX::SimpleMath::Vector3 left_top    { Parameter->Pos.x         ,Parameter->Pos.y          , 1.0f };	// ����
	DirectX::SimpleMath::Vector3 right_top   { Parameter->Pos.x + Parameter->Size.x,Parameter->Pos.y          , 1.0f };	// �E��
	DirectX::SimpleMath::Vector3 left_bottom { Parameter->Pos.x         ,Parameter->Pos.y + Parameter->Size.y , 1.0f };	// ����
	DirectX::SimpleMath::Vector3 right_bottom{ Parameter->Pos.x + Parameter->Size.x,Parameter->Pos.y + Parameter->Size.y , 1.0f };	// �E��

	// ��]�̒��S����`�̒��S�_��
	DirectX::SimpleMath::Vector2 center{ 0,0 };
	center.x = Parameter->Pos.x + Parameter->Size.x * 0.5f;	// �ʒu-(�傫��/2)�Œ��_�ʒu���甼�T�C�Y������=�����ɂȂ�
	center.y = Parameter->Pos.y + Parameter->Size.y * 0.5f;
	// ���_��]
	SpriteMath::rotate(left_top    , center, Parameter->Angle);
	SpriteMath::rotate(left_bottom , center, Parameter->Angle);
	SpriteMath::rotate(right_top   , center, Parameter->Angle);
	SpriteMath::rotate(right_bottom, center, Parameter->Angle);

	// �X�N���[�����W�n����NDC(���K���f�o�C�X���W)�ւ̍��W�ϊ����s��
	left_top     = SpriteMath::ConvertToNDC(left_top    , viewport);	// ���_�ʒu�A�X�N���[���̑傫��
	left_bottom  = SpriteMath::ConvertToNDC(left_bottom , viewport);
	right_top    = SpriteMath::ConvertToNDC(right_top   , viewport);
	right_bottom = SpriteMath::ConvertToNDC(right_bottom, viewport);

	// �e�N�X�`���̎w��̗L���ŕς��Ă���
	UINT width;
	UINT height;
	if (texture) {
		width  = texture->GetWidth();
		height = texture->GetHeight();
	}
	else {
		width  = SpriteTexture->GetWidth();
		height = SpriteTexture->GetHeight();
	}

	const DirectX::SimpleMath::Vector2 tex_left_top    { (Parameter->TexPos.x)                    / width, (Parameter->TexPos.y)						/ height };
	const DirectX::SimpleMath::Vector2 tex_right_top   { (Parameter->TexPos.x + Parameter->TexSize.x) / width, (Parameter->TexPos.y)						/ height };
	const DirectX::SimpleMath::Vector2 tex_left_bottom { (Parameter->TexPos.x)                    / width, (Parameter->TexPos.y + Parameter->TexSize.y)	/ height };
	const DirectX::SimpleMath::Vector2 tex_right_bottom{ (Parameter->TexPos.x + Parameter->TexSize.x) / width, (Parameter->TexPos.y + Parameter->TexSize.y)	/ height };


	// �v�Z���ʂŒ��_�o�b�t�@�I�u�W�F�N�g���X�V����
	// �e�N�X�`�����W�𒸓_�o�b�t�@�ɃZ�b�g����
	HRESULT hr = { S_OK };
	D3D11_MAPPED_SUBRESOURCE mapped_subrecource{};
	// mapped_subrecource��vertex_buffer�Ƀ}�b�s���O
	hr = device_context->Map(VertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subrecource);	// ���I�Ȓ萔�o�b�t�@�[�� Map ���ď������ނƂ��� D3D11_MAP_WRITE_DISCARD ���g�p����
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	Vertex* vertices{ reinterpret_cast<Vertex*>(mapped_subrecource.pData) };	// reinterpret_cast�F���肦�Ȃ��悤�ȕϊ��̂Ƃ��Ɏg�p����H
	if (vertices != nullptr) {	// ���̏㏑��
		vertices[0].position = left_top;
		vertices[1].position = right_top;
		vertices[2].position = left_bottom;
		vertices[3].position = right_bottom;

		for (int i = 0; i < 4; i++) {
			vertices[i].color = Parameter->Color;
		}

		// UV���W�𒸓_�o�b�t�@�ɃZ�b�g
		vertices[0].texcoord = tex_left_top;
		vertices[1].texcoord = tex_right_top;
		vertices[2].texcoord = tex_left_bottom;
		vertices[3].texcoord = tex_right_bottom;

		// �@������ݒ�
		vertices[0].normal =DirectX::SimpleMath::Vector3(0, 0, 1);
		vertices[1].normal =DirectX::SimpleMath::Vector3(0, 0, 1);
		vertices[2].normal =DirectX::SimpleMath::Vector3(0, 0, 1);
		vertices[3].normal =DirectX::SimpleMath::Vector3(0, 0, 1);

	}
	device_context->Unmap(VertexBuffer.Get(), 0);	// �}�b�s���O���� ���_�o�b�t�@���㏑��������K�����s�BMap&Unmap�̓Z�b�g�Ŏg�p����


	// �V�F�[�_�̗L����
	shader->Activate(device_context);

	// �e�N�X�`�����O������̎w�肪����΂������SRV�ɃZ�b�g����
	(texture) ? texture->Set(device_context, 0) : SpriteTexture->Set(device_context, 0);

	// ���_�o�b�t�@�̃o�C���h
	const UINT stride{ sizeof(Vertex) };
	const UINT offset{ 0 };
	device_context->IASetVertexBuffers(
		0,								// ���̓X���b�g�̊J�n�ԍ�
		1,								// ���_�o�b�t�@�̐�
		VertexBuffer.GetAddressOf(),	// ���_�o�b�t�@�̔z��
		&stride,						// �P���_�̃T�C�Y�̔z��
		&offset);						// ���_�o�b�t�@�̊J�n�ʒu�����炷�I�t�Z�b�g�̔z��

	//�v���~�e�B�u�^�C�v�y�уf�[�^�̏����Ɋւ�����̃o�C���h
	device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);	// �v���~�e�B�u�̌`����w��ł��� ����͘A���O�p�`�ɕύX

	// ���X�^���C�U�X�e�[�g�̐ݒ�
	device_context->RSSetState(FRAMEWORK->GetRasterizerState(FRAMEWORK->RS_SOLID_NONE));
	// �v���~�e�B�u�̕`��
	device_context->Draw(4, 0);	// ���_�̐��A�`��J�n���_�Ŏg�����_�o�b�t�@�̍ŏ��̃C���f�b�N�X

	// �e�N�X�`���̖�����
	texture->Set(device_context,0, false);
	// �V�F�[�_�̖�����
	shader->Inactivate(device_context);

	// DeferredContext�̏ꍇ�A�R�}���h���X�g�ɓo�^����
	if (device_context->GetType() == D3D11_DEVICE_CONTEXT_DEFERRED)
	{
		device_context->FinishCommandList(true, comand_list);
	}
}

void Sprite::Render(ID3D11DeviceContext* device_context, Texture* texture, Shader* shader, ID3D11CommandList** comand_list) {
	CreateVertexData(device_context, texture, shader, comand_list);
}

void Sprite::ImguiWindow() {
	static float pos[2]     { Parameter->Pos.x    ,Parameter->Pos.y };
	static float size[2]    { Parameter->Size.x   ,Parameter->Size.y };
	static float angle      { Parameter->Angle};
	static float TexPos[2]  { Parameter->TexPos.x ,Parameter->TexPos .y };
	static float TexSize[2] { Parameter->TexSize.x,Parameter->TexSize.y };
	static float Color[4] = { Parameter->Color.x  ,Parameter->Color.y,Parameter->Color.z,Parameter->Color.w };

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