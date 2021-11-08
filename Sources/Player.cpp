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
	Parameters->Position = { 0.0f,0.0f,0.0f };
	Parameters->Vector = { 0.0f,0.0f,0.0f };
	Parameters->Acceleration = { 0.0f,0.0f,0.0f };
	Parameters->Rotate = { 0.0f,0.0f,0.0f };
	Parameters->Scale = { 1.0f,1.0f,1.0f };
	Parameters->Color = { 1.0f,1.0f,1.0f,1.0f };

	Shots = std::make_unique<Shot>();
	Shots->Initialize();

	Capcule = std::make_unique<Geometric_Capsule>(1.0f, 1.0f, 10, 10);
	testSphere = std::make_unique<Geometric_Sphere>();
}

void Player::Update() {
	Control();

	for (auto& s : ShotsManager)
	{
		if (s->getExist())
		{
			s->Update();
		}
	}
	//Shots->Update();

	// ���f���ɕ`��n�p�����[�^�[��n��
	Model->getParameters()->CopyParam(Parameters.get());
	//Model->setPos(Parameters->Position);
	//Model->setAngle(Parameters->Rotate);
	//Model->setSize(Parameters->Scale);
	//Model->setColor(Parameters->Color);

	//-----------------------------------------------------------------------------------------------------------------------------------------------------
	Capcule->Parameters->CopyParam(Parameters.get());	// Player�ɕt������悤�Ɉʒu�𓯊�
	DirectX::XMFLOAT3 pPos = Parameters->Position;
	Object3d* cPos = Capcule->Parameters.get();
	//float zure = 0.0f;
	//Capcule->Parameters->Position = { pPos.x - cPos->Vector.x , pPos.y,pPos.z - cPos->Vector.z  };
	static const float capculeSize = 1.0f;
	Capcule->Parameters->Scale = DirectX::XMFLOAT3(capculeSize, capculeSize, capculeSize);
	Capcule->Parameters->Color = { 1.0f,1.0f,1.0f,1.0f, };
	Capcule->Parameters->Rotate.x += 90;
	testSphere->Parameters->CopyParam(Parameters.get());
	testSphere->Parameters->Scale = { 0.5f,0.5f,0.5f};
	testSphere->Parameters->Position = { imguiPos[0] ,imguiPos[1] ,imguiPos[2] };
	// ����������A�������ĂȂ������甒�ɂ���
	if (Judge::getInstance()->c_b(*Capcule->Parameters, 1.0f, *testSphere->Parameters))
	{
		testSphere->Parameters->Color = { 0.0f,1.0f,1.0f,1.0f };
	}
	else
	{
		testSphere->Parameters->Color = { 1.0f,1.0f,1.0f,1.0f };
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------------------
}

void Player::Render() {
	for (auto& s : ShotsManager)
	{
		s->Render();
	}
	//Shots->Render();

	Model->Render(SkinnedShader.get());
	Capcule->Render(true);
	testSphere->Render(true);
}

void Player::ImguiPlayer()
{
#ifdef USE_IMGUI
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.0f, 0.6f, 0.1f, 1.0f));	// �����ĂԂ�ImGui::PopStyleColor�������Ȃ��Ƃ����Ȃ��炵��
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.0f, 0.2f, 0.0f, 1.0f));
	//ImGui::SetNextWindowPos(ImVec2(20, 300), ImGuiCond_Once);
	//ImGui::SetNextWindowSize(ImVec2(300, 330), ImGuiCond_Always);

	// ���C�g�������O���[�o���ݒ�
	ImGui::Begin("Player");
	ImGui::SliderFloat3("SpherePos", imguiPos, -10.0f, 10.0f);
	ImGui::Text("Rotate.y: %f", Parameters->Rotate.y);

	ImGui::PopStyleColor();	// ImGui::PushStyleColor��ɂ���Ăяo�����ۂ�
	ImGui::PopStyleColor();

	ImGui::End();
#endif
}

void Player::Control()
{
	static float speed = 0.01f;
	Parameters->Acceleration = { 0.0f, 0.0f, 0.0f };
	Parameters->Velocity = { .0f, 0.0f, 0.0f };	// ���͒��������������߂ɖ��t���[�������� �����Ƃ����n�߂�Ƃ���Ȃ�

	// x����]�ǉ��A�W���o�����b�N�����H�v�C�� �Ǝv��������y����]�����g���C���邩��C���s�K�v�ł́H����
	Parameters->Vector.x = sinf(DirectX::XMConvertToRadians(Parameters->Rotate.y));
	Parameters->Vector.y = sinf(-DirectX::XMConvertToRadians(Parameters->Rotate.x));
	Parameters->Vector.z = cosf(DirectX::XMConvertToRadians(Parameters->Rotate.y));
	Parameters->Vector = Float3Normalize(Parameters->Vector);	// ���K�����Ƃ�

	// �p�x���� �p�x���I�[�o�[�t���[���Ȃ����߂� ���ݕs�A�����Ǝ������悤��
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
		//nowTrg = true;	// ���������ˁH
		//if (oldTrg!=nowTrg)	// �O�t���[���Ɣ�r�A��������Ȃ���Ώ���(�v�͉������ς͔������Ȃ��悤��)
		//{
			// Shot�̐���
			Shots = std::make_unique<Shot>();
			Shots->Initialize();
			Shots->set(Parameters.get());
			ShotsManager.emplace_back(move(Shots));
		//}
		//oldTrg = nowTrg;	// ���̃L�[����ۑ�
	}
	else
	{
		// �����Ă��Ȃ��̂ł��������t���O�͗����܂���
		nowTrg = false;
		oldTrg = false;
	}



	//--------------------------------------------------------

	//Velocity+= acceleration;
	//Position += Velocity;
}