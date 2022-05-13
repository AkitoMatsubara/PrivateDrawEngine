#include "Player.h"
#include "Judge.h"
#include "Stages.h"
#include "XMFLOAT_Helper.h"
#include  "framework.h"

static float imguiPos[3] = { 0.0f };
static float imguiRot= { 0.0f };

void Player::Initialize() {
	Model = std::make_unique<Skinned_Mesh>(".\\Resources\\Player\\Player.fbx");	// �ۃ��f��
	//Model = std::make_unique<Skinned_Mesh>(".\\Resources\\Player\\Player_.fbx");	// 3�p�`���f��

	// �p�����[�^�[�̏�����
	Parameters = std::make_unique<Object3d>();
	Parameters->Position     = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };
	Parameters->Acceleration = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };
	Parameters->Orientation  = DirectX::SimpleMath::Quaternion{ 0.0f,0.0f,0.0f,1.0f };
	Parameters->Scale        = DirectX::SimpleMath::Vector3{ 1.0f,1.0f,1.0f };
	Parameters->Color        = DirectX::SimpleMath::Vector4{ 0.0f,1.0f,0.5f,1.0f };
	Parameters->CurLife      = Parameters->MaxLife = 5;

	ShotsManager = std::make_unique<ShotManager>();
	ShotsManager->Initialize(ShotManager::MASTER::PLAYER);

	Capcule = std::make_unique<Geometric_Capsule>(1.0f, 10, 10);
	testSphere2 = std::make_unique<Geometric_Sphere>();
	playerHitSphere = std::make_unique<Geometric_Sphere>();

	instance = this;
}

void Player::Update() {
	Control();	// ����n

	// �e�̍X�V
	ShotsManager->Update();

	// ���f���ɕ`��n�p�����[�^�[��n��
	Model->getParameters()->CopyParam(Parameters.get());
	// �X�e�[�W�̏�ɏ���Ă��邩����
	if (StageManager::getInstance().RideParts(*Parameters, Parameters->Scale.x * 0.5f)) {
		Parameters->Position.y = 0.0f;	// TODO debug:�����ڏ�X�e�[�W�̏�ɂ���
	}
	else {
		Parameters->Position.y -= 0.05f;
	}


	//-----------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef _DEBUG
	Capcule->Parameters->CopyParam(Parameters.get());	// Player�ɕt������悤�Ɉʒu�𓯊�
	static const float CAPCULESIZE = 0.7f;
	Capcule->Parameters->Scale = DirectX::SimpleMath::Vector3(CAPCULESIZE , CAPCULESIZE, CAPCULESIZE);
	Capcule->Parameters->Color = DirectX::SimpleMath::Vector4{ 1.0f,1.0f,1.0f,1.0f, };

	Capcule->Parameters->Orientation = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(Model->getWorld().Right(), DirectX::XMConvertToRadians(90));

	static const float SPHERESIZE = 0.5f;
	testSphere2->Parameters->CopyParam(Parameters.get());
	testSphere2->Parameters->Scale = DirectX::SimpleMath::Vector3{ SPHERESIZE,SPHERESIZE,SPHERESIZE };
	//testSphere2->Parameters->Position += Capcule->Parameters->Vector * (Capcule->Parameters->Scale *imguiPos[0]);	// �O���m�F�p
	testSphere2->Parameters->Position += DirectX::SimpleMath::Vector3{ imguiPos[0] ,imguiPos[1] ,imguiPos[2] };

	playerHitSphere->Parameters->CopyParam(Parameters.get());
	playerHitSphere->Parameters->Color = DirectX::SimpleMath::Vector4{ 1.0f,1.0f,1.0f,1.0f };

	// ����������A�������ĂȂ������甒�ɂ���
	//if (ShotsManager->isHit(Parameters.get()))
	//if (Judge::getInstance()->c_b(*Capcule->Parameters, 0.5f, *testSphere2->Parameters))
	//if (Judge::getInstance()->JudgeCap_Sphe(*Capcule, *testSphere2))
	//if (Judge::getInstance()->c_b(*Capcule->Parameters.get(), Capcule->Height, Capcule->Radian, *testSphere2->Parameters.get(), testSphere2->Radian))
	//{
	//	testSphere2->Parameters->Color = DirectX::SimpleMath::Vector4{ 1.0f,0.0f,0.0f,1.0f };
	//}
	//else
	//{
	//	testSphere2->Parameters->Color = DirectX::SimpleMath::Vector4{ 1.0f,1.0f,1.0f,1.0f };
	//}
	if (Judge::getInstance()->s_s(*playerHitSphere->Parameters.get(),playerHitSphere->getSize().x*0.5f,*testSphere2->Parameters.get(), testSphere2->Radian))
	{
		testSphere2->Parameters->Color = DirectX::SimpleMath::Vector4{ 1.0f,0.0f,0.0f,1.0f };
	}
	else
	{
		testSphere2->Parameters->Color = DirectX::SimpleMath::Vector4{ 1.0f,1.0f,1.0f,1.0f };
	}
#endif
	//-----------------------------------------------------------------------------------------------------------------------------------------------------
}

