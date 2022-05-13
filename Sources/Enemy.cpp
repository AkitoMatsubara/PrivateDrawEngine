#include "Enemy.h"

#include "Stages.h"
#include "XMFLOAT_Helper.h"

#include "framework.h"

void Enemy::Initialize() {
	Model = std::make_unique<Skinned_Mesh>(".\\Resources\\Enemy\\Enemy.fbx");

	// �p�����[�^�[�̏�����
	Parameters = std::make_unique<Object3d>();
	Parameters->Position = DirectX::SimpleMath::Vector3((rand() % 25) - 12.0f, 0.0f, (rand() % 25) - 12.0f);
	Parameters->Acceleration = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };
	Parameters->Orientation = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle({ 0.0f,1.0f,0.0f }, DirectX::XMConvertToRadians(static_cast<float>(rand() % 180)));
	Parameters->Scale = DirectX::SimpleMath::Vector3{ 1.0f,1.0f,1.0f };
	Parameters->Color = DirectX::SimpleMath::Vector4{ 0.6f,0.6f,1.0f,1.0f };
	Parameters->Exist = true;

	state = ENEMYSTATE::GOTARGET;
	ShotInterval = 5.0f;
	interval = 0.0f;
	shoted = false;
	death = false;

	Capcule = std::make_unique<Geometric_Capsule>(1.0f, 10, 10);
}

void Enemy::Update() {
	Move();	// ���������Ƃ���
	if (death) { Parameters->Exist = false; }
	// ���f���ɕ`��n�p�����[�^�[��n��
	Model->getParameters()->CopyParam(Parameters.get());

#ifdef _DEBUG
	Capcule->Parameters->CopyParam(Parameters.get());
	static constexpr float CAPCULESIZE = 0.6f;
	Capcule->Parameters->Scale = DirectX::SimpleMath::Vector3(CAPCULESIZE * 0.7f, CAPCULESIZE, CAPCULESIZE);
	Capcule->Parameters->Color = DirectX::SimpleMath::Vector4{ 1.0f,1.0f,0.0f,1.0f, };
	Capcule->Parameters->Orientation.x += 90;
#endif
}

void Enemy::Render(Shader* shader) {
	(shader) ? Model->Render(shader) : Model->Render();
	//Capcule->Render(true);
}

void Enemy::Move()
{
	static float speed = 0.01f;
	Parameters->Acceleration = DirectX::SimpleMath::Vector3{ 0.0f, 0.0f, 0.0f };
	Parameters->Velocity = DirectX::SimpleMath::Vector3{ 0.0f, 0.0f, 0.0f };	// ���͒��������������߂ɖ��t���[�������� ���ʂ���Ȃ�?

	static constexpr float MOVE_SPEED = 0.02f;

	//--------------------------------------------------------
	if (StageManager::getInstance().RideParts(*Parameters, Parameters->Scale.x * 0.5f))	// �X�e�[�W�ɏ���Ă��邩�ǂ����A����Ă���΍s������
	{
		Parameters->Position.y = 0.0f;	// TODO debug:�����ڏ�X�e�[�W�̏�ɂ���
		switch (state)
		{
		case ENEMYSTATE::GOTARGET:
			// Target�̕��Ɍ������� �G�̋����ɑg�ݍ������Ǝv���Ă�̂łƂ肠�������������
			GoStraight();
			break;
		case ENEMYSTATE::SHOT:
			Shot();
			break;
		}
	}
	else {
		Parameters->Position.y -= 0.005f;
	}

	Parameters->Position += Parameters->Velocity;

	if (Parameters->Position.y <= -5.0f)Parameters->Exist = false;	// TODO �}�W�b�N�i���o�[
	//--------------------------------------------------------

	//Velocity+= acceleration;
	//Position += Velocity;
}

