#include "Enemy.h"

#include "XMFLOAT_Helper.h"

void Enemy::Initialize() {
	Model = std::make_unique<Skinned_Mesh>(".\\Resources\\Enemy\\Enemy.fbx");	// 3角形化されていない複数メッシュ キューブ
	//if (!Model)
	//{
	//	Model = std::make_shared<Skinned_Mesh>(".\\Resources\\Enemy\\Enemy.fbx");	// 3角形化されていない複数メッシュ キューブ
	//}
	SkinnedShader = std::make_unique<ShaderEx>();
	SkinnedShader->Create(L"Shaders\\skinned_mesh_vs", L"Shaders\\skinned_mesh_ps");

	// パラメーターの初期化
	Parameters = std::make_unique<Object3d>();
	Parameters->Position = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };
	Parameters->Vector = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };
	Parameters->Acceleration = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };
	Parameters->Rotate= DirectX::SimpleMath::Vector3{0.0f,0.0f,0.0f};
	Parameters->Scale= DirectX::SimpleMath::Vector3{1.0f,1.0f,1.0f};
	Parameters->Color= DirectX::SimpleMath::Vector4{1.0f,1.0f,1.0f,1.0f};

	Exist = false;

	Shots = std::make_unique<Shot>();
	Shots->Initialize();

	Capcule = std::make_unique<Geometric_Capsule>(1.0f, 10, 10);
}

void Enemy::Update() {

	Move();

	Shots->Update();

	// モデルに描画系パラメーターを渡す
	Model->setPos(Parameters->Position);
	Model->setAngle(Parameters->Rotate);
	Model->setSize(Parameters->Scale);
	Model->setColor(Parameters->Color);

	Capcule->Parameters->CopyParam(Parameters.get());
	static const float CAPCULESIZE = 0.6f;
	Capcule->Parameters->Scale = DirectX::SimpleMath::Vector3(CAPCULESIZE * 0.7f, CAPCULESIZE, CAPCULESIZE);
	Capcule->Parameters->Color = DirectX::SimpleMath::Vector4{ 1.0f,1.0f,1.0f,1.0f, };
	Capcule->Parameters->Rotate.x += 90;
}

void Enemy::Render() {
	//Shots->Render();

	Model->Render(SkinnedShader.get());
	Capcule->Render(true);
}

void Enemy::Move()
{
	static float speed = 0.01f;
	Parameters->Acceleration = DirectX::SimpleMath::Vector3{ 0.0f, 0.0f, 0.0f };
	Parameters->Velocity = DirectX::SimpleMath::Vector3{ 0.0f, 0.0f, 0.0f };	// 入力中だけ動かすために毎フレーム初期化 普通いらない

	Parameters->calcForward();


		//--------------------------------------------------------
	//前進処理
	if (GetKeyState('W') < 0){
		DirectX::XMStoreFloat3(&Parameters->Vector,DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&Parameters->Vector)));

		float dx = Parameters->Position.x - Parameters->Vector.x * speed;
		float dz = Parameters->Position.z - Parameters->Vector.z * speed;
		Parameters->Position = DirectX::SimpleMath::Vector3{ dx,Parameters->Position.y,dz };
	}
	//後退処理
	if (GetKeyState('S') < 0){
		float dx = Parameters->Position.x + Parameters->Vector.x * speed;
		float dz = Parameters->Position.z + Parameters->Vector.z * speed;
		Parameters->Position = DirectX::SimpleMath::Vector3{ dx,Parameters->Position.y,dz };
	}
	//回転処理
	{
		if (GetKeyState('D') < 0){
			Parameters->Rotate.y -= DirectX::XMConvertToRadians(40);
		}
		if (GetKeyState('A') < 0){
			Parameters->Rotate.y += DirectX::XMConvertToRadians(40);
		}
	}

	//if (GetKeyState('Z') < 0){
	//	Shots->set(Parameters.get());
	//}

	//--------------------------------------------------------

	//Velocity+= acceleration;
	//Position += Velocity;

}

//----------------------------------------------------//
void EnemyManager::Update()
{
	// 存在フラグの立っていない要素は削除する
	for (auto it = Enemys.begin(); it != Enemys.end();)
	{
		if (!it->get()->getExist())
		{
			it = this->Enemys.erase(it);
		}
		else {
			// 存在している敵の更新
			it->get()->Update();
			++it;	// 次へ
		}
	}
}

void EnemyManager::Render()
{
	for (auto it = Enemys.begin(); it != Enemys.end(); ++it)
	{
		// 敵の描画
		it->get()->Render();
	}
}

void EnemyManager::newSet(const Object3d* initData)
{
	{
		std::unique_ptr<Enemy>Enemys = std::make_unique<Enemy>();
		Enemys->Initialize();
		Enemys->Parameters->CopyParam(initData);
		Enemys->setExist(true);
		push(std::move(Enemys));
		Enemys.reset();
	}
}