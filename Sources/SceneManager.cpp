#include "SceneManager.h"

SceneBase::~SceneBase() {
	//ステートの初期化
	ID3D11DeviceContext* device_context = FRAMEWORK->GetDeviceContext();
	device_context->ClearState();
	device_context->Flush();

}

void SceneBase::CreateConstantBuffer(ID3D11Buffer** ppBuffer, u_int size) {
	ID3D11Device* device = FRAMEWORK->GetDevice();

	// コンスタントバッファの設定
	D3D11_BUFFER_DESC buffer_desc{};
	ZeroMemory(&buffer_desc, sizeof(D3D11_BUFFER_DESC));
	buffer_desc.ByteWidth = size;
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	HRESULT hr = device->CreateBuffer(&buffer_desc, nullptr, ppBuffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void SceneBase::imguiSceneChanger() {
	ImGui::Begin(u8"SceneChanger");
	//if (ImGui::Button(u8"SceneTest"))
	//{
	//	setScene(std::make_unique<SceneTest>());
	//}

	//if (ImGui::Button(u8"SceneTest_2"))
	//{
	//	setScene(std::make_unique<SceneTest_2>());
	//}
	ImGui::End();

}

void SceneManager::Update() {
	std::unique_ptr<SceneBase> newScene = nullptr;
	newScene = scene->getScene();
	if (newScene) {				//チェック
		ChangeScene(std::move(newScene));	// 変更開始
	}
	scene->Update();	// 更新
}

void SceneManager::Render() {
	scene->Render();
}

void SceneManager::ChangeScene(std::unique_ptr<SceneBase> newScene) {
	scene = std::move(newScene);
	scene->Initialize();
}

