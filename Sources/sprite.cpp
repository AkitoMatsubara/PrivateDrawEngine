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
Sprite::Sprite()
{
	ID3D11Device* device = FRAMEWORK->GetDevice();
	HRESULT hr{ S_OK };

	// ByteWidth,BindFlags,StructuerByteStrideは可変情報、その他情報はあまり変化することはない
	D3D11_BUFFER_DESC buffer_desc{};					// バッファの使われ方を設定する構造体
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

	hr = device->CreateBuffer(&buffer_desc, &subresource_data, VertexBuffer.GetAddressOf());		// 作成するバッファ情報、作成するバッファの初期化情報、作成したバッファを保存するポインタ
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));	// _ASSERT_EXPR：第一引数条件が満たされなければ第二引数のメッセージを表示する

	if (!SpriteShader)	// 一回だけ生成
	{
		SpriteShader = std::make_unique<ShaderEx>();
		SpriteShader->CreateVS(L"Shaders\\sprite_vs");
		SpriteShader->CreatePS(L"Shaders\\sprite_ps");
	}
	SpriteTexture = std::make_unique<Texture>();

	Parameter = std::make_unique<Object2d>();

	Parameter->Pos =DirectX::SimpleMath::Vector2(0.0f, 0.0f);
	Parameter->TexPos  =DirectX::SimpleMath::Vector2(0.0f, 0.0f);
	Parameter->Angle   = 0.0f;
	Parameter->Color   =DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
}

Sprite::~Sprite() {}

void Sprite::LoadImages(const wchar_t* filename)
{
	// テクスチャのロード
	SpriteTexture->Load(filename);

	//// サイズなどは画像から取得しているためここで設定する
	Parameter->Size = DirectX::SimpleMath::Vector2(static_cast<float>(SpriteTexture->GetWidth()), static_cast<float>(SpriteTexture->GetHeight()));
	Parameter->TexSize = DirectX::SimpleMath::Vector2(static_cast<float>(SpriteTexture->GetWidth()), static_cast<float>(SpriteTexture->GetHeight()));
	//hr = load_texture_from_file(filename, shader_resource_view.GetAddressOf(), &texture2d_desc);
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));	// _ASSERT_EXPR：第一引数条件が満たされなければ第二引数のメッセージを表示する
	//Parameter->Size = DirectX::SimpleMath::Vector2(static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
	//Parameter->TexSize = DirectX::SimpleMath::Vector2(static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
}

