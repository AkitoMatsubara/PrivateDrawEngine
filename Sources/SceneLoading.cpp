#include <thread>
#include "SceneLoading.h"
#include "SceneGame.h"

bool SceneLoading::Initialize() {
	Microsoft::WRL::ComPtr<ID3D11Device> device = FRAMEWORK->GetDevice();	// frameworkからdeviceを取得
	HRESULT hr = { S_OK };

	// シーンコンスタントバッファの設定
	{
		D3D11_BUFFER_DESC buffer_desc{};
		buffer_desc.ByteWidth = sizeof(SceneConstants);
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		hr = device->CreateBuffer(&buffer_desc, nullptr, ConstantBuffers[0].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	// Samplerの設定
	DefaultSampleClamp = std::make_shared<Sampler>(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);

	// 各種クラス設定
	{

		// ロード画像の初期化
		LoadingImage = std::make_unique<Sprite>();
		LoadingImage->LoadImages(L".\\Resources\\black-metal-texture.jpg");
	}

	 // スレッド開始
	std::thread thread(LoadingThread, this);	// LoadingThread関数を実行、thisを引数に設定
	// スレッドの管理を放棄
	thread.detach();

	GpuParticle = std::make_unique<GPUParticle>();
	GpuParticle->Init();

	return true;
}

void SceneLoading::Update() {
	imguiUpdate();

	{
		// 次のシーンへ
		if (NextScene->isReady())
		{
			SceneManager::getInstance().ChangeScene(NextScene.release());
			//SceneManager::getInstance().ChangeScene(std::move(nextScene));
			return;
		}

	}

}

void SceneLoading::Render() {
	HRESULT hr{ S_OK };

	ID3D11DeviceContext* immediate_context = FRAMEWORK->GetDeviceContext();	// DevCon取得

	FRAMEWORK->Clear(ClearColor);	// 一旦クリア

	// ビューポートの設定
	FRAMEWORK->CreateViewPort();

	// サンプラーステートをバインド
	DefaultSampleClamp->Set(0);

	immediate_context->OMSetBlendState(FRAMEWORK->GetBlendState(FRAMEWORK->BS_ALPHA), nullptr, 0xFFFFFFFF);	// ブレンドインターフェースのポインタ、ブレンドファクターの配列値、サンプルカバレッジ(今回はデフォルト指定)

	// 2Dオブジェクトの描画設定
	{
		immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(FRAMEWORK->DS_TRUE), 1);	// 3Dオブジェクトの後ろに出すため一旦
		LoadingImage->Render();
	}
	// 3Dオブジェクトの描画設定
	{
		GpuParticle->Play();
		//D3D11_VIEWPORT viewport;
		//UINT num_viewports{ 1 };
		//immediate_context->RSGetViewports(&num_viewports, &viewport);	// ラスタライザステージにバインドされたviewportの配列を取得

		//float aspect_ratio{ viewport.Width / viewport.Height };	// アスペクト比
		//// 透視投影行列の作成
		//DirectX::XMMATRIX P{DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(30),aspect_ratio,0.1f,100.0f) };	// 視野角,縦横比,近くのZ,遠くのZ

		//DirectX::XMVECTOR eye{DirectX::XMVectorSet(EyePos.x,EyePos.y,EyePos.z,1.0f) };
		//DirectX::XMVECTOR focus;
		////if (!focus_zero) {
		////	//focus = { XMVectorSet(EyePos.x,EyePos.y,EyePos.z + 1,1.0f) };	// カメラ位置の前
		////	focus = { XMVectorSet(skinned_mesh->getPos().x,skinned_mesh->getPos().y,skinned_mesh->getPos().z,1.0f) };	// カメラ位置の前
		////}
		////else {
		//	focus = {DirectX::XMVectorSet(0.0f,0.0f,0.0f,1.0f) };
		////}
		//DirectX::XMVECTOR up{DirectX::XMVectorSet(0.0f,1.0f,0.0f,0.0f) };
		//// ViewMatrixの作成(LH = LeftHand(左手座標系))
		//DirectX::XMMATRIX V{DirectX::XMMatrixLookAtLH(eye, focus, up) };	// カメラ座標、焦点、カメラの上方向

		//// コンスタントバッファ更新
		//scene_constants data{};
		//XMStoreFloat4x4(&data.view_projection, V * P);	// Matrixから4x4へ変換
		//data.light_direction = DirectX::SimpleMath::Vector4{ light_dir[0],light_dir[1],light_dir[2],0 };	// シェーダに渡すライトの向き
		//data.camera_position = DirectX::SimpleMath::Vector4{ EyePos.x,EyePos.y,EyePos.z,0 };				// シェーダに渡すカメラの位置
		//immediate_context->UpdateSubresource(ConstantBuffers[0].Get(), 0, 0, &data, 0, 0);
		//immediate_context->VSSetConstantBuffers(1, 1, ConstantBuffers[0].GetAddressOf());	// cBufferはドローコールのたびに消去されるので都度設定する必要がある
		//immediate_context->PSSetConstantBuffers(1, 1, ConstantBuffers[0].GetAddressOf());

		//immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(DS_TRUE_WRITE), 1);			// 2Dオブジェクトとの前後関係をしっかりするため再設定
	}

#ifdef USE_IMGUI
	//	ImGui::Render();
	//	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#else
	FRAMEWORK->Flip();
#endif
}

void SceneLoading::LoadingThread(SceneLoading* scene) {
	HRESULT hr;
	//	COM関連の初期化でスレッド毎に呼ぶ必要がある
	hr = CoInitialize(nullptr);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// 次のシーンの初期化
	scene->NextScene->Initialize();

	// スレッドが終わる前にCOM関連の終了化
	CoUninitialize();

	// 遷移準備設定
	scene->NextScene->setReady(true);
}

void SceneLoading::imguiUpdate() {
#ifdef USE_IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	imguiSceneChanger();

	// ライト調整等グローバル設定
	ImGui::Begin("Light");
	ImGui::SliderFloat3("Light_Direction", light_dir, -10.0f, 10.0f);
	ImGui::Checkbox("focus Zero", &focus_zero);
	ImGui::End();
#endif
}