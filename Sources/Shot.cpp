#include "Shot.h"
#include "Judge.h"
#include "Player.h"
#include "Enemy.h"

#include "XMFLOAT_Helper.h"

//std::unique_ptr<Skinned_Mesh> Shot::Model = std::make_unique<Skinned_Mesh>(".\\Resources\\Player\\Player.fbx");

void Shot::Initialize()
{
	//if(!Model)	Model = std::make_unique<Skinned_Mesh>(".\\Resources\\Player\\Player.fbx");
	test = std::make_unique<Geometric_Sphere>();
	SkinnedShader = std::make_unique<ShaderEx>();
	SkinnedShader->Create(L"Shaders\\static_mesh_vs", L"Shaders\\static_mesh_ps");

	Parameters = std::make_unique<Object3d>();

	LifeTimer = 0;
	Exist = false;
}

void Shot::Update()
{
	// �Ƃ肠�����O�ɐi�ޓ��������
	static DirectX::SimpleMath::Vector3 vel{ 0,0,0 };
	DirectX::SimpleMath::Vector3 pos = Parameters->Position + Parameters->Vector * 0.1;
	Parameters->Position = pos;

	// ���R���� �K���Ȃ̂ŗv�C��
	LifeTimer += 0.1f;
	if (LifeTimer >= 50.0f) {
		LifeTimer = 0.0f;
		Exist = false;
	}

	// ���f���ɕ`��n�p�����[�^�[��n��
	test->Parameters->CopyParam(Parameters.get());
	test->Parameters->Color = { 1.0f,fmodf(LifeTimer,50.0f),1.0f,1.0f };
	//test->Parameters->CopyParam(Model->getParameters());
	//Model->getParameters()->CopyParam(Parameters.get());
}

void Shot::Render()
{
	if (Exist) {
		//Model->Render(SkinnedShader.get());
		test->Render();
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
	for (auto it = Shots.begin(); it != Shots.end();)
	{
		if (!it->get()->getExist())	// �����c���݂��Ă��Ȃ���!?
		{
			it = this->Shots.erase(it);
		}
		else {	// ���݂��m�F���ꂽ�̂ŏ������܂�
			it->get()->Update();
			for (auto enem = EnemyManager::getInstance().getEnemys()->begin(); enem != EnemyManager::getInstance().getEnemys()->end();++enem)
			{
				if(isHit(enem->get()->Parameters.get()))
				{
					enem->get()->setExist(false);
				}
			}
			++it;	// ����
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

bool ShotManager::isHit(const Object3d* Capcule)
{
	bool hit = false;
	for (auto it = Shots.begin(); it != Shots.end(); ++it)
	{
		hit = Judge::getInstance()->c_b(*Capcule, *it->get()->Parameters);
	}
		return hit;
}