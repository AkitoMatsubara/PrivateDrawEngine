#include "Player.h"
#include "Judge.h"
#include "Stages.h"
#include "XMFLOAT_Helper.h"
#include  "framework.h"

static float imguiPos[3] = { 0.0f };
static float imguiRot= { 0.0f };

void Player::Initialize() {
	Model = std::make_unique<Skinned_Mesh>(".\\Resources\\Player\\Player.fbx");	// 丸モデル
	//Model = std::make_unique<Skinned_Mesh>(".\\Resources\\Player\\Player_.fbx");	// 3角形モデル

	// パラメーターの初期化
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
	Control();	// 操作系

	// 弾の更新
	ShotsManager->Update();

	// モデルに描画系パラメーターを渡す
	Model->getParameters()->CopyParam(Parameters.get());
	// ステージの上に乗っているか判定
	if (StageManager::getInstance().RideParts(*Parameters, Parameters->Scale.x * 0.5f)) {
		Parameters->Position.y = 0.0f;	// TODO debug:見た目上ステージの上にいる
	}
	else {
		Parameters->Position.y -= 0.05f;
	}


	//-----------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef _DEBUG
	Capcule->Parameters->CopyParam(Parameters.get());	// Playerに付随するように位置を同期
	static const float CAPCULESIZE = 0.7f;
	Capcule->Parameters->Scale = DirectX::SimpleMath::Vector3(CAPCULESIZE , CAPCULESIZE, CAPCULESIZE);
	Capcule->Parameters->Color = DirectX::SimpleMath::Vector4{ 1.0f,1.0f,1.0f,1.0f, };

	Capcule->Parameters->Orientation = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(Model->getWorld().Right(), DirectX::XMConvertToRadians(90));

	static const float SPHERESIZE = 0.5f;
	testSphere2->Parameters->CopyParam(Parameters.get());
	testSphere2->Parameters->Scale = DirectX::SimpleMath::Vector3{ SPHERESIZE,SPHERESIZE,SPHERESIZE };
	//testSphere2->Parameters->Position += Capcule->Parameters->Vector * (Capcule->Parameters->Scale *imguiPos[0]);	// 前方確認用
	testSphere2->Parameters->Position += DirectX::SimpleMath::Vector3{ imguiPos[0] ,imguiPos[1] ,imguiPos[2] };

	playerHitSphere->Parameters->CopyParam(Parameters.get());
	playerHitSphere->Parameters->Color = DirectX::SimpleMath::Vector4{ 1.0f,1.0f,1.0f,1.0f };

	// 当たったら青、当たってなかったら白にする
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
		(shader) ? Model->Render(shader) : Model->Render();	// シェーダーが外部から設定されていれば使用する
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
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.0f, 0.6f, 0.1f, 1.0f));	// これ一つ呼ぶとImGui::PopStyleColorを書かないといけないらしい
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.0f, 0.2f, 0.0f, 1.0f));

	// ライト調整等グローバル設定
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
	Parameters->Velocity = DirectX::SimpleMath::Vector3{ 0.0f, 0.0f, 0.0f };	// 入力中だけ動かすために毎フレーム初期化 慣性とかつけ始めるといらない

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
			Parameters->Orientation *= DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(Model->getWorld().Up(), ROTATE_SPEED);

		}

		if (GetKeyState('A') < 0) {
			Parameters->Orientation *= DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(Model->getWorld().Down(), ROTATE_SPEED);
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

	if (GetAsyncKeyState(VK_RETURN) & 1) {
		// Shotの生成
		ShotsManager->newSet(Parameters.get());
		StageManager::getInstance().Check(*Parameters, Parameters->Scale.x * 0.5f);
	}
	//--------------------------------------------------------

	//Velocity+= acceleration;
	//Position += Velocity;
}

int Player::Damage()
{
	return --Parameters->CurLife;	// ダメージを受けるので体力を-1

}