void Player::Render(Shader* shader) {
	if (Parameters->Exist) { 
		(shader) ? Model->Render(shader) : Model->Render();	// �V�F�[�_�[���O������ݒ肳��Ă���Ύg�p����
	}
	ShotsManager->Render();
#ifdef _DEBUG
	//Capcule->Render        (FRAMEWORK->RS_WIRE_NONE);
	//testSphere2->Render    (FRAMEWORK->RS_WIRE_NONE);
	//playerHitSphere->Render(FRAMEWORK->RS_WIRE_NONE);
#endif
}

void Player::ImguiPlayer()
{
#ifdef USE_IMGUI
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.0f, 0.6f, 0.1f, 1.0f));	// �����ĂԂ�ImGui::PopStyleColor�������Ȃ��Ƃ����Ȃ��炵��
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.0f, 0.2f, 0.0f, 1.0f));

	// ���C�g�������O���[�o���ݒ�
	ImGui::Begin("Player");
	ImGui::SliderFloat3("SpherePos", imguiPos, -10.0f, 10.0f);
	//ImGui::SliderFloat("playerRot", &imguiRot, 0.0f, 361.0f);
	//Parameters->Orientation = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(Model->getWorld().Up(), DirectX::XMConvertToRadians(imguiRot));
	ImGui::Checkbox("PlayerExist", &Parameters->Exist);
	ImGui::PopStyleColor(2);

	ImGui::End();
#endif
}

void Player::Control()
{
	static float MOVE_SPEED = 0.1f;
	static float ROTATE_SPEED = DirectX::XMConvertToRadians(5);
	Parameters->Acceleration = DirectX::SimpleMath::Vector3{ 0.0f, 0.0f, 0.0f };
	Parameters->Velocity = DirectX::SimpleMath::Vector3{ 0.0f, 0.0f, 0.0f };	// ���͒��������������߂ɖ��t���[�������� �����Ƃ����n�߂�Ƃ���Ȃ�

	//--------------------------------------------------------
	//�O�㏈��
	{
		if (GetKeyState('W') < 0) {
			// �ǂ����SimpleMath�͉E����W�n�炵����(�E��)�����(����)�O���ƂȂ�݂���
			// ����͍��E�����l�Ȃ̂ŉ��L�̂悤�Ɉ����ɂ͎����t�������g�����ƂɂȂ�
			Parameters->Velocity += Model->getWorld().Backward() * MOVE_SPEED;	// �O���Ɉړ�
		}
		if (GetKeyState('S') < 0) {
			Parameters->Velocity -= Model->getWorld().Backward() * MOVE_SPEED;	// �O���Ɉړ�
		}
	}
	//��]����
	{
		if (GetKeyState('D') < 0) {
			Parameters->Orientation *= DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(Model->getWorld().Up(), ROTATE_SPEED);

		}

		if (GetKeyState('A') < 0) {
			Parameters->Orientation *= DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(Model->getWorld().Down(), ROTATE_SPEED);
		}
		//// debug�p �㉺�~
		//if (GetKeyState('Q') < 0) {
		//	Parameters->Position.y-= 0.01f;
		//}
		//if (GetKeyState('E') < 0) {
		//	Parameters->Position.y += 0.01f;
		//}
	}
	Parameters->Position += Parameters->Velocity;

	if (GetAsyncKeyState(VK_RETURN) & 1) {
		// Shot�̐���
		ShotsManager->newSet(Parameters.get());
		StageManager::getInstance().Check(*Parameters, Parameters->Scale.x * 0.5f);
	}
	//--------------------------------------------------------

	//Velocity+= acceleration;
	//Position += Velocity;
}

int Player::Damage()
{
	return --Parameters->CurLife;	// �_���[�W���󂯂�̂ő̗͂�-1

}
