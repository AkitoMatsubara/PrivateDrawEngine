#include "sprite.h"
#include "misc.h"
#include <sstream>

// 頂点情報のセット

vFormat_t vertices[]
{
	{{-1.0, +1.0, 0},{1,1,1,1}},	// 左上 , RGB色
	{{+1.0, +1.0, 0},{1,0,0,1}},	// 右上 , RGB色
	{{-1.0, -1.0, 0},{0,1,0,1}},	// 左下 , RGB色
	{{+1.0, -1.0, 0},{0,0,1,1}},	// 右下 , RGB色
};


// 頂点バッファオブジェクトの生成
Sprite::Sprite(ID3D11Device* device) {
	HRESULT hr{ S_OK };

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
	subresource_data.SysMemSlicePitch = 0;				//	深度レベル	 同上

	hr = device->CreateBuffer(&buffer_desc, &subresource_data, &vertex_buffer);		// 作成するバッファ情報、作成するバッファの初期化情報、作成したバッファを保存するポインタ
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));	// _ASSERT_EXPR：第一引数条件が満たされなければ第二引数のメッセージを表示する

	// 頂点シェーダーオブジェクトの生成
	const char* vs_cso_name{ "Shaders\\sprite_vs.cso" };

	FILE* fp{};
	fopen_s(&fp, vs_cso_name, "rb");	// ファイルポインタ、ファイル名、rb：読み取り専用のバイナリモード
	_ASSERT_EXPR_A(fp, "_PS.CSO File not found.");

	fseek(fp, 0, SEEK_END);	// ファイルポインタ、移動バイト数、ファイルの先頭(_SET)、現在位置(_CUR)、終端(_END)
	long vs_cso_sz{ ftell(fp) };	// ファイルの読み書き位置を取得
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]>vs_cso_data{ std::make_unique<unsigned char[]>(vs_cso_sz) };	// unique_ptrにmake_uniqueで実体生成
	fread(vs_cso_data.get(), vs_cso_sz, 1, fp);	// 読み込みデータの格納先、読み込みデータのバイト長さ、読み込みデータの数、ファイルポインタ
	fclose(fp);

	hr = device->CreateVertexShader(vs_cso_data.get(), vs_cso_sz, nullptr, &vertex_shader);	// シェーダのポインター、シェーダーサイズ、dynamic linkageで使うポインタ、作成したバッファを保存するポインタ
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// 入力レイアウトオブジェクトの生成
	D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	{
		{
		"POSITION",						// セマンティクス名	HLSL側のシグネチャ(型や変数名の組み合わせの事？)の名前と一致させることで送信した頂点情報を受信することができる
		0,								// セマンティクス番号 同名でも識別できるように番号を割り当てる。番号を変更することでHLSLで別の情報だと認識できる
		DXGI_FORMAT_R32G32B32_FLOAT,	// フォーマット	R23G23B23は実質float3
		0,								// 入力スロット番号	入寮レイアウトをどの入力スロットに対して反映されるかを指定する
		D3D11_APPEND_ALIGNED_ELEMENT,	// 要素から先頭までのオフセット値	各データの配列先頭が何倍と離れているか。D3D11_APPEND_ALIGNED_ELEMENTを指定でオフセット値を設定すると同義
		D3D11_INPUT_PER_VERTEX_DATA,	// 入力データの種類	頂点データとインスタンスデータの２種類
		0								// 繰り返し回数(頂点データの時は０)	上記でインスタンスデータを設定した場合に意味を持つ
		},
		{"COLOR",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	};

	//	引数：IL(入力レイアウト)の構成情報、ILの要素数、VSのポインタ、VSのサイズ、作成したILを保存するポインタ
	hr = device->CreateInputLayout(input_element_desc, _countof(input_element_desc), vs_cso_data.get(), vs_cso_sz, &input_layout);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// ピクセルシェーダオブジェクトの生成
	const char* ps_cso_name{ "Shaders\\sprite_ps.cso" };

	fopen_s(&fp, ps_cso_name, "rb");	// ファイルポインタ、ファイル名、rb：読み取り専用のバイナリモード
	_ASSERT_EXPR_A(fp, "_VS.CSO File not found.");

	fseek(fp, 0, SEEK_END);	// ファイルポインタ、移動バイト数、ファイルの先頭(_SET)、現在位置(_CUR)、終端(_END)
	long ps_cso_sz{ ftell(fp) };	// ファイルの読み書き位置を取得
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]>ps_cso_data{ std::make_unique<unsigned char[]>(ps_cso_sz) };	// unique_ptrにmake_uniqueで実体生成
	fread(ps_cso_data.get(), ps_cso_sz, 1, fp);	// 読み込みデータの格納先、読み込みデータのバイト長さ、読み込みデータの数、ファイルポインタ
	fclose(fp);

	hr = device->CreatePixelShader(ps_cso_data.get(), ps_cso_sz, nullptr, &pixel_shader);	// シェーダのポインター、シェーダーサイズ、dynamic linkageで使うポインタ、作成したバッファを保存するポインタ
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

}

