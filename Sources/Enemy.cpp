#include "Enemy.h"

#include "Stages.h"
#include "XMFLOAT_Helper.h"

void Enemy::Initialize() {
	Model = std::make_unique<Skinned_Mesh>(".\\Resources\\Enemy\\Enemy.fbx");	// 3角形化されていない複数メッシュ キューブ

	// パラメーターの初期化
	Parameters = std::make_unique<Object3d>();
	Parameters->Position = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };
	Parameters->Vector = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };
	Parameters->Acceleration = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };
	Parameters->Orientation = DirectX::SimpleMath::Quaternion{ 0.0f,0.0f,0.0f,1.0f };
	Parameters->Scale = DirectX::SimpleMath::Vector3{ 1.0f,1.0f,1.0f };
	Parameters->Color = DirectX::SimpleMath::Vector4{ 1.0f,1.0f,1.0f,1.0f };

	state = ENEMYSTATE::GOTARGET;
	ShotInterval = 5.0f;
	interval = 0.0f;
	shoted = false;

	Capcule = std::make_unique<Geometric_Capsule>(1.0f, 10, 10);
}

void Enemy::Update() {
	Move();	// うごかすところ

	// モデルに描画系パラメーターを渡す
	Model->getParameters()->CopyParam(Parameters.get());

#ifdef _DEBUG
	Capcule->Parameters->CopyParam(Parameters.get());
	static constexpr float CAPCULESIZE = 0.6f;
	Capcule->Parameters->Scale = DirectX::SimpleMath::Vector3(CAPCULESIZE * 0.7f, CAPCULESIZE, CAPCULESIZE);
	Capcule->Parameters->Color = DirectX::SimpleMath::Vector4{ 1.0f,1.0f,1.0f,1.0f, };
	Capcule->Parameters->Orientation.x += 90;
#endif
}

void Enemy::Render() {
	Model->Render();
	//Capcule->Render(true);
}

void Enemy::Move()
{
	static float speed = 0.01f;
	Parameters->Acceleration = DirectX::SimpleMath::Vector3{ 0.0f, 0.0f, 0.0f };
	Parameters->Velocity = DirectX::SimpleMath::Vector3{ 0.0f, 0.0f, 0.0f };	// 入力中だけ動かすために毎フレーム初期化 普通いらない?

	Parameters->calcForward();
	static constexpr float MOVE_SPEED = 0.02f;

	//--------------------------------------------------------
	if (StageManager::getInstance().RideParts(*Parameters))	// ステージに乗っているかどうか、乗っていれば行動する
	{
		switch (state)
		{
		case ENEMYSTATE::GOTARGET:
			//// Targetの方に向く処理 敵の挙動に組み込もうと思ってるのでとりあえず作った次第
			//FocusTarget(360.0f, 100.0f);
			//Parameters->Velocity += Model->getWorld().Backward() * MOVE_SPEED;	// モデル前方に移動
			GoStraight();
			break;
		case ENEMYSTATE::SHOT:
			Shot();
			break;
		}
	}
	// TODO デバッグ用 本実装終わったら消そうね
	if (GetAsyncKeyState('Q') <0)
	{
		state = ENEMYSTATE::SHOT;
	}
	if (GetAsyncKeyState('P') <0)
	{
		state = ENEMYSTATE::GOTARGET;
	}

	Parameters->Position += Parameters->Velocity;

	if (Parameters->Position.y <= -5.0f)Parameters->Exist = false;	// TODO マジックナンバー
	//--------------------------------------------------------

	//Velocity+= acceleration;
	//Position += Velocity;
}

void Enemy::FocusTarget(float focusAngle, float focusRange)
{
	float fov = DirectX::XMConvertToRadians(focusAngle * 0.5f);	// タゲを追う視野角を半分に計算。-fov～fovの角度にいたら追跡開始するので
	DirectX::SimpleMath::Vector3 d = Target.Position - Parameters->Position; // 方向ベクトル
	if (d.Length() <= 0 || d.Length() >= focusRange)return;	// ターゲットとの距離が0以下 もしくは 有効距離外の場合 は向きません
	d.Normalize();

	DirectX::SimpleMath::Vector3 axis;	// 回転軸
	DirectX::SimpleMath::Vector3 forward = Model->getWorld().Backward();	// 回転軸 SimpleMathの関係上座標系が逆です
	FLOAT angle;	// 回転角
	forward.Cross(d, axis);	// 前方と目標の外積
	if (axis == DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f))return;	// 軸の計算に失敗したら先には進まない 失敗…？
	angle = acosf(forward.Dot(d));	// 前方と目標の間の角度
	if (fabs(angle) > 1e-8f)	// 回転角が微小な場合は、回転を行わない
	{
		DirectX::SimpleMath::Quaternion q;
		q = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(axis, angle);// 軸で回転角回す
		if (-fov < angle && angle < fov) {	// 必要回転角が視野角以内に収まっていれば向く
			Parameters->Orientation = DirectX::SimpleMath::Quaternion::Slerp(Parameters->Orientation, Parameters->Orientation * q, 0.01f);// 徐々に向くやつ
		}
	}
}

void Enemy::GoStraight()
{
	static constexpr float MOVE_SPEED = 0.02f;
	// Targetの方に向く処理 敵の挙動に組み込もうと思ってるのでとりあえず作った次第
	FocusTarget(360.0f, 100.0f);
	Parameters->Velocity += Model->getWorld().Backward() * MOVE_SPEED;	// モデル前方に移動
	// いつもの距離確認
	DirectX::SimpleMath::Vector3 dist = Target.Position - Parameters->Position;
	if (dist.Length() <= 3.5f) { state = ENEMYSTATE::SHOT; }	// TODO マジックナンバー

}


void Enemy::Shot()
{
	if (!shoted)
	{
		EnemyManager::getInstance().getShotManager()->newSet(Parameters.get());
		StageManager::getInstance().Check(*Parameters);	// 床にダメージ
		shoted = true;
	}
	else
	{
		FocusTarget(360.0f, 100.0f);
		interval += 0.1f;	// TODO マジックナンバー? どう分けよう?
	}
	if(interval>=ShotInterval)
	{
		interval = 0.0f;
		shoted = false;
	}

	// いつもの距離確認
	DirectX::SimpleMath::Vector3 dist = Target.Position - Parameters->Position;
	if (dist.Length() >= 3.5f) { state = ENEMYSTATE::GOTARGET; }	// TODO マジックナンバー

}

//--------------------------------------------------//
//					EnemyManager					//
//--------------------------------------------------//

void EnemyManager::Initialize()
{
	Enemys.clear();

	shotsManager = std::make_unique<ShotManager>();
	shotsManager->Initialize(ShotManager::MASTER::ENEMY);
}

void EnemyManager::Update()
{
	// 存在フラグの立っていない要素は削除する
	for (auto enem = Enemys.begin(); enem != Enemys.end();)
	{
		if (!enem->get()->getExist())
		{
			enem = Enemys.erase(enem);	//要素削除は次のイテレータを返すため手動で次に進む必要がない
		}
		else {
			// 存在している敵の更新
			enem->get()->Update();
			++enem;	// 次へ
		}
	}

	shotsManager->Update();
}

void EnemyManager::Render()
{
	for (auto enem = Enemys.begin(); enem != Enemys.end(); ++enem)
	{
		// 敵の描画
		enem->get()->Render();
	}
	shotsManager->Render();
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
	}
}