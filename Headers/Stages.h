#pragma once


#include "skinned_mesh.h"
#include "Object3d.h"
#include "shaderEx.h"

#include <memory>
#include <array>
#include <SimpleMath.h>

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

	void onObject(const DirectX::SimpleMath::Vector3& objPos);
};
// ステージパーツの総管理クラス というかパーツ全部持ってる
class StageManager
{
private:
	const static int ROW_PARTS = 15;	// 行数。奇数での設定お願いします
	const static int COL_PARTS = 15;	// 列数。奇数での設定お願いします
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
	// StagePartsのonObject()に渡す仲介
	void Check(const DirectX::SimpleMath::Vector3& obj);

	// 取得、格納系
	size_t getSize() { return Stages.size(); };
};