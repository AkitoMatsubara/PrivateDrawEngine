#include "Player.h"
#include "Judge.h"
#include "Stages.h"
#include "XMFLOAT_Helper.h"
#include  "framework.h"

static float imguiPos[3] = { 0.0f };

void Player::Initialize() {
	Model = std::make_unique<Skinned_Mesh>(".\\Resources\\Player\\Player.fbx");

	// �p�����[�^�[�̏�����
	Parameters = std::make_unique<Object3d>();
	Parameters->Position     = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };
	Parameters->Vector       = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };
	Parameters->Acceleration = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };
	Parameters->Orientation = DirectX::SimpleMath::Quaternion{ 0.0f,0.0f,0.0f,1.0f };
	Parameters->Scale        = DirectX::SimpleMath::Vector3{ 1.0f,1.0f,1.0f };
	Parameters->Color        = DirectX::SimpleMath::Vector4{ 1.0f,1.0f,1.0f,1.0f };

	ShotsManager = std::make_unique<ShotManager>();
	ShotsManager->Initialize(ShotManager::MASTER::PLAYER);

	Capcule = std::make_unique<Geometric_Capsule>(1.0f, 10, 10);
	testSphere = std::make_unique<Geometric_Sphere>();
	instance = this;	// 1P�z��̂� 2P�s��
}

void Player::Update() {
	Control();	// ����n

	// �e�̍X�V
	ShotsManager->Update();

	// ���f���ɕ`��n�p�����[�^�[��n��
	Model->getParameters()->CopyParam(Parameters.get());

	StageManager::getInstance().RideParts(*Parameters);


	//-----------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef _DEBUG
	Capcule->Parameters->CopyParam(Parameters.get());	// Player�ɕt������悤�Ɉʒu�𓯊�
	DirectX::SimpleMath::Vector3 pPos = Parameters->Position;
	Object3d* cPos = Capcule->Parameters.get();
	static const float CAPCULESIZE = 0.7f;
	Capcule->Parameters->Scale = DirectX::SimpleMath::Vector3(CAPCULESIZE , CAPCULESIZE, CAPCULESIZE);
	Capcule->Parameters->Color = DirectX::SimpleMath::Vector4{ 1.0f,1.0f,1.0f,1.0f, };
	//Capcule->Parameters->Rotate.x += 90;
	Capcule->Parameters->Orientation=DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(Model->getWorld().Right(), DirectX::XMConvertToRadians(90));
	static const float SPHERESIZE = 0.5f;
	testSphere->Parameters->CopyParam(Parameters.get());
	testSphere->Parameters->Scale = DirectX::SimpleMath::Vector3{ SPHERESIZE,SPHERESIZE,SPHERESIZE };
	//testSphere->Parameters->Position += Capcule->Parameters->Vector * (Capcule->Parameters->Scale *imguiPos[0]);	// �O���m�F�p
	testSphere->Parameters->Position += DirectX::SimpleMath::Vector3{ imguiPos[0] ,imguiPos[1] ,imguiPos[2] };

	// ����������A�������ĂȂ������甒�ɂ���
	//if (ShotsManager->isHit(Parameters.get()))
	//if (Judge::getInstance()->c_b(*Capcule->Parameters, 0.5f, *testSphere->Parameters))
	if (Judge::getInstance()->JudgeCap_Sphe(*Capcule, *testSphere))
	{
		testSphere->Parameters->Color = DirectX::SimpleMath::Vector4{ 0.0f,1.0f,1.0f,1.0f };
	}
	else
	{
		testSphere->Parameters->Color = DirectX::SimpleMath::Vector4{ 1.0f,1.0f,1.0f,1.0f };
	}
#endif
	//-----------------------------------------------------------------------------------------------------------------------------------------------------
}

void Player::Render() {
	ShotsManager->Render();
	Model->Render();
#ifdef _DEBUG
	Capcule->Render(true);
	testSphere->Render(true);
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
	ImGui::PopStyleColor(2);

	ImGui::End();
#endif
}

void Player::Control()
{
	static float MOVE_SPEED = 0.05f;
	static float ROTATE = DirectX::XMConvertToRadians(3);
	Parameters->Acceleration = DirectX::SimpleMath::Vector3{ 0.0f, 0.0f, 0.0f };
	Parameters->Velocity = DirectX::SimpleMath::Vector3{ 0.0f, 0.0f, 0.0f };	// ���͒��������������߂ɖ��t���[�������� �����Ƃ����n�߂�Ƃ���Ȃ�

	// ��]�ǉ��A�W���o�����b�N�����H�v�C�� �Ǝv��������y����]�����g���C���邩��C���s�K�v�ł́H����
	Parameters->calcForward();

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
			Parameters->Orientation *= DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(Model->getWorld().Up(), ROTATE);

		}

		if (GetKeyState('A') < 0) {
			Parameters->Orientation *= DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(Model->getWorld().Down(), ROTATE);
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

	if (GetAsyncKeyState(VK_LBUTTON) & 1) {
		// Shot�̐���
		ShotsManager->newSet(Parameters.get());
		StageManager::getInstance().Check(*Parameters);
	}
	//--------------------------------------------------------

	//Velocity+= acceleration;
	//Position += Velocity;
}