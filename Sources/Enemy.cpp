#include "Enemy.h"

#include "XMFLOAT_Helper.h"

void Enemy::Initialize() {
	Model = std::make_unique<Skinned_Mesh>(".\\Resources\\Enemy\\Enemy.fbx");	// 3�p�`������Ă��Ȃ��������b�V�� �L���[�u
	SkinnedShader = std::make_unique<ShaderEx>();
	SkinnedShader->Create(L"Shaders\\skinned_mesh_vs", L"Shaders\\skinned_mesh_ps");

	// �p�����[�^�[�̏�����
	Parameters = std::make_unique<Object3d>();
	Parameters->Position={0.0f,0.0f,0.0f};
	Parameters->Vector = { 0.0f,0.0f,0.0f };
	Parameters->Acceleration = { 0.0f,0.0f,0.0f };
	Parameters->Rotate={0.0f,0.0f,0.0f};
	Parameters->Scale={1.0f,1.0f,1.0f};
	Parameters->Color={1.0f,1.0f,1.0f,1.0f};

	Exist = false;

	Shots = std::make_unique<Shot>();
	Shots->Initialize();

	Capcule = std::make_unique<Geometric_Capsule>(1.0f, 1.0f, 10, 10);
}

void Enemy::Update() {

	Move();

	Shots->Update();

	// ���f���ɕ`��n�p�����[�^�[��n��
	Model->setPos(Parameters->Position);
	Model->setAngle(Parameters->Rotate);
	Model->setSize(Parameters->Scale);
	Model->setColor(Parameters->Color);

	Capcule->Parameters->CopyParam(Parameters.get());
	Capcule->Parameters->Color = { 1.0f,1.0f,1.0f,1.0f, };
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
	Parameters->Acceleration={0.0f, 0.0f, 0.0f};
	Parameters->Velocity={.0f, 0.0f, 0.0f};	// ���͒��������������߂ɖ��t���[�������� ���ʂ���Ȃ�

	Parameters->Vector.x = sinf(DirectX::XMConvertToRadians(Parameters->Rotate.y));
	Parameters->Vector.z = cosf(DirectX::XMConvertToRadians(Parameters->Rotate.y));



		//--------------------------------------------------------
	//�O�i����
	if (GetKeyState('W') < 0){
		DirectX::XMStoreFloat3(&Parameters->Vector,DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&Parameters->Vector)));

		float dx = Parameters->Position.x - Parameters->Vector.x * speed;
		float dz = Parameters->Position.z - Parameters->Vector.z * speed;
		Parameters->Position = { dx,Parameters->Position.y,dz };
	}
	//��ޏ���
	if (GetKeyState('S') < 0){
		float dx = Parameters->Position.x + Parameters->Vector.x * speed;
		float dz = Parameters->Position.z + Parameters->Vector.z * speed;
		Parameters->Position = { dx,Parameters->Position.y,dz };
	}
	//��]����
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
	// ���݃t���O�̗����Ă��Ȃ��v�f�͍폜����
	for (auto it = Enemys.begin(); it != Enemys.end();)
	{
		if (!it->get()->getExist())
		{
			it = this->Enemys.erase(it);
		}
		else {
			// ���݂��Ă���G�̍X�V
			it->get()->Update();
			++it;	// ����
		}
	}
}

void EnemyManager::Render()
{
	for (auto it = Enemys.begin(); it != Enemys.end(); ++it)
	{
		// �G�̕`��
		it->get()->Render();
	}
}

void EnemyManager::newSet(const Object3d* initData)
{
	std::unique_ptr<Enemy>Enemys = std::make_unique<Enemy>();
	Enemys->Initialize();
	Enemys->Parameters->CopyParam(initData);
	Enemys->setExist(true);
	push(std::move(Enemys));
}