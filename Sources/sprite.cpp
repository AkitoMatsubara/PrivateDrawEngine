#include "framework.h"
#include "texture.h"
#include "sprite.h"
#include "misc.h"
#include <sstream>
#include <WICTextureLoader.h>

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"


// 頂点情報のセット
Vertex vertices[]
{
	{{-1.0,+1.0,0},{0,0,1},{0,0},{1,1,1,1}},	// 左上,法線,RGB色,UV座標
	{{+1.0,+1.0,0},{0,0,1},{1,0},{1,1,1,1}},	// 右上,法線,RGB色,UV座標
	{{-1.0,-1.0,0},{0,0,1},{0,1},{1,1,1,1}},	// 左下,法線,RGB色,UV座標
	{{+1.0,-1.0,0},{0,0,1},{1,1},{1,1,1,1}},	// 右下,法線,RGB色,UV座標
};


// 頂点バッファオブジェクトの生成
Sprite::Sprite(const wchar_t* filename)
{
	ID3D11Device* device = FRAMEWORK->GetDevice();
	HRESULT hr{ S_OK };


	// テクスチャのロード(上記処理をモジュール化)
	load_texture_from_file(filename, shader_resource_view.GetAddressOf(), &texture2d_desc);

	// ByteWidth,BindFlags,StructuerByteStrideは可変情報、その他情報はあまり変化することはない
	D3D11_BUFFER_DESC buffer_desc{};			// バッファの使われ方を設定する構造体
	buffer_desc.ByteWidth = sizeof(vertices);			// バッファの大きさ
	buffer_desc.Usage = D3D11_USAGE_DYNAMIC;			// バッファへの各項目でのアクセス許可を指定 現在はGPU（読み取り専用）とCPU（書き込み専用）の両方からアクセスできる設定
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// バインド方法 この設定頂点バッファやコンスタントバッファとして使用することを決める
	buffer_desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;// リソースに対してのCPUアクセス方法 ０でアクセス不要の設定になる。現在は書き込み専用
	buffer_desc.MiscFlags = 0;							// その他のオプション 不要なら０を指定  ちなみに...misc：miscellaneousの略称。「雑多な」という意味
	buffer_desc.StructureByteStride = 0;				// 構造体のサイズ バッファを構造体として確保する場合に使用 ０指定でも動作する

	D3D11_SUBRESOURCE_DATA subresource_data{};			// 作成するバッファの初期化データを保存する構造体
	subresource_data.pSysMem = vertices;				// バッファを初期化するデータを指定 どの情報で初期化するか
	subresource_data.SysMemPitch = 0;					// メモリのピッチ 2D or 3Dテクスチャの場合に使用する
	subresource_data.SysMemSlicePitch = 0;				// 深度レベル	同上

	hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer.GetAddressOf());		// 作成するバッファ情報、作成するバッファの初期化情報、作成したバッファを保存するポインタ
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));	// _ASSERT_EXPR：第一引数条件が満たされなければ第二引数のメッセージを表示する

	// ラスタライザオブジェクトの生成
	D3D11_RASTERIZER_DESC rasterizer_desc{};
	/*-----塗りつぶし 前面描画-----*/
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;	// レンダリングに使う塗りつぶしモード D3D11_FILL_SOLID|D3D11_FILL_WIREFRAME
	rasterizer_desc.CullMode = D3D11_CULL_BACK;	// 描画する法線方向 D3D11_CULL_NONE(両面描画)|D3D11_CULL_FRONT(後面描画)|D3D11_CULL_BACK(前面描画)
	rasterizer_desc.FrontCounterClockwise = FALSE;	// 三角形が前面か背面かを決定する TRUEの時、頂点が反対周りだと前向きとみなされる
	rasterizer_desc.DepthBias = 0;					// 深度バイアス 同一深度上に表示するときに優先度を決めるのに使用したりする
	rasterizer_desc.DepthBiasClamp = 0;			// 上記同様     ピクセルの最大深度バイアス
	rasterizer_desc.SlopeScaledDepthBias = 0;		// 上記同様     特定のピクセルの傾きのスカラー
	rasterizer_desc.DepthClipEnable = TRUE;		// 距離に基づいてクリッピングを有効にするか
	rasterizer_desc.ScissorEnable = FALSE;			// シザー矩形カリングを使用するか シザー矩形：描画領域の指定によく使われる
	rasterizer_desc.MultisampleEnable = FALSE;		// マルチサンプリングアンチエイリアス(MSAA)のRTVを使用している時、tureで四辺形ラインアンチエイリアス、falseでアルファラインアンチエイリアスを使用
													// MSAAを使用するにはリソース生成時にDX11_SAMPLE_DESC::Countを1より上の値を設定する必要がある
	rasterizer_desc.AntialiasedLineEnable = FALSE;	// MSAAのRTVを使用している時、線分描画でMultisampleEnableがfalseの時にアンチエイリアスを有効にする
	hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	/*-----ワイヤーフレーム 前面描画-----*/
	rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	rasterizer_desc.AntialiasedLineEnable = TRUE;
	hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states[1].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	/*-----ワイヤーフレーム 両面描画-----*/
	rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizer_desc.CullMode = D3D11_CULL_NONE;
	rasterizer_desc.AntialiasedLineEnable = TRUE;
	hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states[2].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	param.Pos =DirectX::SimpleMath::Vector2(0.0f, 0.0f);
	param.Size = DirectX::SimpleMath::Vector2(static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
	param.TexPos  =DirectX::SimpleMath::Vector2(0.0f, 0.0f);
	param.TexSize = DirectX::SimpleMath::Vector2(static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
	param.Angle   = 0.0f;
	param.Color   =DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
}

Sprite::~Sprite() {
	rerease_all_textures();
}


void Sprite::CreateVertexData(Shader* shader,DirectX::SimpleMath::Vector2 pos,DirectX::SimpleMath::Vector2 size, float angle,
	DirectX::SimpleMath::Vector4 color,DirectX::SimpleMath::Vector2 TexPos,DirectX::SimpleMath::Vector2 TexSize) {
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

	//// 回転を実装 簡単に関数を実装する方法、ラムダ式というらしい
	//auto rotate = [](DirectX::SimpleMath::Vector3& pos,DirectX::SimpleMath::Vector2 center, float angle) {
	//	pos.x -= center.x;	// 一度中心点分ずらす
	//	pos.y -= center.y;

	//	float cos{ cosf(XMConvertToRadians(angle)) };	// DegreeなのでRadianに変換
	//	float sin{ sinf(XMConvertToRadians(angle)) };
	//	float tx{ pos.x };	// 回転前の頂点座標
	//	float ty{ pos.y };
	//	pos.x = tx * cos - sin * ty;	// 回転の公式
	//	pos.y = tx * sin + cos * ty;

	//	pos.x += center.x;	// ずらした分戻す
	//	pos.y += center.y;
	//};

	// 回転の中心を矩形の中心点に
	DirectX::SimpleMath::Vector2 center{ 0,0 };
	center.x = pos.x + size.x * 0.5f;	// 位置-(大きさ/2)で頂点位置から半サイズ分動く=半分になる
	center.y = pos.y + size.y * 0.5f;
	rotate(left_top, center, angle);
	rotate(left_bottom, center, angle);
	rotate(right_top, center, angle);
	rotate(right_bottom, center, angle);

	// スクリーン座標系からNDC(正規化デバイス座標)への座標変換を行う
	left_top     = ConvertToNDC(left_top    , viewport);	// 頂点位置、スクリーンの大きさ
	left_bottom  = ConvertToNDC(left_bottom , viewport);
	right_top    = ConvertToNDC(right_top   , viewport);
	right_bottom = ConvertToNDC(right_bottom, viewport);

	DirectX::SimpleMath::Vector2 TexLeft_top    { (TexPos.x)             / texture2d_desc.Width , (TexPos.y)				/ texture2d_desc.Height };
	DirectX::SimpleMath::Vector2 TexRight_top   { (TexPos.x + TexSize.x) / texture2d_desc.Width , (TexPos.y)				/ texture2d_desc.Height };
	DirectX::SimpleMath::Vector2 TexLeft_bottom { (TexPos.x)             / texture2d_desc.Width , (TexPos.y + TexSize.y)	/ texture2d_desc.Height };
	DirectX::SimpleMath::Vector2 TexRight_bottom{ (TexPos.x + TexSize.x) / texture2d_desc.Width , (TexPos.y + TexSize.y)	/ texture2d_desc.Height };


	// 計算結果で頂点バッファオブジェクトを更新する
	// テクスチャ座標を頂点バッファにセットする
	HRESULT hr = { S_OK };
	D3D11_MAPPED_SUBRESOURCE mapped_subrecource{};
	// mapped_subrecourceをvertex_bufferにマッピング
	hr = immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subrecource);	// 動的な定数バッファーを Map して書き込むときは D3D11_MAP_WRITE_DISCARD を使用する
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	Vertex* vertices{ reinterpret_cast<Vertex*>(mapped_subrecource.pData) };	// reinterpret_cast：ありえないような変換のときに使用する？
	if (vertices != nullptr) {	// 情報の上書き
		vertices[0].position = left_top;
		vertices[1].position = right_top;
		vertices[2].position = left_bottom;
		vertices[3].position = right_bottom;

		for (int i = 0; i < 4; i++) {
			vertices[i].color = color;
		}

		// UV座標を頂点バッファにセット
		vertices[0].texcoord = TexLeft_top;
		vertices[1].texcoord = TexRight_top;
		vertices[2].texcoord = TexLeft_bottom;
		vertices[3].texcoord = TexRight_bottom;

		// 法線情報を設定
		vertices[0].normal =DirectX::SimpleMath::Vector3(0, 0, 1);
		vertices[1].normal =DirectX::SimpleMath::Vector3(0, 0, 1);
		vertices[2].normal =DirectX::SimpleMath::Vector3(0, 0, 1);
		vertices[3].normal =DirectX::SimpleMath::Vector3(0, 0, 1);

	}
	immediate_context->Unmap(vertex_buffer.Get(), 0);	// マッピング解除 頂点バッファを上書きしたら必ず実行。Map&Unmapはセットで使用する

	// SRVバインド
	immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());	// レジスタ番号、シェーダリソースの数、SRVのポインタ

	// 頂点バッファのバインド
	UINT stride{ sizeof(Vertex) };
	UINT offset{ 0 };
	immediate_context->IASetVertexBuffers(
		0,								// 入力スロットの開始番号
		1,								// 頂点バッファの数
		vertex_buffer.GetAddressOf(),	// 頂点バッファの配列
		&stride,						// １頂点のサイズの配列
		&offset);				// 頂点バッファの開始位置をずらすオフセットの配列

	//プリミティブタイプ及びデータの順序に関する情報のバインド
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);	// プリミティブの形状を指定できる？ 今回は連続三角形に変更

	// シェーダの有効化
	shader->Activate();

	// ラスタライザステートの設定
	immediate_context->RSSetState(rasterizer_states[0].Get());
	// プリミティブの描画
	immediate_context->Draw(4, 0);	// 頂点の数、描画開始時点で使う頂点バッファの最初のインデックス
	// シェーダの無効化
	shader->Inactivate();
}

