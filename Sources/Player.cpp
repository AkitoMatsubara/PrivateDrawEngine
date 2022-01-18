#include "Player.h"
#include "Judge.h"
#include "Stages.h"
#include "XMFLOAT_Helper.h"
#include  "framework.h"

static float imguiPos[3] = { 0.0f };

void Player::Initialize() {
	Model = std::make_unique<Skinned_Mesh>(".\\Resources\\Player\\Player.fbx");

	// パラメーターの初期化
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
	instance = this;	// 1P想定のみ 2P不可
}

void Player::Update() {
	Control();	// 操作系

	// 弾の更新
	ShotsManager->Update();

	// モデルに描画系パラメーターを渡す
	Model->getParameters()->CopyParam(Parameters.get());

	StageManager::getInstance().RideParts(*Parameters);


	//-----------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef _DEBUG
	Capcule->Parameters->CopyParam(Parameters.get());	// Playerに付随するように位置を同期
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
	//testSphere->Parameters->Position += Capcule->Parameters->Vector * (Capcule->Parameters->Scale *imguiPos[0]);	// 前方確認用
	testSphere->Parameters->Position += DirectX::SimpleMath::Vector3{ imguiPos[0] ,imguiPos[1] ,imguiPos[2] };

	// 当たったら青、当たってなかったら白にする
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
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.0f, 0.6f, 0.1f, 1.0f));	// これ一つ呼ぶとImGui::PopStyleColorを書かないといけないらしい
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.0f, 0.2f, 0.0f, 1.0f));

	// ライト調整等グローバル設定
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
	Parameters->Velocity = DirectX::SimpleMath::Vector3{ 0.0f, 0.0f, 0.0f };	// 入力中だけ動かすために毎フレーム初期化 完成とかつけ始めるといらない

	// 回転追加、ジンバルロック発生？要修正 と思ったけどy軸回転しか使わん気するから修正不必要では？これ
	Parameters->calcForward();

	//--------------------------------------------------------
	//前後処理
	{
		if (GetKeyState('W') < 0) {
			// どうやらSimpleMathは右手座標系らしくて(右手)後方＝(左手)前方となるみたい
			// これは左右も同様なので下記のように扱うには実質逆方向を使うことになる
			Parameters->Velocity += Model->getWorld().Backward() * MOVE_SPEED;	// 前方に移動
		}
		if (GetKeyState('S') < 0) {
			Parameters->Velocity -= Model->getWorld().Backward() * MOVE_SPEED;	// 前方に移動
		}
	}
	//回転処理
	{
		if (GetKeyState('D') < 0) {
			Parameters->Orientation *= DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(Model->getWorld().Up(), ROTATE);

		}

		if (GetKeyState('A') < 0) {
			Parameters->Orientation *= DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(Model->getWorld().Down(), ROTATE);
		}
		//// debug用 上下降
		//if (GetKeyState('Q') < 0) {
		//	Parameters->Position.y-= 0.01f;
		//}
		//if (GetKeyState('E') < 0) {
		//	Parameters->Position.y += 0.01f;
		//}
	}
	Parameters->Position += Parameters->Velocity;

	if (GetAsyncKeyState(VK_LBUTTON) & 1) {
		// Shotの生成
		ShotsManager->newSet(Parameters.get());
		StageManager::getInstance().Check(*Parameters);
	}
	//--------------------------------------------------------

	//Velocity+= acceleration;
	//Position += Velocity;
}