#pragma once

#include "shaderEx.h"
#include "Object2d.h"
#include "texture.h"

#include <d3d11.h>
#include <SimpleMath.h>

#include <wrl.h>


class Sprite {
protected:
	// �����o�ϐ�
	Microsoft::WRL::ComPtr<ID3D11Buffer>	VertexBuffer;

	static inline std::unique_ptr<ShaderEx> SpriteShader;	// �f�t�H���g�Ŏg�p����V�F�[�_
	std::unique_ptr<Texture> SpriteTexture;	// �摜��ێ������Ă���

	std::unique_ptr<Object2d> Parameter;

public:
	// �R���X�g���N�^�A�f�X�g���N�^
	Sprite();
	virtual ~Sprite();	// ���ׂĂ�COM�I�u�W�F�N�g���������

	// �����o�֐�

	// �摜�̓ǂݍ���
	void LoadImages(const wchar_t* filename);

	// ���_���̐����A�X�V
	void CreateVertexData(Texture* texture, Shader* shader);

	// �����o�ϐ��̃p�����[�^�ŕ`��
	void Render(Texture* texture = nullptr, Shader* shader = SpriteShader.get());	// immediate(�ΏۂƂȂ�f�[�^���̂��̂��R�[�h���ɋL�������̂𑦒l�Ƃ���)

	//// �ʒu�����w�肷��Render �A�j���[�V�����s��
	//void Render(Shader* shader, DirectX::SimpleMath::Vector2 Pos, DirectX::SimpleMath::Vector2 Size);

	//// �e�L�X�g�摜����e�L�X�g��؂蔲���ĕ`��(�摜�Ȃ̂Ńt�H�[�}�b�g�Œ�)
	//void Text_Out(ID3D11DeviceContext* immediate_context, std::string s, XMFLOAT2 pos, XMFLOAT2 size, XMFLOAT4 color);

	// param��ҏW����imgui�E�B���h�E
	void ImguiWindow();

	// XMFLOAT2���m�̊���Z
	DirectX::SimpleMath::Vector2 Division(DirectX::SimpleMath::Vector2 val1, DirectX::SimpleMath::Vector2 val2);

	// �Z�b�^�[
	void setPos    (DirectX::SimpleMath::Vector2 pos)     { Parameter->Pos     = pos; }
	void setSize(DirectX::SimpleMath::Vector2 Size)    { Parameter->Size    = Size; }
	void setTexPos (DirectX::SimpleMath::Vector2 texPos)  { Parameter->TexPos  = texPos; }
	void setTexSize(DirectX::SimpleMath::Vector2 texSize) { Parameter->TexSize = texSize; }
	void setRotate(float angle)      { Parameter->Angle   = angle; }
	void setColor  (DirectX::SimpleMath::Vector4 color)   { Parameter->Color   = color; }

	void setPos    (float posX, float posY)           { Parameter->Pos     = DirectX::SimpleMath::Vector2(posX, posY); }
	void setSize   (float SizeX, float SizeY)         { Parameter->Size    = DirectX::SimpleMath::Vector2(SizeX, SizeY); }
	void setTexPos (float texPosX, float texPosY)     { Parameter->TexPos  = DirectX::SimpleMath::Vector2(texPosX, texPosY); }
	void setTexSize(float texSizeX, float texSizeY)   { Parameter->TexSize = DirectX::SimpleMath::Vector2(texSizeX, texSizeY); }
	void setColor(float r, float g, float b, float a) { Parameter->Color   = DirectX::SimpleMath::Vector4(r, g, b, a); }

	// �Q�b�^�[
	DirectX::SimpleMath::Vector2 getPos()     { return Parameter->Pos;}
	DirectX::SimpleMath::Vector2 getSize()    { return Parameter->Size;}
	DirectX::SimpleMath::Vector2 getTexPos()  { return Parameter->TexPos;}
	DirectX::SimpleMath::Vector2 getTexSize() { return Parameter->TexSize;}
	float getAngle() { return Parameter->Angle; }
	DirectX::SimpleMath::Vector4 getColor  () { return Parameter->Color;}
	Texture* getTexture() { return SpriteTexture.get(); }
};

namespace SpriteMath
{
	// Render���Ŏg�����_��]�p�֐� sprite_Batch�ł��g�p����̂�class Sprite�O�w�b�_�[�ɋL�q
	inline void rotate(DirectX::SimpleMath::Vector3& pos, DirectX::SimpleMath::Vector2 center, float angle) {
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

	// �X�N���[�����W�n����NDC(���K���f�o�C�X���W)�ւ̍��W�ϊ����s��
	// ��`�̍���̃X�N���[�����W�Ƌ�`�T�C�Y��n��
	inline DirectX::SimpleMath::Vector3 ConvertToNDC(DirectX::SimpleMath::Vector3 pos, D3D11_VIEWPORT viewport) {
		pos.x = (pos.x * 2.0f / viewport.Width) - 1.0f;		// x�l���Q�{�A���̌�X�N���[���T�C�Y�Ŋ����ĂP�������Ɛ��K�������
		pos.y = 1.0f - (pos.y * 2.0f / viewport.Height);	// y�l���Q�{�A�X�N���[���T�C�Y�Ŋ��������̂łP�������Ɛ��K��	x�ƈႤ�̂͂����炭����E����W�n�̊֌W
		// �����sprite(�摜)�Ȃ̂�z�l�͕ύX����K�v�Ȃ�
		return pos;
	}
}