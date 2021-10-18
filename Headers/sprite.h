#pragma once
#include "shader.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <iostream>

#include <wrl.h>


//// ���_�t�H�[�}�b�g
//struct Vertex {
//	XMFLOAT3 position;
//	XMFLOAT3 normal;
//	XMFLOAT2 texcoord;	// TextuerCoordinate�̗��AUV���W�̎擾�Ȃǂɂ悭�g����l�q
//	XMFLOAT4 color;
//};

// ��`�p�̃X�e�[�^�X
struct SpriteParam {
	DirectX::XMFLOAT2 Pos;		// �`��ʒu
	DirectX::XMFLOAT2 Size;		// �`��T�C�Y
	DirectX::XMFLOAT2 TexPos;	// �e�N�X�`���̊J�n�ʒu
	DirectX::XMFLOAT2 TexSize;	// �e�N�X�`���̎g�p�T�C�Y
	float Angle;		// ��]�p�x
	DirectX::XMFLOAT4 Color;		// ���Z�F
};

class Sprite {
private:
	// �����o�ϐ�
	Microsoft::WRL::ComPtr<ID3D11VertexShader>			vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>			input_layout;
	Microsoft::WRL::ComPtr<ID3D11Buffer>				vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shader_resource_view;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>		rasterizer_states[3];	// 0:�Жʓh��Ԃ�,1:�Жʃ��C���[�t���[��,2:���ʃ��C���[�t���[��

	D3D11_TEXTURE2D_DESC	texture2d_desc;

	SpriteParam param;


	// �����g�p�����o�֐�
	DirectX::XMFLOAT3 ConvertToNDC(DirectX::XMFLOAT3 val, D3D11_VIEWPORT viewport);

public:
	// �R���X�g���N�^�A�f�X�g���N�^
	Sprite(const wchar_t* filename, const char* vs_cso_name = "Shaders\\sprite_vs.cso", const char* ps_cso_name = "Shaders\\sprite_ps.cso");
	~Sprite();	// ���ׂĂ�COM�I�u�W�F�N�g���������

	// �����o�֐�

	// ���_���̐����A�X�V
	void CreateVertexData(Shader* shader, DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size, float angle, DirectX::XMFLOAT4 color
		, DirectX::XMFLOAT2 TexPos, DirectX::XMFLOAT2 TexSize);

	// �����o�ϐ��łł���Ȃ炢��Ȃ��H�Ǝv���폜������ς����ۑ�I��
	void Render(Shader* shader, DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size, float andle, DirectX::XMFLOAT4 color
		, DirectX::XMFLOAT2 TexPos, DirectX::XMFLOAT2 TexSize);	// immediate(�ΏۂƂȂ�f�[�^���̂��̂��R�[�h���ɋL�������̂𑦒l�Ƃ���)

	// �����o�ϐ��̃p�����[�^�ŕ`��
	void Render(Shader* shader);	// immediate(�ΏۂƂȂ�f�[�^���̂��̂��R�[�h���ɋL�������̂𑦒l�Ƃ���)

	// �ʒu�����w�肷��Render �A�j���[�V�����s��
	void Render(Shader* shader, DirectX::XMFLOAT2 Pos, DirectX::XMFLOAT2 Size);

	//// �e�L�X�g�摜����e�L�X�g��؂蔲���ĕ`��(�摜�Ȃ̂Ńt�H�[�}�b�g�Œ�)
	//void Text_Out(ID3D11DeviceContext* immediate_context, std::string s, XMFLOAT2 pos, XMFLOAT2 size, XMFLOAT4 color);

	// param��ҏW����imgui�E�B���h�E
	void ImguiWindow();

	// XMFLOAT2���m�̊���Z
	DirectX::XMFLOAT2 Division(DirectX::XMFLOAT2 val1, DirectX::XMFLOAT2 val2);


	// �Z�b�^�[
	void setPos    (DirectX::XMFLOAT2 pos)     { param.Pos     = pos; }
	void setSize   (DirectX::XMFLOAT2 Size)    { param.Size    = Size; }
	void setTexPos (DirectX::XMFLOAT2 texPos)  { param.TexPos  = texPos; }
	void setTexSize(DirectX::XMFLOAT2 texSize) { param.TexSize = texSize; }
	void setAngle  (float angle)      { param.Angle   = angle; }
	void setColor  (DirectX::XMFLOAT4 color)   { param.Color   = color; }

	void setPos    (float posX, float posY)           { param.Pos     = DirectX::XMFLOAT2(posX, posY); }
	void setSize   (float SizeX, float SizeY)         { param.Size    = DirectX::XMFLOAT2(SizeX, SizeY); }
	void setTexPos (float texPosX, float texPosY)     { param.TexPos  = DirectX::XMFLOAT2(texPosX, texPosY); }
	void setTexSize(float texSizeX, float texSizeY)   { param.TexSize = DirectX::XMFLOAT2(texSizeX, texSizeY); }
	void setColor(float r, float g, float b, float a) { param.Color   = DirectX::XMFLOAT4(r, g, b, a); }

	// �Q�b�^�[
	DirectX::XMFLOAT2 getPos()     { return param.Pos;}
	DirectX::XMFLOAT2 getSize()    { return param.Size;}
	DirectX::XMFLOAT2 getTexPos()  { return param.TexPos;}
	DirectX::XMFLOAT2 getTexSize() { return param.TexSize;}
	float getAngle() { return param.Angle; }
	DirectX::XMFLOAT4 getColor  () { return param.Color;}
};

// render���Ŏg�����_��]�p�֐� sprite_Batch�ł��g�p����̂�class Sprite�O�w�b�_�[�ɋL�q
inline void rotate(DirectX::XMFLOAT3& pos, DirectX::XMFLOAT2 center, float angle) {
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
