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
	// 2重for文 4x4だからね
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

	FbxManager* fbx_manager{ FbxManager::Create() };	// マネージャの生成

	// 引数ファイル名.fbx拡張子を.cerealに変換、ファイル名.cerealが存在している場合は.cerealをロード、存在しない場合には.fbxをロードする
	// 名前でファイル有無を判断をするので、中身が違っても同名ファイルであれば新規作成はされないので手動で削除する必要がある
	std::filesystem::path cereal_filename(fbx_filename);
	cereal_filename.replace_extension("cereal");

	//既に.cerealがあった場合はそれを読み込む
	if (std::filesystem::exists(cereal_filename.c_str()))
	{
		std::ifstream ifs(cereal_filename.c_str(), std::ios::binary);
		cereal::BinaryInputArchive deserialization(ifs);
		deserialization(scene_view, meshes, materials);
	}
	// なければfbxを読みこみ、シリアライズさせる
	else
	{
		FbxScene* fbx_scene{ FbxScene::Create(fbx_manager,"") };	// sceneにfbx内ファイル内の情報を流し込む

		FbxImporter* fbx_importer{ FbxImporter::Create(fbx_manager,"") };	// インポーターの生成
		bool import_status{ false };
		import_status = fbx_importer->Initialize(fbx_filename);
		_ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

		import_status = fbx_importer->Import(fbx_scene);
		_ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

		FbxGeometryConverter fbx_converter(fbx_manager);
		if (triangulate) {
			fbx_converter.Triangulate(fbx_scene, true, false);	// 四角ポリゴンを三角ポリゴンに変換 作り直しなので非常に重い
			fbx_converter.RemoveBadPolygonsFromMeshes(fbx_scene);
		}

		// 関数作成
		std::function<void(FbxNode*)> traverse{ [&](FbxNode* fbx_node) {
			scene::node& node{scene_view.nodes.emplace_back()};
			// 引数のfbx_nodeの情報をコピー
			node.attribute = fbx_node->GetNodeAttribute() ? fbx_node->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::EType::eUnknown;
			node.name = fbx_node->GetName();
			node.unique_id = fbx_node->GetUniqueID();
			node.parent_index = scene_view.indexof(fbx_node->GetParent() ? fbx_node->GetParent()->GetUniqueID() : 0);	// 親が存在していれば番号を取得
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
			// ノードデータをデバッグとして出力ウィンドウに表示する
			std::string node_name = fbx_node->GetName();
			uint64_t uid = fbx_node->GetUniqueID();
			uint64_t parent_uid = fbx_node->GetParent() ? fbx_node->GetParent()->GetUniqueID() : 0;	// 親が存在していれば番号を取得
			int32_t type = fbx_node->GetNodeAttribute() ? fbx_node->GetNodeAttribute()->GetAttributeType() : 0;

			// 情報アウトプット用デバッグ
			std::stringstream debug_string;
			debug_string << node_name << ":" << uid << ":" << parent_uid << ":" << type << "\n";
			OutputDebugStringA(debug_string.str().c_str());
		}
#endif
		fbx_manager->Destroy();

		// std::ofstream ofs(ファイル名,オープンモード)
		std::ofstream ofs(cereal_filename.c_str(), std::ios::binary);
		cereal::BinaryOutputArchive serialization(ofs);
		serialization(scene_view, meshes, materials);
	}
	// マテリアル情報がない場合に備え予めダミーテクスチャをセット
	make_dummy_texture(dummyTexture.GetAddressOf(), 0xFFFFFFFF, 16);

	Create_com_buffers(fbx_filename);

	rasterizer.SetRasterizer(device);
	CstNo = cstNo;
	// 各種パラメータの初期化
	Parameters = std::make_unique<Object3d>();
	Parameters->Position = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
	Parameters->Scale = DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f);
	Parameters->Rotate = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
	Parameters->Color = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
}

