#include "shader.h"

#include "misc.h"
#include <sstream>


HRESULT create_vs_from_cso(ID3D11Device* device, const char* vs_cso_name, ID3D11VertexShader** vertex_shader,
	ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements) {
	HRESULT hr = { S_OK };

	// 頂点シェーダーオブジェクトの生成

	FILE* fp{ nullptr };
	fopen_s(&fp, vs_cso_name, "rb");	// ファイルポインタ、ファイル名、rb：読み取り専用のバイナリモード
	_ASSERT_EXPR_A(fp, "VS.CSO File not found.");

	fseek(fp, 0, SEEK_END);	// ファイルポインタ、移動バイト数、ファイルの先頭(_SET)、現在位置(_CUR)、終端(_END)
	long vs_cso_sz{ ftell(fp) };	// ファイルの読み書き位置を取得
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]>vs_cso_data{ std::make_unique<unsigned char[]>(vs_cso_sz) };	// unique_ptrにmake_uniqueで実体生成
	fread(vs_cso_data.get(), vs_cso_sz, 1, fp);	// 読み込みデータの格納先、読み込みデータのバイト長さ、読み込みデータの数、ファイルポインタ
	fclose(fp);

	hr = device->CreateVertexShader(vs_cso_data.get(), vs_cso_sz, nullptr, vertex_shader);	// シェーダのポインター、シェーダーサイズ、dynamic linkageで使うポインタ、作成したバッファを保存するポインタ
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	if (input_layout) {
		//	引数：IL(入力レイアウト)の構成情報、ILの要素数、VSのポインタ、VSのサイズ、作成したILを保存するポインタ
		hr = device->CreateInputLayout(input_element_desc, num_elements, vs_cso_data.get(), vs_cso_sz, input_layout);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	return hr;
}

HRESULT create_ps_from_cso(ID3D11Device* device, const char* ps_cso_name, ID3D11PixelShader** pixel_shader) {
	HRESULT hr = { S_OK };

	// 頂点シェーダーオブジェクトの生成

	FILE* fp{ nullptr };
	fopen_s(&fp, ps_cso_name, "rb");	// ファイルポインタ、ファイル名、rb：読み取り専用のバイナリモード
	_ASSERT_EXPR_A(fp, "PS CSO File not found.");

	fseek(fp, 0, SEEK_END);	// ファイルポインタ、移動バイト数、ファイルの先頭(_SET)、現在位置(_CUR)、終端(_END)
	long ps_cso_sz{ ftell(fp) };	// ファイルの読み書き位置を取得
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]>ps_cso_data{ std::make_unique<unsigned char[]>(ps_cso_sz) };	// unique_ptrにmake_uniqueで実体生成
	fread(ps_cso_data.get(), ps_cso_sz, 1, fp);	// 読み込みデータの格納先、読み込みデータのバイト長さ、読み込みデータの数、ファイルポインタ
	fclose(fp);

	hr = device->CreatePixelShader(ps_cso_data.get(), ps_cso_sz, nullptr, pixel_shader);	// シェーダのポインター、シェーダーサイズ、dynamic linkageで使うポインタ、作成したバッファを保存するポインタ
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	return hr;
}
