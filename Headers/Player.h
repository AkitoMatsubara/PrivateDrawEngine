#pragma once
#include "skinned_mesh.h"
#include "shaderEx.h"

#include <memory>



class Player {
	// �ϐ�
private:
	DirectX::XMFLOAT3 Position;		// ���[���h�ʒu
	DirectX::XMFLOAT3 Vector;		// ����
	DirectX::XMFLOAT3 acceleration;	// �����x
	DirectX::XMFLOAT3 Velocity;		// ���x

	DirectX::XMFLOAT3 Size;			// �傫��
	DirectX::XMFLOAT3 Rotate;		// �e����]�l
	DirectX::XMFLOAT4 Color;			// �F

	// ���f�����
	std::unique_ptr<Skinned_Mesh> Model;
	// �f�t�H���g�̃V�F�[�_�[
	std:: unique_ptr<ShaderEx> SkinnedShader = nullptr;

protected:
public:
	// �֐�
private:
protected:
public:
	Player() {};
	~Player() {};

	void Initialize();
	void Update();
	void Render();

};