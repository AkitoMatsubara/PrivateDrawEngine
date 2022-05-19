#pragma once

#include "shaderEx.h"
#include "Object3d.h"
#include "skinned_mesh.h"

#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include <SimpleMath.h>
#include <vector>
#include <string>
#include <fbxsdk.h>
#include <unordered_map>

#include <cereal/archives/binary.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/unordered_map.hpp>


class InstanceSkinnedMesh {
public:
	// ���_�\����
	struct Vertex {
		DirectX::SimpleMath::Vector3 position;
		DirectX::SimpleMath::Vector3 normal;
		DirectX::SimpleMath::Vector2 texcoord;
		DirectX::SimpleMath::Matrix matWorld;
		template<class T>
		void serialize(T& archive) { archive(position, normal, texcoord); }
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

		template<class T>
		void serialize(T& archive) { archive(unique_id, name, Ka, Kd, Ks, texture_filenames); }
	};
	// <�L�[�^,�l�^>�I�u�W�F�N�g��
	std::unordered_map<uint64_t, Material>materials;
	static inline Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> dummyTexture;	// fbx�Ƀ}�e���A�����ݒ肳��Ă��Ȃ��ꍇ�Ɏg�p����

	struct Mesh {
	public:
		uint64_t unique_id{ 0 };
		std::string name;
		// �V�[���̃m�[�h�z����Q�Ƃ���C���f�b�N�X
		int64_t node_index{ 0 };

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		struct Subset {
			uint64_t material_unique_id{ 0 };
			std::string material_name;

			uint32_t start_index_location{ 0 };
			uint32_t index_count{ 0 };

			template<class T>
			void serialize(T& archive) { archive(material_unique_id, material_name, start_index_location, index_count); }
		};
		std::vector<Subset> subsets;

		DirectX::SimpleMath::Matrix default_global_transform{
			1,0,0,0,	// _11,_12,_13,_14
			0,1,0,0,	// _21,_22,_23,_24
			0,0,1,0,	// _31,_32,_33,_34
			0,0,0,1		// _41,_42,_43,_44
		};

		template<class T>
		void serialize(T& archive) { archive(unique_id, name, node_index, vertices, indices, subsets, default_global_transform); }

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
		friend class InstanceSkinnedMesh;
	};
	std::vector<Mesh>meshes;

	enum { CST_RIGHT_Y, CST_LEFT_Y, CST_RIGHT_Z, CST_LEFT_Z, CST_END };	// coordinate_system_transforms�ϐ��p


	//�C���X�^���X�`��----------------------------------------------------------------------------------
	static inline std::unique_ptr<ShaderEx> InstanceShader;	// �f�t�H���g�̃V�F�[�_ �C���X�^���X�`��p
	Microsoft::WRL::ComPtr<ID3D11Buffer> InputBuffer = NULL; // �C���X�^���X�`��p�\�����o�b�t�@
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SRV;	// GPU�ɍ\���̂𑗂邽�߂�SRV StructuredBuffer�Ƃ���
	struct InstanceData {	// HLSL�ɑ���\����
		DirectX::SimpleMath::Matrix world;
		DirectX::SimpleMath::Vector4 color;
	};
	std::vector<InstanceData> instanceData;	// �����̏���ێ�����̂ŃR���e�i

	struct WorldData {	// ���[���h�s����v�Z���邽�߂Ɋe�l��ێ�����\����
		DirectX::SimpleMath::Vector3 size;	// �傫����ۑ�
		DirectX::SimpleMath::Vector3 position;	// ���[���h�ʒu��ۑ�
		DirectX::SimpleMath::Quaternion orientation;	// ��]��ۑ�
		DirectX::SimpleMath::Vector4 color;	// �F��ۑ�
	};
	std::vector<WorldData> worldData;	// �����̏���ێ�����̂ŃR���e�i
	//-------------------------------------------------------------------------------------------------

private:
	DirectX::SimpleMath::Matrix world;	// ���[���h�s��

	Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBuffers;

