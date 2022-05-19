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
		LoadingImage->LoadImages(L".\\Resources\\Loading\\Loading.png");
	}

	 // スレッド開始
	std::thread threadTemp(LoadingThread, this);	// LoadingThread関数を実行、thisを引数に設定
	Thread.swap(threadTemp);

	// スレッドの管理を放棄
	Thread.detach();

	GpuParticle = std::make_unique<GPUParticle>();
	GpuParticle->Init();

	//ディファード コンテキスト
	device->CreateDeferredContext(NULL, deferredContext.GetAddressOf());
	return true;
}

void SceneLoading::Update() {
	imguiUpdate();

	{
		// 次のシーンへ
		if (NextScene->isReady())
		{
			SceneManager::getInstance().setLoadComplete(true);	// ロード完了
			SceneManager::getInstance().ChangeScene(NextScene.release());
			return;
		}

	}

}

void SceneLoading::Render() {
	HRESULT hr{ S_OK };

	if (!NextScene->isReady())
	{
#if 1	// deferredContext メインスレッドとサブスレッドで同時にDeviceContextにアクセスするとメモリ破壊が起こるので別々に分ける
		static const Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context = FRAMEWORK->GetDeviceContext();	// コマンド走査用
		Microsoft::WRL::ComPtr<ID3D11CommandList> CommandList;
		CommandList.Reset();	// 中身の削除

		FRAMEWORK->Clear(ClearColor, deferredContext.Get());

		// ビューポートの設定
		FRAMEWORK->CreateViewPort(deferredContext.Get(), SCREEN_WIDTH, SCREEN_HEIGHT);

		// サンプラーステートをバインド
		DefaultSampleClamp->Set(deferredContext.Get(), 0);

		deferredContext->OMSetBlendState(FRAMEWORK->GetBlendState(FRAMEWORK->BS_ALPHA), nullptr, 0xFFFFFFFF);	// ブレンドインターフェースのポインタ、ブレンドファクターの配列値、サンプルカバレッジ(今回はデフォルト指定)


		// シーンコンスタントバッファ更新
		SceneConstants data{};
		XMStoreFloat4x4(&data.view_projection, Camera::getInstance().GetView() * Camera::getInstance().GetProjection());	// Matrixから4x4へ変換
		data.light_direction = DirectX::SimpleMath::Vector4{ light_dir[0],light_dir[1],light_dir[2],0 };	// シェーダに渡すライトの向き
		data.camera_position = DirectX::SimpleMath::Vector4{ Camera::getInstance().GetPos().x,Camera::getInstance().GetPos().y,Camera::getInstance().GetPos().z,0 };	// シェーダに渡すカメラの位置
		data.view = Camera::getInstance().GetView();
		data.projection = Camera::getInstance().GetProjection();
		deferredContext->UpdateSubresource(ConstantBuffers[0].Get(), 0, 0, &data, 0, 0);
		deferredContext->VSSetConstantBuffers(1, 1, ConstantBuffers[0].GetAddressOf());
		deferredContext->PSSetConstantBuffers(1, 1, ConstantBuffers[0].GetAddressOf());

		// 2Dオブジェクトの描画設定
		{
			deferredContext->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(FRAMEWORK->DS_FALSE), 1);	// 3Dオブジェクトの後ろに出すため一旦
			LoadingImage->Render(deferredContext.Get(), LoadingImage->getTexture(), LoadingImage->getShader(), CommandList.GetAddressOf());
			deferredContext->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(FRAMEWORK->DS_TRUE_WRITE), 1);			// 2Dオブジェクトとの前後関係をしっかりするため再設

		}
		// 3Dオブジェクトの描画設定
		{
			//GpuParticle->Play(deferredContext.Get());
			//immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(DS_TRUE_WRITE), 1);			// 2Dオブジェクトとの前後関係をしっかりするため再設定
		}

		//ディファード コンテキストで作成したコマンドリストを実行
		immediate_context->ExecuteCommandList(CommandList.Get(), FALSE);
#else
		static const Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context = FRAMEWORK->GetDeviceContext();
		Microsoft::WRL::ComPtr<ID3D11CommandList> CommandList;
		FRAMEWORK->Clear(ClearColor);	// 一旦クリア

		// ビューポートの設定
		FRAMEWORK->CreateViewPort(SCREEN_WIDTH, SCREEN_HEIGHT, immediate_context.Get());

		// サンプラーステートをバインド
		DefaultSampleClamp->Set(immediate_context.Get(), 0);

		immediate_context->OMSetBlendState(FRAMEWORK->GetBlendState(FRAMEWORK->BS_ALPHA), nullptr, 0xFFFFFFFF);	// ブレンドインターフェースのポインタ、ブレンドファクターの配列値、サンプルカバレッジ(今回はデフォルト指定)

		// シーンコンスタントバッファ更新
		SceneConstants data{};
		XMStoreFloat4x4(&data.view_projection, Camera::getInstance().GetView() * Camera::getInstance().GetProjection());	// Matrixから4x4へ変換
		data.light_direction = DirectX::SimpleMath::Vector4{ light_dir[0],light_dir[1],light_dir[2],0 };	// シェーダに渡すライトの向き
		data.camera_position = DirectX::SimpleMath::Vector4{ Camera::getInstance().GetPos().x,Camera::getInstance().GetPos().y,Camera::getInstance().GetPos().z,0 };	// シェーダに渡すカメラの位置
		data.view = Camera::getInstance().GetView();
		data.projection = Camera::getInstance().GetProjection();
		immediate_context->UpdateSubresource(ConstantBuffers[0].Get(), 0, 0, &data, 0, 0);
		immediate_context->VSSetConstantBuffers(0, 1, ConstantBuffers[0].GetAddressOf());	// cBufferはドローコールのたびに消去されるので都度設定する必要がある
		immediate_context->PSSetConstantBuffers(0, 1, ConstantBuffers[0].GetAddressOf());

		// 2Dオブジェクトの描画設定
		{
			immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(FRAMEWORK->DS_FALSE), 1);	// 3Dオブジェクトの後ろに出すため一旦
			//LoadingImage->Render(immediate_context.Get(), LoadingImage->getTexture(), LoadingImage->getShader(), CommandList.GetAddressOf());
			LoadingImage->Render(immediate_context.Get(), LoadingImage->getTexture(), LoadingImage->getShader(), CommandList.GetAddressOf());
			immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(FRAMEWORK->DS_TRUE_WRITE), 1);			// 2Dオブジェクトとの前後関係をしっかりするため再設
		}
		// 3Dオブジェクトの描画設定
		{
			//GpuParticle->Play(deferredContext.Get());
			//immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(DS_TRUE_WRITE), 1);			// 2Dオブジェクトとの前後関係をしっかりするため再設定
		}

		//ディファード コンテキストで作成したコマンドリストを実行
		//immediate_context->ExecuteCommandList(CommandList.Get(), FALSE);
		#endif
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