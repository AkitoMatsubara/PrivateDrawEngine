#pragma once

#include "Object3d.h"
#include "skinned_mesh.h"
#include "geometric_primitive.h"
#include <memory>


// 弾クラス
class Shot
{
	// 変数
private:
	//static std::unique_ptr<Skinned_Mesh> Model;	// staticにしたかったけど上手く行かないので一旦削除
	std::unique_ptr<Skinned_Mesh> Model;	// そのままモデル
	std::unique_ptr<Geometric_Sphere> test;	// テストモデル まる

	// デフォルトのシェーダー
	std::unique_ptr<ShaderEx> SkinnedShader = nullptr;

	std::unique_ptr<Object3d> Parameters;
	float Timer;	// 射出時間の保有
	bool Exist;		// 発射済みフラグ
public:

	// 関数
private:
public:
	std::unique_ptr<Shot> clone()const;

	Shot(){};
	Shot(Shot&shot)
	{
		this->Model = std::move(shot.Model);
		this->test = std::move(shot.test);
		this->Timer = shot.Timer;
	};
	~Shot(){};

	void Initialize();
	void Update();
	void Render();

	void set(const Object3d* parent);	// オブジェクトは全てObject3dクラスを保有している前提で各種パラメータをコピー
	bool getExist() { return Exist; }
};

