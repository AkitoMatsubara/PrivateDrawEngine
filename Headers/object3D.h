#pragma once
#include <SimpleMath.h>

// ��{��3d�I�u�W�F�N�g�ɕK�v�ȃp�����[�^�[�B
class Object3d
{
	// �ϐ�
private:
public:
	DirectX::SimpleMath::Vector3 Position;		// ���[���h�ʒu
	DirectX::SimpleMath::Vector3 Vector;		// ����	�Ƃ���������O�������������Ă���
	DirectX::SimpleMath::Vector3 Acceleration;	// �����x
	DirectX::SimpleMath::Vector3 Velocity;		// ���x

	DirectX::SimpleMath::Vector3 Scale;		// �傫��
	//DirectX::SimpleMath::Vector3 Rotate;	// �e����]�l
	DirectX::SimpleMath::Quaternion Orientation;	// �e����]�l
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
	Object3d()
	{
		Position = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };
		Vector = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };
		Acceleration = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };
		Velocity = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };

		Scale = DirectX::SimpleMath::Vector3{ 1.0f,1.0f,1.0f };
		//Rotate = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };
		Orientation = DirectX::SimpleMath::Quaternion{ 0,0,0,1 };
		Color = DirectX::SimpleMath::Vector4{ 0.0f,0.0f,0.0f,1.0f };

		Exist = false;
		MaxLife = 1;
		CurLife = MaxLife;
	};
	~Object3d() {};

	// �O���x�N�g�������Ȃ̉�]�p�x����v�Z
	void calcForward()
	{
		//Vector.x = sinf(DirectX::XMConvertToRadians(Rotate.y));
		//Vector.y = sinf(-DirectX::XMConvertToRadians(Rotate.x));
		//Vector.z = cosf(DirectX::XMConvertToRadians(Rotate.y));
		Vector.x = sinf(DirectX::XMConvertToRadians(Orientation.y));
		Vector.y = sinf(-DirectX::XMConvertToRadians(Orientation.x));
		Vector.z = cosf(DirectX::XMConvertToRadians(Orientation.y));
		Vector.Normalize();	// �����x�N�g���Ȃ̂Ő��K�����Ƃ�
	}

	// Object3d�̃p�����[�^���R�s�[����
	void CopyParam(const Object3d* src) {
		Position = src->Position;
		Vector = src->Vector;
		Acceleration = src->Acceleration;
		Velocity = src->Velocity;
		Scale = src->Scale;
		//Rotate = src->Rotate;
		Orientation = src->Orientation;
		Color = src->Color;
		Exist = src->Exist;
		MaxLife = src->MaxLife;
		CurLife = src->CurLife;
	}

protected:
};