void Sprite::Render(Shader* shader,DirectX::SimpleMath::Vector2 pos,DirectX::SimpleMath::Vector2 size, float angle,DirectX::SimpleMath::Vector4 color,DirectX::SimpleMath::Vector2 TexPos,DirectX::SimpleMath::Vector2 TexSize) {
	CreateVertexData(shader, pos, size, angle, color, TexPos, TexSize);
}

void Sprite::Render(Shader* shader) {
	CreateVertexData(shader, param.Pos, param.Size, param.Angle, param.Color, param.TexPos, param.TexSize);
}

void Sprite::Render(Shader* shader,DirectX::SimpleMath::Vector2 Pos,DirectX::SimpleMath::Vector2 Size) {
	CreateVertexData(shader, Pos, Size, param.Angle, param.Color, param.TexPos, param.TexSize);
}

//void Sprite::Text_Out(ID3D11DeviceContext* immediate_context, std::string s,DirectX::SimpleMath::Vector2 pos,DirectX::SimpleMath::Vector2 size,DirectX::SimpleMath::Vector4 color) {
//	DirectX::SimpleMath::Vector2 TexPos(static_cast<float>(texture2d_desc.Width / 16), static_cast<float>(texture2d_desc.Height / 16));
//	float carriage = 0;
//	for (const char c : s) {
//		Render(immediate_context,DirectX::SimpleMath::Vector2(pos.x + carriage, pos.y), size, 0, color,
//			DirectX::SimpleMath::Vector2(TexPos.x * (c & 0x0F), TexPos.y * (c >> 4)), TexPos);
//		carriage += size.x;
//	}
//}