void Skinned_Mesh::Render(Shader* shader, int rasterize) {
	ID3D11DeviceContext* immediate_context = FRAMEWORK->GetDeviceContext();

	rasterize = (rasterize == Rasterizer::RASTERIZER_STATE::SOLID_NONE) ? wireframe : rasterize;	// ラスタライザの指定方法 デフォルト(両面描画)以外の指定ならワイヤーフレーム切り替えを無効にする

	for (const Mesh& mesh : meshes) {
		// 単位をセンチメートルからメートルに変更するため、scale_factorを0.01に設定する
		const float scale_factor = 1.0f;
		DirectX::XMMATRIX C{ XMLoadFloat4x4(&coordinate_system_transforms[CstNo]) * DirectX::XMMatrixScaling(scale_factor,scale_factor,scale_factor) };

		DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(Parameters->Scale.x,Parameters->Scale.y,Parameters->Scale.z) };	// 拡縮
		DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(Parameters->Rotate.x), DirectX::XMConvertToRadians(Parameters->Rotate.y), DirectX::XMConvertToRadians(Parameters->Rotate.z)) };	// 回転
		DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(Parameters->Position.x,Parameters->Position.y,Parameters->Position.z) };	// 平行移動

		DirectX::SimpleMath::Matrix world;
		XMStoreFloat4x4(&world, C * S * R * T);	// ワールド変換行列作成

		uint32_t stride{ sizeof(Vertex) };	// stride:刻み幅
		uint32_t offset{ 0 };
		immediate_context->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		immediate_context->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//immediate_context->IASetInputLayout(input_layout.Get());

		//immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
		//immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

		// シェーダの設定
		shader->Activate();
		// ラスタライザステートの設定
		immediate_context->RSSetState(rasterizer.states[rasterize].Get());

		Constants data{ world,Parameters->Color };
		DirectX::XMStoreFloat4x4(&data.world, DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&mesh.default_global_transform), DirectX::XMLoadFloat4x4(&world)));	// グローバルのTransformとworld行列を掛けてworld座標に変換している

		for (const Mesh::Subset& subset : mesh.subsets) {	// マテリアル別メッシュの数回すよ
			if (subset.material_unique_id != 0)	// unique_idの確認
			{
				const Material& material = materials.at(subset.material_unique_id);
				if (materials.size() > 0)	// マテリアル情報があるか確認
				{
					immediate_context->PSSetShaderResources(0, 1, material.srv[0].GetAddressOf());
					XMStoreFloat4(&data.material_color, DirectX::XMVectorMultiply(DirectX::XMLoadFloat4(&Parameters->Color), DirectX::XMLoadFloat4(&material.Kd)));	// マテリアルとカラーを合成
				}
			}
			else
			{
				immediate_context->PSSetShaderResources(0, 1, dummyTexture.GetAddressOf());	// ダミーテクスチャを使用する
			}

			immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());
			immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
			immediate_context->DrawIndexed(subset.index_count, subset.start_index_location, 0);	// 描画するインデックスの数,最初のインデックスの場所,頂点バッファから読み取る前に追加する値
		}
	}
	// シェーダの無効化
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
		subresource_data.pSysMem = mesh.vertices.data();	// どの情報で初期化するか
		subresource_data.SysMemPitch = 0;
		subresource_data.SysMemSlicePitch = 0;

		hr = device->CreateBuffer(&buffer_desc, &subresource_data, mesh.vertex_buffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * mesh.indices.size());
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		subresource_data.pSysMem = mesh.indices.data();		// どの情報で初期化するか

		hr = device->CreateBuffer(&buffer_desc, &subresource_data, mesh.index_buffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
#if 1
		mesh.vertices.clear();
		mesh.indices.clear();
#endif
	}

	// マテリアル数に応じてテクスチャをセット
	for (std::unordered_map<uint64_t, Material>::iterator iterator = materials.begin(); iterator != materials.end(); ++iterator) {
		if (iterator->second.texture_filenames[0].size() > 0) {	// secondは値にアクセスするために使用する
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
	buffer_desc.ByteWidth = sizeof(Constants);	// Constantsの型を使用
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// ConstantBufferとして使用することをきめる
	hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void Skinned_Mesh::Fetch_Meshes(FbxScene* fbx_scene, std::vector<Mesh>& meshes) {
	for (const scene::node& node : scene_view.nodes) {
		if (node.attribute != FbxNodeAttribute::EType::eMesh) {	// Mesh属性じゃなかったら
			continue;
		}

		FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };	//ノード名取得
		FbxMesh* fbx_mesh{ fbx_node->GetMesh() };	// fbx_nodeのメッシュを取得

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
				// これは次の手順でカウンタとして使用され、ゼロにリセットされる
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
	const size_t node_count{ scene_view.nodes.size() };	// ノードのサイズ
	for (size_t node_index = 0; node_index < node_count; ++node_index) {
		const scene::node& node{ scene_view.nodes.at(node_index) };	// 指定番号のノード取得
		const FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };	// ノードを検索、情報取得

		const int material_count{ fbx_node->GetMaterialCount() };	// マテリアル数の取得
		for (int material_index = 0; material_index < material_count; ++material_index) {
			const FbxSurfaceMaterial* fbx_material{ fbx_node->GetMaterial(material_index) };	// FbxSurfaceMaterial の取得

			Material material;
			material.name = fbx_material->GetName();
			material.unique_id = fbx_material->GetUniqueID();
			FbxProperty fbx_property;
			// Kdの取得
			{
				fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sDiffuse);	// Diffuseの取得
				if (fbx_property.IsValid()) {	// 有効かどうかのチェック
					const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
					material.Kd.x = static_cast<float>(color[0]);
					material.Kd.y = static_cast<float>(color[1]);
					material.Kd.z = static_cast<float>(color[2]);
					material.Kd.w = 1.0f;

					const FbxFileTexture* fbx_texture{ fbx_property.GetSrcObject<FbxFileTexture>() };	// テクスチャ情報の取得
					material.texture_filenames[0] = fbx_texture ? fbx_texture->GetRelativeFileName() : "";	// テクスチャ名の取得
				}
			}
			// Ksの取得
			{
				fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sSpecular);	// Specularの取得
				if (fbx_property.IsValid()) {	// 有効かどうかのチェック
					const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
					material.Ks.x = static_cast<float>(color[0]);
					material.Ks.y = static_cast<float>(color[1]);
					material.Ks.z = static_cast<float>(color[2]);
					material.Ks.w = 1.0f;
				}
			}
			// Kaの取得
			{
				fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sAmbient);	// Ambientの取得
				if (fbx_property.IsValid()) {	// 有効かどうかのチェック
					const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
					material.Ka.x = static_cast<float>(color[0]);
					material.Ka.y = static_cast<float>(color[1]);
					material.Ka.z = static_cast<float>(color[2]);
					material.Ka.w = 1.0f;
				}
			}
			materials.emplace(material.unique_id, std::move(material));	// unique番目にmaterialを格納する
		}
	}
}

