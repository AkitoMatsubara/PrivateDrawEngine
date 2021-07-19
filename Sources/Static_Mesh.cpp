#include "Static_Mesh.h"
#include "shader.h"
#include "texture.h"
#include <fstream>
#include <filesystem>
using namespace std;

Static_Mesh::Static_Mesh(ID3D11Device* device, const wchar_t* obj_filename, const char* vs_cso_name, const char* ps_cso_name) {
	{
		vector<Vertex> vertices;
		vector<uint32_t> indices;
		uint32_t current_index{ 0 };

		vector<XMFLOAT3>positions;
		vector<XMFLOAT3>normals;
		vector<XMFLOAT2>texcoords;
		vector<wstring>mtl_filenames;

		std::wifstream fin(obj_filename);	// 読み取り操作のできるファイルストリーム
		_ASSERT_EXPR(fin, L"OBJ file not found.");
		wchar_t command[256];
		while (fin)
		{
			fin >> command;
			if (0 == wcscmp(command, L"v"))
			{
				float x, y, z;
				fin >> x >> y >> z;
				positions.push_back({ x, y, z });	// 書き取り 頂点情報を保存する
				fin.ignore(1024, L'\n');	// 改行までスキップする
			}
			else if (0 == wcscmp(command, L"vn"))	// "v"をファイル内に見つけたら
			{
				FLOAT i, j, k;
				fin >> i >> j >> k;
				normals.push_back({ i, j, k });	// 書き取り 法線情報を保存する
				fin.ignore(1024, L'\n');
			}
			else if (0 == wcscmp(command, L"vt"))
			{
				float u, v;
				fin >> u >> v;
				texcoords.push_back({ u, 1.0f - v });	// UV座標の保存
				fin.ignore(1024, L'\n');
			}
			else if (0 == wcscmp(command, L"f"))
			{
				for (size_t i = 0; i < 3; i++)
				{
					Vertex vertex;
					size_t v, vt, vn;
					fin >> v;
					vertex.position = positions.at(v - 1);
					if (L'/' == fin.peek())
					{
						fin.ignore(1);
						if (L'/' != fin.peek())
						{
							fin >> vt;
							vertex.texcoord = texcoords.at(vt - 1);
						}
						if (L'/' == fin.peek())
						{
							fin.ignore(1);
							fin >> vn;
							vertex.normal = normals.at(vn - 1);
						}
					}
					vertices.push_back(vertex);
					indices.push_back(current_index++);
				}
				fin.ignore(1024, L'\n');
			}
			else if (0 == wcscmp(command, L"mtllib"))
			{
				wchar_t mtllib[256];
				fin >> mtllib;
				mtl_filenames.push_back(mtllib);
			}
			else
			{
				fin.ignore(1024, L'\n');
			}
		}
		fin.close();
		// マテリアルの読み込み
		{
			std::filesystem::path mtl_filename(obj_filename);	// ファイルのパスを扱うクラス
			mtl_filename.replace_filename(std::filesystem::path(mtl_filenames[0]).filename());	// パスに含まれるファイル名を置き換える
			fin.open(mtl_filename);
			_ASSERT_EXPR(fin, L"'MTL file not found.");

			while (fin)
			{
				fin >> command;
				if (0 == wcscmp(command, L"map_Kd"))
				{
					fin.ignore();
					wchar_t map_Kd[256];
					fin >> map_Kd;

					std::filesystem::path path(obj_filename);
					path.replace_filename(std::filesystem::path(map_Kd).filename());
					texture_filename = path;
					fin.ignore(1024, L'\n');
				}
				else
				{
					fin.ignore(1024, L'\n');
				}
			}
			fin.close();
		}
		Create_com_buffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());
	}
	HRESULT hr{ S_OK };

	D3D11_INPUT_ELEMENT_DESC input_element_desc[]{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL"	, 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// テクスチャのロード
	D3D11_TEXTURE2D_DESC texture2d_desc{};
	load_texture_from_file(device, texture_filename.c_str(), shader_resource_view.GetAddressOf(), &texture2d_desc);

	// シェーダ作成
	create_vs_from_cso(device, vs_cso_name, vertex_shader.GetAddressOf(), input_layout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
	create_ps_from_cso(device, ps_cso_name, pixel_shader.GetAddressOf());

	// コンスタントバッファ作成
	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(Constants);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// ラスタライザオブジェクトの生成
	D3D11_RASTERIZER_DESC rasterizer_desc{};
	/*-----塗りつぶし 前面描画-----*/
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;	// レンダリングに使う塗りつぶしモード D3D11_FILL_SOLID|D3D11_FILL_WIREFRAME
	rasterizer_desc.CullMode = D3D11_CULL_BACK;	    // 描画する法線方向 D3D11_CULL_NONE(両面描画)|D3D11_CULL_FRONT(後面描画)|D3D11_CULL_BACK(前面描画)
	rasterizer_desc.FrontCounterClockwise = FALSE;	// 三角形が前面か背面かを決定する TRUEの時、頂点が反対周りだと前向きとみなされる
	rasterizer_desc.DepthBias = 0;					// 深度バイアス 同一深度上に表示するときに優先度を決めるのに使用したりする
	rasterizer_desc.DepthBiasClamp = 0;			    // 上記同様     ピクセルの最大深度バイアス
	rasterizer_desc.SlopeScaledDepthBias = 0;		// 上記同様     特定のピクセルの傾きのスカラー
	rasterizer_desc.DepthClipEnable = TRUE;		    // 距離に基づいてクリッピングを有効にするか
	rasterizer_desc.ScissorEnable = FALSE;			// シザー矩形カリングを使用するか シザー矩形：描画領域の指定によく使われる
	rasterizer_desc.MultisampleEnable = FALSE;		// マルチサンプリングアンチエイリアス(MSAA)のRTVを使用している時、tureで四辺形ラインアンチエイリアス、falseでアルファラインアンチエイリアスを使用
													// MSAAを使用するにはリソース生成時にDX11_SAMPLE_DESC::Countを1より上の値を設定する必要がある
	rasterizer_desc.AntialiasedLineEnable = FALSE;	// MSAAのRTVを使用している時、線分描画でMultisampleEnableがfalseの時にアンチエイリアスを有効にする
	hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	/*-----塗り潰し 前面描画-----*/
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_NONE;
	rasterizer_desc.AntialiasedLineEnable = TRUE;
	hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states[1].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	/*-----ワイヤーフレーム 前面描画-----*/
	rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	rasterizer_desc.AntialiasedLineEnable = TRUE;
	hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states[2].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	/*-----ワイヤーフレーム 両面描画-----*/
	rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizer_desc.CullMode = D3D11_CULL_NONE;
	rasterizer_desc.AntialiasedLineEnable = TRUE;
	hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states[3].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// 各種パラメータの初期化
	param.Pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	param.Size = XMFLOAT3(1.0f, 1.0f, 1.0f);
	param.Angle = XMFLOAT3(0.0f, 0.0f, 0.0f);
	param.Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
}

void Static_Mesh::Create_com_buffers(ID3D11Device* device, Vertex* vertices, size_t vertex_count, uint32_t* indices, size_t index_count) {
	HRESULT hr{ S_OK };

	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * vertex_count);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subresource_data;
	subresource_data.pSysMem = vertices;	// どの情報で初期化するか
	subresource_data.SysMemPitch = 0;
	subresource_data.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * index_count);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	subresource_data.pSysMem = indices;		// どの情報で初期化するか

	hr = device->CreateBuffer(&buffer_desc, &subresource_data, index_buffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void Static_Mesh::Render(ID3D11DeviceContext* immediate_context, const XMFLOAT4X4& world, const XMFLOAT4& material_color, bool WireFrame) {
	uint32_t stride{ sizeof(Vertex) };
	uint32_t offset{ 0 };

	// 頂点バッファのバインド
	immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);

	// インデックスバッファのバインド
	immediate_context->IASetIndexBuffer(index_buffer.Get(),		// インデックスを格納したオブジェクトのポインタ
		DXGI_FORMAT_R32_UINT,		// インデックスバッファ内のデータのフォーマット(16bitか32bitのどちらか)
		0);							// オフセット

	//プリミティブタイプ及びデータの順序に関する情報のバインド
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 入力レイアウトオブジェクトのバインド
	immediate_context->IASetInputLayout(input_layout.Get());

	// シェーダのバインド
	immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
	immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

	Constants data{ world,material_color };
	// メモリからマップ不可能なメモリに作成されたサブリソースにデータをコピー
	immediate_context->UpdateSubresource(constant_buffer.Get(),	// 宛先リソースへのポインタ
		0,	// 宛先サブリソースを識別するインデックス
		0, &data, 0, 0);

	// 定数バッファの設定
	immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

	// ラスタライザステートの設定
	wireframe = WireFrame;
	immediate_context->RSSetState(rasterizer_states[wireframe].Get());

	D3D11_BUFFER_DESC buffer_desc{};
	index_buffer->GetDesc(&buffer_desc);
	immediate_context->DrawIndexed(buffer_desc.ByteWidth / sizeof(uint32_t), 0, 0);	// 描画するインデックスの数,最初のインデックスの場所,頂点バッファから読み取る前に追加する値
}

