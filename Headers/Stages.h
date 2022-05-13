#pragma once


#include "instance_skinned_mesh.h"
#include "skinned_mesh.h"
#include "Object3d.h"
#include "shaderEx.h"

#include <memory>
#include <array>
#include <SimpleMath.h>

class StageParts
{
	// 変数
private:

public:
	std::unique_ptr<Object3d> Parameters;
	std::unique_ptr<Skinned_Mesh> Model;	// そのままモデル
	static inline std::unique_ptr<InstanceSkinnedMesh> InstanceModel;	// インスタンス描画するモデル

	int draw_amount;	// インスタンス描画用 描画する回数を指す
	//関数
private:
public:
	StageParts(int n) :draw_amount(n) {};	// コンストラクタでインスタンス描画数を決める
	~StageParts() {};

	void Initialize();
	void Update(bool inst);
	void Render(UINT drawCount, bool inst, Shader* shader = 0);

	//bool onObject(const Object3d& obj);	// 三角形モデルのとき 前方に少しずらして判定をとってた
	bool onObjectSphere(const Object3d& obj, const float& radian);	// 球モデル時の判定
	void Damage();
};

// ステージパーツの総管理クラス というかパーツ全部持ってる
class StageManager
{
private:
	const static int ROW_PARTS = 19;	// 行数。奇数での設定お願いします
	const static int COL_PARTS = 19;	// 列数。奇数での設定お願いします
	const static int PARTS_SIZE = ROW_PARTS * COL_PARTS;	// 縦*横の全てのパーツ数

	std::array<std::unique_ptr<StageParts>, PARTS_SIZE> Stages;	// そのままモデル
	static inline bool inst = true;	// インスタンス描画の有無

public:
	StageManager();
	void Initialize();
	void Update();
	void Render(Shader* shader = nullptr);

	static StageManager& getInstance()
	{
		static StageManager instance;
		return instance;
	}
	// StagePartsの上で弾を発射したときに呼び出す。
	///<summary>どの床に乗ってるか判定、ダメージ加算</summary>
	void Check(const Object3d& obj, const float& radian);

	/// <summary>床の上に乗っているかの判定</summary>
	bool RideParts(Object3d& obj, const float& radian);

	// 取得、格納系
	size_t getSize() { return Stages.size(); };
};