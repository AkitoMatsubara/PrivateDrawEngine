
#include "Player.h"

void Player::Initialize() {
	Model = make_unique<Skinned_Mesh>(".\\Resources\\cube.003.1.fbx", Skinned_Mesh::CST_RIGHT_Z, true);	// 3�p�`������Ă��Ȃ��������b�V�� �L���[�u
	SkinnedShader = std::make_unique<ShaderEx>();
	SkinnedShader->Create(L"Shaders\\skinned_mesh_vs", L"Shaders\\skinned_mesh_ps");

	Position = { 0,0,0 };
	Size = { 1,1,1 };
	Rotate = { 0,0,0 };
	Color = { 1,1,1,1 };

}

void Player::Update() {

}

void Player::Render() {
	Model->Render(SkinnedShader.get());
}