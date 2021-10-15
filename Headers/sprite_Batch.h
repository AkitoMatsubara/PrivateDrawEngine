#pragma once

#include "sprite.h"

#include <vector>


// sprite�ƈႢspriteBatch�͕����̕`��ɑ΂��Ĉ�x�����ǂݍ��܂Ȃ��ėǂ��I�Ȋ���
class sprite_Batch {
private:
	// �����o�ϐ�
	ComPtr<ID3D11VertexShader>			vertex_shader;
	ComPtr<ID3D11PixelShader>			pixel_shader;
	ComPtr<ID3D11InputLayout>			input_layout;
	ComPtr<ID3D11Buffer>				vertex_buffer;
	ComPtr<ID3D11ShaderResourceView>	shader_resource_view;
	ComPtr<ID3D11RasterizerState>		rasterizer_states[3];	// 0:�Жʓh��Ԃ�,1:�Жʃ��C���[�t���[��,2:���ʃ��C���[�t���[��

	D3D11_TEXTURE2D_DESC				texture2d_desc;

	SpriteParam param{};

	// Sprite_Batch�ǉ��ϐ�
	const size_t max_vertices;
	std::vector<Vertex> vertices;

	// �����g�p�����o�֐�
	// dx,dy����`�̍���̃X�N���[�����W�Adw,dh����`�T�C�Y
	XMFLOAT3 ConvertToNDC(XMFLOAT3 val, D3D11_VIEWPORT viewport);

public:
	// �R���X�g���N�^�A�f�X�g���N�^
	sprite_Batch(const wchar_t* filename, size_t max_sprites = 1, const char* vs_cso_name = "Shaders\\sprite_vs.cso", const char* ps_cso_name = "Shaders\\sprite_ps.cso");
	sprite_Batch(const wchar_t* filename, const char* vs_cso_name = "Shaders\\sprite_vs.cso", const char* ps_cso_name = "Shaders\\sprite_ps.cso");
	~sprite_Batch();	// ���ׂĂ�COM�I�u�W�F�N�g���������

	// �����o�֐�
	void begin();
	void end();	// ���_�o�b�t�@�̍X�V�A�e�X�e�[�g�̃o�C���h�A�h���[�R�[�����L�q

	// ���_���̐����A�X�V
	void CreateVertexData(XMFLOAT2 pos, XMFLOAT2 size, float angle, XMFLOAT4 color
		, XMFLOAT2 TexPos, XMFLOAT2 TexSize);

	// �����o�ϐ��łł���Ȃ炢��Ȃ��H�Ǝv���폜������ς����ۑ�I��
	void Render(XMFLOAT2 pos, XMFLOAT2 size, float andle, XMFLOAT4 color
		, XMFLOAT2 TexPos, XMFLOAT2 TexSize);	// immediate(�ΏۂƂȂ�f�[�^���̂��̂��R�[�h���ɋL�������̂𑦒l�Ƃ���)


	// �����o�ϐ��̃p�����[�^�ŕ`��
	void Render();	// immediate(�ΏۂƂȂ�f�[�^���̂��̂��R�[�h���ɋL�������̂𑦒l�Ƃ���)

		// �e�N�X�`���ʒu�����w�肷��Render �ۑ�I��(ry
	void Render(XMFLOAT2 TexPos, XMFLOAT2 TexSize);

	// XMFLOAT2���m�̊���Z
	XMFLOAT2 Division(XMFLOAT2 val1, XMFLOAT2 val2);

	// �Z�b�^�[
	void setPos    (XMFLOAT2 pos)     { param.Pos     = pos; }
	void setSize   (XMFLOAT2 Size)    { param.Size    = Size; }
	void setTexPos (XMFLOAT2 texPos)  { param.TexPos  = texPos; }
	void setTexSize(XMFLOAT2 texSize) { param.TexSize = texSize; }
	void setAngle  (float angle)      { param.Angle   = angle; }
	void setColor  (XMFLOAT4 color)   { param.Color   = color; }

	void setPos    (float posX, float posY)             { param.Pos     = XMFLOAT2(posX, posY); }
	void setSize   (float SizeX, float SizeY)           { param.Size    = XMFLOAT2(SizeX, SizeY); }
	void setTexPos (float texPosX, float texPosY)       { param.TexPos  = XMFLOAT2(texPosX, texPosY); }
	void setTexSize(float texSizeX, float texSizeY)     { param.TexSize = XMFLOAT2(texSizeX, texSizeY); }
	void setColor  (float r, float g, float b, float a) { param.Color   = XMFLOAT4(r, g, b, a); }

	// �Q�b�^�[
	XMFLOAT2 getPos()     { return param.Pos; }
	XMFLOAT2 getSize()    { return param.Size; }
	XMFLOAT2 getTexPos()  { return param.TexPos; }
	XMFLOAT2 getTexSize() { return param.TexSize; }
	float	 getAngle()   { return param.Angle; }
	XMFLOAT4 getColor()   { return param.Color; }
};
