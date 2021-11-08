
#include <sstream>
#include <WICTextureLoader.h>

#include "framework.h"
#include "shader.h"
#include "Sprite_Batch.h"
#include "texture.h"
#include "misc.h"


// 頂点バッファオブジェクトの生成
sprite_Batch::sprite_Batch(const wchar_t* filename, size_t max_sprites, const char* vs_cso_name, const char* ps_cso_name) :max_vertices(max_sprites * 6)
{
	ID3D11Device* device = FRAMEWORK->GetDevice();
	HRESULT hr{ S_OK };

	std::unique_ptr<Vertex[]> vertices{ std::make_unique<Vertex[]>(max_vertices) };

	// テクスチャのロード
	load_texture_from_file(filename, shader_resource_view.GetAddressOf(), &texture2d_desc);

	// ByteWidth,BindFlags,StructuerByteStrideは可変情報、その他情報はあまり変化することはない
	D3D11_BUFFER_DESC buffer_desc{};			                // バッファの使われ方を設定する構造体
	buffer_desc.ByteWidth = sizeof(Vertex) * (UINT)max_vertices;// バッファの大きさ
	buffer_desc.Usage = D3D11_USAGE_DYNAMIC;			        // バッファへの各項目でのアクセス許可を指定 現在はGPU（読み取り専用）とCPU（書き込み専用）の両方からアクセスできる設定
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	        // バインド方法 この設定頂点バッファやコンスタントバッファとして使用することを決める
	buffer_desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;        // リソースに対してのCPUアクセス方法 ０でアクセス不要の設定になる。現在は書き込み専用
	buffer_desc.MiscFlags = 0;							        // その他のオプション 不要なら０を指定  ちなみに...misc：miscellaneousの略称。「雑多な」という意味
	buffer_desc.StructureByteStride = 0;				        // 構造体のサイズ バッファを構造体として確保する場合に使用 ０指定でも動作する

	D3D11_SUBRESOURCE_DATA subresource_data{};			        // 作成するバッファの初期化データを保存する構造体
	subresource_data.pSysMem = vertices.get();			        // バッファを初期化するデータを指定 どの情報で初期化するか
	subresource_data.SysMemPitch = 0;					        // メモリのピッチ 2D or 3Dテクスチャの場合に使用する
	subresource_data.SysMemSlicePitch = 0;				        //	深度レベル	 同上

	hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer.GetAddressOf());		// 作成するバッファ情報、作成するバッファの初期化情報、作成したバッファを保存するポインタ
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));	// _ASSERT_EXPR：第一引数条件が満たされなければ第二引数のメッセージを表示する

	//	// 入力レイアウトオブジェクトの生成
	//D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	//{
	//	{
	//	"POSITION",						// セマンティクス名	HLSL側のシグネチャ(型や変数名の組み合わせの事？)の名前と一致させることで送信した頂点情報を受信することができる
	//	0,								// セマンティクス番号 同名でも識別できるように番号を割り当てる。番号を変更することでHLSLで別の情報だと認識できる
	//	DXGI_FORMAT_R32G32B32_FLOAT,	// フォーマット	R23G23B23は実質float3
	//	0,								// 入力スロット番号	入寮レイアウトをどの入力スロットに対して反映されるかを指定する
	//	D3D11_APPEND_ALIGNED_ELEMENT,	// 要素から先頭までのオフセット値	各データの配列先頭が何バイト離れているか。
	//									// D3D11_APPEND_ALIGNED_ELEMENTを指定でオフセット値を自動計算 手計算ならフォーマットサイズを加算していく
	//	D3D11_INPUT_PER_VERTEX_DATA,	// 入力データの種類	頂点データとインスタンスデータの２種類
	//	0								// 繰り返し回数(頂点データの時は０)	上記でインスタンスデータを設定した場合に意味を持つ
	//	},
	//	{"COLOR"   ,0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	//	{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT		,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	//};

	//// 頂点シェーダーオブジェクトの生成
	//create_vs_from_cso(vs_cso_name, vertex_shader.GetAddressOf(), input_layout.GetAddressOf(), input_element_desc, _countof(input_element_desc));

	//// ピクセルシェーダオブジェクトの生成
	//create_ps_from_cso(ps_cso_name, pixel_shader.GetAddressOf());

	param.Pos     = DirectX::XMFLOAT2(0.0f, 0.0f);
	param.Size    = DirectX::XMFLOAT2(texture2d_desc.Width, texture2d_desc.Height);
	param.TexPos  = DirectX::XMFLOAT2(0.0f, 0.0f);
	param.TexSize = DirectX::XMFLOAT2(texture2d_desc.Width, texture2d_desc.Height);
	param.Angle   = 0.0f;
	param.Color   = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
}

