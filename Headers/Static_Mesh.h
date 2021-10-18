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


class Static_Mesh {
public:
	struct Vertex {
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 texcoord;
	};
	struct Subset {
		std::wstring usemtl;
		uint32_t index_start{ 0 };	// indexbufferのスタート位置
		uint32_t index_count{ 0 };	// 頂点数
	};
	struct Material {
		std::wstring name;
		DirectX::XMFLOAT4 Ka{ 0.2f,0.2f,0.2f,1.0f };	// アンビエント
		DirectX::XMFLOAT4 Kd{ 0.8f,0.8f,0.8f,1.0f };	// ディヒューズ
		DirectX::XMFLOAT4 Ks{ 1.0f,1.0f,1.0f,1.0f };	// スペキュラ
		static const int TEXTURE_NUM = 2;
		std::wstring texture_filenames[TEXTURE_NUM];								// [0]:色テクスチャ [1]法線マップとして使用
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view[TEXTURE_NUM];	// [0]:色テクスチャ [1]法線マップとして使用
	};
	struct Constants {
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4 material_color;
	};



private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;

	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;
	std::vector<Subset> subsets;
	std::vector<Material> materials;

	Rasterizer rasterizer;

	bool wireframe;	// ワイヤーフレーム表示の有無

	struct PrimitivParam {
		DirectX::XMFLOAT3 Pos;		// 描画位置
		DirectX::XMFLOAT3 Size;		// 描画サイズ
		DirectX::XMFLOAT3 Angle;		// 回転角度
		DirectX::XMFLOAT4 Color;		// 加算色
	}param;

public:
	Static_Mesh(const wchar_t* obj_filename, const char* vs_cso_name = "Shaders\\static_mesh_vs.cso", const char* ps_cso_name = "Shaders\\static_mesh_ps.cso");
	virtual ~Static_Mesh() = default;

	void Render(Shader* shader,const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color, bool WireFrame);	// 外部からワールド行列を取りたい時
	void Render(Shader* shader);							// 内部のワールド行列を使用=移動などを内部で完結させている

	// paramを編集するimguiウィンドウ
	void imguiWindow(const char* beginname = "static_mesh");

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
	void Create_com_buffers(ID3D11Device* device, Vertex* vertices, size_t vertex_count, uint32_t* indices, size_t index_count);

};