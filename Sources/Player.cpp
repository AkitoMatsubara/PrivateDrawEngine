#include "Player.h"
#include "Judge.h"
#include "XMFLOAT_Helper.h"

static float imguiPos[3] = { 0.0f };

void Player::Initialize() {
	Model = std::make_unique<Skinned_Mesh>(".\\Resources\\Player\\Player.fbx");	// 3角形化されていない複数メッシュ キューブ
	SkinnedShader = std::make_unique<ShaderEx>();
	SkinnedShader->Create(L"Shaders\\skinned_mesh_vs", L"Shaders\\skinned_mesh_ps");

	// パラメーターの初期化
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
	Control();	// 操作系

	// 弾の更新
	ShotsManager->Update();
	//Shots->Update();

	// モデルに描画系パラメーターを渡す
	Model->getParameters()->CopyParam(Parameters.get());

	//-----------------------------------------------------------------------------------------------------------------------------------------------------
	Capcule->Parameters->CopyParam(Parameters.get());	// Playerに付随するように位置を同期
	DirectX::SimpleMath::Vector3 pPos = Parameters->Position;
	Object3d* cPos = Capcule->Parameters.get();
	static const float capculeSize = 1.0f;
	Capcule->Parameters->Scale = DirectX::SimpleMath::Vector3(capculeSize, capculeSize, capculeSize);
	Capcule->Parameters->Color = DirectX::SimpleMath::Vector4{ 1.0f,1.0f,1.0f,1.0f, };
	Capcule->Parameters->Rotate.x += 90;
	testSphere->Parameters->CopyParam(Parameters.get());
	testSphere->Parameters->Scale = DirectX::SimpleMath::Vector3{ 0.5f,0.5f,0.5f };
	testSphere->Parameters->Position = DirectX::SimpleMath::Vector3{ imguiPos[0] ,imguiPos[1] ,imguiPos[2] };

	// 当たったら青、当たってなかったら白にする
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
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.0f, 0.6f, 0.1f, 1.0f));	// これ一つ呼ぶとImGui::PopStyleColorを書かないといけないらしい
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.0f, 0.2f, 0.0f, 1.0f));

	// ライト調整等グローバル設定
	ImGui::Begin("Player");
	ImGui::SliderFloat3("SpherePos", imguiPos, -10.0f, 10.0f);
	ImGui::Text("vector: %d", ShotsManager.get()->getSize());

	ImGui::PopStyleColor();	// ImGui::PushStyleColor一つにつき一つ呼び出すっぽい
	ImGui::PopStyleColor();

	ImGui::End();
#endif
}

void Player::Control()
{
	static float speed = 0.01f;
	Parameters->Acceleration = DirectX::SimpleMath::Vector3{ 0.0f, 0.0f, 0.0f };
	Parameters->Velocity = DirectX::SimpleMath::Vector3{ .0f, 0.0f, 0.0f };	// 入力中だけ動かすために毎フレーム初期化 完成とかつけ始めるといらない

	// x軸回転追加、ジンバルロック発生？要修正 と思ったけどy軸回転しか使わん気するから修正不必要では？これ
	Parameters->Vector.x = sinf(DirectX::XMConvertToRadians(Parameters->Rotate.y));
	Parameters->Vector.y = sinf(-DirectX::XMConvertToRadians(Parameters->Rotate.x));
	Parameters->Vector.z = cosf(DirectX::XMConvertToRadians(Parameters->Rotate.y));
	Parameters->Vector = Float3Normalize(Parameters->Vector);	// 方向ベクトルなので正規化しとく

	// 角度制限 角度がオーバーフローしないために
	Parameters->Rotate.y = fmodf(Parameters->Rotate.y, 360);	// fmodf(x,y) float型でx/yの余りを返す
	//--------------------------------------------------------
	//前進処理
	if (GetKeyState('W') < 0) {
		Parameters->Position += Parameters->Vector * speed;
	}
	//後退処理
	if (GetKeyState('S') < 0) {
		Parameters->Position -= Parameters->Vector * speed;
	}
	//回転処理
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

	static bool nowTrg = false;	// 現在のキー状況
	static bool oldTrg = false;	// フレーム前のキー情報
	if (GetKeyState('Z') < 0) {
		nowTrg = true;	// 今押したね？
		if (oldTrg != nowTrg){	// 前フレームと比較、同じじゃなければ処理(要は押しっぱは反応しないように)
			// Shotの生成
			ShotsManager->newSet(Parameters.get());
		}
		oldTrg = nowTrg;	// 今のキー情報を保存
	}
	else{
		// 押していないのでそもそもフラグは立ちません
		nowTrg = false;
		oldTrg = false;
	}

	//--------------------------------------------------------

	//Velocity+= acceleration;
	//Position += Velocity;
}