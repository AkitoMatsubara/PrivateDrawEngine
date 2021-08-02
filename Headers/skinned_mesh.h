#pragma once
#include <d3d11.h>>
#include <wrl.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <fbxsdk.h>

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include "geometric_primitive.h"
#include "Rasterizer.h"

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

struct scene {
	struct node {
		uint64_t unique_id{ 0 };
		string name;
		FbxNodeAttribute::EType attribute{ FbxNodeAttribute::EType::eUnknown };	// attribute:����
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
	// ���_�\����
	struct Vertex {
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT2 texcoord;
	};
	struct Constants {
		XMFLOAT4X4 world;
		XMFLOAT4 material_color;
	};

	struct Mesh{
		uint64_t unique_id{ 0 };
		string name;
		// �V�[���̃m�[�h�z����Q�Ƃ���C���f�b�N�X
		int64_t node_index{ 0 };

		vector<Vertex> vertices;
		vector<uint32_t> indices;

	private:
		ComPtr<ID3D11Buffer> vertex_buffer;
		ComPtr<ID3D11Buffer> index_buffer;
		friend class Skinned_Mesh;
	};
	vector<Mesh>meshes;


private:
	ComPtr<ID3D11VertexShader> vertex_shader;
	ComPtr<ID3D11PixelShader> pixel_shader;
	ComPtr<ID3D11InputLayout> input_layout;
	ComPtr<ID3D11Buffer> constant_buffer;

	Rasterizer rasterizer;
	unique_ptr<Geometric_Cube> Bounty_Box;

	bool wireframe;	// ���C���[�t���[���\���̗L��
	bool dispBounty;	// �o�E���e�B�{�b�N�X�̕\��

	struct PrimitivParam {
		XMFLOAT3 Pos;		// �`��ʒu
		XMFLOAT3 Size;		// �`��T�C�Y
		XMFLOAT3 Angle;		// ��]�p�x
		XMFLOAT4 Color;		// ���Z�F
	}param;

public:
	Skinned_Mesh(ID3D11Device* device, const char* fbx_filename, const char* vs_cso_name = "Shaders\\skinned_mesh_vs.cso", const char* ps_cso_name = "Shaders\\skinned_mesh_ps.cso", bool triangulate = false);

	virtual ~Skinned_Mesh() = default;

	// ���b�V���̎��o��
	void Fetch_Meshes(FbxScene* fbx_scene, vector<Mesh>& meshes);

	void Render(ID3D11DeviceContext* immediate_context);

	// param��ҏW����imgui�E�B���h�E
	void imguiWindow(const char* beginname = "skinned_mesh");


	// �Z�b�^�[
	void setPos(XMFLOAT3 pos) { param.Pos = pos; }
	void setSize(XMFLOAT3 Size) { param.Size = Size; }
	void setAngle(XMFLOAT3 angle) { param.Angle = angle; }
	void setColor(XMFLOAT4 color) { param.Color = color; }

	void setPos(float posX, float posY, float posZ) { param.Pos = XMFLOAT3(posX, posY, posZ); }
	void setSize(float sizeX, float sizeY, float sizeZ) { param.Size = XMFLOAT3(sizeX, sizeY, sizeZ); }
	void setAngle(float angleX, float angleY, float angleZ) { param.Angle = XMFLOAT3(angleX, angleY, angleZ); }
	void setColor(float r, float g, float b, float a) { param.Color = XMFLOAT4(r, g, b, a); }

	// �Q�b�^�[
	XMFLOAT3 getPos() { return param.Pos; }
	XMFLOAT3 getSize() { return param.Size; }
	XMFLOAT3 getAngle() { return param.Angle; }
	XMFLOAT4 getColor() { return param.Color; }

private:
	void Create_com_buffers(ID3D11Device* device, const char* fbx_filename, const char* vs_cso_name, const char* ps_cso_name);
protected:
	scene scene_view;
};