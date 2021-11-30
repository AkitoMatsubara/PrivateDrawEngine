#pragma once


#include "skinned_mesh.h"
#include "Object3d.h"
#include "shaderEx.h"

#include <memory>
#include <array>
#include <vector>

class Enemy;

class StageParts
{
	// 変数
private:
	// デフォルトのシェーダー
	static inline std::unique_ptr<ShaderEx> SkinnedShader;

public:
	std::unique_ptr<Object3d> Parameters;
	std::unique_ptr<Skinned_Mesh> Model;	// そのままモデル

	//関数
private:
public:
	StageParts();
	~StageParts() {};

	void Initialize();
	void Update();
	void Render();

	void setStageFbx(const char* fbx_filename, int cstNo = 0, const bool triangulate = false);	// SkinnedMeshのイニシャライザをそのまま呼び出し

	int getStageObj();
};
// ステージパーツの総管理クラス というかパーツ全部持ってる
class StageManager
{
private:
	const static int ROW_PARTS = 11;	// 行数。奇数での設定お願いします
	const static int COL_PARTS = 11;	// 列数。奇数での設定お願いします
	const static int PARTS_SIZE = ROW_PARTS * COL_PARTS;	// 縦*横の全てのパーツ数

	std::array<std::unique_ptr<StageParts>, PARTS_SIZE> Stages;	// そのままモデル

public:
	StageManager();
	void Initialize();
	void Update();
	void Render();

	static StageManager& getInstance()
	{
		static StageManager instance;
		return instance;
	}

	// 生成、格納系
	void newSet(const Object3d* initData);

	size_t getSize() { return Stages.size(); };
};