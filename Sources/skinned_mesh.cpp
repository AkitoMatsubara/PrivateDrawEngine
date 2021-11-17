#include <sstream>
#include <functional>
#include <filesystem>
#include <fstream>
#include <SimpleMath.h>

#include "framework.h"

#include "misc.h"
#include "shader.h"
#include "texture.h"
#include "skinned_mesh.h"

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Skinned_Mesh::dummyTexture;

inline DirectX::SimpleMath::Matrix ConvertToXmfloat4x4(const FbxAMatrix& fbxamatrix) {
	DirectX::SimpleMath::Matrix value;
	// 2�dfor�� 4x4�������
	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			value.m[row][column] = static_cast<float>(fbxamatrix[row][column]);
		}
	}
	return value;
}

inline DirectX::SimpleMath::Vector3 ConvertToXmfloat3(const FbxDouble3& fbxdouble3) {
	DirectX::SimpleMath::Vector3 value;
	value.x = static_cast<float>(fbxdouble3[0]);
	value.y = static_cast<float>(fbxdouble3[1]);
	value.z = static_cast<float>(fbxdouble3[2]);
	return value;
}

inline DirectX::SimpleMath::Vector4 ConvertToXmfloat4(const FbxDouble4& fbxdouble4) {
	DirectX::SimpleMath::Vector4 value;
	value.x = static_cast<float>(fbxdouble4[0]);
	value.y = static_cast<float>(fbxdouble4[1]);
	value.z = static_cast<float>(fbxdouble4[2]);
	value.w = static_cast<float>(fbxdouble4[3]);
	return value;
}

Skinned_Mesh::Skinned_Mesh(const char* fbx_filename, int cstNo, bool triangulate) {
	ID3D11Device* device = FRAMEWORK->GetDevice();

	FbxManager* fbx_manager{ FbxManager::Create() };	// �}�l�[�W���̐���

	// �����t�@�C����.fbx�g���q��.cereal�ɕϊ��A�t�@�C����.cereal�����݂��Ă���ꍇ��.cereal�����[�h�A���݂��Ȃ��ꍇ�ɂ�.fbx�����[�h����
	// ���O�Ńt�@�C���L���𔻒f������̂ŁA���g������Ă������t�@�C���ł���ΐV�K�쐬�͂���Ȃ��̂Ŏ蓮�ō폜����K�v������
	std::filesystem::path cereal_filename(fbx_filename);
	cereal_filename.replace_extension("cereal");

	//����.cereal���������ꍇ�͂����ǂݍ���
	if (std::filesystem::exists(cereal_filename.c_str()))
	{
		std::ifstream ifs(cereal_filename.c_str(), std::ios::binary);
		cereal::BinaryInputArchive deserialization(ifs);
		deserialization(scene_view, meshes, materials);
	}
	// �Ȃ����fbx��ǂ݂��݁A�V���A���C�Y������
	else
	{
		FbxScene* fbx_scene{ FbxScene::Create(fbx_manager,"") };	// scene��fbx���t�@�C�����̏��𗬂�����

		FbxImporter* fbx_importer{ FbxImporter::Create(fbx_manager,"") };	// �C���|�[�^�[�̐���
		bool import_status{ false };
		import_status = fbx_importer->Initialize(fbx_filename);
		_ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

		import_status = fbx_importer->Import(fbx_scene);
		_ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

		FbxGeometryConverter fbx_converter(fbx_manager);
		if (triangulate) {
			fbx_converter.Triangulate(fbx_scene, true, false);	// �l�p�|���S�����O�p�|���S���ɕϊ� ��蒼���Ȃ̂Ŕ��ɏd��
			fbx_converter.RemoveBadPolygonsFromMeshes(fbx_scene);
		}

		// �֐��쐬
		std::function<void(FbxNode*)> traverse{ [&](FbxNode* fbx_node) {
			scene::node& node{scene_view.nodes.emplace_back()};
			// ������fbx_node�̏����R�s�[
			node.attribute = fbx_node->GetNodeAttribute() ? fbx_node->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::EType::eUnknown;
			node.name = fbx_node->GetName();
			node.unique_id = fbx_node->GetUniqueID();
			node.parent_index = scene_view.indexof(fbx_node->GetParent() ? fbx_node->GetParent()->GetUniqueID() : 0);	// �e�����݂��Ă���Δԍ����擾
			for (int child_index = 0; child_index < fbx_node->GetChildCount(); ++child_index) {
				traverse(fbx_node->GetChild(child_index));
			}
		}
		};
		traverse(fbx_scene->GetRootNode());

		Fetch_Meshes(fbx_scene, meshes);
		Fetch_Materials(fbx_scene, materials);

#if 0
		for (const scene::node& node : scene_view.nodes) {
			FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };
			// �m�[�h�f�[�^���f�o�b�O�Ƃ��ďo�̓E�B���h�E�ɕ\������
			std::string node_name = fbx_node->GetName();
			uint64_t uid = fbx_node->GetUniqueID();
			uint64_t parent_uid = fbx_node->GetParent() ? fbx_node->GetParent()->GetUniqueID() : 0;	// �e�����݂��Ă���Δԍ����擾
			int32_t type = fbx_node->GetNodeAttribute() ? fbx_node->GetNodeAttribute()->GetAttributeType() : 0;

			// ���A�E�g�v�b�g�p�f�o�b�O
			std::stringstream debug_string;
			debug_string << node_name << ":" << uid << ":" << parent_uid << ":" << type << "\n";
			OutputDebugStringA(debug_string.str().c_str());
		}
#endif
		fbx_manager->Destroy();

		// std::ofstream ofs(�t�@�C����,�I�[�v�����[�h)
		std::ofstream ofs(cereal_filename.c_str(), std::ios::binary);
		cereal::BinaryOutputArchive serialization(ofs);
		serialization(scene_view, meshes, materials);
	}
	// �}�e���A����񂪂Ȃ��ꍇ�ɔ����\�߃_�~�[�e�N�X�`�����Z�b�g
	make_dummy_texture(dummyTexture.GetAddressOf(), 0xFFFFFFFF, 16);

	Create_com_buffers(fbx_filename);

	rasterizer.SetRasterizer(device);
	CstNo = cstNo;
	// �e��p�����[�^�̏�����
	Parameters = std::make_unique<Object3d>();
	Parameters->Position = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
	Parameters->Scale = DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f);
	Parameters->Rotate = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
	Parameters->Color = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
}

