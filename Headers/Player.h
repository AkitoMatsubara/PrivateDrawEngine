#pragma once
#include "Shot.h"
#include "skinned_mesh.h"
#include "shaderEx.h"
#include "Object3d.h"

#include "geometric_primitive.h"

#include <memory>
#include <vector>

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

class Player {
	// ------------------�ϐ�------------------
private:
	// ���f�����
	std::unique_ptr<Skinned_Mesh> Model;

	// �J�v�Z���^�œ����蔻���邽�߂ɉ�������
	std::unique_ptr<Geometric_Capsule> Capcule;
	std::unique_ptr<Geometric_Sphere> testSphere;

	std::unique_ptr<ShotManager> ShotsManager;
public:
	std::unique_ptr<Object3d> Parameters;

	// ------------------�֐�------------------
private:
	void Control(); // ����֐�
public:
	Player() {};
	~Player() {};

	void Initialize();
	void Update();
	void Render();

	void ImguiPlayer();
	ShotManager* getShotManager() { return ShotsManager.get(); }
	static Player* getInstance()
	{
		static Player* instance;
		return instance;
	}
};