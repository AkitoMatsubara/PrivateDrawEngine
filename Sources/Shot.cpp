#include "Shot.h"
#include "Judge.h"
#include "Player.h"
#include "Enemy.h"

#include "XMFLOAT_Helper.h"

void Shot::Initialize()
{
	Model = std::make_unique<Skinned_Mesh>(".\\Resources\\Shots\\Shot.fbx");
	//Sphere = std::make_unique<Geometric_Sphere>();

	Parameters = std::make_unique<Object3d>();
	LifeTimer = 0;
	Exist = false;
}

void Shot::Update()
{
	// とりあえず前に進む動きを作る
	static const float MOVE_SPEED = 0.3f;
	Parameters->Velocity = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };	// 例のごとく毎フレーム加速度リセット
	Parameters->Velocity += Model->getWorld().Backward() * MOVE_SPEED;
	Parameters->Position += Parameters->Velocity;

	// 自然消滅 適当なので要修正
	LifeTimer += 0.1f;
	if (LifeTimer >= 20.0f) {
		LifeTimer = 0.0f;
		Exist = false;
	}

	// モデルに描画系パラメーターを渡す
	//Sphere->Parameters->CopyParam(Parameters.get());
	//Sphere->Parameters->Color = DirectX::SimpleMath::Vector4{ 1.0f,LifeTimer/20.0f,1.0f,1.0f };
	Model->getParameters()->CopyParam(Parameters.get());
}

void Shot::Render(ID3D11DeviceContext* device_context)
{
	if (Exist) { Model->Render(device_context); }
}

void Shot::set(const Object3d* parent)
{
	Parameters->CopyParam(parent);
	Parameters->Scale = DirectX::SimpleMath::Vector3{ 0.5f,0.5f,0.5f };
	Exist = true;
}

//----------------------------------------------------//
void ShotManager::Update()
{
	// 存在フラグの立っていない要素は削除する
	for (auto shots = Shots.begin(); shots != Shots.end();)
	{
		if (!shots->get()->getExist())	// こいつ…存在していないぞ!?
		{
			shots = this->Shots.erase(shots);	// 存在していない弾は消す
		}
		else {	// 存在が確認されたので処理します
			shots->get()->Update();

			if (Master == MASTER::PLAYER)	// プレイヤーが発射した弾の処理 敵とは主に当たり判定の処理が違う
			{
				for (auto enem = EnemyManager::getInstance().getEnemys()->begin(); enem != EnemyManager::getInstance().getEnemys()->end(); ++enem)
				{
					// 大前提判定を取る者の存在判定 両方とも存在している且つ
					if (shots->get()->getExist() && enem->get()->getExist())
					{
						// 当たり判定、戻り値trueであれば
						if (isHits(enem->get()->Parameters.get(), shots->get()))
						{
							//enem->get()->setExist(false);
							enem->get()->Destroy();
							shots->get()->setExist(false);
							break;	// 対象の弾が消えるのでEnemyFor文を終了
						}
					}
				}
			}
			else if (Master == MASTER::ENEMY)	// 敵が発射した弾の処理
			{
				// TODO インスタンスの使用 現状の一人プレイは問題ないが2Pを追加するとなれば改修必須
				if (shots->get()->getExist() && Player::getInstance()->Parameters->Exist)
				{
					if (isHits(Player::getInstance()->Parameters.get(), shots->get()))
					{
						// あたってます
						shots->get()->setExist(false);
						// ダメージ処理、残体力が0以下になれば存在削除
						// TODO Playerの生死をShotが操作していいものだろうか…
						if (Player::getInstance()->Damage()<=0) { Player::getInstance()->Parameters->Exist = false; }
					}
				}
			}
			++shots;	// 次へ
		}
	}
}

void ShotManager::Render(ID3D11DeviceContext* device_context)
{
	for (auto it = Shots.begin(); it != Shots.end(); ++it)
	{
		// 弾の描画
		it->get()->Render(device_context);
	}
}

void ShotManager::newSet(const Object3d* initData)
{
	std::unique_ptr<Shot>Shots = std::make_unique<Shot>();
	Shots->Initialize();
	Shots->set(initData);
	push(std::move(Shots));
}

bool ShotManager::isHit(const Object3d* capcule, const Shot* shots)
{
	if (Judge::getInstance()->c_b(*capcule, *shots->Parameters))
	{
		return true;
	}
	return false;
}

bool ShotManager::isHits(const Object3d* sphere, const Shot* shots)
{
	if (Judge::getInstance()->s_s(*sphere, (sphere->Scale.x * 0.5f), *shots->Parameters, (shots->Parameters->Scale.x)))
	{
		return true;
	}
	return false;
}