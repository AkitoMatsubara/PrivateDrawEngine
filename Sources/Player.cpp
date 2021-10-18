#include "Player.h"

#include "XMFLOAT_Helper.h"

void Player::Initialize() {
	Model = make_unique<Skinned_Mesh>(".\\Resources\\cube.003.0.fbx");	// 3�p�`������Ă��Ȃ��������b�V�� �L���[�u
	SkinnedShader = std::make_unique<ShaderEx>();
	SkinnedShader->Create(L"Shaders\\skinned_mesh_vs", L"Shaders\\skinned_mesh_ps");

	// �p�����[�^�[�̏�����
	Position     = { 0,0,0 };
	Vector       = { 0,0,0 };
	acceleration = { 0,0,0 };
	Velocity     = { 0,0,0 };
	Size         = { 1,1,1 };
	Rotate       = { 0,0,0 };
	Color        = { 1,1,1,1 };
}

void Player::Update() {
	static float speed = 0.001f;
	acceleration = { 0,0,0 };
	Velocity = { 0,0,0 };	// ���͒��������������߂ɖ��t���[�������� ���ʂ���Ȃ�

	if (GetKeyState('D') < 0)       acceleration.x += speed;	// �E��
	if (GetKeyState('A') < 0)       acceleration.x -= speed;	// ����
	if (GetKeyState('W') < 0)       acceleration.y += speed;	// ���
	if (GetKeyState('S') < 0)       acceleration.y -= speed;	// ����

	Velocity+= acceleration;
	Position += Velocity;

	// ���f���ɕ`��n�p�����[�^�[��n��
	Model->setPos(Position);
	Model->setAngle(Rotate);
	Model->setSize(Size);
	Model->setColor(Color);
}

void Player::Render() {
	Model->Render(SkinnedShader.get());
}