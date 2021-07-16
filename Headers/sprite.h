#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <iostream>

#include <wrl.h>
using namespace Microsoft::WRL;
using namespace DirectX;
using namespace std;



// ���_�t�H�[�}�b�g
struct Vertex {
	XMFLOAT3 position;
	XMFLOAT4 color;
	XMFLOAT2 texcoord;	// TextuerCoordinate�̗��AUV���W�̎擾�Ȃǂɂ悭�g����l�q
};

// ��`�p�̃X�e�[�^�X
struct SpriteParam {
	XMFLOAT2 Pos;		// �`��ʒu
	XMFLOAT2 Size;		// �`��T�C�Y
	XMFLOAT2 TexPos;	// �e�N�X�`���̊J�n�ʒu
	XMFLOAT2 TexSize;	// �e�N�X�`���̎g�p�T�C�Y
	float Angle;		// ��]�p�x
	XMFLOAT4 Color;		// ���Z�F
};

class Sprite {
private:
	// �����o�ϐ�
	ComPtr<ID3D11VertexShader>			vertex_shader;
	ComPtr<ID3D11PixelShader>			pixel_shader;
	ComPtr<ID3D11InputLayout>			input_layout;
	ComPtr<ID3D11Buffer>				vertex_buffer;
	ComPtr<ID3D11ShaderResourceView>	shader_resource_view;
	ComPtr<ID3D11RasterizerState>		rasterizer_states[3];	// 0:�Жʓh��Ԃ�,1:�Жʃ��C���[�t���[��,2:���ʃ��C���[�t���[��

	D3D11_TEXTURE2D_DESC	texture2d_desc;

	SpriteParam param;


	// �����g�p�����o�֐�
	XMFLOAT3 ConvertToNDC(XMFLOAT3 val, D3D11_VIEWPORT viewport);

public:
	// �R���X�g���N�^�A�f�X�g���N�^
	Sprite(ID3D11Device* device, const wchar_t* filename, const char* vs_cso_name = "Shaders\\sprite_vs.cso", const char* ps_cso_name = "Shaders\\sprite_ps.cso");
	~Sprite();	// ���ׂĂ�COM�I�u�W�F�N�g���������

	// �����o�֐�

	// ���_���̐����A�X�V
	void CreateVertexData(ID3D11DeviceContext* immediate_context, XMFLOAT2 pos, XMFLOAT2 size, float angle, XMFLOAT4 color
		, XMFLOAT2 TexPos, XMFLOAT2 TexSize);

	// �����o�ϐ��łł���Ȃ炢��Ȃ��H�Ǝv���폜������ς����ۑ�I��
	void Render(ID3D11DeviceContext* immediate_context, XMFLOAT2 pos, XMFLOAT2 size, float andle, XMFLOAT4 color
		, XMFLOAT2 TexPos, XMFLOAT2 TexSize);	// immediate(�ΏۂƂȂ�f�[�^���̂��̂��R�[�h���ɋL�������̂𑦒l�Ƃ���)

	// �����o�ϐ��̃p�����[�^�ŕ`��
	void Render(ID3D11DeviceContext* immediate_context);	// immediate(�ΏۂƂȂ�f�[�^���̂��̂��R�[�h���ɋL�������̂𑦒l�Ƃ���)

	// �ʒu�����w�肷��Render �A�j���[�V�����s��
	void Render(ID3D11DeviceContext* immediate_context, XMFLOAT2 Pos, XMFLOAT2 Size);

	// �e�L�X�g�摜����e�L�X�g��؂蔲���ĕ`��(�摜�Ȃ̂Ńt�H�[�}�b�g�Œ�)
	void Text_Out(ID3D11DeviceContext* immediate_context, std::string s, XMFLOAT2 pos, XMFLOAT2 size, XMFLOAT4 color);

	// param��ҏW����imgui�E�B���h�E
	void ImguiWindow();

	// XMFLOAT2���m�̊���Z
	XMFLOAT2 Division(XMFLOAT2 val1, XMFLOAT2 val2);


	// �Z�b�^�[
	void setPos    (XMFLOAT2 pos)     { param.Pos     = pos; }
	void setSize   (XMFLOAT2 Size)    { param.Size    = Size; }
	void setTexPos (XMFLOAT2 texPos)  { param.TexPos  = texPos; }
	void setTexSize(XMFLOAT2 texSize) { param.TexSize = texSize; }
	void setAngle  (float angle)      { param.Angle   = angle; }
	void setColor  (XMFLOAT4 color)   { param.Color   = color; }

	void setPos    (float posX, float posY)           { param.Pos     = XMFLOAT2(posX, posY); }
	void setSize   (float SizeX, float SizeY)         { param.Size    = XMFLOAT2(SizeX, SizeY); }
	void setTexPos (float texPosX, float texPosY)     { param.TexPos  = XMFLOAT2(texPosX, texPosY); }
	void setTexSize(float texSizeX, float texSizeY)   { param.TexSize = XMFLOAT2(texSizeX, texSizeY); }
	void setColor(float r, float g, float b, float a) { param.Color   = XMFLOAT4(r, g, b, a); }

	// �Q�b�^�[
	XMFLOAT2 getPos()     { return param.Pos;}
	XMFLOAT2 getSize()    { return param.Size;}
	XMFLOAT2 getTexPos()  { return param.TexPos;}
	XMFLOAT2 getTexSize() { return param.TexSize;}
	float getAngle() { return param.Angle; }
	XMFLOAT4 getColor  () { return param.Color;}
};

// render���Ŏg�����_��]�p�֐� sprite_Batch�ł��g�p����̂�class Sprite�O�w�b�_�[�ɋL�q
inline void rotate(XMFLOAT3& pos, XMFLOAT2 center, float angle) {
	pos.x -= center.x;	// ��x���S�_�����炷
	pos.y -= center.y;

	float cos{ cosf(XMConvertToRadians(angle)) };	// Degree�Ȃ̂�Radian�ɕϊ�
	float sin{ sinf(XMConvertToRadians(angle)) };
	float tx{ pos.x };	// ��]�O�̒��_���W
	float ty{ pos.y };
	pos.x = tx * cos - sin * ty;	// ��]�̌���
	pos.y = tx * sin + cos * ty;

	pos.x += center.x;	// ���炵�����߂�
	pos.y += center.y;
	}