sprite_Batch::~sprite_Batch() {
	//vertex_shader->Release();
	//pixel_shader->Release();
	//input_layout->Release();
	//vertex_buffer->Release();
	//shader_resource_view->Release();

	rerease_all_textures();
}

void sprite_Batch::begin() {
	ID3D11DeviceContext* immediate_context = FRAMEWORK->GetDeviceContext();

	vertices.clear();
	// シェーダのバインド
	immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
	immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
	// SRVバインド
	immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());	// レジスタ番号、シェーダリソースの数、SRVのポインタ
}

void sprite_Batch::end() {
	ID3D11DeviceContext* immediate_context = FRAMEWORK->GetDeviceContext();

	// テクスチャ座標を頂点バッファにセットする
	HRESULT hr = { S_OK };
	D3D11_MAPPED_SUBRESOURCE mapped_subrecource{};
	// mapped_subrecourceをvertex_bufferにマッピング
	hr = immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subrecource);	// 動的な定数バッファーを Map して書き込むときは D3D11_MAP_WRITE_DISCARD を使用する
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	size_t vertex_count = vertices.size();
	_ASSERT_EXPR(max_vertices >= vertex_count, "Buffer overflow");
	Vertex* data{ reinterpret_cast<Vertex*>(mapped_subrecource.pData) };	// reinterpret_cast：ありえないような変換のときに使用する？
	if (data != nullptr) {	// 情報の上書き
		// verticesのデータをコピー
		const Vertex* p = vertices.data();
		memcpy_s(data, max_vertices * sizeof(Vertex), p, vertex_count * sizeof(Vertex));	// memcpy_s：Buffer間でのバイトのコピー(コピー先ポインタ、コピー先サイズ、コピー元ポインタ、コピー元サイズ)
	}
	immediate_context->Unmap(vertex_buffer.Get(), 0);	// マッピング解除 頂点バッファを上書きしたら必ず実行。Map&Unmapはセットで使用する

	// 頂点バッファのバインド
	UINT stride{ sizeof(Vertex) };
	UINT offset{ 0 };
	immediate_context->IASetVertexBuffers(
		0,				               // 入力スロットの開始番号
		1,				               // 頂点バッファの数
		vertex_buffer.GetAddressOf(),  // 頂点バッファの配列
		&stride,		               // １頂点のサイズの配列
		&offset);		               // 頂点バッファの開始位置をずらすオフセットの配列

	//プリミティブタイプ及びデータの順序に関する情報のバインド
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// プリミティブの形状を指定できる？
											// D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST は3頂点、D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIPは四頂点 連続三角形
	// 入力レイアウトオブジェクトのバインド
	immediate_context->IASetInputLayout(input_layout.Get());	// 入力レイアウトの設定

	// ラスタライザステートの設定
	immediate_context->RSSetState(rasterizer_states[0].Get());

	// プリミティブの描画
	immediate_context->Draw(static_cast<UINT>(vertex_count), 0);	// 頂点の数、描画開始時点で使う頂点バッファの最初のインデックス

}