void Skinned_Mesh::Render(Shader* shader, int rasterize) {
	ID3D11DeviceContext* immediate_context = FRAMEWORK->GetDeviceContext();

	rasterize = (rasterize == Rasterizer::RASTERIZER_STATE::SOLID_NONE) ? wireframe : rasterize;	// ���X�^���C�U�̎w����@ �f�t�H���g(���ʕ`��)�ȊO�̎w��Ȃ烏�C���[�t���[���؂�ւ��𖳌��ɂ���

	for (const Mesh& mesh : meshes) {
		// �P�ʂ��Z���`���[�g�����烁�[�g���ɕύX���邽�߁Ascale_factor��0.01�ɐݒ肷��
		const float scale_factor = 1.0f;
		DirectX::XMMATRIX C{ XMLoadFloat4x4(&coordinate_system_transforms[CstNo]) * DirectX::XMMatrixScaling(scale_factor,scale_factor,scale_factor) };

		DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(Parameters->Scale.x,Parameters->Scale.y,Parameters->Scale.z) };	// �g�k
		DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(Parameters->Rotate.x), DirectX::XMConvertToRadians(Parameters->Rotate.y), DirectX::XMConvertToRadians(Parameters->Rotate.z)) };	// ��]
		DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(Parameters->Position.x,Parameters->Position.y,Parameters->Position.z) };	// ���s�ړ�

		DirectX::SimpleMath::Matrix world;
		XMStoreFloat4x4(&world, C * S * R * T);	// ���[���h�ϊ��s��쐬

		uint32_t stride{ sizeof(Vertex) };	// stride:���ݕ�
		uint32_t offset{ 0 };
		immediate_context->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		immediate_context->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//immediate_context->IASetInputLayout(input_layout.Get());

		//immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
		//immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

		// �V�F�[�_�̐ݒ�
		shader->Activate();
		// ���X�^���C�U�X�e�[�g�̐ݒ�
		immediate_context->RSSetState(rasterizer.states[rasterize].Get());

		Constants data{ world,Parameters->Color };
		DirectX::XMStoreFloat4x4(&data.world, DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&mesh.default_global_transform), DirectX::XMLoadFloat4x4(&world)));	// �O���[�o����Transform��world�s����|����world���W�ɕϊ����Ă���

		for (const Mesh::Subset& subset : mesh.subsets) {	// �}�e���A���ʃ��b�V���̐��񂷂�
			if (subset.material_unique_id != 0)	// unique_id�̊m�F
			{
				const Material& material = materials.at(subset.material_unique_id);
				if (materials.size() > 0)	// �}�e���A����񂪂��邩�m�F
				{
					immediate_context->PSSetShaderResources(0, 1, material.srv[0].GetAddressOf());
					XMStoreFloat4(&data.material_color, DirectX::XMVectorMultiply(DirectX::XMLoadFloat4(&Parameters->Color), DirectX::XMLoadFloat4(&material.Kd)));	// �}�e���A���ƃJ���[������
				}
			}
			else
			{
				immediate_context->PSSetShaderResources(0, 1, dummyTexture.GetAddressOf());	// �_�~�[�e�N�X�`�����g�p����
			}

			immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());
			immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
			immediate_context->DrawIndexed(subset.index_count, subset.start_index_location, 0);	// �`�悷��C���f�b�N�X�̐�,�ŏ��̃C���f�b�N�X�̏ꏊ,���_�o�b�t�@����ǂݎ��O�ɒǉ�����l
		}
	}
	// �V�F�[�_�̖�����
	shader->Inactivate();
}

