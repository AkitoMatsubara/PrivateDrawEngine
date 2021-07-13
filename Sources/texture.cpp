
#pragma once
#include "texture.h"

#include <WICTextureLoader.h>
using namespace DirectX;

#include <wrl.h>
using namespace Microsoft::WRL;

#include <string>
#include <map>
using namespace std;


#include "misc.h"

static map < wstring, ComPtr <ID3D11ShaderResourceView>> resources;


HRESULT load_texture_from_file(ID3D11Device* device, const wchar_t* filename, ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc) {
	HRESULT hr{ S_OK };
	ComPtr<ID3D11Resource> resource{};

	// 画像ファイルのロードとSRVオブジェクトの生成
	auto it = resources.find(filename);	// クラスのテンプレートパラメータkey_type型のキーを受け取って検索する。見つからなかったらend()を返す
	if (it != resources.end()) {	// end()じゃない＝filenameが見つかったら
		*shader_resource_view = it->second.Get();	// secondはyオブジェクト、ここではSRV。ちなみにxオブジェクトはfirst
		(*shader_resource_view)->AddRef();	// カプセル化されたインターフェイスポインタを呼び出す
		(*shader_resource_view)->GetResource(resource.GetAddressOf());
	}
	else {
		hr = CreateWICTextureFromFile(device, filename, resource.GetAddressOf(), shader_resource_view);	// resourceとsrvが作成される
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		resources.insert(make_pair(filename, *shader_resource_view));	// insert：mapコンテナの拡張
	}

	// テクスチャ情報の取得
	ComPtr<ID3D11Texture2D> texture2d{};
	hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());	// 特定のインターフェイスをサポートしているかを判別する
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	texture2d->GetDesc(texture2d_desc);

	return hr;
}

void rerease_all_textures() {
	resources.clear();
}