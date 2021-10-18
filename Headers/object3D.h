#pragma once

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <vector>
#include <string>
#include <memory>


class Object3D {
public:
	bool wireframe;	// ワイヤーフレーム表示の有無

	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizer_states[4];	// 0:片面塗りつぶし,1:両面塗り潰し,2:片面ワイヤーフレーム,3:両面ワイヤーフレーム

	struct PrimitivParam {
		DirectX::XMFLOAT3 Pos;		// 描画位置
		DirectX::XMFLOAT3 Size;		// 描画サイズ
		DirectX::XMFLOAT3 Angle;		// 回転角度
		DirectX::XMFLOAT4 Color;		// 加算色
	}param;

public:
	Object3D(ID3D11Device* device);

	// セッター
	void setPos(DirectX::XMFLOAT3 pos) { param.Pos = pos; }
	void setSize(DirectX::XMFLOAT3 Size) { param.Size = Size; }
	void setAngle(DirectX::XMFLOAT3 angle) { param.Angle = angle; }
	void setColor(DirectX::XMFLOAT4 color) { param.Color = color; }

	void setPos(float posX, float posY, float posZ) { param.Pos = DirectX::XMFLOAT3(posX, posY, posZ); }
	void setSize(float sizeX, float sizeY, float sizeZ) { param.Size = DirectX::XMFLOAT3(sizeX, sizeY, sizeZ); }
	void setAngle(float angleX, float angleY, float angleZ) { param.Angle = DirectX::XMFLOAT3(angleX, angleY, angleZ); }
	void setColor(float r, float g, float b, float a) { param.Color = DirectX::XMFLOAT4(r, g, b, a); }

	// ゲッター
	DirectX::XMFLOAT3 getPos() { return param.Pos; }
	DirectX::XMFLOAT3 getSize() { return param.Size; }
	DirectX::XMFLOAT3 getAngle() { return param.Angle; }
	DirectX::XMFLOAT4 getColor() { return param.Color; }
protected:
}
