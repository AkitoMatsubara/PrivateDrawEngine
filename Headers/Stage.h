#pragma once


#include "skinned_mesh.h"
#include "shaderEx.h"

#include <memory>
#include <array>

class Stage
{
	// 変数
private:
	// デフォルトのシェーダー
	std::unique_ptr<ShaderEx> SkinnedShader;

	const static int ROW_PARTS = 21;	// 行数。奇数での設定お願いします
	const static int COL_PARTS = 21;	// 列数。奇数での設定お願いします
	const static int PARTS_SIZE = ROW_PARTS * COL_PARTS;	// 縦*横の全てのパーツ数
public:
	//std::unique_ptr<Skinned_Mesh> Model;	// そのままモデル
	std::array<std::unique_ptr<Skinned_Mesh>, PARTS_SIZE> Model;	// そのままモデル

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