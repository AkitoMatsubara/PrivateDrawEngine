#include "Player.h"
#include "Judge.h"
#include "XMFLOAT_Helper.h"

static float imguiPos[3] = { 0.0f };

void Player::Initialize() {
	Model = std::make_unique<Skinned_Mesh>(".\\Resources\\Player\\Player.fbx");	// 3�p�`������Ă��Ȃ��������b�V�� �L���[�u
	SkinnedShader = std::make_unique<ShaderEx>();
	SkinnedShader->Create(L"Shaders\\skinned_mesh_vs", L"Shaders\\skinned_mesh_ps");

	// �p�����[�^�[�̏�����
	Parameters = std::make_unique<Object3d>();
	Parameters->Position     = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };
	Parameters->Vector       = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };
	Parameters->Acceleration = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };
	Parameters->Rotate       = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };
	Parameters->Scale        = DirectX::SimpleMath::Vector3{ 1.0f,1.0f,1.0f };
	Parameters->Color        = DirectX::SimpleMath::Vector4{ 1.0f,1.0f,1.0f,1.0f };

	//Shots = std::make_unique<Shot>();
	//Shots->Initialize();
	ShotsManager = std::make_unique<ShotManager>();
	ShotsManager->Initialize();

	Capcule = std::make_unique<Geometric_Capsule>(1.0f, 1.0f, 10, 10);
	testSphere = std::make_unique<Geometric_Sphere>();
}

void Player::Update() {
	Control();	// ����n

	// �e�̍X�V
	ShotsManager->Update();
	//Shots->Update();

	// ���f���ɕ`��n�p�����[�^�[��n��
	Model->getParameters()->CopyParam(Parameters.get());

	//-----------------------------------------------------------------------------------------------------------------------------------------------------
	Capcule->Parameters->CopyParam(Parameters.get());	// Player�ɕt������悤�Ɉʒu�𓯊�
	DirectX::SimpleMath::Vector3 pPos = Parameters->Position;
	Object3d* cPos = Capcule->Parameters.get();
	static const float capculeSize = 1.0f;
	Capcule->Parameters->Scale = DirectX::SimpleMath::Vector3(capculeSize, capculeSize, capculeSize);
	Capcule->Parameters->Color = DirectX::SimpleMath::Vector4{ 1.0f,1.0f,1.0f,1.0f, };
	Capcule->Parameters->Rotate.x += 90;
	testSphere->Parameters->CopyParam(Parameters.get());
	testSphere->Parameters->Scale = DirectX::SimpleMath::Vector3{ 0.5f,0.5f,0.5f };
	testSphere->Parameters->Position = DirectX::SimpleMath::Vector3{ imguiPos[0] ,imguiPos[1] ,imguiPos[2] };

	// ����������A�������ĂȂ������甒�ɂ���
	if (ShotsManager->isHit(Parameters.get()))
	//if (Judge::getInstance()->c_b(*Parameters, *testSphere->Parameters))
	{
		testSphere->Parameters->Color = DirectX::SimpleMath::Vector4{ 0.0f,1.0f,1.0f,1.0f };
	}
	else
	{
		testSphere->Parameters->Color = DirectX::SimpleMath::Vector4{ 1.0f,1.0f,1.0f,1.0f };
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------------------
}

void Player::Render() {
	ShotsManager->Render();

	Model->Render(SkinnedShader.get());
	Capcule->Render(true);
	testSphere->Render(true);
}

void Player::ImguiPlayer()
{
#ifdef USE_IMGUI
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.0f, 0.6f, 0.1f, 1.0f));	// �����ĂԂ�ImGui::PopStyleColor�������Ȃ��Ƃ����Ȃ��炵��
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.0f, 0.2f, 0.0f, 1.0f));

	// ���C�g�������O���[�o���ݒ�
	ImGui::Begin("Player");
	ImGui::SliderFloat3("SpherePos", imguiPos, -10.0f, 10.0f);
	ImGui::Text("vector: %d", ShotsManager.get()->getSize());

	ImGui::PopStyleColor();	// ImGui::PushStyleColor��ɂ���Ăяo�����ۂ�
	ImGui::PopStyleColor();

	ImGui::End();
#endif
}

void Player::Control()
{
	static float speed = 0.01f;
	Parameters->Acceleration = DirectX::SimpleMath::Vector3{ 0.0f, 0.0f, 0.0f };
	Parameters->Velocity = DirectX::SimpleMath::Vector3{ .0f, 0.0f, 0.0f };	// ���͒��������������߂ɖ��t���[�������� �����Ƃ����n�߂�Ƃ���Ȃ�

	// x����]�ǉ��A�W���o�����b�N�����H�v�C�� �Ǝv��������y����]�����g���C���邩��C���s�K�v�ł́H����
	Parameters->Vector.x = sinf(DirectX::XMConvertToRadians(Parameters->Rotate.y));
	Parameters->Vector.y = sinf(-DirectX::XMConvertToRadians(Parameters->Rotate.x));
	Parameters->Vector.z = cosf(DirectX::XMConvertToRadians(Parameters->Rotate.y));
	Parameters->Vector = Float3Normalize(Parameters->Vector);	// �����x�N�g���Ȃ̂Ő��K�����Ƃ�

	// �p�x���� �p�x���I�[�o�[�t���[���Ȃ����߂�
	Parameters->Rotate.y = fmodf(Parameters->Rotate.y, 360);	// fmodf(x,y) float�^��x/y�̗]���Ԃ�
	//--------------------------------------------------------
	//�O�i����
	if (GetKeyState('W') < 0) {
		Parameters->Position += Parameters->Vector * speed;
	}
	//��ޏ���
	if (GetKeyState('S') < 0) {
		Parameters->Position -= Parameters->Vector * speed;
	}
	//��]����
	{
		if (GetKeyState('D') < 0) {
			Parameters->Rotate.y += DirectX::XMConvertToRadians(40);
		}
		if (GetKeyState('A') < 0) {
			Parameters->Rotate.y -= DirectX::XMConvertToRadians(40);
		}
		if (GetKeyState('Q') < 0) {
			Parameters->Rotate.x += DirectX::XMConvertToRadians(40);
		}
		if (GetKeyState('E') < 0) {
			Parameters->Rotate.x -= DirectX::XMConvertToRadians(40);
		}
	}

	static bool nowTrg = false;	// ���݂̃L�[��
	static bool oldTrg = false;	// �t���[���O�̃L�[���
	if (GetKeyState('Z') < 0) {
		nowTrg = true;	// ���������ˁH
		if (oldTrg != nowTrg){	// �O�t���[���Ɣ�r�A��������Ȃ���Ώ���(�v�͉������ς͔������Ȃ��悤��)
			// Shot�̐���
			ShotsManager->newSet(Parameters.get());
		}
		oldTrg = nowTrg;	// ���̃L�[����ۑ�
	}
	else{
		// �����Ă��Ȃ��̂ł��������t���O�͗����܂���
		nowTrg = false;
		oldTrg = false;
	}

	//--------------------------------------------------------

	//Velocity+= acceleration;
	//Position += Velocity;
}