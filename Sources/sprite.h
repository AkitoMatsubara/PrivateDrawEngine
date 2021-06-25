#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"


// ���_�t�H�[�}�b�g
typedef struct VERTEX_FORMAT {
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT2 texcoord;	// TextuerCoordinate�̗��AUV���W�̎擾�Ȃǂɂ悭�g����l�q
}vFormat_t;	// typedef�̗��_�E�ϐ��錾���Ɂustruct VERTEX_FORMAT �ϐ����v�ƋL�q�����Z���錾�ł���

typedef struct STATUS {
	DirectX::XMFLOAT2 Pos;		// �`��ʒu
	DirectX::XMFLOAT2 Size;		// �`��T�C�Y
	DirectX::XMFLOAT2 TexPos;	// �e�N�X�`���̊J�n�ʒu
	DirectX::XMFLOAT2 TexSize;	// �e�N�X�`���̎g�p�T�C�Y
	float Angle;				// ��]�p�x
	DirectX::XMFLOAT4 Color;	// ���Z�F
}Status_t;	// �u_t�v��typedef�Œ�`�����\���̂ɂ��銵�K������Fhttps://www.cc.kyoto-su.ac.jp/~yamada/programming/struct.html

class Sprite {
private:
	// �����o�ϐ�
	ID3D11VertexShader*			vertex_shader;
	ID3D11PixelShader*			pixel_shader;
	ID3D11InputLayout*			input_layout;
	ID3D11Buffer*				vertex_buffer;
	ID3D11ShaderResourceView*	shader_resource_view;
	D3D11_TEXTURE2D_DESC		texture2d_desc;

	Status_t Status;


	// �����o�ϐ�
	// dx,dy����`�̍���̃X�N���[�����W�Adw,dh����`�T�C�Y
	DirectX::XMFLOAT3 ConvertToNDC(DirectX::XMFLOAT3 val, D3D11_VIEWPORT viewport);

public:
	// �R���X�g���N�^�A�f�X�g���N�^
	Sprite(ID3D11Device* device, const wchar_t* filename);
	~Sprite();	// ���ׂĂ�COM�I�u�W�F�N�g���������

	// �����o�֐�
	void render(ID3D11DeviceContext* immediate_context, DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size, float andle, DirectX::XMFLOAT4 color);	// immediate(�ΏۂƂȂ�f�[�^���̂��̂��R�[�h���ɋL�������̂𑦒l�Ƃ���)

	// �N���b�s���O�\render�֐�
	void render(ID3D11DeviceContext* immediate_context, DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size, float andle, DirectX::XMFLOAT4 color
		, DirectX::XMFLOAT2 sPos, DirectX::XMFLOAT2 sSize);	// immediate(�ΏۂƂȂ�f�[�^���̂��̂��R�[�h���ɋL�������̂𑦒l�Ƃ���)

	// �N���b�s���O�\render�֐� �����o�ϐ��̃p�����[�^�ŕ`��
	void render(ID3D11DeviceContext* immediate_context);	// immediate(�ΏۂƂȂ�f�[�^���̂��̂��R�[�h���ɋL�������̂𑦒l�Ƃ���)

	// Status��ҏW����imgui�E�B���h�E
	void imguiWindow();

	// DirectX::XMFLOAT2���m�̊���Z
	DirectX::XMFLOAT2 division(DirectX::XMFLOAT2 val1, DirectX::XMFLOAT2 val2);

	// �Z�b�^�[
	void setPos    (DirectX::XMFLOAT2 pos)     { Status.Pos     = pos; }
	void setSize   (DirectX::XMFLOAT2 Size)    { Status.Size    = Size; }
	void setTexPos (DirectX::XMFLOAT2 texPos)  { Status.TexPos  = texPos; }
	void setTexSize(DirectX::XMFLOAT2 texSize) { Status.TexSize = texSize; }
	void setAngle  (float angle)               { Status.Angle   = angle; }
	void setColor  (DirectX::XMFLOAT4 color)   { Status.Color   = color; }

	void setPos    (float posX, float posY)           { Status.Pos     = DirectX::XMFLOAT2(posX, posY); }
	void setSize   (float SizeX, float SizeY)         { Status.Size    = DirectX::XMFLOAT2(SizeX, SizeY); }
	void setTexPos (float texPosX, float texPosY)     { Status.TexPos  = DirectX::XMFLOAT2(texPosX, texPosY); }
	void setTexSize(float texSizeX, float texSizeY)   { Status.TexSize = DirectX::XMFLOAT2(texSizeX, texSizeY); }
	void setColor(float r, float g, float b, float a) { Status.Color   = DirectX::XMFLOAT4(r, g, b, a); }

	// �Q�b�^�[
	DirectX::XMFLOAT2 getPos()     { return Status.Pos;}
	DirectX::XMFLOAT2 getSize()    { return Status.Size;}
	DirectX::XMFLOAT2 getTexPos()  { return Status.TexPos;}
	DirectX::XMFLOAT2 getTexSize() { return Status.TexSize;}
	float getAngle() { return Status.Angle; }
	DirectX::XMFLOAT4 getColor  () { return Status.Color;}
};
