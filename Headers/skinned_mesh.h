#pragma once

#include "Rasterizer.h"
#include "shaderEx.h"
#include "Object3d.h"

#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include <SimpleMath.h>
#include <vector>
#include <string>
#include <fbxsdk.h>
#include <unordered_map>

struct scene {
	struct node {
		uint64_t unique_id{ 0 };
		std::string name;
		FbxNodeAttribute::EType attribute{ FbxNodeAttribute::EType::eUnknown };	// attribute:属性
		int64_t parent_index{ -1 };
	};

	std::vector<node>nodes;
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
		DirectX::SimpleMath::Vector3 position;
		DirectX::SimpleMath::Vector3 normal;
		DirectX::SimpleMath::Vector2 texcoord;
	};
	struct Constants {
		DirectX::SimpleMath::Matrix world;
		DirectX::SimpleMath::Vector4 material_color;
	};

	struct Material {
		uint64_t unique_id{ 0 };
		std::string name;
		DirectX::SimpleMath::Vector4 Ka{ 0.2f,0.2f,0.2f,1.0f };
		DirectX::SimpleMath::Vector4 Kd{ 0.8f,0.8f,0.8f,1.0f };
		DirectX::SimpleMath::Vector4 Ks{ 1.0f,1.0f,1.0f,1.0f };

		std::string texture_filenames[4];
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv[4];
	};
	// <キー型,値型>オブジェクト名
	std::unordered_map<uint64_t, Material>materials;
	static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> dummyTexture;	// fbxにマテリアルが設定されていない場合に使用する

	struct Mesh {
		uint64_t unique_id{ 0 };
		std::string name;
		// シーンのノード配列を参照するインデックス
		int64_t node_index{ 0 };

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		struct Subset {
			uint64_t material_unique_id{ 0 };
			std::string material_name;

			uint32_t start_index_location{ 0 };
			uint32_t index_count{ 0 };
		};
		std::vector<Subset> subsets;

		DirectX::SimpleMath::Matrix default_global_transform{
			1,0,0,0,	// _11,_12,_13,_14
			0,1,0,0,	// _21,_22,_23,_24
			0,0,1,0,	// _31,_32,_33,_34
			0,0,0,1		// _41,_42,_43,_44
		};

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
		friend class Skinned_Mesh;
	};
	std::vector<Mesh>meshes;

	enum { CST_RIGHT_Y, CST_LEFT_Y, CST_RIGHT_Z, CST_LEFT_Z, CST_END };	// coordinate_system_transforms変数用

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;

	Rasterizer rasterizer;

	bool wireframe;	// ワイヤーフレーム表示の有無

	//struct PrimitivParam {
	//	DirectX::SimpleMath::Vector3 Position;		// 描画位置
	//	DirectX::SimpleMath::Vector3 Scale;		// 描画サイズ
	//	DirectX::SimpleMath::Vector3 Rotate;		// 回転角度
	//	DirectX::SimpleMath::Vector4 Color;		// 加算色
	//}Parameters;
	std::unique_ptr<Object3d> Parameters;


	DirectX::SimpleMath::Matrix coordinate_system_transforms[CST_END] = {
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
	void Fetch_Meshes(FbxScene* fbx_scene, std::vector<Mesh>& meshes);
	// マテリアルの取り出し
	void Fetch_Materials(FbxScene* fbx_scene, std::unordered_map<uint64_t, Material>& materials);

	void Render(Shader* shader, int rasterize = 0);

	// paramを編集するimguiウィンドウ
	void imguiWindow(const char* beginname = "skinned_mesh");


	// セッター
	void setPos(DirectX::SimpleMath::Vector3 pos) { Parameters->Position = pos; }
	void setSize(DirectX::SimpleMath::Vector3 Size) { Parameters->Scale = Size; }
	void setAngle(DirectX::SimpleMath::Vector3 angle) { Parameters->Rotate = angle; }
	void setColor(DirectX::SimpleMath::Vector4 color) { Parameters->Color = color; }

	void setPos(float posX, float posY, float posZ) { Parameters->Position = DirectX::SimpleMath::Vector3(posX, posY, posZ); }
	void setSize(float sizeX, float sizeY, float sizeZ) { Parameters->Scale = DirectX::SimpleMath::Vector3(sizeX, sizeY, sizeZ); }
	void setAngle(float angleX, float angleY, float angleZ) { Parameters->Rotate = DirectX::SimpleMath::Vector3(angleX, angleY, angleZ); }
	void setColor(float r, float g, float b, float a) { Parameters->Color = DirectX::SimpleMath::Vector4(r, g, b, a); }

	// ゲッター
	DirectX::SimpleMath::Vector3 getPos() { return Parameters->Position; }
	DirectX::SimpleMath::Vector3 getSize() { return Parameters->Scale; }
	DirectX::SimpleMath::Vector3 getAngle() { return Parameters->Rotate; }
	DirectX::SimpleMath::Vector4 getColor() { return Parameters->Color; }
	Object3d* getParameters() { return Parameters.get(); }

private:
	void Create_com_buffers(const char* fbx_filename);
protected:
	scene scene_view;
};