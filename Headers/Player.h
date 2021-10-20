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

	// セッター
	void setPos(DirectX::XMFLOAT3 pos)     { Position = pos; }
	void setSize(DirectX::XMFLOAT3 Size)   { Size = Size; }
	void setAngle(DirectX::XMFLOAT3 angle) { Rotate = angle; }
	void setColor(DirectX::XMFLOAT4 color) { Color = color; }

	void setPos(float posX, float posY, float posZ)         { Position = DirectX::XMFLOAT3(posX, posY, posZ); }
	void setSize(float sizeX, float sizeY, float sizeZ)     { Size = DirectX::XMFLOAT3(sizeX, sizeY, sizeZ); }
	void setAngle(float angleX, float angleY, float angleZ) { Rotate = DirectX::XMFLOAT3(angleX, angleY, angleZ); }
	void setColor(float r, float g, float b, float a)       { Color = DirectX::XMFLOAT4(r, g, b, a); }

	// ゲッター
	DirectX::XMFLOAT3 getPos()   { return Position; }
	DirectX::XMFLOAT3 getSize()  { return Size; }
	DirectX::XMFLOAT3 getAngle() { return Rotate; }
	DirectX::XMFLOAT4 getColor() { return Color; }
};