Sprite::~Sprite() {
	vertex_shader->Release();
	pixel_shader->Release();
	input_layout->Release();
	vertex_buffer->Release();
}

void Sprite::render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh, DirectX::XMFLOAT4 color) {
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

	DirectX::XMFLOAT3 left_top     = { dx     ,dy      ,0};	// 左上
	DirectX::XMFLOAT3 left_bottom  = { dx     ,dy + dh ,0};	// 左下
	DirectX::XMFLOAT3 right_top    = { dx + dw,dy      ,0};	// 右上
	DirectX::XMFLOAT3 right_bottom = { dx + dw,dy + dh ,0};	// 右下

	// スクリーン座標系からNDC(正規化デバイス座標)への座標変換を行う
	left_top     = Convert_Screen_to_NDC(left_top    , viewport);	// 頂点位置、スクリーンの大きさ
	left_bottom  = Convert_Screen_to_NDC(left_bottom , viewport);
	right_top    = Convert_Screen_to_NDC(right_top   , viewport);
	right_bottom = Convert_Screen_to_NDC(right_bottom, viewport);

	// 計算結果で頂点バッファオブジェクトを更新する
	HRESULT hr = { S_OK };
	D3D11_MAPPED_SUBRESOURCE mapped_subrecource{};
	// mapped_subrecourceをvertex_bufferにマッピング
	hr = immediate_context->Map(vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subrecource);	// 動的な定数バッファーを Map して書き込むときは D3D11_MAP_WRITE_DISCARD を使用する
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	vFormat_t* vertices{ reinterpret_cast<vFormat_t*>(mapped_subrecource.pData) };	// reinterpret_cast：ありえないような変換のときに使用する？
	if (vertices != nullptr) {	// 情報の上書き
		vertices[0].position = left_top;
		vertices[1].position = right_top;
		vertices[2].position = left_bottom;
		vertices[3].position = right_bottom;

		for (int i = 0; i < 4; i++) {
			vertices[i].color = color;
		}
	}
	immediate_context->Unmap(vertex_buffer, 0);	// マッピング解除 頂点バッファを上書きしたら必ず実行。Map&Unmapはセットで使用する

	// 頂点バッファのバインド
	UINT stride{ sizeof(vFormat_t) };
	UINT offset{ 0 };
	immediate_context->IASetVertexBuffers(
		0,				// 入力スロットの開始番号
		1,				// 頂点バッファの数
		&vertex_buffer,	// 頂点バッファの配列
		&stride,		// １頂点のサイズの配列
		&offset);		// 頂点バッファの開始位置をずらすオフセットの配列

	//プリミティブタイプ及びデータの順序に関する情報のバインド
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);	// プリミティブの形状を指定できる？ 今回は連続三角形に変更

	// 入力レイアウトオブジェクトのバインド
	immediate_context->IASetInputLayout(input_layout);	// 入力レイアウトの設定

	// シェーダのバインド
	immediate_context->VSSetShader(vertex_shader, nullptr, 0);
	immediate_context->PSSetShader(pixel_shader, nullptr, 0);

	// プリミティブの描画
	immediate_context->Draw(4, 0);	// 頂点の数、描画開始時点で使う頂点バッファの最初のインデックス



}

DirectX::XMFLOAT3 Sprite::Convert_Screen_to_NDC(DirectX::XMFLOAT3 pos, D3D11_VIEWPORT viewport) {
	pos.x = (pos.x * 2 / viewport.Width) - 1.0f;	// x値を２倍、その後スクリーンサイズで割って１を引くと正規化される
	pos.y = 1.0f - (pos.y * 2.0f / viewport.Height);	// y値を２倍、スクリーンサイズで割ったもので１を引くと正規化	xと違うのはおそらく左手右手座標系の関係
	// 今回はsprite(画像)なのでz値は変更する必要なし
	return pos;
}