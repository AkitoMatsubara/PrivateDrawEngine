#pragma once
#include "skinned_mesh.h"
#include "shaderEx.h"

#include <memory>


using namespace DirectX;

class Player {
	// �ϐ�
private:
	XMFLOAT3 Position;		// ���[���h�ʒu
	XMFLOAT3 Vector;		// ����
	XMFLOAT3 acceleration;	// �����x
	XMFLOAT3 Velocity;		// ���x

	XMFLOAT3 Size;			// �傫��
	XMFLOAT3 Rotate;		// �e����]�l
	XMFLOAT4 Color;			// �F

	// ���f�����
	unique_ptr<Skinned_Mesh> Model;
	// �f�t�H���g�̃V�F�[�_�[
	unique_ptr<ShaderEx> SkinnedShader = nullptr;

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