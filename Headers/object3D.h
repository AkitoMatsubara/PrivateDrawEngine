#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <vector>
#include <string>
#include <memory>

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include "Geometric_primitive.h"

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

class Object3D {
public:
	bool wireframe;	// ワイヤーフレーム表示の有無
	bool dispBounty;	// バウンティボックスの表示

	ComPtr<ID3D11RasterizerState>	rasterizer_states[4];	// 0:片面塗りつぶし,1:両面塗り潰し,2:片面ワイヤーフレーム,3:両面ワイヤーフレーム

	struct PrimitivParam {
		XMFLOAT3 Pos;		// 描画位置
		XMFLOAT3 Size;		// 描画サイズ
		XMFLOAT3 Angle;		// 回転角度
		XMFLOAT4 Color;		// 加算色
	}param;

	unique_ptr<Geometric_Cube> Bounty_Box;

public:
	Object3D(ID3D11Device* device);

	// セッター
	void setPos(XMFLOAT3 pos) { param.Pos = pos; }
	void setSize(XMFLOAT3 Size) { param.Size = Size; }
	void setAngle(XMFLOAT3 angle) { param.Angle = angle; }
	void setColor(XMFLOAT4 color) { param.Color = color; }

	void setPos(float posX, float posY, float posZ) { param.Pos = XMFLOAT3(posX, posY, posZ); }
	void setSize(float sizeX, float sizeY, float sizeZ) { param.Size = XMFLOAT3(sizeX, sizeY, sizeZ); }
	void setAngle(float angleX, float angleY, float angleZ) { param.Angle = XMFLOAT3(angleX, angleY, angleZ); }
	void setColor(float r, float g, float b, float a) { param.Color = XMFLOAT4(r, g, b, a); }

	// ゲッター
	XMFLOAT3 getPos() { return param.Pos; }
	XMFLOAT3 getSize() { return param.Size; }
	XMFLOAT3 getAngle() { return param.Angle; }
	XMFLOAT4 getColor() { return param.Color; }
protected:
	// バウンティボックスの生成
	void CreateBountyBox();
}
