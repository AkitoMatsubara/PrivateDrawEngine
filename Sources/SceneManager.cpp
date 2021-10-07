#include "SceneManager.h"

void SceneBase::imguiSceneChanger() {
	ImGui::Begin(u8"SceneChanger");
	if (ImGui::Button(u8"SceneTest"))
	{
		setScene(std::make_unique<SceneTest>());
	}

	if (ImGui::Button(u8"SceneTest_2"))
	{
		setScene(std::make_unique<SceneTest_2>());
	}
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

void SceneManager::ChangeScene(std::unique_ptr<SceneBase> newScene){
	scene = std::move(newScene);
	scene->Initialize();
}

