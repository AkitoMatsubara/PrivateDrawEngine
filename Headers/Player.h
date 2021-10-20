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

	// �Z�b�^�[
	void setPos(DirectX::XMFLOAT3 pos)     { Position = pos; }
	void setSize(DirectX::XMFLOAT3 Size)   { Size = Size; }
	void setAngle(DirectX::XMFLOAT3 angle) { Rotate = angle; }
	void setColor(DirectX::XMFLOAT4 color) { Color = color; }

	void setPos(float posX, float posY, float posZ)         { Position = DirectX::XMFLOAT3(posX, posY, posZ); }
	void setSize(float sizeX, float sizeY, float sizeZ)     { Size = DirectX::XMFLOAT3(sizeX, sizeY, sizeZ); }
	void setAngle(float angleX, float angleY, float angleZ) { Rotate = DirectX::XMFLOAT3(angleX, angleY, angleZ); }
	void setColor(float r, float g, float b, float a)       { Color = DirectX::XMFLOAT4(r, g, b, a); }

	// �Q�b�^�[
	DirectX::XMFLOAT3 getPos()   { return Position; }
	DirectX::XMFLOAT3 getSize()  { return Size; }
	DirectX::XMFLOAT3 getAngle() { return Rotate; }
	DirectX::XMFLOAT4 getColor() { return Color; }
};