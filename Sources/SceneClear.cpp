#include "SceneClear.h"
#include "SceneTitle.h"

#include "Enemy.h"
#include "Stages.h"

#include "UseComputeShader.h"

#include <SimpleMath.h>
bool SceneClear::Initialize() {
	Microsoft::WRL::ComPtr<ID3D11Device> device = FRAMEWORK->GetDevice();	// frameworkからdeviceを取得
	HRESULT hr = { S_OK };

	// シーンコンスタントバッファの設定
	CreateConstantBuffer(ConstantBuffers[0].GetAddressOf(), sizeof(SceneConstants));

	// Samplerの設定
	DefaultSampleClamp = std::make_shared<Sampler>(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);

	// 各種クラス設定
	{
		camera = std::make_unique<Camera>();

		// spriteオブジェクトを生成(今回は先頭の１つだけを生成する)
		Sprites = std::make_unique<Sprite>();
		Sprites->LoadImages(L".\\Resources\\Clear\\Clear.png");
		Sprites->setSize(SCREEN_WIDTH, SCREEN_HEIGHT);


		player = std::make_unique<Player>();
		player->Initialize();

		EnemyManager::getInstance().Initialize();

		StageManager::getInstance().Initialize();
	}


	camera->SetProjection(DirectX::XMConvertToRadians(30), camera->GetWidth() / camera->GetHeight(), camera->GetNear(), camera->GetFar());
	return true;
}

void SceneClear::Update() {
	const float elapsed_time = FRAMEWORK->GetElapsedTime();
	// シーン切り替え
	if (GetAsyncKeyState('T') & 1)
	{
		setScene(std::make_unique<SceneTitle>());
	}

	player->Update();
	StageManager::getInstance().Update();

	// カメラ設定
	camera->Set(camera->GetPos(), player->Parameters->Position, DirectX::XMFLOAT3(0, 1, 0));
	// カメラ操作
	camera->Operate();

	// 敵関連
	{
		// お試し右クリックでランダム位置に敵を生成
		// TODO デバッグ用
		if (GetAsyncKeyState(VK_RBUTTON) < 0)
		{
			Object3d desc;
			desc.CopyParam(player->Parameters.get());
			desc.Position = DirectX::SimpleMath::Vector3((rand() % 25) - 12.0f, 0.0f, (rand() % 25) - 12.0f);
			desc.Orientation *= DirectX::SimpleMath::Quaternion::CreateFromAxisAngle({ 0.0f,1.0f,0.0f }, DirectX::XMConvertToRadians(static_cast<float>(rand() % 180)));
			EnemyManager::getInstance().newSet(&desc);
			//EnemyManager::getInstance().newSet(player->Parameters.get());
		}
		for (auto it = EnemyManager::getInstance().getEnemys()->begin(); it != EnemyManager::getInstance().getEnemys()->end(); ++it)
		{
			// すべての敵にターゲット設定
			it->get()->setTarget(*player->Parameters);
		}

		EnemyManager::getInstance().Update();	// 敵更新
	}

	imguiUpdate();
}

void SceneClear::Render() {
	HRESULT hr{ S_OK };

	static const Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context = FRAMEWORK->GetDeviceContext();

	FRAMEWORK->Clear(ClearColor,immediate_context.Get());	// 一旦クリア

	// ビューポートの設定
	FRAMEWORK->CreateViewPort(immediate_context.Get());

	// サンプラーステートをバインド
	DefaultSampleClamp->Set(immediate_context.Get(), 0);

	immediate_context->OMSetBlendState(FRAMEWORK->GetBlendState(FRAMEWORK->BS_ALPHA), nullptr, 0xFFFFFFFF);	// ブレンドインターフェースのポインタ、ブレンドファクターの配列値、サンプルカバレッジ(今回はデフォルト指定)

	// 2Dオブジェクトの描画設定
	{
		immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(FRAMEWORK->DS_TRUE), 1);		// 3Dオブジェクトの後ろに出すため一旦
		Sprites->Render(immediate_context.Get());
		immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(FRAMEWORK->DS_TRUE_WRITE), 1);	// 2Dオブジェクトとの前後関係をしっかりするため再設定
	}
	// 3Dオブジェクトの描画設定
	{
		camera->Activate();

		// コンスタントバッファ更新
		SceneConstants data{};
		XMStoreFloat4x4(&data.view_projection, camera->GetView() * camera->GetProjection());	// Matrixから4x4へ変換
		data.light_direction = DirectX::SimpleMath::Vector4{ light_dir[0],light_dir[1],light_dir[2],0 };	// シェーダに渡すライトの向き
		data.camera_position = DirectX::SimpleMath::Vector4{ camera->GetPos().x,camera->GetPos().y,camera->GetPos().z,0 };				// シェーダに渡すカメラの位置
		immediate_context->UpdateSubresource(ConstantBuffers[0].Get(), 0, 0, &data, 0, 0);
		immediate_context->VSSetConstantBuffers(1, 1, ConstantBuffers[0].GetAddressOf());	// cBufferはドローコールのたびに消去されるので都度設定する必要がある
		immediate_context->PSSetConstantBuffers(1, 1, ConstantBuffers[0].GetAddressOf());

		{
			StageManager::getInstance().Render(immediate_context.Get());
			player->Render(immediate_context.Get());
			EnemyManager::getInstance().Render(immediate_context.Get());
		}
	}

#ifdef USE_IMGUI
	//	ImGui::Render();
	//	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#else
	FRAMEWORK->Flip();
#endif
}

void SceneClear::imguiUpdate() {
#ifdef USE_IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.0f, 0.6f, 0.1f, 1.0f));	// これ一つ呼ぶとImGui::PopStyleColorを書かないといけないらしい
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.0f, 0.2f, 0.0f, 1.0f));

	//imguiSceneChanger();
	// 2D用 内部関数で完結させてる
	//sprites->ImguiWindow();
	// 3D用パラメータ
	player->ImguiPlayer();
	// ライト調整等グローバル設定
	ImGui::Begin("SceneImGui Object Counts");
	//ImGui::SliderFloat3("Light_Direction", light_dir, -10.0f, 10.0f);
	ImGui::Separator();
	ImGui::Text("PlayerShots: %d", player->getShotManager()->getSize());
	ImGui::Text("EnemyShots: %d", EnemyManager::getInstance().getShotManager()->getSize());
	ImGui::Text("Enemys: %d", EnemyManager::getInstance().getSize());
	ImGui::Text("StageParts: %d", StageManager::getInstance().getSize());
	ImGui::Separator();	// 分割線
	ImGui::Text("Total Objects: %d", StageManager::getInstance().getSize() + EnemyManager::getInstance().getEnemys()->size()
		+ player->getShotManager()->getSize() + EnemyManager::getInstance().getShotManager()->getSize());
	if (ImGui::Button("Scene Initialize")) { SceneClear::Initialize(); }

	ImGui::PopStyleColor(2);	// ImGui::PushStyleColor一つにつき引数一つ増えるっぽい
	ImGui::End();
#endif
}