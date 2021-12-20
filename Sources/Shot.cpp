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
	// �Ƃ肠�����O�ɐi�ޓ��������
	static const float MOVE_SPEED = 0.1f;
	Parameters->Velocity = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };	// ��̂��Ƃ����t���[�������x���Z�b�g
	Parameters->Velocity += Model->getWorld().Backward() * MOVE_SPEED;
	Parameters->Position += Parameters->Velocity;

	// ���R���� �K���Ȃ̂ŗv�C��
	LifeTimer += 0.1f;
	if (LifeTimer >= 20.0f) {
		LifeTimer = 0.0f;
		Exist = false;
	}

	// ���f���ɕ`��n�p�����[�^�[��n��
	//Sphere->Parameters->CopyParam(Parameters.get());
	//Sphere->Parameters->Color = DirectX::SimpleMath::Vector4{ 1.0f,LifeTimer/20.0f,1.0f,1.0f };
	Model->getParameters()->CopyParam(Parameters.get());
}

void Shot::Render()
{
	if (Exist) {
		Model->Render();
		//Sphere->Render();
	}
}

void Shot::set(const Object3d* parent)
{
	Parameters->CopyParam(parent);
	Exist = true;
}

//----------------------------------------------------//
void ShotManager::Update()
{
	// ���݃t���O�̗����Ă��Ȃ��v�f�͍폜����
	for (auto shots = Shots.begin(); shots != Shots.end();)
	{
		if (!shots->get()->getExist())	// �����c���݂��Ă��Ȃ���!?
		{
			shots = this->Shots.erase(shots);	// ���݂��Ă��Ȃ��e�͏���
		}
		else {	// ���݂��m�F���ꂽ�̂ŏ������܂�
			shots->get()->Update();
			if (Master == MASTER::PLAYER)	// �v���C���[�����˂����e�̏��� �G�Ƃ͎�ɓ����蔻��̏������Ⴄ
			{
				for (auto enem = EnemyManager::getInstance().getEnemys()->begin(); enem != EnemyManager::getInstance().getEnemys()->end(); ++enem)
				{
					// ��O�񔻒�����҂̑��ݔ��� �����Ƃ����݂��Ă��銎��
					if (shots->get()->getExist() && enem->get()->getExist())
					{
						// �����蔻��A�߂�ltrue�ł����
						if (isHit(enem->get()->Parameters.get(), shots->get()))
						{
							enem->get()->setExist(false);
							// TODO ���ݏ����������R�����g�A�E�g���Ă���̂̓f�o�b�O�p ��x�̒e�ŕ����̏�������
							shots->get()->setExist(false);
							break;
						}
					}
				}
			}
			else if (Master == MASTER::ENEMY)	// �G�����˂����e�̏���
			{
				// TODO �C���X�^���X�̎g�p ����̈�l�v���C�͖��Ȃ���2P��ǉ�����ƂȂ�Ή��C�K�{
				if (shots->get()->getExist() && Player::getInstance()->Parameters->Exist)
				{
					if (isHit(Player::getInstance()->Parameters.get(), shots->get()))
					{
						// �������Ă܂�
						shots->get()->setExist(false);
						//break;
					}
				}
			}
			++shots;	// ����
		}
	}
}

void ShotManager::Render()
{
	for (auto it = Shots.begin(); it != Shots.end(); ++it)
	{
		// �e�̕`��
		it->get()->Render();
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
	if (Judge::getInstance()->c_b(*capcule, 1.0f, *shots->Parameters))
	{
		return true;
	}
	return false;
}