void Sprite::CreateVertexData(ID3D11DeviceContext* device_context, Texture* texture, Shader* shader, ID3D11CommandList** comand_list) {
	// スクリーン(ビューポート)のサイズを取得する
	D3D11_VIEWPORT viewport{};
	UINT num_viewports{ 1 };
	device_context->RSGetViewports(&num_viewports, &viewport);

	// 引数から矩形の各頂点の位置を計算する
	/*		left_top	*----*	right_top			*/
	/*					|   /|						*/
	/*					|  / |						*/
	/*					| /  |						*/
	/*		left_bottom	*----*	right_bottom		*/

	DirectX::SimpleMath::Vector3 left_top    { Parameter->Pos.x         ,Parameter->Pos.y          , 1.0f };	// 左上
	DirectX::SimpleMath::Vector3 right_top   { Parameter->Pos.x + Parameter->Size.x,Parameter->Pos.y          , 1.0f };	// 右上
	DirectX::SimpleMath::Vector3 left_bottom { Parameter->Pos.x         ,Parameter->Pos.y + Parameter->Size.y , 1.0f };	// 左下
	DirectX::SimpleMath::Vector3 right_bottom{ Parameter->Pos.x + Parameter->Size.x,Parameter->Pos.y + Parameter->Size.y , 1.0f };	// 右下

	// 回転の中心を矩形の中心点に
	DirectX::SimpleMath::Vector2 center{ 0,0 };
	center.x = Parameter->Pos.x + Parameter->Size.x * 0.5f;	// 位置-(大きさ/2)で頂点位置から半サイズ分動く=半分になる
	center.y = Parameter->Pos.y + Parameter->Size.y * 0.5f;
	// 頂点回転
	SpriteMath::rotate(left_top    , center, Parameter->Angle);
	SpriteMath::rotate(left_bottom , center, Parameter->Angle);
	SpriteMath::rotate(right_top   , center, Parameter->Angle);
	SpriteMath::rotate(right_bottom, center, Parameter->Angle);

	// スクリーン座標系からNDC(正規化デバイス座標)への座標変換を行う
	left_top     = SpriteMath::ConvertToNDC(left_top    , viewport);	// 頂点位置、スクリーンの大きさ
	left_bottom  = SpriteMath::ConvertToNDC(left_bottom , viewport);
	right_top    = SpriteMath::ConvertToNDC(right_top   , viewport);
	right_bottom = SpriteMath::ConvertToNDC(right_bottom, viewport);

	// テクスチャの指定の有無で変えている
	UINT width;
	UINT height;
	if (texture) {
		width  = texture->GetWidth();
		height = texture->GetHeight();
	}
	else {
		width  = SpriteTexture->GetWidth();
		height = SpriteTexture->GetHeight();
	}

	const DirectX::SimpleMath::Vector2 tex_left_top    { (Parameter->TexPos.x)                    / width, (Parameter->TexPos.y)						/ height };
	const DirectX::SimpleMath::Vector2 tex_right_top   { (Parameter->TexPos.x + Parameter->TexSize.x) / width, (Parameter->TexPos.y)						/ height };
	const DirectX::SimpleMath::Vector2 tex_left_bottom { (Parameter->TexPos.x)                    / width, (Parameter->TexPos.y + Parameter->TexSize.y)	/ height };
	const DirectX::SimpleMath::Vector2 tex_right_bottom{ (Parameter->TexPos.x + Parameter->TexSize.x) / width, (Parameter->TexPos.y + Parameter->TexSize.y)	/ height };


	// 計算結果で頂点バッファオブジェクトを更新する
	// テクスチャ座標を頂点バッファにセットする
	HRESULT hr = { S_OK };
	D3D11_MAPPED_SUBRESOURCE mapped_subrecource{};
	// mapped_subrecourceをvertex_bufferにマッピング
	hr = device_context->Map(VertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subrecource);	// 動的な定数バッファーを Map して書き込むときは D3D11_MAP_WRITE_DISCARD を使用する
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	Vertex* vertices{ reinterpret_cast<Vertex*>(mapped_subrecource.pData) };	// reinterpret_cast：ありえないような変換のときに使用する？
	if (vertices != nullptr) {	// 情報の上書き
		vertices[0].position = left_top;
		vertices[1].position = right_top;
		vertices[2].position = left_bottom;
		vertices[3].position = right_bottom;

		for (int i = 0; i < 4; i++) {
			vertices[i].color = Parameter->Color;
		}

		// UV座標を頂点バッファにセット
		vertices[0].texcoord = tex_left_top;
		vertices[1].texcoord = tex_right_top;
		vertices[2].texcoord = tex_left_bottom;
		vertices[3].texcoord = tex_right_bottom;

		// 法線情報を設定
		vertices[0].normal =DirectX::SimpleMath::Vector3(0, 0, 1);
		vertices[1].normal =DirectX::SimpleMath::Vector3(0, 0, 1);
		vertices[2].normal =DirectX::SimpleMath::Vector3(0, 0, 1);
		vertices[3].normal =DirectX::SimpleMath::Vector3(0, 0, 1);

	}
	device_context->Unmap(VertexBuffer.Get(), 0);	// マッピング解除 頂点バッファを上書きしたら必ず実行。Map&Unmapはセットで使用する


	// シェーダの有効化
	shader->Activate(device_context);

	// テクスチャが外部からの指定があればそちらをSRVにセットする
	(texture) ? texture->Set(device_context, 0) : SpriteTexture->Set(device_context, 0);

	// 頂点バッファのバインド
	const UINT stride{ sizeof(Vertex) };
	const UINT offset{ 0 };
	device_context->IASetVertexBuffers(
		0,								// 入力スロットの開始番号
		1,								// 頂点バッファの数
		VertexBuffer.GetAddressOf(),	// 頂点バッファの配列
		&stride,						// １頂点のサイズの配列
		&offset);						// 頂点バッファの開始位置をずらすオフセットの配列

	//プリミティブタイプ及びデータの順序に関する情報のバインド
	device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);	// プリミティブの形状を指定できる 今回は連続三角形に変更

	// ラスタライザステートの設定
	device_context->RSSetState(FRAMEWORK->GetRasterizerState(FRAMEWORK->RS_SOLID_NONE));
	// プリミティブの描画
	device_context->Draw(4, 0);	// 頂点の数、描画開始時点で使う頂点バッファの最初のインデックス

	// テクスチャの無効化
	texture->Set(device_context,0, false);
	// シェーダの無効化
	shader->Inactivate(device_context);

	// DeferredContextの場合、コマンドリストに登録する
	if (device_context->GetType() == D3D11_DEVICE_CONTEXT_DEFERRED)
	{
		device_context->FinishCommandList(true, comand_list);
	}
}