DirectX::SimpleMath::Vector3 Sprite::ConvertToNDC(DirectX::SimpleMath::Vector3 pos, D3D11_VIEWPORT viewport) {
	pos.x = (pos.x * 2 / viewport.Width) - 1.0f;	// x値を２倍、その後スクリーンサイズで割って１を引くと正規化される
	pos.y = 1.0f - (pos.y * 2.0f / viewport.Height);	// y値を２倍、スクリーンサイズで割ったもので１を引くと正規化	xと違うのはおそらく左手右手座標系の関係
	// 今回はsprite(画像)なのでz値は変更する必要なし
	return pos;
}

void Sprite::ImguiWindow() {
	static float pos[2]     { param.Pos.x    ,param.Pos.y };
	static float size[2]    { param.Size.x   ,param.Size.y };
	static float angle      { param.Angle};
	static float TexPos[2]  { param.TexPos.x ,param.TexPos .y };
	static float TexSize[2] { param.TexSize.x,param.TexSize.y };
	static float Color[4] = { param.Color.x  ,param.Color.y,param.Color.z,param.Color.w };

	ImGui::Begin("Sprite_param");

	ImGui::SliderFloat2("Position", pos, -1280, 720);
	ImGui::SliderFloat2("Size", size, 0, 1960);
	ImGui::SliderFloat2("TexPos", TexPos, 0, 1960);
	ImGui::SliderFloat2("TexSize", TexSize, 0, 1960);
	ImGui::SliderFloat("angle", &angle, 0, 360);
	ImGui::ColorEdit4(u8"Color", (float*)&Color);

	ImGui::End();

	setPos    (DirectX::SimpleMath::Vector2(pos[0], pos[1]));
	setSize   (DirectX::SimpleMath::Vector2(size[0], size[1]));
	setAngle  (angle);
	setTexPos (DirectX::SimpleMath::Vector2(TexPos[0], TexPos[1]));
	setTexSize(DirectX::SimpleMath::Vector2(TexSize[0], TexSize[1]));
	setColor  (DirectX::SimpleMath::Vector4(Color[0], Color[1], Color[2], Color[3]));

}


DirectX::SimpleMath::Vector2 Sprite::Division(DirectX::SimpleMath::Vector2 val1,DirectX::SimpleMath::Vector2 val2) {
	DirectX::SimpleMath::Vector2 valOut;
	valOut.x = val1.x / val2.x;
	valOut.y = val1.y / val2.y;
	return valOut;
}