void Enemy::FocusTarget(float focusAngle, float focusRange)
{
	float fov = DirectX::XMConvertToRadians(focusAngle * 0.5f);	// �^�Q��ǂ�����p�𔼕��Ɍv�Z�B-fov�`fov�̊p�x�ɂ�����ǐՊJ�n����̂�
	DirectX::SimpleMath::Vector3 d = Target.Position - Parameters->Position; // �����x�N�g��
	if (d.Length() <= 0 || d.Length() >= focusRange) return;	// �^�[�Q�b�g�Ƃ̋�����0�ȉ� �������� �L�������O�̏ꍇ �͌����܂���
	d.Normalize();

	DirectX::SimpleMath::Vector3 axis;	// ��]��
	DirectX::SimpleMath::Vector3 forward = Model->getWorld().Backward();	// ��]�� SimpleMath�̊֌W����W�n���t�ł�
	FLOAT angle;	// ��]�p
	forward.Cross(d, axis);	// �O���ƖڕW�̊O��
	if (axis == DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f)) return;	// ���̌v�Z�Ɏ��s�������ɂ͐i�܂Ȃ� ���s�c�H
	angle = acosf(forward.Dot(d));	// �O���ƖڕW�̊Ԃ̊p�x
	if (fabs(angle) > 1e-8f)	// ��]�p�������ȏꍇ�́A��]���s��Ȃ�
	{
		DirectX::SimpleMath::Quaternion q;
		q = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(axis, angle);// ���ŉ�]�p��
		if (-fov < angle && angle < fov) {	// �K�v��]�p������p�ȓ��Ɏ��܂��Ă���Ό���
			Parameters->Orientation = DirectX::SimpleMath::Quaternion::Slerp(Parameters->Orientation, Parameters->Orientation * q, 0.05f);// ���X�Ɍ������
		}
	}
}

void Enemy::GoStraight()
{
	static constexpr float MOVE_SPEED = 0.02f;
	// Target�̕��Ɍ������� �G�̋����ɑg�ݍ������Ǝv���Ă�̂łƂ肠�������������
	FocusTarget(280.0f, 100.0f);
	Parameters->Velocity += Model->getWorld().Backward() * MOVE_SPEED;	// ���f���O���Ɉړ�
	// �����̋����m�F
	DirectX::SimpleMath::Vector3 dist = Target.Position - Parameters->Position;
	if (dist.Length() <= 3.5f) { state = ENEMYSTATE::SHOT; }	// TODO �}�W�b�N�i���o�[

}


void Enemy::Shot()
{
	if (!shoted)
	{
		EnemyManager::getInstance().getShotManager()->newSet(Parameters.get());
		StageManager::getInstance().Check(*Parameters, Parameters->Scale.x * 0.5f);	// ���Ƀ_���[�W
		shoted = true;
	}
	else
	{
		FocusTarget(360.0f, 100.0f);
		interval += 0.1f;	// TODO �}�W�b�N�i���o�[? �ǂ������悤?
	}
	if(interval>=ShotInterval)
	{
		interval = 0.0f;
		shoted = false;
	}

	// �����̋����m�F
	DirectX::SimpleMath::Vector3 dist = Target.Position - Parameters->Position;
	if (dist.Length() >= 3.5f) { state = ENEMYSTATE::GOTARGET; }	// TODO �}�W�b�N�i���o�[

}

bool Enemy::Destroy() {
	death = true;
	return true;
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
	// ���Ԃ𑀍삵�Ă���̂Ŕ͈�for�ł͖��������Ȃ̂ł����for���ɂȂ���
	for (auto enem = Enemys.begin(); enem != Enemys.end();)
	{
		if (!enem->get()->getExist())
		{
			enem = Enemys.erase(enem);	//�v�f�폜�͎��̃C�e���[�^��Ԃ����ߎ蓮�Ŏ��ɐi�ޕK�v���Ȃ�
		}
		else {
			// ���݂��Ă���G�̍X�V
			enem->get()->Update();
			++enem;	// ����
		}
	}
	shotsManager->Update();
}

void EnemyManager::Render(Shader* shader)
{
	for (auto enem = Enemys.begin(); enem != Enemys.end(); ++enem)
	{
		// �G�̕`��
		enem->get()->Render(shader);
	}
	shotsManager->Render();
}

void EnemyManager::newSet(const Object3d* initData)
{
	{
		std::unique_ptr<Enemy>Enemys = std::make_unique<Enemy>();
		Enemys->Initialize();
		Enemys->setTarget(*initData);
		push(std::move(Enemys));
	}
}