#pragma once
#include <SimpleMath.h>

// ��{��3d�I�u�W�F�N�g�ɕK�v�ȃp�����[�^�[�B
class Object2d
{
	// �ϐ�
private:
public:
	DirectX::SimpleMath::Vector2 Pos;	// ���[���h�ʒu
	DirectX::SimpleMath::Vector2 Size;		// �傫��
	DirectX::SimpleMath::Vector2 TexPos;	// �e�N�X�`���̊J�n�ʒu
	DirectX::SimpleMath::Vector2 TexSize;	// �e�N�X�`���̎g�p�T�C�Y
	float Angle;							// ��]�l
	DirectX::SimpleMath::Vector4 Color;		// �F

	// ���̑� �g������g��Ȃ������肷���� //
	bool Exist;		// ���݃t���O
	int MaxLife;	// �̗͏���Ƃ������̗͂�
	int CurLife;	// ���݂�(�c��)�̗�
protected:

	// �֐�
private:
public:
	// �R���X�g���N�^�ŏ��������Ă��� �g���̂ɂ��������l�����ĂȂ������炨����������˂��H
	Object2d()
	{
		Pos = DirectX::SimpleMath::Vector2{ 0.0f,0.0f };
		Size = DirectX::SimpleMath::Vector2{ 1.0f,1.0f };
		TexPos = DirectX::SimpleMath::Vector2{ 0.0f,0.0f };
		TexSize = DirectX::SimpleMath::Vector2{ 1.0f,1.0f };
		Angle = 0.0f;
		Color = DirectX::SimpleMath::Vector4{ 1.0f,1.0f,1.0f,1.0f };

		Exist = true;
		MaxLife = 1;
		CurLife = MaxLife;
	};
	~Object2d() {};

	// Object2d�̃p�����[�^���R�s�[����
	void CopyParam(const Object2d* src) {
		Pos = src->Pos;
		Size = src->Size;
		TexPos = src->TexPos;
		TexSize = src->TexSize;
		Angle = src->Angle;
		Color = src->Color;

		Exist = src->Exist;
		MaxLife = src->MaxLife;
		CurLife = src->CurLife;
	}

protected:
};