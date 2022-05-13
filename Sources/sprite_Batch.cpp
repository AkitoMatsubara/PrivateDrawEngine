
#include <sstream>
#include <WICTextureLoader.h>

#include "framework.h"
#include "shader.h"
#include "Sprite_Batch.h"
#include "texture.h"
#include "misc.h"


// 頂点バッファオブジェクトの生成
sprite_Batch::sprite_Batch(const wchar_t* filename, size_t max_sprites) :max_vertices(max_sprites * 6)
{
	ID3D11Device* device = FRAMEWORK->GetDevice();
	HRESULT hr{ S_OK };

	std::unique_ptr<Vertex[]> vertices{ std::make_unique<Vertex[]>(max_vertices) };

	// テクスチャのロード
	Texture::LoadTextureFromFile(filename, shader_resource_view.GetAddressOf(), &texture2d_desc);

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

	hr = device->CreateBuffer(&buffer_desc, &subresource_data, VertexBuffer.GetAddressOf());		// 作成するバッファ情報、作成するバッファの初期化情報、作成したバッファを保存するポインタ
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));	// _ASSERT_EXPR：第一引数条件が満たされなければ第二引数のメッセージを表示する

	if (!SpriteShader)	// 一回だけ生成
	{
		SpriteShader = std::make_unique<ShaderEx>();
		//SpriteShader->Create(L"Shaders\\sprite_vs", L"Shaders\\sprite_ps");
		SpriteShader->CreateVS(L"Shaders\\sprite_vs");
		SpriteShader->CreatePS(L"Shaders\\sprite_ps");
	}

	Parameters = std::make_unique <Object2d>();
	Parameters->Pos     = DirectX::SimpleMath::Vector2(0.0f, 0.0f);
	Parameters->Size    = DirectX::SimpleMath::Vector2(static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
	Parameters->TexPos  = DirectX::SimpleMath::Vector2(0.0f, 0.0f);
	Parameters->TexSize = DirectX::SimpleMath::Vector2(static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
	Parameters->Angle   = 0.0f;
	Parameters->Color   = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
}

sprite_Batch::~sprite_Batch() {}

void sprite_Batch::begin() {
	ID3D11DeviceContext* immediate_context = FRAMEWORK->GetDeviceContext();

	vertices.clear();
	// SRVバインド
	immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());	// レジスタ番号、シェーダリソースの数、SRVのポインタ
}

void sprite_Batch::end(ShaderEx* shader) {
	ID3D11DeviceContext* immediate_context = FRAMEWORK->GetDeviceContext();

	// テクスチャ座標を頂点バッファにセットする
	HRESULT hr = { S_OK };
	D3D11_MAPPED_SUBRESOURCE mapped_subrecource{};
	// mapped_subrecourceをvertex_bufferにマッピング
	hr = immediate_context->Map(VertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subrecource);	// 動的な定数バッファーを Map して書き込むときは D3D11_MAP_WRITE_DISCARD を使用する
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	size_t vertex_count = vertices.size();
	_ASSERT_EXPR(max_vertices >= vertex_count, "Buffer overflow");
	Vertex* data{ reinterpret_cast<Vertex*>(mapped_subrecource.pData) };	// reinterpret_cast：ありえないような変換のときに使用する？
	if (data != nullptr) {	// 情報の上書き
		// verticesのデータをコピー
		const Vertex* p = vertices.data();
		memcpy_s(data, max_vertices * sizeof(Vertex), p, vertex_count * sizeof(Vertex));	// memcpy_s：Buffer間でのバイトのコピー(コピー先ポインタ、コピー先サイズ、コピー元ポインタ、コピー元サイズ)
	}
	immediate_context->Unmap(VertexBuffer.Get(), 0);	// マッピング解除 頂点バッファを上書きしたら必ず実行。Map&Unmapはセットで使用する

	// 頂点バッファのバインド
	UINT stride{ sizeof(Vertex) };
	UINT offset{ 0 };
	immediate_context->IASetVertexBuffers(
		0,				               // 入力スロットの開始番号
		1,				               // 頂点バッファの数
		VertexBuffer.GetAddressOf(),  // 頂点バッファの配列
		&stride,		               // １頂点のサイズの配列
		&offset);		       // 頂点バッファの開始位置をずらすオフセットの配列

	//プリミティブタイプ及びデータの順序に関する情報のバインド
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// プリミティブの形状を指定できる？
											// D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST は3頂点、D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIPは四頂点 連続三角形
	// シェーダのバインド
	shader->Activate();

	// ラスタライザステートの設定
	immediate_context->RSSetState(rasterizer_states[0].Get());

	// プリミティブの描画
	immediate_context->Draw(static_cast<UINT>(vertex_count), 0);	// 頂点の数、描画開始時点で使う頂点バッファの最初のインデックス

	// シェーダの無効化
	shader->Inactivate();

}

