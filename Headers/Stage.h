#pragma once

#include "skinned_mesh.h"
#include "shaderEx.h"

#include <memory>

class Stage
{
	// 変数
private:
	// デフォルトのシェーダー
	std::unique_ptr<ShaderEx> SkinnedShader;

public:
	std::unique_ptr<Skinned_Mesh> Model;	// そのままモデル

	//関数
private:
public:
	Stage(const char* fbx_filename = ".\\Resources\\Stage\\Stage.fbx", int cstNo = 0, const bool triangulate = false);
	~Stage() {};

	void Initialize();
	void Update();
	void Render();

	void setStageFbx(const char* fbx_filename, int cstNo = 0, const bool triangulate = false);	// SkinnedMeshのイニシャライザをそのまま呼び出し
};