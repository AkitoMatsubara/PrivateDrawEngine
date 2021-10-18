#pragma once
#include "skinned_mesh.h"
#include "shaderEx.h"

#include <memory>



class Player {
	// 変数
private:
	DirectX::XMFLOAT3 Position;		// ワールド位置
	DirectX::XMFLOAT3 Vector;		// 方向
	DirectX::XMFLOAT3 acceleration;	// 加速度
	DirectX::XMFLOAT3 Velocity;		// 速度

	DirectX::XMFLOAT3 Size;			// 大きさ
	DirectX::XMFLOAT3 Rotate;		// 各軸回転値
	DirectX::XMFLOAT4 Color;			// 色

	// モデル情報
	std::unique_ptr<Skinned_Mesh> Model;
	// デフォルトのシェーダー
	std:: unique_ptr<ShaderEx> SkinnedShader = nullptr;

protected:
public:
	// 関数
private:
protected:
public:
	Player() {};
	~Player() {};

	void Initialize();
	void Update();
	void Render();

};