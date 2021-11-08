#pragma once
#include "Shot.h"
#include "skinned_mesh.h"
#include "shaderEx.h"
#include "Object3d.h"

#include "geometric_primitive.h"
#include <memory>

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

//#include <vector>
class Player {
	// ------------------�ϐ�------------------
private:
	// ���f�����
	std::unique_ptr<Skinned_Mesh> Model;
	// �f�t�H���g�̃V�F�[�_�[
	std::unique_ptr<ShaderEx> SkinnedShader;

	// �J�v�Z���^�œ����蔻���邽�߂ɉ�������
	std::unique_ptr<Geometric_Capsule> Capcule;
	std::unique_ptr<Geometric_Sphere> testSphere;

	std::unique_ptr<Shot> Shots;
	std::vector<std::unique_ptr<Shot>> ShotsManager;
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
};