void Static_Mesh::Render(ID3D11DeviceContext* immediate_context) {
	XMMATRIX S{ XMMatrixScaling(param.Size.x,param.Size.y,param.Size.z) };				// 拡縮
	XMMATRIX R{ XMMatrixRotationRollPitchYaw(XMConvertToRadians(param.Angle.x),XMConvertToRadians(param.Angle.y),XMConvertToRadians(param.Angle.z)) };	// 回転
	XMMATRIX T{ XMMatrixTranslation(param.Pos.x,param.Pos.y,param.Pos.z) };					// 平行移動

	XMFLOAT4X4 world;
	XMStoreFloat4x4(&world, S * R * T);	// ワールド変換行列作成

	uint32_t stride{ sizeof(Vertex) };
	uint32_t offset{ 0 };

	// SRVバインド
	immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());	// レジスタ番号、シェーダリソースの数、SRVのポインタ

	// 頂点バッファのバインド
	immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);

	// インデックスバッファのバインド
	immediate_context->IASetIndexBuffer(
		index_buffer.Get(),			// インデックスを格納したオブジェクトのポインタ
		DXGI_FORMAT_R32_UINT,		// インデックスバッファ内のデータのフォーマット(16bitか32bitのどちらか)
		0);							// オフセット

	//プリミティブタイプ及びデータの順序に関する情報のバインド
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 入力レイアウトオブジェクトのバインド
	immediate_context->IASetInputLayout(input_layout.Get());

	// シェーダのバインド
	immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
	immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

	Constants data{ world,param.Color };
	// メモリからマップ不可能なメモリに作成されたサブリソースにデータをコピー
	immediate_context->UpdateSubresource(constant_buffer.Get(),	// 宛先リソースへのポインタ
		0,	// 宛先サブリソースを識別するインデックス
		0, &data, 0, 0);

	// 定数バッファの設定
	immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

	// ラスタライザステートの設定
	immediate_context->RSSetState(rasterizer_states[wireframe].Get());

	D3D11_BUFFER_DESC buffer_desc{};
	index_buffer->GetDesc(&buffer_desc);
	immediate_context->DrawIndexed(buffer_desc.ByteWidth / sizeof(uint32_t), 0, 0);	// 描画するインデックスの数,最初のインデックスの場所,頂点バッファから読み取る前に追加する値
}

