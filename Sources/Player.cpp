#include "Player.h"

#include "XMFLOAT_Helper.h"

void Player::Initialize() {
	Model = make_unique<Skinned_Mesh>(".\\Resources\\cube.003.0.fbx");	// 3角形化されていない複数メッシュ キューブ
	SkinnedShader = std::make_unique<ShaderEx>();
	SkinnedShader->Create(L"Shaders\\skinned_mesh_vs", L"Shaders\\skinned_mesh_ps");

	// パラメーターの初期化
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
	Velocity = { 0,0,0 };	// 入力中だけ動かすために毎フレーム初期化 普通いらない

	if (GetKeyState('D') < 0)       acceleration.x += speed;	// 右に
	if (GetKeyState('A') < 0)       acceleration.x -= speed;	// 左に
	if (GetKeyState('W') < 0)       acceleration.y += speed;	// 上に
	if (GetKeyState('S') < 0)       acceleration.y -= speed;	// 下に

	Velocity+= acceleration;
	Position += Velocity;

	// モデルに描画系パラメーターを渡す
	Model->setPos(Position);
	Model->setAngle(Rotate);
	Model->setSize(Size);
	Model->setColor(Color);
}

void Player::Render() {
	Model->Render(SkinnedShader.get());
}