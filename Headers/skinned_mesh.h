#pragma once

#include "Rasterizer.h"
#include "shaderEx.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <fbxsdk.h>
#include <unordered_map>

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

struct scene {
	struct node {
		uint64_t unique_id{ 0 };
		string name;
		FbxNodeAttribute::EType attribute{ FbxNodeAttribute::EType::eUnknown };	// attribute:属性
		int64_t parent_index{ -1 };
	};
	vector<node>nodes;
	int64_t indexof(uint64_t unique_id)const {
		int64_t index{ 0 };
		for (const node& node : nodes) {
			if (node.unique_id == unique_id) {
				return index;
			}
			index++;
		}
		return -1;
	}
};

class Skinned_Mesh {
public:
	// 頂点構造体
	struct Vertex {
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT2 texcoord;
	};
	struct Constants {
		XMFLOAT4X4 world;
		XMFLOAT4 material_color;
	};

	struct Material {
		uint64_t unique_id{ 0 };
		string name;
		XMFLOAT4 Ka{ 0.2f,0.2f,0.2f,1.0f };
		XMFLOAT4 Kd{ 0.8f,0.8f,0.8f,1.0f };
		XMFLOAT4 Ks{ 1.0f,1.0f,1.0f,1.0f };

		string texture_filenames[4];
		ComPtr<ID3D11ShaderResourceView> srv[4];
	};
	// <キー型,値型>オブジェクト名
	unordered_map<uint64_t, Material>materials;
	static ComPtr<ID3D11ShaderResourceView> dummyTexture;	// fbxにマテリアルが設定されていない場合に使用する

	struct Mesh{
		uint64_t unique_id{ 0 };
		string name;
		// シーンのノード配列を参照するインデックス
		int64_t node_index{ 0 };

		vector<Vertex> vertices;
		vector<uint32_t> indices;

		struct Subset {
			uint64_t material_unique_id{ 0 };
			string material_name;

			uint32_t start_index_location{ 0 };
			uint32_t index_count{ 0 };
		};
		vector<Subset> subsets;

		XMFLOAT4X4 default_global_transform{
			1,0,0,0,	// _11,_12,_13,_14
			0,1,0,0,	// _21,_22,_23,_24
			0,0,1,0,	// _31,_32,_33,_34
			0,0,0,1		// _41,_42,_43,_44
		};

	private:
		ComPtr<ID3D11Buffer> vertex_buffer;
		ComPtr<ID3D11Buffer> index_buffer;
		friend class Skinned_Mesh;
	};
	vector<Mesh>meshes;

	enum { CST_RIGHT_Y, CST_LEFT_Y, CST_RIGHT_Z, CST_LEFT_Z, CST_END };	// coordinate_system_transforms変数用

private:
	//ComPtr<ID3D11VertexShader> vertex_shader;
	//ComPtr<ID3D11PixelShader> pixel_shader;
	//ComPtr<ID3D11InputLayout> input_layout;
	ComPtr<ID3D11Buffer> constant_buffer;

	Rasterizer rasterizer;

	bool wireframe;	// ワイヤーフレーム表示の有無

	struct PrimitivParam {
		XMFLOAT3 Pos;		// 描画位置
		XMFLOAT3 Size;		// 描画サイズ
		XMFLOAT3 Angle;		// 回転角度
		XMFLOAT4 Color;		// 加算色
	}param;

	 XMFLOAT4X4 coordinate_system_transforms[CST_END]={
		{-1, 0, 0, 0,
		  0, 1, 0, 0,
		  0, 0, 1, 0,
		  0, 0, 0, 1},	// 0:右手座標系,Y-UP
		{ 1, 0, 0, 0,
		  0, 1, 0, 0,
		  0, 0, 1, 0,
		  0, 0, 0, 1},	// 1:左手座標系,Y-UP
		{-1, 0, 0, 0,
		  0, 0,-1, 0,
		  0, 1, 0, 0,
		  0, 0, 0, 1},	// 2:右手座標系,Z-UP
		{ 1, 0, 0, 0,
		  0, 0, 1, 0,
		  0, 1, 0, 0,
		  0, 0, 0, 1},	// 1:左手座標系,Z-UP
	};
	 int CstNo;

public:
	Skinned_Mesh(const char* fbx_filename, int cstNo = 0, const bool triangulate = false);

	virtual ~Skinned_Mesh() = default;

	// メッシュの取り出し
	void Fetch_Meshes(FbxScene* fbx_scene, vector<Mesh>& meshes);
	// マテリアルの取り出し
	void Fetch_Materials(FbxScene* fbx_scene, unordered_map<uint64_t, Material>& materials);

	void Render(Shader* shader, int rasterize = 0);

	// paramを編集するimguiウィンドウ
	void imguiWindow(const char* beginname = "skinned_mesh");


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

private:
	void Create_com_buffers(const char* fbx_filename);
protected:
	scene scene_view;
};