void sprite_Batch::CreateVertexData(DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size, float angle, DirectX::XMFLOAT4 color, DirectX::XMFLOAT2 TexPos, DirectX::XMFLOAT2 TexSize) {
	ID3D11DeviceContext* immediate_context = FRAMEWORK->GetDeviceContext();

	// スクリーン(ビューポート)のサイズを取得する
	D3D11_VIEWPORT viewport{};
	UINT num_viewports{ 1 };
	immediate_context->RSGetViewports(&num_viewports, &viewport);

	// 引数から矩形の各頂点の位置を計算する
	/*		left_top	*----*	right_top			*/
	/*					|   /|						*/
	/*					|  / |						*/
	/*					| /  |						*/
	/*		left_bottom	*----*	right_bottom		*/

	DirectX::XMFLOAT3 left_top    { pos.x         ,pos.y          ,0 };	// 左上
	DirectX::XMFLOAT3 right_top   { pos.x + size.x,pos.y          ,0 };	// 右上
	DirectX::XMFLOAT3 left_bottom { pos.x         ,pos.y + size.y ,0 };	// 左下
	DirectX::XMFLOAT3 right_bottom{ pos.x + size.x,pos.y + size.y ,0 };	// 右下

	// 回転の中心を矩形の中心点に
	DirectX::XMFLOAT2 center{ 0,0 };
	center.x = pos.x + size.x * 0.5f;	// 位置-(大きさ/2)で頂点位置から半サイズ分動く=半分になる
	center.y = pos.y + size.y * 0.5f;
	// 頂点回転
	rotate(left_top    , center, angle);
	rotate(left_bottom , center, angle);
	rotate(right_top   , center, angle);
	rotate(right_bottom, center, angle);

	// スクリーン座標系からNDC(正規化デバイス座標)への座標変換を行う
	left_top     = ConvertToNDC(left_top    , viewport);	// 頂点位置、スクリーンの大きさ
	left_bottom  = ConvertToNDC(left_bottom , viewport);
	right_top    = ConvertToNDC(right_top   , viewport);
	right_bottom = ConvertToNDC(right_bottom, viewport);

	DirectX::XMFLOAT2 TexLeft_top    { (TexPos.x)             / texture2d_desc.Width , (TexPos.y)             / texture2d_desc.Height };
	DirectX::XMFLOAT2 TexRight_top   { (TexPos.x + TexSize.x) / texture2d_desc.Width , (TexPos.y)             / texture2d_desc.Height };
	DirectX::XMFLOAT2 TexLeft_bottom { (TexPos.x)             / texture2d_desc.Width , (TexPos.y + TexSize.y) / texture2d_desc.Height };
	DirectX::XMFLOAT2 TexRight_bottom{ (TexPos.x + TexSize.x) / texture2d_desc.Width , (TexPos.y + TexSize.y) / texture2d_desc.Height };


	// 計算結果で頂点バッファオブジェクトを更新する
	//				三角形を2つ作る				//
	/*				0	*----*1	 *4				*/
	/*					||||/   /|				*/
	/*					|||/   /||				*/
	/*					||/   /|||				*/
	/*				2	*	3*---*5				*/

	vertices.push_back({ { left_top.x    ,left_top.y,	 0 }, {0,0,1}, { TexLeft_top.x,		TexLeft_top.y	 }, { color.x,color.y,color.z,color.w } });	// 左上
	vertices.push_back({ { right_top.x   ,right_top.y,	 0 }, {0,0,1}, { TexRight_top.x,	TexRight_top.y	 }, { color.x,color.y,color.z,color.w } });	// 右上
	vertices.push_back({ { left_bottom.x ,left_bottom.y, 0 }, {0,0,1}, { TexLeft_top.x,		TexLeft_bottom.y }, { color.x,color.y,color.z,color.w } });	// 左下
	vertices.push_back({ { left_bottom.x ,left_bottom.y, 0 }, {0,0,1}, { TexLeft_top.x,		TexLeft_bottom.y }, { color.x,color.y,color.z,color.w } });	// 左下
	vertices.push_back({ { right_top.x   ,right_top.y,	 0 }, {0,0,1}, { TexRight_top.x,	TexRight_top.y	 }, { color.x,color.y,color.z,color.w } });	// 右上
	vertices.push_back({ { right_bottom.x,right_bottom.y,0 }, {0,0,1}, { TexRight_bottom.x,	TexRight_bottom.y}, { color.x,color.y,color.z,color.w } });	// 右下
}

void sprite_Batch::Render(DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size, float angle, DirectX::XMFLOAT4 color, DirectX::XMFLOAT2 sPos, DirectX::XMFLOAT2 sSize) {
	CreateVertexData(pos, size, angle, color, sPos, sSize);
}

void sprite_Batch::Render() {
	CreateVertexData(param.Pos, param.Size, param.Angle, param.Color, param.TexPos, param.TexSize);
}

void sprite_Batch::Render(DirectX::XMFLOAT2 Pos, DirectX::XMFLOAT2 Size) {
	CreateVertexData(Pos, Size, 0, param.Color, param.TexPos, param.TexSize);
}

DirectX::XMFLOAT3 sprite_Batch::ConvertToNDC(DirectX::XMFLOAT3 pos, D3D11_VIEWPORT viewport) {
	pos.x = (pos.x * 2 / viewport.Width) - 1.0f;	// x値を２倍、その後スクリーンサイズで割って１を引くと正規化される
	pos.y = 1.0f - (pos.y * 2.0f / viewport.Height);	// y値を２倍、スクリーンサイズで割ったもので１を引くと正規化	xと違うのはおそらく左手右手座標系の関係
	// 今回はsprite(画像)なのでz値は変更する必要なし
	return pos;
}