#include "Player.h"

#include "XMFLOAT_Helper.h"

void Player::Initialize() {
	Model = std::make_unique<Skinned_Mesh>(".\\Resources\\Player\\Player.fbx");	// 3�p�`������Ă��Ȃ��������b�V�� �L���[�u
	SkinnedShader = std::make_unique<ShaderEx>();
	SkinnedShader->Create(L"Shaders\\static_mesh_vs", L"Shaders\\static_mesh_ps");

	// �p�����[�^�[�̏�����
	Position     = { 0,0,0 };
	Vector       = { 0,0,0 };
	acceleration = { 0,0,0 };
	Velocity     = { 0,0,0 };
	Scale        = { 1,1,1 };
	Rotate       = { 0,0,0 };
	Color        = { 1,1,1,1 };
}

void Player::Update() {
	static float speed = 0.01f;
	acceleration = { 0,0,0 };
	Velocity = { 0,0,0 };	// ���͒��������������߂ɖ��t���[�������� ���ʂ���Ȃ�


	//--------------------------------------------------------
	//�O�i����
	if (GetKeyState('W') < 0){
		Position.x += sinf(DirectX::XMConvertToRadians(Rotate.y)) * speed;
		Position.z += cosf(DirectX::XMConvertToRadians(Rotate.y)) * speed;
	}
	//��ޏ���
	if (GetKeyState('S') < 0){
		Position.x -= sinf(DirectX::XMConvertToRadians(Rotate.y)) * speed;
		Position.z -= cosf(DirectX::XMConvertToRadians(Rotate.y)) * speed;
	}
	//��]����
	{
		if (GetKeyState('D') < 0){
			Rotate.y += DirectX::XMConvertToRadians(40);
		}
		if (GetKeyState('A') < 0){
			Rotate.y -= DirectX::XMConvertToRadians(40);
		}
	}
	//--------------------------------------------------------

	Velocity+= acceleration;
	Position += Velocity;

	// ���f���ɕ`��n�p�����[�^�[��n��
	Model->setPos(Position);
	Model->setAngle(Rotate);
	Model->setSize(Scale);
	Model->setColor(Color);
}

void Player::Render() {
	Model->Render(SkinnedShader.get());
}