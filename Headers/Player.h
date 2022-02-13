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
	std::unique_ptr<Geometric_Sphere> playerHitSphere;
	std::unique_ptr<Geometric_Sphere> testSphere2;

	std::unique_ptr<ShotManager> ShotsManager;
	inline static Player* instance;

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

	ShotManager* getShotManager() { return ShotsManager.get(); }
	static Player* getInstance() { return instance; }

	void ImguiPlayer();

	int Damage();	// �_���[�W���Z�A�_���v�Z���HP��Ԃ��B���S�����Ɏg����Ǝv����
};