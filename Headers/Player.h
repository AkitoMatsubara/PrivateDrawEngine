#pragma once
#include "skinned_mesh.h"
#include "shaderEx.h"
#include <DirectXMath.h>
#include <memory>


using namespace DirectX;

class Player {
	// 変数
private:
	XMFLOAT3 Position;	// ワールド位置
	XMFLOAT3 Size;		// 大きさ
	XMFLOAT3 Rotate;	// 各軸回転値
	XMFLOAT4 Color;		// 色

	// モデル情報
	unique_ptr<Skinned_Mesh> Model;
	unique_ptr<ShaderEx> SkinnedShader = nullptr;

protected:
public:
	// 関数
private:
protected:
public:
	Player();
	~Player();

	void Initialize();
	void Update();
	void Render();

};