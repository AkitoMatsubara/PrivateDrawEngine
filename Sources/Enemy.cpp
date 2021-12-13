#include "Enemy.h"

#include "XMFLOAT_Helper.h"

void Enemy::Initialize() {
	Model = std::make_unique<Skinned_Mesh>(".\\Resources\\Enemy\\Enemy.fbx");	// 3�p�`������Ă��Ȃ��������b�V�� �L���[�u

	// �p�����[�^�[�̏�����
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
	Move();	// ���������Ƃ���

	// �e�̍X�V
	ShotsManager->Update();
	Shots->Update();

	// ���f���ɕ`��n�p�����[�^�[��n��
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
	Parameters->Velocity = DirectX::SimpleMath::Vector3{ 0.0f, 0.0f, 0.0f };	// ���͒��������������߂ɖ��t���[�������� ���ʂ���Ȃ�

	Parameters->calcForward();

	//--------------------------------------------------------
//�O�i����
	if (GetKeyState('W') < 0) {
		DirectX::XMStoreFloat3(&Parameters->Vector, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&Parameters->Vector)));

		float dx = Parameters->Position.x - Parameters->Vector.x * speed;
		float dz = Parameters->Position.z - Parameters->Vector.z * speed;
		Parameters->Position = DirectX::SimpleMath::Vector3{ dx,Parameters->Position.y,dz };
	}
	//��ޏ���
	if (GetKeyState('S') < 0) {
		float dx = Parameters->Position.x + Parameters->Vector.x * speed;
		float dz = Parameters->Position.z + Parameters->Vector.z * speed;
		Parameters->Position = DirectX::SimpleMath::Vector3{ dx,Parameters->Position.y,dz };
	}
	//��]����
	{
		if (GetKeyState('D') < 0) {
			Parameters->Orientation *= DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(Model->getWorld().Up(), DirectX::XMConvertToRadians(4));
		}
		if (GetKeyState('A') < 0) {
			Parameters->Orientation *= DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(Model->getWorld().Up(), DirectX::XMConvertToRadians(4));
		}
	}
	if (GetAsyncKeyState('Q') & 1) {
		// Shot�̐���
		ShotsManager->newSet(Parameters.get());
	}

	// Target�̕��Ɍ������� �G�̋����ɑg�ݍ������Ǝv���Ă�̂łƂ肠�������������
	{
		DirectX::SimpleMath::Vector3 d = Target.Position - Parameters->Position; // �����x�N�g��
		if (d.Length() <= 0)return;	// Target�Ƃ̋�����0�ȉ��̏ꍇ�A�������T�Z����ƃG���[�f�����̂Ői�݂܂���
		d.Normalize();

		DirectX::SimpleMath::Vector3 axis;	// ��]��
		DirectX::SimpleMath::Vector3 forward = Model->getWorld().Backward();	// ��]�� SimpleMath�̊֌W��t�ł�
		FLOAT		angle;	// ��]�p
		forward.Cross(d, axis);	// �O���ƖڕW�̊O��
		angle = acosf(forward.Dot(d));	// �O���ƖڕW�̊Ԃ̊p�x
		if (fabs(angle) > 1e-8f)	// ��]�p�������ȏꍇ�́A��]���s��Ȃ�
		{
			DirectX::SimpleMath::Quaternion q;
			q = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(axis, angle);// ���ŉ�]�p��
			if (angle < DirectX::XMConvertToRadians(100) && angle> DirectX::XMConvertToDegrees(-100)) {	// ����p200�x�ȓ��ɖڕW������Ƃ��̂݌���
				Parameters->Orientation = DirectX::SimpleMath::Quaternion::Slerp(Parameters->Orientation, Parameters->Orientation * q, 0.02f);// ���X�Ɍ������
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