void Skinned_Mesh::Create_com_buffers(const char* fbx_filename) {
	ID3D11Device* device = FRAMEWORK->GetDevice();

	HRESULT hr{ S_OK };

	for (Mesh& mesh : meshes) {
		D3D11_BUFFER_DESC buffer_desc{};
		buffer_desc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * mesh.vertices.size());
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA subresource_data;
		subresource_data.pSysMem = mesh.vertices.data();	// �ǂ̏��ŏ��������邩
		subresource_data.SysMemPitch = 0;
		subresource_data.SysMemSlicePitch = 0;

		hr = device->CreateBuffer(&buffer_desc, &subresource_data, mesh.vertex_buffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * mesh.indices.size());
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		subresource_data.pSysMem = mesh.indices.data();		// �ǂ̏��ŏ��������邩

		hr = device->CreateBuffer(&buffer_desc, &subresource_data, mesh.index_buffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
#if 1
		mesh.vertices.clear();
		mesh.indices.clear();
#endif
	}

	// �}�e���A�����ɉ����ăe�N�X�`�����Z�b�g
	for (std::unordered_map<uint64_t, Material>::iterator iterator = materials.begin(); iterator != materials.end(); ++iterator) {
		if (iterator->second.texture_filenames[0].size() > 0) {	// second�͒l�ɃA�N�Z�X���邽�߂Ɏg�p����
			std::filesystem::path path(fbx_filename);
			path.replace_filename(iterator->second.texture_filenames[0]);
			D3D11_TEXTURE2D_DESC texture2d_desc;
			load_texture_from_file(path.c_str(), iterator->second.srv[0].GetAddressOf(), &texture2d_desc);
		}
		else {
			make_dummy_texture(iterator->second.srv[0].GetAddressOf(), 0xFFFFFFFF, 16);
		}
	}

	D3D11_BUFFER_DESC buffer_desc{};
	ZeroMemory(&buffer_desc, sizeof(D3D11_BUFFER_DESC));
	buffer_desc.ByteWidth = sizeof(Constants);	// Constants�̌^���g�p
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// ConstantBuffer�Ƃ��Ďg�p���邱�Ƃ����߂�
	hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void Skinned_Mesh::Fetch_Meshes(FbxScene* fbx_scene, std::vector<Mesh>& meshes) {
	for (const scene::node& node : scene_view.nodes) {
		if (node.attribute != FbxNodeAttribute::EType::eMesh) {	// Mesh��������Ȃ�������
			continue;
		}

		FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };	//�m�[�h���擾
		FbxMesh* fbx_mesh{ fbx_node->GetMesh() };	// fbx_node�̃��b�V�����擾

		Mesh& mesh{ meshes.emplace_back() };
		mesh.unique_id = fbx_mesh->GetNode()->GetUniqueID();
		mesh.name = fbx_mesh->GetName();
		mesh.node_index = scene_view.indexof(mesh.unique_id);
		mesh.default_global_transform = ConvertToXmfloat4x4(fbx_mesh->GetNode()->EvaluateGlobalTransform());

		std::vector <Mesh::Subset>& subsets{ mesh.subsets };
		const int material_count{ fbx_mesh->GetNode()->GetMaterialCount() };
		subsets.resize(material_count > 0 ? material_count : 1);
		for (int material_index = 0; material_index < material_count; ++material_index) {
			const FbxSurfaceMaterial* fbx_material{ fbx_mesh->GetNode()->GetMaterial(material_index) };
			subsets.at(material_index).material_name = fbx_material->GetName();
			subsets.at(material_index).material_unique_id = fbx_material->GetUniqueID();
		}
		if (material_count > 0) {
			const int polygon_count{ fbx_mesh->GetPolygonCount() };
			for (int polygon_index = 0; polygon_index < polygon_count; ++polygon_index) {
				const int material_index{ fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(polygon_index) };
				subsets.at(material_index).index_count += 3;
			}
			uint32_t offset{ 0 };
			for (Mesh::Subset& subset : subsets) {
				subset.start_index_location = offset;
				offset += subset.index_count;
				// ����͎��̎菇�ŃJ�E���^�Ƃ��Ďg�p����A�[���Ƀ��Z�b�g�����
				subset.index_count = 0;
			}
		}

		const int polygon_count{ fbx_mesh->GetPolygonCount() };
		mesh.vertices.resize(polygon_count * 3LL);
		mesh.indices.resize(polygon_count * 3LL);

		FbxStringList uv_names;
		fbx_mesh->GetUVSetNames(uv_names);
		const FbxVector4* control_points{ fbx_mesh->GetControlPoints() };
		for (int polygon_index = 0; polygon_index < polygon_count; ++polygon_index) {
			const int mterial_index{ material_count > 0 ? fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(polygon_index) : 0 };
			Mesh::Subset& subset{ subsets.at(mterial_index) };
			const uint32_t offset{ subset.start_index_location + subset.index_count };
			for (int position_in_polygon = 0; position_in_polygon < 3; ++position_in_polygon) {
				const int vertex_index{ polygon_index * 3 + position_in_polygon };

				Vertex vertex;
				const int polygon_vertex{ fbx_mesh->GetPolygonVertex(polygon_index, position_in_polygon) };
				vertex.position.x = static_cast<float>(control_points[polygon_vertex][0]);
				vertex.position.y = static_cast<float>(control_points[polygon_vertex][1]);
				vertex.position.z = static_cast<float>(control_points[polygon_vertex][2]);

				if (fbx_mesh->GetElementNormalCount() > 0) {
					FbxVector4 normal;
					fbx_mesh->GetPolygonVertexNormal(polygon_index, position_in_polygon, normal);
					vertex.normal.x = static_cast<float>(normal[0]);
					vertex.normal.y = static_cast<float>(normal[1]);
					vertex.normal.z = static_cast<float>(normal[2]);
				}
				if (fbx_mesh->GetElementUVCount() > 0) {
					FbxVector2 uv;
					bool unmapped_uv;
					fbx_mesh->GetPolygonVertexUV(polygon_index, position_in_polygon,
						uv_names[0], uv, unmapped_uv);
					vertex.texcoord.x = static_cast<float>(uv[0]);
					vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
				}

				mesh.vertices.at(vertex_index) = std::move(vertex);
				//mesh.indices.at(vertex_index) = vertex_index;
				mesh.indices.at(static_cast<size_t>(offset) + position_in_polygon) = vertex_index;
				subset.index_count++;
			}
		}
	}
}