void Skinned_Mesh::imguiWindow(const char* beginname) {
	float pos[3]{ Parameters->Position.x ,Parameters->Position.y ,Parameters->Position.z };
	float size[3]{ Parameters->Scale.x ,Parameters->Scale.y ,Parameters->Scale.z };
	float angle[3]{ Parameters->Rotate.x,Parameters->Rotate.y,Parameters->Rotate.z };
	float Color[4]{ Parameters->Color.x ,Parameters->Color.y,Parameters->Color.z,Parameters->Color.w };

	ImGui::Begin(beginname);	// 識別ID 同じIDだと一緒のウィンドウにまとめられる

	ImGui::SliderFloat3(u8"Position", pos, -5, 5);
	ImGui::SliderFloat3(u8"Size", size, 0, 5);
	ImGui::SliderFloat3(u8"angle", angle, -360, 360);
	ImGui::ColorEdit4(u8"Color", (float*)&Color);
	ImGui::Checkbox(u8"WireFrame", &wireframe);

	ImGui::End();	// ウィンドウ終了
	// パラメータ代入
	setPos(DirectX::SimpleMath::Vector3(pos[0], pos[1], pos[2]));
	setSize(DirectX::SimpleMath::Vector3(size[0], size[1], size[2]));
	setAngle(DirectX::SimpleMath::Vector3(angle[0], angle[1], angle[2]));
	setColor(DirectX::SimpleMath::Vector4(Color[0], Color[1], Color[2], Color[3]));
}