	bool wireframe;	// ���C���[�t���[���\���̗L��

	std::unique_ptr<Object3d> Parameters;

	DirectX::SimpleMath::Matrix coordinate_system_transforms[CST_END] = {
		{-1, 0, 0, 0,
		  0, 1, 0, 0,
		  0, 0, 1, 0,
		  0, 0, 0, 1},	// 0:�E����W�n,Y-UP
		{ 1, 0, 0, 0,
		  0, 1, 0, 0,
		  0, 0, 1, 0,
		  0, 0, 0, 1},	// 1:������W�n,Y-UP
		{-1, 0, 0, 0,
		  0, 0,-1, 0,
		  0, 1, 0, 0,
		  0, 0, 0, 1},	// 2:�E����W�n,Z-UP
		{ 1, 0, 0, 0,
		  0, 0, 1, 0,
		  0, 1, 0, 0,
		  0, 0, 0, 1},	// 1:������W�n,Z-UP
	};
	int CstNo;


public:
	InstanceSkinnedMesh(const char* fbx_filename, int draw_amount, int cstNo = 0, const bool triangulate = false);

	virtual ~InstanceSkinnedMesh() = default;

	// ���b�V���̎��o��
	void Fetch_Meshes(FbxScene* fbx_scene, std::vector<Mesh>& meshes);
	// �}�e���A���̎��o��
	void Fetch_Materials(FbxScene* fbx_scene, std::unordered_map<uint64_t, Material>& materials);
	// ���[���h�s��̌v�Z
	DirectX::SimpleMath::Matrix CulcWorldMatrix(const float scale_factor);
	// ���[���h�ʒu���]�̕ۑ�
	void addWorldData();
	// �F���w�肷�邽�߂̊֐�
	void setColor(int no, DirectX::XMFLOAT4 color) { worldData.at(no).color = color; };

	void Render(ID3D11DeviceContext* device_context, UINT drawInstance);	// �f�t�H���g�ŃV�F�[�_�������Ă���̂Ŗ��w��ł���

	// param��ҏW����imgui�E�B���h�E
	void imguiWindow(const char* beginname = "Instance_Skinned_Mesh");

	// �Z�b�^�[
	void setPos(DirectX::SimpleMath::Vector3 pos) { Parameters->Position = pos; }
	void setScale(DirectX::SimpleMath::Vector3 Size) { Parameters->Scale = Size; }
	void setOrientation(DirectX::SimpleMath::Quaternion orientation) { Parameters->Orientation = orientation; }
	void setColor(DirectX::SimpleMath::Vector4 color) { Parameters->Color = color; }

	void setPos(float posX, float posY, float posZ) { Parameters->Position = DirectX::SimpleMath::Vector3(posX, posY, posZ); }
	void setScale(float sizeX, float sizeY, float sizeZ) { Parameters->Scale = DirectX::SimpleMath::Vector3(sizeX, sizeY, sizeZ); }
	void setOrientation(float angleX, float angleY, float angleZ) { Parameters->Orientation = DirectX::SimpleMath::Quaternion(angleX, angleY, angleZ, Parameters->Orientation.w); }
	void setColor(float r, float g, float b, float a) { Parameters->Color = DirectX::SimpleMath::Vector4(r, g, b, a); }

	// �Q�b�^�[
	DirectX::SimpleMath::Vector3 getPos() { return Parameters->Position; }
	DirectX::SimpleMath::Vector3 getSize() { return Parameters->Scale; }
	DirectX::SimpleMath::Quaternion getAngle() { return Parameters->Orientation; }
	DirectX::SimpleMath::Vector4 getColor() { return Parameters->Color; }
	DirectX::SimpleMath::Matrix getWorld() { return world; }
	Object3d* getParameters() { return Parameters.get(); }

private:
	void Create_com_buffers(const char* fbx_filename);
protected:
	scene scene_view;
};