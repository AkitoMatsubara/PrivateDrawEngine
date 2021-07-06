#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include "sprite.h"

// sprite�ƈႢspriteBatch�͕����̕`��ɑ΂��Ĉ�x�����ǂݍ��܂Ȃ��ėǂ��I�Ȋ���
class sprite_Batch {
private:
	// �����o�ϐ�
	ID3D11VertexShader* vertex_shader;
	ID3D11PixelShader* pixel_shader;
	ID3D11InputLayout* input_layout;
	ID3D11Buffer* vertex_buffer;
	ID3D11ShaderResourceView* shader_resource_view;
	D3D11_TEXTURE2D_DESC texture2d_desc;

	Status_t Status{};

	// Sprite_Batch�ǉ��ϐ�
	const size_t max_vertices;
	std::vector<vFormat_t> vertices;

	// �����g�p�����o�֐�
	// dx,dy����`�̍���̃X�N���[�����W�Adw,dh����`�T�C�Y
	DirectX::XMFLOAT3 ConvertToNDC(DirectX::XMFLOAT3 val, D3D11_VIEWPORT viewport);

public:
	// �R���X�g���N�^�A�f�X�g���N�^
	sprite_Batch(ID3D11Device* device, const wchar_t* filename, size_t max_sprites);
	sprite_Batch(ID3D11Device* device, const wchar_t* filename);
	~sprite_Batch();	// ���ׂĂ�COM�I�u�W�F�N�g���������

	// �����o�֐�
	void begin(ID3D11DeviceContext* immediate_context);
	void end(ID3D11DeviceContext* immediate_context);	// ���_�o�b�t�@�̍X�V�A�e�X�e�[�g�̃o�C���h�A�h���[�R�[�����L�q

		// �N���b�s���O�\render�֐�	�����o�ϐ��łł���Ȃ炢��Ȃ��H�Ǝv���폜
	void render(ID3D11DeviceContext* immediate_context, DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size, float andle, DirectX::XMFLOAT4 color
		, DirectX::XMFLOAT2 sPos, DirectX::XMFLOAT2 sSize);	// immediate(�ΏۂƂȂ�f�[�^���̂��̂��R�[�h���ɋL�������̂𑦒l�Ƃ���)


	// �����o�ϐ��̃p�����[�^�ŕ`��
	void render(ID3D11DeviceContext* immediate_context);	// immediate(�ΏۂƂȂ�f�[�^���̂��̂��R�[�h���ɋL�������̂𑦒l�Ƃ���)

	// Status��ҏW����imgui�E�B���h�E
	void imguiWindow();

	// DirectX::XMFLOAT2���m�̊���Z
	DirectX::XMFLOAT2 division(DirectX::XMFLOAT2 val1, DirectX::XMFLOAT2 val2);

	// render���Ŏg�����_��]�p�֐�
	void rotate(DirectX::XMFLOAT3& pos, DirectX::XMFLOAT2 center, float angle);

	// �Z�b�^�[
	void setPos(DirectX::XMFLOAT2 pos) { Status.Pos = pos; }
	void setSize(DirectX::XMFLOAT2 Size) { Status.Size = Size; }
	void setTexPos(DirectX::XMFLOAT2 texPos) { Status.TexPos = texPos; }
	void setTexSize(DirectX::XMFLOAT2 texSize) { Status.TexSize = texSize; }
	void setAngle(float angle) { Status.Angle = angle; }
	void setColor(DirectX::XMFLOAT4 color) { Status.Color = color; }

	void setPos(float posX, float posY) { Status.Pos = DirectX::XMFLOAT2(posX, posY); }
	void setSize(float SizeX, float SizeY) { Status.Size = DirectX::XMFLOAT2(SizeX, SizeY); }
	void setTexPos(float texPosX, float texPosY) { Status.TexPos = DirectX::XMFLOAT2(texPosX, texPosY); }
	void setTexSize(float texSizeX, float texSizeY) { Status.TexSize = DirectX::XMFLOAT2(texSizeX, texSizeY); }
	void setColor(float r, float g, float b, float a) { Status.Color = DirectX::XMFLOAT4(r, g, b, a); }

	// �Q�b�^�[
	DirectX::XMFLOAT2 getPos() { return Status.Pos; }
	DirectX::XMFLOAT2 getSize() { return Status.Size; }
	DirectX::XMFLOAT2 getTexPos() { return Status.TexPos; }
	DirectX::XMFLOAT2 getTexSize() { return Status.TexSize; }
	float getAngle() { return Status.Angle; }
	DirectX::XMFLOAT4 getColor() { return Status.Color; }
};
