#include "SceneTitle.h"
#include "SceneGame.h"
#include "SceneLoading.h"

bool SceneTitle::Initialize() {
	Microsoft::WRL::ComPtr<ID3D11Device> device = FRAMEWORK->GetDevice();	// frameworkからdeviceを取得
// シーンコンスタントバッファの設定
	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(SceneConstants);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	HRESULT hr = device->CreateBuffer(&buffer_desc, nullptr, ConstantBuffers[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// Blenderの設定
	//blender.setBlend(device.Get());

	// Samplerの設定
	DefaultSampleClamp = std::make_shared<Sampler>(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);

	// 各種クラス設定
	{
		// spriteオブジェクトを生成(今回は先頭の１つだけを生成する)
		TitleImage = std::make_unique<Sprite>();
		TitleImage->LoadImages(L".\\Resources\\Title\\Title.png");

		// Geometric_primitiveオブジェクトの生成
		{
			Grid = std::make_unique<Geometric_Cube>();
			Grid->setPos(DirectX::SimpleMath::Vector3(0, -1, 0));
			Grid->setSize(DirectX::SimpleMath::Vector3(10, 0.1f, 10));
		}

		GpuParticle = std::make_unique<GPUParticle>();
		GpuParticle->Init();

		skinned_mesh = std::make_unique<Skinned_Mesh>(".\\Resources\\Player\\Player.fbx");

		Camera::getInstance().SetProjection(DirectX::XMConvertToRadians(30), Camera::getInstance().GetWidth() / Camera::getInstance().GetHeight(), Camera::getInstance().GetNear(), Camera::getInstance().GetFar());
	}
	return true;
}

void SceneTitle::Update() {
	imguiUpdate();
	const float elapsed_time = FRAMEWORK->GetElapsedTime();

	// シーン切り替え
	//if (GetKeyState('G') < 0) {
	//	setScene(std::make_unique<SceneLoading>(std::make_unique<SceneGame>()));
	//}
	if (GetAsyncKeyState(VK_RETURN) & 1)
	{
		setScene(std::make_unique<SceneLoading>(std::make_unique<SceneGame>()));
	}
	//if (GetAsyncKeyState('G') & 1) setScene(std::make_unique<SceneGame>());
	// カメラ操作
	// カメラ設定
	Camera::getInstance().Set(Camera::getInstance().GetPos(), DirectX::SimpleMath::Vector3(0, 0, 0), DirectX::XMFLOAT3(0, 1, 0));
	// カメラ操作
	Camera::getInstance().Operate();
	Camera::getInstance().Activate();
}

void SceneTitle::Render() {
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
		immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(FRAMEWORK->DS_FALSE), 1);	// 3Dオブジェクトの後ろに出すため一旦
		TitleImage->Render(immediate_context.Get());
		immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(FRAMEWORK->DS_TRUE_WRITE), 1);			// 2Dオブジェクトとの前後関係をしっかりするため再設定
	}
	// 3Dオブジェクトの描画設定
	{
		// コンスタントバッファ更新
		SceneConstants data{};
		XMStoreFloat4x4(&data.view_projection, Camera::getInstance().GetView() * Camera::getInstance().GetProjection());	// Matrixから4x4へ変換
		data.light_direction = DirectX::SimpleMath::Vector4{ light_dir[0],light_dir[1],light_dir[2],0 };	// シェーダに渡すライトの向き
		data.camera_position = DirectX::SimpleMath::Vector4{ eyePos.x,eyePos.y,eyePos.z,0 };				// シェーダに渡すカメラの位置
		data.view = Camera::getInstance().GetView();
		data.projection = Camera::getInstance().GetProjection();
		immediate_context->UpdateSubresource(ConstantBuffers[0].Get(), 0, 0, &data, 0, 0);
		immediate_context->VSSetConstantBuffers(1, 1, ConstantBuffers[0].GetAddressOf());	// cBufferはドローコールのたびに消去されるので都度設定する必要がある
		immediate_context->PSSetConstantBuffers(1, 1, ConstantBuffers[0].GetAddressOf());
		{
			// 3DオブジェクトRender内に移植 現状ここである必要なし？
			Grid->Render(true);
			//skinned_mesh->Render(immediate_context.Get());
			//GpuParticle->Play(immediate_context.Get());
		}
	}

#ifdef USE_IMGUI
	//	ImGui::Render();
	//	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#else
	FRAMEWORK->Flip();
#endif
}

void SceneTitle::imguiUpdate() {
#ifdef USE_IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	imguiSceneChanger();
	// 2D用 内部関数で完結させてる
	TitleImage->ImguiWindow();
	// 3D用パラメータ
	//skinned_mesh->imguiWindow("fbx");

	// ライト調整等グローバル設定
	ImGui::Begin("Light");
	ImGui::SliderFloat3("Light_Direction", light_dir, -10.0f, 10.0f);
	ImGui::Checkbox("focus Zero", &focus_zero);
	ImGui::End();
#endif
}