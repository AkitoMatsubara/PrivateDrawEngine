#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>

using namespace DirectX;
using namespace Microsoft::WRL;

class Geometric_primitive {
public:
	struct Vertex {
		XMFLOAT3 position;
		XMFLOAT3 normal;
	};
	struct Constants {
		XMFLOAT4X4 world;
		XMFLOAT4 material_color;
	};

private:
	ComPtr<ID3D11Buffer> vertex_buffer;
	ComPtr<ID3D11Buffer> index_buffer;

	ComPtr<ID3D11VertexShader> vertex_shader;
	ComPtr<ID3D11PixelShader> pixel_shader;
	ComPtr<ID3D11InputLayout> input_layout;
	ComPtr<ID3D11Buffer> constant_buffer;

	// 頂点情報用列挙型 値を入れるのでは読みにくかったので言葉で表せるように
	enum FACE {
		TOP_FACE,
		BOTTOM_FACE,
		FRONT_FACE,
		BACK_FACE,
		RIGHT_FACE,
		LEFT_FACE,
	};
	enum {
		LEFT_TOP,
		RIGHT_TOP,
		LEFT_BOTTOM,
		RIGHT_BOTTOM,
	};
	const float SIZE   = 1.0f;
	const float LEFT   = -SIZE * 0.5f;
	const float RIGHT  = +SIZE * 0.5f;
	const float BOTTOM = -SIZE * 0.5f;
	const float TOP    = +SIZE * 0.5f;
	const float FRONT  = -SIZE * 0.5f;
	const float BACK   = +SIZE * 0.5f;

	struct primitivParam {
		XMFLOAT3 Pos;		// 描画位置
		XMFLOAT3 Size;		// 描画サイズ
		XMFLOAT3 Angle;		// 回転角度
		XMFLOAT4 Color;		// 加算色
	}param;

public:
	Geometric_primitive(ID3D11Device* device, const char* vs_cso_name = "Shaders\\geometric_primitive_vs.cso", const char* ps_cso_name = "Shaders\\geometric_primitive_ps.cso");
	virtual ~Geometric_primitive() = default;

	void Render(ID3D11DeviceContext* immediate_context, const XMFLOAT4X4& world, const XMFLOAT4& material_color);
	void Render(ID3D11DeviceContext* immediate_context, const XMFLOAT4& material_color);

	// paramを編集するimguiウィンドウ
	void imguiWindow();

	// セッター
	void setPos    (XMFLOAT3 pos)   { param.Pos = pos; }
	void setSize   (XMFLOAT3 Size)  { param.Size = Size; }
	void setAngle  (XMFLOAT3 angle) { param.Angle = angle; }
	void setColor  (XMFLOAT4 color) { param.Color = color; }

	void setPos  (float posX, float posY,float posZ)    { param.Pos = XMFLOAT3(posX, posY,posZ); }
	void setSize (float sizeX, float sizeY,float sizeZ) { param.Size = XMFLOAT3(sizeX, sizeY,sizeZ); }
	void setAngle (float angleX, float angleY,float angleZ) { param.Angle = XMFLOAT3(angleX, angleY,angleZ); }
	void setColor(float r, float g, float b, float a)   { param.Color = XMFLOAT4(r, g, b, a); }

	// ゲッター
	XMFLOAT3 getPos() { return param.Pos; }
	XMFLOAT3 getSize() { return param.Size; }
	XMFLOAT3 getAngle() { return param.Angle; }
	XMFLOAT4 getColor() { return param.Color; }

protected:
	void Create_com_buffers(ID3D11Device* device, Vertex* vertices, size_t vertex_count, uint32_t* indices, size_t index_count);

};