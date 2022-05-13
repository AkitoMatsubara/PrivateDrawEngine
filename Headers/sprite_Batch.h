#pragma once

#include "sprite.h"
#include "shaderEx.h"
#include "Object2d.h"

#include <vector>


// sprite�ƈႢspriteBatch�͕����̕`��ɑ΂��Ĉ�x�����ǂݍ��܂Ȃ��ėǂ��I�Ȋ���
class sprite_Batch {
private:
	// �����o�ϐ�
	Microsoft::WRL::ComPtr<ID3D11VertexShader>			vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11Buffer>				VertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shader_resource_view;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>		rasterizer_states[3];	// 0:�Жʓh��Ԃ�,1:�Жʃ��C���[�t���[��,2:���ʃ��C���[�t���[��

	D3D11_TEXTURE2D_DESC	texture2d_desc;

	//SpriteParam Parameters{};
	std::unique_ptr<Object2d> Parameters;
	static inline std::unique_ptr<ShaderEx> SpriteShader;	// �f�t�H���g�Ŏg�p����V�F�[�_

	// Sprite_Batch�ǉ��ϐ�
	const size_t max_vertices;
	std::vector<Vertex> vertices;
public:
	// �R���X�g���N�^�A�f�X�g���N�^
	sprite_Batch(const wchar_t* filename, size_t max_sprites = 1);
	~sprite_Batch();	// ���ׂĂ�COM�I�u�W�F�N�g���������

	// �����o�֐�
	void begin();
	void end(ShaderEx* shader = SpriteShader.get());	// ���_�o�b�t�@�̍X�V�A�e�X�e�[�g�̃o�C���h�A�h���[�R�[�����L�q

	// ���_���̐����A�X�V
	void CreateVertexData(DirectX::SimpleMath::Vector2 pos, DirectX::SimpleMath::Vector2 size, float angle, DirectX::SimpleMath::Vector4 color
		, DirectX::SimpleMath::Vector2 TexPos, DirectX::SimpleMath::Vector2 TexSize);

	// �����o�ϐ��łł���Ȃ炢��Ȃ��H�Ǝv���폜������ς����ۑ�I��
	void Render(DirectX::SimpleMath::Vector2 pos, DirectX::SimpleMath::Vector2 size, float andle, DirectX::SimpleMath::Vector4 color
		, DirectX::SimpleMath::Vector2 TexPos, DirectX::SimpleMath::Vector2 TexSize);	// immediate(�ΏۂƂȂ�f�[�^���̂��̂��R�[�h���ɋL�������̂𑦒l�Ƃ���)


	// �����o�ϐ��̃p�����[�^�ŕ`��
	void Render();	// immediate(�ΏۂƂȂ�f�[�^���̂��̂��R�[�h���ɋL�������̂𑦒l�Ƃ���)

	// �e�N�X�`���ʒu�����w�肷��Render �ۑ�I��(ry
	void Render(DirectX::SimpleMath::Vector2 TexPos, DirectX::SimpleMath::Vector2 TexSize);

	// �Z�b�^�[
	void setPos    (DirectX::SimpleMath::Vector2 pos)     { Parameters->Pos     = pos; }
	void setSize   (DirectX::SimpleMath::Vector2 Size)    { Parameters->Size    = Size; }
	void setTexPos (DirectX::SimpleMath::Vector2 texPos)  { Parameters->TexPos  = texPos; }
	void setTexSize(DirectX::SimpleMath::Vector2 texSize) { Parameters->TexSize = texSize; }
	void setAngle  (float angle)      { Parameters->Angle   = angle; }
	void setColor  (DirectX::SimpleMath::Vector4 color)   { Parameters->Color   = color; }

	void setPos    (float posX, float posY)             { Parameters->Pos     = DirectX::SimpleMath::Vector2(posX, posY); }
	void setSize   (float SizeX, float SizeY)           { Parameters->Size    = DirectX::SimpleMath::Vector2(SizeX, SizeY); }
	void setTexPos (float texPosX, float texPosY)       { Parameters->TexPos  = DirectX::SimpleMath::Vector2(texPosX, texPosY); }
	void setTexSize(float texSizeX, float texSizeY)     { Parameters->TexSize = DirectX::SimpleMath::Vector2(texSizeX, texSizeY); }
	void setColor  (float r, float g, float b, float a) { Parameters->Color   = DirectX::SimpleMath::Vector4(r, g, b, a); }

	// �Q�b�^�[
	DirectX::SimpleMath::Vector2 getPos()     { return Parameters->Pos; }
	DirectX::SimpleMath::Vector2 getSize()    { return Parameters->Size; }
	DirectX::SimpleMath::Vector2 getTexPos()  { return Parameters->TexPos; }
	DirectX::SimpleMath::Vector2 getTexSize() { return Parameters->TexSize; }
	float	 getAngle()   { return Parameters->Angle; }
	DirectX::SimpleMath::Vector4 getColor()   { return Parameters->Color; }
};
