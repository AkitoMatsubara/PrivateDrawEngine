#pragma once
//#include <d3d11.h>
//#include <DirectXMath.h>

//#include "imgui.h"
//#include "imgui_impl_dx11.h"
//#include "imgui_impl_win32.h"

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
	sprite_Batch(ID3D11Device* device, const wchar_t* filename, size_t max_sprites = 1, const char* vs_cso_name = "Shaders\\sprite_vs.cso", const char* ps_cso_name = "Shaders\\sprite_ps.cso");
	sprite_Batch(ID3D11Device* device, const wchar_t* filename, const char* vs_cso_name = "Shaders\\sprite_vs.cso", const char* ps_cso_name = "Shaders\\sprite_ps.cso");
	~sprite_Batch();	// ���ׂĂ�COM�I�u�W�F�N�g���������

	// �����o�֐�
	void begin(ID3D11DeviceContext* immediate_context);
	void end(ID3D11DeviceContext* immediate_context);	// ���_�o�b�t�@�̍X�V�A�e�X�e�[�g�̃o�C���h�A�h���[�R�[�����L�q

	// ���_���̐����A�X�V
	void CreateVertexData(ID3D11DeviceContext* immediate_context, XMFLOAT2 pos, XMFLOAT2 size, float angle, XMFLOAT4 color
		, XMFLOAT2 TexPos, XMFLOAT2 TexSize);

	// �����o�ϐ��łł���Ȃ炢��Ȃ��H�Ǝv���폜������ς����ۑ�I��
	void Render(ID3D11DeviceContext* immediate_context, XMFLOAT2 pos, XMFLOAT2 size, float andle, XMFLOAT4 color
		, XMFLOAT2 TexPos, XMFLOAT2 TexSize);	// immediate(�ΏۂƂȂ�f�[�^���̂��̂��R�[�h���ɋL�������̂𑦒l�Ƃ���)


	// �����o�ϐ��̃p�����[�^�ŕ`��
	void Render(ID3D11DeviceContext* immediate_context);	// immediate(�ΏۂƂȂ�f�[�^���̂��̂��R�[�h���ɋL�������̂𑦒l�Ƃ���)

		// �e�N�X�`���ʒu�����w�肷��Render �ۑ�I��(ry
	void Render(ID3D11DeviceContext* immediate_context, XMFLOAT2 TexPos, XMFLOAT2 TexSize);

	// Status��ҏW����imgui�E�B���h�E
	void imguiWindow();

	// XMFLOAT2���m�̊���Z
	XMFLOAT2 division(XMFLOAT2 val1, XMFLOAT2 val2);

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
