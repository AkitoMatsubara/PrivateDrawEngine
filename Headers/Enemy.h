#pragma once
#include "Shot.h"
#include "skinned_mesh.h"
#include "shaderEx.h"
#include "Object3d.h"

#include <memory>



class Enemy {
	// 変数
private:

	// モデル情報
	std::unique_ptr<Skinned_Mesh> Model;
	// デフォルトのシェーダー
	std:: unique_ptr<ShaderEx> SkinnedShader = nullptr;

	std::unique_ptr<Shot> Shots;
protected:
public:
		std::unique_ptr<Object3d> Parameters;
// 関数
private:
	void Move(); // 制御関数
protected:
public:
	 Enemy() {};
	~Enemy() {};

	void Initialize();
	void Update();
	void Render();

};