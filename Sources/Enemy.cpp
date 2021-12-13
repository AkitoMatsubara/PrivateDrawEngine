#include "Enemy.h"

#include "XMFLOAT_Helper.h"

void Enemy::Initialize() {
	Model = std::make_unique<Skinned_Mesh>(".\\Resources\\Enemy\\Enemy.fbx");	// 3角形化されていない複数メッシュ キューブ

	// パラメーターの初期化
	Parameters = std::make_unique<Object3d>();
	Parameters->Position = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };
	Parameters->Vector = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };
	Parameters->Acceleration = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };
	//Parameters->Rotate= DirectX::SimpleMath::Vector3{0.0f,0.0f,0.0f};
	Parameters->Orientation = DirectX::SimpleMath::Quaternion{ 0.0f,0.0f,0.0f,1.0f };
	Parameters->Scale = DirectX::SimpleMath::Vector3{ 1.0f,1.0f,1.0f };
	Parameters->Color = DirectX::SimpleMath::Vector4{ 1.0f,1.0f,1.0f,1.0f };

	Shots = std::make_unique<Shot>();
	Shots->Initialize();
	ShotsManager = std::make_unique<ShotManager>();
	ShotsManager->Initialize();

	Capcule = std::make_unique<Geometric_Capsule>(1.0f, 10, 10);
}

void Enemy::Update() {
	Move();	// うごかすところ

	// 弾の更新
	ShotsManager->Update();
	Shots->Update();

	// モデルに描画系パラメーターを渡す
	Model->getParameters()->CopyParam(Parameters.get());
	Capcule->Parameters->CopyParam(Parameters.get());
	static constexpr float CAPCULESIZE = 0.6f;
	Capcule->Parameters->Scale = DirectX::SimpleMath::Vector3(CAPCULESIZE * 0.7f, CAPCULESIZE, CAPCULESIZE);
	Capcule->Parameters->Color = DirectX::SimpleMath::Vector4{ 1.0f,1.0f,1.0f,1.0f, };
	Capcule->Parameters->Orientation.x += 90;
}

void Enemy::Render() {
	ShotsManager->Render();
	//Shots->Render();

	Model->Render();
	//Capcule->Render(true);
}

void Enemy::Move()
{
	static float speed = 0.01f;
	Parameters->Acceleration = DirectX::SimpleMath::Vector3{ 0.0f, 0.0f, 0.0f };
	Parameters->Velocity = DirectX::SimpleMath::Vector3{ 0.0f, 0.0f, 0.0f };	// 入力中だけ動かすために毎フレーム初期化 普通いらない

	Parameters->calcForward();

	//--------------------------------------------------------
//前進処理
	if (GetKeyState('W') < 0) {
		DirectX::XMStoreFloat3(&Parameters->Vector, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&Parameters->Vector)));

		float dx = Parameters->Position.x - Parameters->Vector.x * speed;
		float dz = Parameters->Position.z - Parameters->Vector.z * speed;
		Parameters->Position = DirectX::SimpleMath::Vector3{ dx,Parameters->Position.y,dz };
	}
	//後退処理
	if (GetKeyState('S') < 0) {
		float dx = Parameters->Position.x + Parameters->Vector.x * speed;
		float dz = Parameters->Position.z + Parameters->Vector.z * speed;
		Parameters->Position = DirectX::SimpleMath::Vector3{ dx,Parameters->Position.y,dz };
	}
	//回転処理
	{
		if (GetKeyState('D') < 0) {
			Parameters->Orientation *= DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(Model->getWorld().Up(), DirectX::XMConvertToRadians(4));
		}
		if (GetKeyState('A') < 0) {
			Parameters->Orientation *= DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(Model->getWorld().Up(), DirectX::XMConvertToRadians(4));
		}
	}
	if (GetAsyncKeyState('Q') & 1) {
		// Shotの生成
		ShotsManager->newSet(Parameters.get());
	}

	// Targetの方に向く処理 敵の挙動に組み込もうと思ってるのでとりあえず作った次第
	{
		DirectX::SimpleMath::Vector3 d = Target.Position - Parameters->Position; // 方向ベクトル
		if (d.Length() <= 0)return;	// Targetとの距離が0以下の場合、これより先卦算するとエラー吐かれるので進みません
		d.Normalize();

		DirectX::SimpleMath::Vector3 axis;	// 回転軸
		DirectX::SimpleMath::Vector3 forward = Model->getWorld().Backward();	// 回転軸 SimpleMathの関係上逆です
		FLOAT		angle;	// 回転角
		forward.Cross(d, axis);	// 前方と目標の外積
		angle = acosf(forward.Dot(d));	// 前方と目標の間の角度
		if (fabs(angle) > 1e-8f)	// 回転角が微小な場合は、回転を行わない
		{
			DirectX::SimpleMath::Quaternion q;
			q = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(axis, angle);// 軸で回転角回す
			if (angle < DirectX::XMConvertToRadians(100) && angle> DirectX::XMConvertToDegrees(-100)) {	// 視野角200度以内に目標があるときのみ向く
				Parameters->Orientation = DirectX::SimpleMath::Quaternion::Slerp(Parameters->Orientation, Parameters->Orientation * q, 0.02f);// 徐々に向くやつ
			}
		}
	}

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
		Enemys->setTarget(*initData);
		Enemys->setExist(true);
		push(std::move(Enemys));
		Enemys.reset();
	}
}