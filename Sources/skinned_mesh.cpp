
#include "misc.h"
#include "skinned_mesh.h"
#include <sstream>
#include <functional>
using namespace DirectX;

Skinned_Mesh::Skinned_Mesh(ID3D11Device* device, const char* fbx_filename, bool triangulate) {
	FbxManager* fbx_manager{ FbxManager::Create() };	// マネージャの生成
	FbxScene* fbx_scene{ FbxScene::Create(fbx_manager,"") };	// sceneにfbx内ファイル内の情報を流し込む

	FbxImporter* fbx_importer{ FbxImporter::Create(fbx_manager,"") };	// インポーターの生成
	bool import_status{ false };
	import_status = fbx_importer->Initialize(fbx_filename);
	_ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

	import_status = fbx_importer->Import(fbx_scene);
	_ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

	FbxGeometryConverter fbx_conoverter(fbx_manager);
	if (triangulate) {
		fbx_conoverter.Triangulate(fbx_scene, true, false);	// 四角ポリゴンを三角ポリゴンに変換 作り直しなので非常に重い
		fbx_conoverter.RemoveBadPolygonsFromMeshes(fbx_scene);
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

#if 1
	for (const scene::node& node : scene_view.nodes) {
		FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };
		// ノードデータをデバッグとして出力ウィンドウに表示する
		std::string node_name = fbx_node->GetName();
		uint64_t uid = fbx_node->GetUniqueID();
		uint64_t parent_uid = fbx_node->GetParent() ? fbx_node->GetParent()->GetUniqueID() : 0;	// 親が存在していれば番号を取得
		int32_t type = fbx_node->GetNodeAttribute() ? fbx_node->GetNodeAttribute()->GetAttributeType() : 0;

		std::stringstream debug_string;
		debug_string << node_name << ":" << uid << ":" << parent_uid << ":" << type << "\n";
		OutputDebugStringA(debug_string.str().c_str());
	}
#endif
	fbx_manager->Destroy();
}