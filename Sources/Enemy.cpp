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

	state = ENEMYSTATE::GOTARGET;
	ShotInterval = 5.0f;
	interval = 0.0f;
	shoted = false;

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
	Parameters->Velocity = DirectX::SimpleMath::Vector3{ 0.0f, 0.0f, 0.0f };	// ���͒��������������߂ɖ��t���[�������� ���ʂ���Ȃ�?

	Parameters->calcForward();
	static float MOVE_SPEED = 0.02f;

	//--------------------------------------------------------
	switch (state)
	{
	case ENEMYSTATE::GOTARGET:
		// Target�̕��Ɍ������� �G�̋����ɑg�ݍ������Ǝv���Ă�̂łƂ肠�������������
		FocusTarget(360.0f, 100.0f);
		Parameters->Velocity += Model->getWorld().Backward() * MOVE_SPEED;	// ���f���O���Ɉړ�
		break;
	case ENEMYSTATE::SHOT:
		Shot();
	}

	// TODO �f�o�b�O�p ��������
	if (GetAsyncKeyState('Q') <0)
	{
		state = ENEMYSTATE::SHOT;
	}
	if (GetAsyncKeyState('P') <0)
	{
		state = ENEMYSTATE::GOTARGET;
	}

	Parameters->Position += Parameters->Velocity;
	//--------------------------------------------------------

	//Velocity+= acceleration;
	//Position += Velocity;
}

void Enemy::FocusTarget(float focusAngle, float focusRange)
{
	float fov = DirectX::XMConvertToRadians(focusAngle * 0.5f);	// �^�Q��ǂ�����p�𔼕��Ɍv�Z�B-fov�`fov�̊p�x�ɂ�����ǐՊJ�n����̂�
	DirectX::SimpleMath::Vector3 d = Target.Position - Parameters->Position; // �����x�N�g��
	if (d.Length() <= 0 || d.Length() >= focusRange)return;	// �^�[�Q�b�g�Ƃ̋�����0�ȉ� �������� �L�������O�̏ꍇ �͌����܂���
	d.Normalize();

	DirectX::SimpleMath::Vector3 axis;	// ��]��
	DirectX::SimpleMath::Vector3 forward = Model->getWorld().Backward();	// ��]�� SimpleMath�̊֌W����W�n���t�ł�
	FLOAT angle;	// ��]�p
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

void Enemy::Shot()
{
	if (!shoted)
	{
		EnemyManager::getInstance().getShotManager()->newSet(Parameters.get());
		StageManager::getInstance().Check(*Parameters);	// ���Ƀ_���[�W
		shoted = true;
	}
	else
	{
		interval += 0.1f;
	}
	if(interval>=ShotInterval)
	{
		interval = 0.0f;
		shoted = false;
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
	for (auto enem = Enemys.begin(); enem != Enemys.end();)
	{
		if (!enem->get()->getExist())
		{
			enem = Enemys.erase(enem);	//�v�f�폜�͎��̃C�e���[�^��Ԃ����ߎ蓮�Ŏ��ɐi�ޕK�v���Ȃ�
		}
		else {
			// ���݂��Ă���G�̍X�V
			enem->get()->Update();
			//if (GetAsyncKeyState('Q') < 0) {
			//	// Shot�̐���
			//	// TODO: �b�� �v�C��
			//	shotsManager->newSet(it->get()->Parameters.get());
			//	StageManager::getInstance().Check(*it->get()->Parameters);	// ���Ƀ_���[�W
			//}

			++enem;	// ����
		}
	}

	shotsManager->Update();
}

void EnemyManager::Render()
{
	for (auto enem = Enemys.begin(); enem != Enemys.end(); ++enem)
	{
		// �G�̕`��
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