void sprite_Batch::CreateVertexData(DirectX::SimpleMath::Vector2 pos, DirectX::SimpleMath::Vector2 size, float angle, DirectX::SimpleMath::Vector4 color, DirectX::SimpleMath::Vector2 TexPos, DirectX::SimpleMath::Vector2 TexSize) {
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

	DirectX::SimpleMath::Vector3 left_top    { pos.x         ,pos.y          ,0 };	// 左上
	DirectX::SimpleMath::Vector3 right_top   { pos.x + size.x,pos.y          ,0 };	// 右上
	DirectX::SimpleMath::Vector3 left_bottom { pos.x         ,pos.y + size.y ,0 };	// 左下
	DirectX::SimpleMath::Vector3 right_bottom{ pos.x + size.x,pos.y + size.y ,0 };	// 右下

	// 回転の中心を矩形の中心点に
	DirectX::SimpleMath::Vector2 center{ 0,0 };
	center.x = pos.x + size.x * 0.5f;	// 位置-(大きさ/2)で頂点位置から半サイズ分動く=半分になる
	center.y = pos.y + size.y * 0.5f;
	// 頂点回転
	SpriteMath::rotate(left_top    , center, angle);
	SpriteMath::rotate(left_bottom , center, angle);
	SpriteMath::rotate(right_top   , center, angle);
	SpriteMath::rotate(right_bottom, center, angle);

	// スクリーン座標系からNDC(正規化デバイス座標)への座標変換を行う
	left_top     = SpriteMath::ConvertToNDC(left_top    , viewport);	// 頂点位置、スクリーンの大きさ
	left_bottom  = SpriteMath::ConvertToNDC(left_bottom , viewport);
	right_top    = SpriteMath::ConvertToNDC(right_top   , viewport);
	right_bottom = SpriteMath::ConvertToNDC(right_bottom, viewport);

	DirectX::SimpleMath::Vector2 TexLeft_top    { (TexPos.x)             / texture2d_desc.Width , (TexPos.y)             / texture2d_desc.Height };
	DirectX::SimpleMath::Vector2 TexRight_top   { (TexPos.x + TexSize.x) / texture2d_desc.Width , (TexPos.y)             / texture2d_desc.Height };
	DirectX::SimpleMath::Vector2 TexLeft_bottom { (TexPos.x)             / texture2d_desc.Width , (TexPos.y + TexSize.y) / texture2d_desc.Height };
	DirectX::SimpleMath::Vector2 TexRight_bottom{ (TexPos.x + TexSize.x) / texture2d_desc.Width , (TexPos.y + TexSize.y) / texture2d_desc.Height };


	/* 計算結果で頂点バッファオブジェクトを更新する */
	/*				三角形を2つ作る					*/
	/*				0	*----*1	 *4					*/
	/*					||||/   /|					*/
	/*					|||/   /||					*/
	/*					||/   /|||					*/
	/*				2	*	3*---*5					*/

	vertices.push_back({ { left_top.x    ,left_top.y,	 0 }, {0,0,1}, { TexLeft_top.x,		TexLeft_top.y	 }, { color.x,color.y,color.z,color.w } });	// 左上
	vertices.push_back({ { right_top.x   ,right_top.y,	 0 }, {0,0,1}, { TexRight_top.x,	TexRight_top.y	 }, { color.x,color.y,color.z,color.w } });	// 右上
	vertices.push_back({ { left_bottom.x ,left_bottom.y, 0 }, {0,0,1}, { TexLeft_top.x,		TexLeft_bottom.y }, { color.x,color.y,color.z,color.w } });	// 左下
	vertices.push_back({ { left_bottom.x ,left_bottom.y, 0 }, {0,0,1}, { TexLeft_top.x,		TexLeft_bottom.y }, { color.x,color.y,color.z,color.w } });	// 左下
	vertices.push_back({ { right_top.x   ,right_top.y,	 0 }, {0,0,1}, { TexRight_top.x,	TexRight_top.y	 }, { color.x,color.y,color.z,color.w } });	// 右上
	vertices.push_back({ { right_bottom.x,right_bottom.y,0 }, {0,0,1}, { TexRight_bottom.x,	TexRight_bottom.y}, { color.x,color.y,color.z,color.w } });	// 右下
}

void sprite_Batch::Render(DirectX::SimpleMath::Vector2 pos, DirectX::SimpleMath::Vector2 size, float angle, DirectX::SimpleMath::Vector4 color, DirectX::SimpleMath::Vector2 sPos, DirectX::SimpleMath::Vector2 sSize) {
	CreateVertexData(pos, size, angle, color, sPos, sSize);
}

void sprite_Batch::Render() {
	CreateVertexData(Parameters->Pos, Parameters->Size, Parameters->Angle, Parameters->Color, Parameters->TexPos, Parameters->TexSize);
}

void sprite_Batch::Render(DirectX::SimpleMath::Vector2 Pos, DirectX::SimpleMath::Vector2 Size) {
	CreateVertexData(Pos, Size, 0, Parameters->Color, Parameters->TexPos, Parameters->TexSize);
}