#pragma once
#include <d3d11.h>
#include <SimpleMath.h>
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
		DirectX::SimpleMath::Vector3 position;
		DirectX::SimpleMath::Vector3 normal;
		DirectX::SimpleMath::Vector2 texcoord;
	};
	struct Subset {
		std::wstring usemtl;
		uint32_t index_start{ 0 };	// indexbuffer�̃X�^�[�g�ʒu
		uint32_t index_count{ 0 };	// ���_��
	};
	struct Material {
		std::wstring name;
		DirectX::SimpleMath::Vector4 Ka{ 0.2f,0.2f,0.2f,1.0f };	// �A���r�G���g
		DirectX::SimpleMath::Vector4 Kd{ 0.8f,0.8f,0.8f,1.0f };	// �f�B�q���[�Y
		DirectX::SimpleMath::Vector4 Ks{ 1.0f,1.0f,1.0f,1.0f };	// �X�y�L����
		static const int TEXTURE_NUM = 2;
		std::wstring texture_filenames[TEXTURE_NUM];								// [0]:�F�e�N�X�`�� [1]�@���}�b�v�Ƃ��Ďg�p
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view[TEXTURE_NUM];	// [0]:�F�e�N�X�`�� [1]�@���}�b�v�Ƃ��Ďg�p
	};
	struct Constants {
		DirectX::SimpleMath::Matrix world;
		DirectX::SimpleMath::Vector4 material_color;
	};



private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;

	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;
	std::vector<Subset> subsets;
	std::vector<Material> materials;

	Rasterizer rasterizer;

	bool wireframe;	// ���C���[�t���[���\���̗L��

	struct PrimitivParam {
		DirectX::SimpleMath::Vector3 Pos;		// �`��ʒu
		DirectX::SimpleMath::Vector3 Size;		// �`��T�C�Y
		DirectX::SimpleMath::Vector3 Angle;		// ��]�p�x
		DirectX::SimpleMath::Vector4 Color;		// ���Z�F
	}param;

public:
	Static_Mesh(const wchar_t* obj_filename, const char* vs_cso_name = "Shaders\\static_mesh_vs.cso", const char* ps_cso_name = "Shaders\\static_mesh_ps.cso");
	virtual ~Static_Mesh() = default;

	void Render(Shader* shader,const DirectX::SimpleMath::Matrix& world, const DirectX::SimpleMath::Vector4& material_color, bool WireFrame);	// �O�����烏�[���h�s�����肽����
	void Render(Shader* shader);							// �����̃��[���h�s����g�p=�ړ��Ȃǂ�����Ŋ��������Ă���

	// param��ҏW����imgui�E�B���h�E
	void imguiWindow(const char* beginname = "static_mesh");

	// �Z�b�^�[
	void setPos(DirectX::SimpleMath::Vector3 pos) { param.Pos = pos; }
	void setSize(DirectX::SimpleMath::Vector3 Size) { param.Size = Size; }
	void setAngle(DirectX::SimpleMath::Vector3 angle) { param.Angle = angle; }
	void setColor(DirectX::SimpleMath::Vector4 color) { param.Color = color; }

	void setPos(float posX, float posY, float posZ) { param.Pos = DirectX::SimpleMath::Vector3(posX, posY, posZ); }
	void setSize(float sizeX, float sizeY, float sizeZ) { param.Size = DirectX::SimpleMath::Vector3(sizeX, sizeY, sizeZ); }
	void setAngle(float angleX, float angleY, float angleZ) { param.Angle = DirectX::SimpleMath::Vector3(angleX, angleY, angleZ); }
	void setColor(float r, float g, float b, float a) { param.Color = DirectX::SimpleMath::Vector4(r, g, b, a); }

	// �Q�b�^�[
	DirectX::SimpleMath::Vector3 getPos() { return param.Pos; }
	DirectX::SimpleMath::Vector3 getSize() { return param.Size; }
	DirectX::SimpleMath::Vector3 getAngle() { return param.Angle; }
	DirectX::SimpleMath::Vector4 getColor() { return param.Color; }

protected:
	void Create_com_buffers(ID3D11Device* device, Vertex* vertices, size_t vertex_count, uint32_t* indices, size_t index_count);

};