void Sprite::Render(ID3D11DeviceContext* device_context, Texture* texture, Shader* shader, ID3D11CommandList** comand_list) {
	CreateVertexData(device_context, texture, shader, comand_list);
}

void Sprite::ImguiWindow() {
	static float pos[2]     { Parameter->Pos.x    ,Parameter->Pos.y };
	static float size[2]    { Parameter->Size.x   ,Parameter->Size.y };
	static float angle      { Parameter->Angle};
	static float TexPos[2]  { Parameter->TexPos.x ,Parameter->TexPos .y };
	static float TexSize[2] { Parameter->TexSize.x,Parameter->TexSize.y };
	static float Color[4] = { Parameter->Color.x  ,Parameter->Color.y,Parameter->Color.z,Parameter->Color.w };

	ImGui::Begin("Sprite_param");

	ImGui::SliderFloat2("Position", pos, -1280, 720);
	ImGui::SliderFloat2("Size", size, 0, 1960);
	ImGui::SliderFloat2("TexPos", TexPos, 0, 1960);
	ImGui::SliderFloat2("TexSize", TexSize, 0, 1960);
	ImGui::SliderFloat("angle", &angle, 0, 360);
	ImGui::ColorEdit4("Color", (float*)&Color);

	ImGui::End();

	setPos    (DirectX::SimpleMath::Vector2(pos[0], pos[1]));
	setSize   (DirectX::SimpleMath::Vector2(size[0], size[1]));
	setRotate(angle);
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


void FontRender(float x, float y, std::string string, int size, DirectX::XMFLOAT4 color)
{
	/* 型変換 */
	std::wstring w_str;
	size_t wcs_buff_size = string.length() + 1;	// 文字列長算出
	wchar_t* wcs = new wchar_t[wcs_buff_size];	// 文字列長分のcharを確保

	// 出力先、文字列長、文字列
	mbstowcs_s(nullptr, wcs, wcs_buff_size, string.c_str(), _TRUNCATE);

	w_str = wcs;	// 保存
	delete[] wcs;	// いらないので削除

	// 描画・文字生成
	float rx = x;
	float ry = y;
	int rw = 0, rh = 0;
	int before_char_rw = 0, y_cnt = 0;
	std::wstring now_char;	// 描画する文字

	for (int i = 0; i < w_str.length(); ++i)
	{
		// そもそも末端であれば実行しない
		if (w_str[i] == '\0') return;

		if (w_str[i] == '\n')	// 改行コードであれば
		{
			before_char_rw = 0;
			++y_cnt;

			rx = 0.0f;
			ry = y + (y_cnt * rh);
			continue;
		}

		//文字スプライトを取得
		now_char = w_str[i];	// 今描画対象の文字
		//// 文字列の画像を保持したクラスを呼び出す
		//スプライトclass* spr = Get_FontData(now_char.c_str(), size);	// 描画文字、サイズ

		////描画サイズ調整
		//float exrate = size / static_cast<float>(font_data_[now_char].font_size_);	// フォントサイズによって調整

		////描画幅、高さ
		//rw = static_cast<int>(spr->TextureWidth() * exrate);	// ズレ修正
		//rh = static_cast<int>(spr->TextureHeight() * exrate);

		////描画位置
		//rx += before_char_rw; // 横幅を確保した座標

		//spr->描画(rx, ry, rw, rh, color);	// 文字描画

		//before_char_rw = rw; //変更
	}
}