void Skinned_Mesh::Fetch_Materials(FbxScene* fbx_scene, std::unordered_map<uint64_t, Material>& materials) {
	const size_t node_count{ scene_view.nodes.size() };	// �m�[�h�̃T�C�Y
	for (size_t node_index = 0; node_index < node_count; ++node_index) {
		const scene::node& node{ scene_view.nodes.at(node_index) };	// �w��ԍ��̃m�[�h�擾
		const FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };	// �m�[�h�������A���擾

		const int material_count{ fbx_node->GetMaterialCount() };	// �}�e���A�����̎擾
		for (int material_index = 0; material_index < material_count; ++material_index) {
			const FbxSurfaceMaterial* fbx_material{ fbx_node->GetMaterial(material_index) };	// FbxSurfaceMaterial �̎擾

			Material material;
			material.name = fbx_material->GetName();
			material.unique_id = fbx_material->GetUniqueID();
			FbxProperty fbx_property;
			// Kd�̎擾
			{
				fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sDiffuse);	// Diffuse�̎擾
				if (fbx_property.IsValid()) {	// �L�����ǂ����̃`�F�b�N
					const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
					material.Kd.x = static_cast<float>(color[0]);
					material.Kd.y = static_cast<float>(color[1]);
					material.Kd.z = static_cast<float>(color[2]);
					material.Kd.w = 1.0f;

					const FbxFileTexture* fbx_texture{ fbx_property.GetSrcObject<FbxFileTexture>() };	// �e�N�X�`�����̎擾
					material.texture_filenames[0] = fbx_texture ? fbx_texture->GetRelativeFileName() : "";	// �e�N�X�`�����̎擾
				}
			}
			// Ks�̎擾
			{
				fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sSpecular);	// Specular�̎擾
				if (fbx_property.IsValid()) {	// �L�����ǂ����̃`�F�b�N
					const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
					material.Ks.x = static_cast<float>(color[0]);
					material.Ks.y = static_cast<float>(color[1]);
					material.Ks.z = static_cast<float>(color[2]);
					material.Ks.w = 1.0f;
				}
			}
			// Ka�̎擾
			{
				fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sAmbient);	// Ambient�̎擾
				if (fbx_property.IsValid()) {	// �L�����ǂ����̃`�F�b�N
					const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
					material.Ka.x = static_cast<float>(color[0]);
					material.Ka.y = static_cast<float>(color[1]);
					material.Ka.z = static_cast<float>(color[2]);
					material.Ka.w = 1.0f;
				}
			}
			materials.emplace(material.unique_id, std::move(material));	// unique�Ԗڂ�material���i�[����
		}
	}
}

