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

#include "shader.h"
#include "misc.h"
#include "Rasterizer.h"


using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

class Static_Mesh {
public:
	struct Vertex {
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT2 texcoord;
	};
	struct Subset {
		wstring usemtl;
		uint32_t index_start{ 0 };	// indexbufferのスタート位置
		uint32_t index_count{ 0 };	// 頂点数
	};
	struct Material {
		wstring name;
		XMFLOAT4 Ka{ 0.2f,0.2f,0.2f,1.0f };	// アンビエント
		XMFLOAT4 Kd{ 0.8f,0.8f,0.8f,1.0f };	// ディヒューズ
		XMFLOAT4 Ks{ 1.0f,1.0f,1.0f,1.0f };	// スペキュラ
		static const int TEXTURE_NUM = 2;
		wstring texture_filenames[TEXTURE_NUM];								// [0]:色テクスチャ [1]法線マップとして使用
		ComPtr<ID3D11ShaderResourceView> shader_resource_view[TEXTURE_NUM];	// [0]:色テクスチャ [1]法線マップとして使用
	};
	struct Constants {
		XMFLOAT4X4 world;
		XMFLOAT4 material_color;
	};



private:
	ComPtr<ID3D11Buffer> vertex_buffer;
	ComPtr<ID3D11Buffer> index_buffer;

	//ComPtr<ID3D11VertexShader> vertex_shader;
	//ComPtr<ID3D11PixelShader> pixel_shader;
	//ComPtr<ID3D11InputLayout> input_layout;
	ComPtr<ID3D11Buffer> constant_buffer;
	//ComPtr<ID3D11RasterizerState>	rasterizer_states[4];	// 0:片面塗りつぶし,1:両面塗り潰し,2:片面ワイヤーフレーム,3:両面ワイヤーフレーム
	vector<Subset> subsets;
	vector<Material> materials;

	Rasterizer rasterizer;

	bool wireframe;	// ワイヤーフレーム表示の有無

	struct PrimitivParam {
		XMFLOAT3 Pos;		// 描画位置
		XMFLOAT3 Size;		// 描画サイズ
		XMFLOAT3 Angle;		// 回転角度
		XMFLOAT4 Color;		// 加算色
	}param;

public:
	Static_Mesh(const wchar_t* obj_filename, const char* vs_cso_name = "Shaders\\static_mesh_vs.cso", const char* ps_cso_name = "Shaders\\static_mesh_ps.cso");
	virtual ~Static_Mesh() = default;

	void Render(Shader* shader,const XMFLOAT4X4& world, const XMFLOAT4& material_color, bool WireFrame);	// 外部からワールド行列を取りたい時
	void Render(Shader* shader);							// 内部のワールド行列を使用=移動などを内部で完結させている

	// paramを編集するimguiウィンドウ
	void imguiWindow(const char* beginname = "static_mesh");

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
	void Create_com_buffers(ID3D11Device* device, Vertex* vertices, size_t vertex_count, uint32_t* indices, size_t index_count);

};