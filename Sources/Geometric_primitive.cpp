#include "framework.h"
#include "geometric_primitive.h"
#include "misc.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
std::unique_ptr<ShaderEx> Geometric_Primitive::GeometricShader = nullptr;

Geometric_Primitive::Geometric_Primitive(const WCHAR* vs_name, const WCHAR* ps_name) {
	ID3D11Device* device = FRAMEWORK->GetDevice();

	HRESULT hr{ S_OK };

	// シェーダ作成 基本デバッグくらいにしか使わないと思ってるので一つのシェーダ固定前提に作成、staticで一度だけロードするように
	if (!GeometricShader)
	{
		GeometricShader = std::make_unique<ShaderEx>();
		GeometricShader->CreateVS(vs_name);
		GeometricShader->CreatePS(ps_name);
	}
	// コンスタントバッファ作成
	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(Constants);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// ラスタライザオブジェクトの生成
	rasterizer.SetRasterizer(device);

	// 各種パラメータの初期化
	Parameters = std::make_unique<Object3d>();
	Parameters->Position = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
	Parameters->Scale = DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f);
	//Parameters->Rotate = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
	Parameters->Orientation = DirectX::SimpleMath::Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
	Parameters->Color = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
}

void Geometric_Primitive::Create_com_buffers(Vertex* vertices, size_t vertex_count, uint32_t* indices, size_t index_count) {
	ID3D11Device* device = FRAMEWORK->GetDevice();
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

void Geometric_Primitive::Render(bool wireframe) {
	ID3D11DeviceContext* immediate_context = FRAMEWORK->GetDeviceContext();
	uint32_t stride{ sizeof(Vertex) };
	uint32_t offset{ 0 };

	// 頂点バッファのバインド
	immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);

	// インデックスバッファのバインド
	immediate_context->IASetIndexBuffer(index_buffer.Get(),		// インデックスを格納したオブジェクトのポインタ
		DXGI_FORMAT_R32_UINT,	// インデックスバッファ内のデータのフォーマット(16bitか32bitのどちらか)
		0);				// オフセット

//プリミティブタイプ及びデータの順序に関する情報のバインド
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// シェーダの有効化
	GeometricShader->Activate();

	DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(Parameters->Scale.x,Parameters->Scale.y,Parameters->Scale.z) };	// 拡縮
	//DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(Parameters->Rotate.x), DirectX::XMConvertToRadians(Parameters->Rotate.y), DirectX::XMConvertToRadians(Parameters->Rotate.z)) };	// 回転
	DirectX::XMMATRIX R{ DirectX::XMMatrixRotationQuaternion(Parameters->Orientation) };	// 回転
	DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(Parameters->Position.x,Parameters->Position.y,Parameters->Position.z) };	// 平行移動

	XMStoreFloat4x4(&world, S * R * T);	// ワールド変換行列作成

	Constants data{ world,Parameters->Color };
	// メモリからマップ不可能なメモリに作成されたサブリソースにデータをコピー
	immediate_context->UpdateSubresource(constant_buffer.Get(),	// 宛先リソースへのポインタ
		0,														// 宛先サブリソースを識別するインデックス
		0, &data, 0, 0);

	// 定数バッファの設定
	immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

	// ラスタライザステートの設定
	immediate_context->RSSetState(rasterizer.states[wireframe].Get());


	D3D11_BUFFER_DESC buffer_desc{};
	index_buffer->GetDesc(&buffer_desc);
	immediate_context->DrawIndexed(buffer_desc.ByteWidth / sizeof(uint32_t), 0, 0);	// 描画するインデックスの数,最初のインデックスの場所,頂点バッファから読み取る前に追加する値

	// シェーダの無効化
	GeometricShader->Inactivate();
}

void Geometric_Primitive::imguiWindow(const char* beginname) {
	float pos[3]{ Parameters->Position.x ,Parameters->Position.y ,Parameters->Position.z };
	float size[3]{ Parameters->Scale.x ,Parameters->Scale.y ,Parameters->Scale.z };
	//float angle[3]{ Parameters->Rotate.x,Parameters->Rotate.y,Parameters->Rotate.z };
	float Color[4]{ Parameters->Color.x ,Parameters->Color.y,Parameters->Color.z,Parameters->Color.w };

	ImGui::Begin(beginname);	// 識別ID 同じIDだと一緒のウィンドウにまとめられる

	ImGui::SliderFloat3(u8"Position", pos, -5, 5);
	ImGui::SliderFloat3(u8"Size", size, 0, 5);
	//ImGui::SliderFloat3(u8"angle", angle, -360, 360);
	ImGui::ColorEdit4(u8"Color", (float*)&Color);

	ImGui::End();
	setPos(DirectX::SimpleMath::Vector3(pos[0], pos[1], pos[2]));
	setSize(DirectX::SimpleMath::Vector3(size[0], size[1], size[2]));
	//setAngle(DirectX::SimpleMath::Vector3(angle[0], angle[1], angle[2]));
	setColor(DirectX::SimpleMath::Vector4(Color[0], Color[1], Color[2], Color[3]));
}