void Skinned_Mesh::imguiWindow(const char* beginname) {
	float pos[3]{ Parameters->Position.x ,Parameters->Position.y ,Parameters->Position.z };
	float size[3]{ Parameters->Scale.x ,Parameters->Scale.y ,Parameters->Scale.z };
	float angle[3]{ Parameters->Rotate.x,Parameters->Rotate.y,Parameters->Rotate.z };
	float Color[4]{ Parameters->Color.x ,Parameters->Color.y,Parameters->Color.z,Parameters->Color.w };

	ImGui::Begin(beginname);	// ����ID ����ID���ƈꏏ�̃E�B���h�E�ɂ܂Ƃ߂���

	ImGui::SliderFloat3(u8"Position", pos, -5, 5);
	ImGui::SliderFloat3(u8"Size", size, 0, 5);
	ImGui::SliderFloat3(u8"angle", angle, -360, 360);
	ImGui::ColorEdit4(u8"Color", (float*)&Color);
	ImGui::Checkbox(u8"WireFrame", &wireframe);

	ImGui::End();	// �E�B���h�E�I��
	// �p�����[�^���
	setPos(DirectX::SimpleMath::Vector3(pos[0], pos[1], pos[2]));
	setSize(DirectX::SimpleMath::Vector3(size[0], size[1], size[2]));
	setAngle(DirectX::SimpleMath::Vector3(angle[0], angle[1], angle[2]));
	setColor(DirectX::SimpleMath::Vector4(Color[0], Color[1], Color[2], Color[3]));
}