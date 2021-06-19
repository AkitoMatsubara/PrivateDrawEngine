#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

// ���_�t�H�[�}�b�g
typedef struct VERTEX_FORMAT {
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 color;
}vFormat_t;	// typedef�̗��_�E�ϐ��錾���Ɂustruct VERTEX_FORMAT �ϐ����v�ƋL�q�����Z���錾�ł���

class Sprite {
private:
	// �����o�ϐ�
	ID3D11VertexShader* vertex_shader;
	ID3D11PixelShader*	pixel_shader;
	ID3D11InputLayout*	input_layout;
	ID3D11Buffer*		vertex_buffer;


public:
	// �R���X�g���N�^�A�f�X�g���N�^
	Sprite(ID3D11Device* device);
	~Sprite();	// ���ׂĂ�COM�I�u�W�F�N�g���������

	// �����o�֐�
	void render(ID3D11DeviceContext* immediate_context, DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size, float andle, DirectX::XMFLOAT4 color);	// immediate(�ΏۂƂȂ�f�[�^���̂��̂��R�[�h���ɋL�������̂𑦒l�Ƃ���)

	// dx,dy����`�̍���̃X�N���[�����W�Adw,dh����`�T�C�Y
	DirectX::XMFLOAT3 Convert_Screen_to_NDC(DirectX::XMFLOAT3 val, D3D11_VIEWPORT viewport);
};