void Static_Mesh::imguiWindow(const char* beginname) {
	float pos[3]{ param.Pos.x ,param.Pos.y ,param.Pos.z };
	float size[3]{ param.Size.x ,param.Size.y ,param.Size.z };
	float angle[3]{ param.Angle.x,param.Angle.y,param.Angle.z };
	float Color[4]{ param.Color.x ,param.Color.y,param.Color.z,param.Color.w };

	ImGui::Begin(beginname);	// 識別ID 同じIDだと一緒のウィンドウにまとめられる

	ImGui::SliderFloat3(u8"Position", pos, -5, 5);
	ImGui::SliderFloat3(u8"Size", size, 0, 5);
	ImGui::SliderFloat3(u8"angle", angle, -360, 360);
	ImGui::ColorEdit4(u8"Color", (float*)&Color);
	ImGui::Checkbox(u8"WireFrame", &wireframe);

	ImGui::End();
	setPos(DirectX::XMFLOAT3(pos[0], pos[1], pos[2]));
	setSize(DirectX::XMFLOAT3(size[0], size[1], size[2]));
	setAngle(DirectX::XMFLOAT3(angle[0], angle[1], angle[2]));
	setColor(DirectX::XMFLOAT4(Color[0], Color[1], Color[2], Color[3]));
}