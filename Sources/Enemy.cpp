#include "Enemy.h"

#include "Stages.h"
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

	Capcule = std::make_unique<Geometric_Capsule>(1.0f, 10, 10);
}

void Enemy::Update() {
	Move();	// ���������Ƃ���


	// ���f���ɕ`��n�p�����[�^�[��n��
	Model->getParameters()->CopyParam(Parameters.get());

	Capcule->Parameters->CopyParam(Parameters.get());
	static constexpr float CAPCULESIZE = 0.6f;
	Capcule->Parameters->Scale = DirectX::SimpleMath::Vector3(CAPCULESIZE * 0.7f, CAPCULESIZE, CAPCULESIZE);
	Capcule->Parameters->Color = DirectX::SimpleMath::Vector4{ 1.0f,1.0f,1.0f,1.0f, };
	Capcule->Parameters->Orientation.x += 90;
}

void Enemy::Render() {
	Model->Render();
	//Capcule->Render(true);
}

void Enemy::Move()
{
	static float speed = 0.01f;
	Parameters->Acceleration = DirectX::SimpleMath::Vector3{ 0.0f, 0.0f, 0.0f };
	Parameters->Velocity = DirectX::SimpleMath::Vector3{ 0.0f, 0.0f, 0.0f };	// ���͒��������������߂ɖ��t���[�������� ���ʂ���Ȃ�

	Parameters->calcForward();
	static float MOVE_SPEED = 0.02f;

	//--------------------------------------------------------
//�O�i����
	if (GetKeyState('W') < 0) {
		Parameters->Velocity += Model->getWorld().Backward() * MOVE_SPEED;	// �O���Ɉړ�
	}
	//��ޏ���
	if (GetKeyState('S') < 0) {
		Parameters->Velocity += Model->getWorld().Backward() * MOVE_SPEED;	// �O���Ɉړ�
	}
	//��]����
	{
		//if (GetKeyState('D') < 0) {
		//	Parameters->Orientation *= DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(Model->getWorld().Up(), DirectX::XMConvertToRadians(4));
		//}
		//if (GetKeyState('A') < 0) {
		//	Parameters->Orientation *= DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(Model->getWorld().Up(), DirectX::XMConvertToRadians(4));
		//}
	}

	// Target�̕��Ɍ������� �G�̋����ɑg�ݍ������Ǝv���Ă�̂łƂ肠�������������
	FocusTarget(180.0f);

	Parameters->Position += Parameters->Velocity;
	//--------------------------------------------------------

	//Velocity+= acceleration;
	//Position += Velocity;
}

void Enemy::FocusTarget(float focusRange)
{
	float fov = DirectX::XMConvertToRadians(focusRange * 0.5f);	// �^�Q��ǂ�����p�𔼕��Ɍv�Z�B-fov�`fov�̊p�x�ɂ�����ǐՊJ�n����̂�
	DirectX::SimpleMath::Vector3 d = Target.Position - Parameters->Position; // �����x�N�g��
	if (d.Length() <= 0)return;	// �^�[�Q�b�g�Ƃ̋�����0�ȉ��̏ꍇ�A�������T�Z����ƃG���[�f�����̂Ői�݂܂���
	d.Normalize();

	DirectX::SimpleMath::Vector3 axis;	// ��]��
	DirectX::SimpleMath::Vector3 forward = Model->getWorld().Backward();	// ��]�� SimpleMath�̊֌W��t�ł�
	FLOAT		angle;	// ��]�p
	forward.Cross(d, axis);	// �O���ƖڕW�̊O��
	if (axis == DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f))return;	// ���̌v�Z�Ɏ��s�������ɂ͐i�܂Ȃ� ���s�c�H
	angle = acosf(forward.Dot(d));	// �O���ƖڕW�̊Ԃ̊p�x
	if (fabs(angle) > 1e-8f)	// ��]�p�������ȏꍇ�́A��]���s��Ȃ�
	{
		DirectX::SimpleMath::Quaternion q;
		q = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(axis, angle);// ���ŉ�]�p��
		if (-fov < angle && angle < fov) {	// �K�v��]�p������p�ȓ��Ɏ��܂��Ă���Ό���
			Parameters->Orientation = DirectX::SimpleMath::Quaternion::Slerp(Parameters->Orientation, Parameters->Orientation * q, 0.01f);// ���X�Ɍ������
		}
	}
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
	// ���݃t���O�̗����Ă��Ȃ��v�f�͍폜����
	for (auto it = Enemys.begin(); it != Enemys.end();)
	{
		if (!it->get()->getExist())
		{
			it = Enemys.erase(it);	//�v�f�폜�͎��̃C�e���[�^��Ԃ����ߎ蓮�Ŏ��ɐi�ޕK�v���Ȃ�
		}
		else {
			// ���݂��Ă���G�̍X�V
			it->get()->Update();
			if (GetAsyncKeyState('Q') < 0) {
				// Shot�̐���
				// TODO: �b�� �v�C��
				shotsManager->newSet(it->get()->Parameters.get());
				StageManager::getInstance().Check(*it->get()->Parameters);	// ���Ƀ_���[�W

			}

			++it;	// ����
		}
	}

	shotsManager->Update();
}

void EnemyManager::Render()
{
	for (auto it = Enemys.begin(); it != Enemys.end(); ++it)
	{
		// �G�̕`��
		it->get()->Render();
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