#include "SceneTitle.h"
#include "SceneGame.h"
#include "SceneLoading.h"

bool SceneTitle::Initialize() {
	Microsoft::WRL::ComPtr<ID3D11Device> device = FRAMEWORK->GetDevice();	// frameworkからdeviceを取得
// シーンコンスタントバッファの設定
	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(scene_constants);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	HRESULT hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// Blenderの設定
	//blender.setBlend(device.Get());

	// Samplerの設定
	sampleClamp = std::make_shared<Sampler>(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);

	// 各種クラス設定
	{
		// spriteオブジェクトを生成(今回は先頭の１つだけを生成する)
		TitleImage = std::make_unique<Sprite>();
		TitleImage->LoadImages(L".\\Resources\\screenshot.jpg");
		TitleImage->setSize(1280, 720);

		// Geometric_primitiveオブジェクトの生成
		{
			grid = std::make_unique<Geometric_Cube>();
			grid->setPos(DirectX::SimpleMath::Vector3(0, -1, 0));
			grid->setSize(DirectX::SimpleMath::Vector3(10, 0.1f, 10));
			GeomtricShader = std::make_unique<ShaderEx>();
			GeomtricShader->CreateVS(L"Shaders\\geometric_primitive_vs");
			GeomtricShader->CreatePS(L"Shaders\\geometric_primitive_ps");
		}

		//skinned_mesh = make_unique<Skinned_Mesh>(".\\Resources\\cube.000.fbx");		// テクスチャ、マテリアル無し
		skinned_mesh = std::make_unique<Skinned_Mesh>(".\\Resources\\cube.001.0.fbx");	// テクスチャ使用
		//skinned_mesh = make_unique<Skinned_Mesh>(".\\Resources\\cube.001.1.fbx");	// 埋め込みテクスチャ
		//skinned_mesh = make_unique<Skinned_Mesh>(".\\Resources\\cube.002.0.fbx");	// 3種テクスチャ使用
		//skinned_mesh = make_unique<Skinned_Mesh>(".\\Resources\\cube.002.1.fbx");	// テクスチャ有り無し、マテリアル有り無し混合
		//skinned_mesh = make_unique<Skinned_Mesh>(".\\Resources\\cube.003.0.fbx");	// 複数メッシュ キューブと猿
		//skinned_mesh = make_unique<Skinned_Mesh>(".\\Resources\\cube.003.1.fbx", Skinned_Mesh::CST_RIGHT_Z, true);	// 3角形化されていない複数メッシュ キューブ
		camera = std::make_unique<Camera>();
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
	static float speed = 7.0f;
	if (GetKeyState('D') < 0)  eyePos.x += speed * elapsed_time;	// 右に
	if (GetKeyState('A') < 0)  eyePos.x -= speed * elapsed_time;	// 左に
	if (GetKeyState('W') < 0)  eyePos.z += speed * elapsed_time;	// 前に
	if (GetKeyState('S') < 0)  eyePos.z -= speed * elapsed_time;	// 後ろに
	if (GetKeyState(VK_SPACE) < 0)  eyePos.y += speed * elapsed_time;	// 上に
	if (GetKeyState(VK_SHIFT) < 0)  eyePos.y -= speed * elapsed_time;	// 下に

	// 透視投影行列の作成
	camera->Set(eyePos, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0, 1, 0));
	camera->SetProjection(DirectX::XMConvertToRadians(30), camera->GetWidth() / camera->GetHeight(), camera->GetNear(), camera->GetFar());
}

void SceneTitle::Render() {
	HRESULT hr{ S_OK };

	ID3D11DeviceContext* immediate_context = FRAMEWORK->GetDeviceContext();	// DevCon取得

	FRAMEWORK->Clear(ClearColor);	// 一旦クリア

		// ビューポートの設定
	FRAMEWORK->CreateViewPort();

	// サンプラーステートをバインド
	sampleClamp->Set(0);

	immediate_context->OMSetBlendState(FRAMEWORK->GetBlendState(FRAMEWORK->BS_ALPHA), nullptr, 0xFFFFFFFF);	// ブレンドインターフェースのポインタ、ブレンドファクターの配列値、サンプルカバレッジ(今回はデフォルト指定)

	// 2Dオブジェクトの描画設定
	{
		immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(FRAMEWORK->DS_TRUE), 1);	// 3Dオブジェクトの後ろに出すため一旦
		TitleImage->Render();
	}
	// 3Dオブジェクトの描画設定
	{
		camera->Activate();

		// コンスタントバッファ更新
		scene_constants data{};
		XMStoreFloat4x4(&data.view_projection, camera->GetView() * camera->GetProjection());	// Matrixから4x4へ変換
		data.light_direction = DirectX::SimpleMath::Vector4{ light_dir[0],light_dir[1],light_dir[2],0 };	// シェーダに渡すライトの向き
		data.camera_position = DirectX::SimpleMath::Vector4{ eyePos.x,eyePos.y,eyePos.z,0 };				// シェーダに渡すカメラの位置
		immediate_context->UpdateSubresource(constant_buffer[0].Get(), 0, 0, &data, 0, 0);
		immediate_context->VSSetConstantBuffers(1, 1, constant_buffer[0].GetAddressOf());	// cBufferはドローコールのたびに消去されるので都度設定する必要がある
		immediate_context->PSSetConstantBuffers(1, 1, constant_buffer[0].GetAddressOf());

		immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(FRAMEWORK->DS_TRUE_WRITE), 1);			// 2Dオブジェクトとの前後関係をしっかりするため再設定

		{
			// 3DオブジェクトRender内に移植 現状ここである必要なし？
			grid->Render(true);
			skinned_mesh->Render();
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
	skinned_mesh->imguiWindow("fbx");

	// ライト調整等グローバル設定
	ImGui::Begin("Light");
	ImGui::SliderFloat3("Light_Direction", light_dir, -10.0f, 10.0f);
	ImGui::Checkbox("focus Zero", &focus_zero);
	ImGui::End();
#endif
}