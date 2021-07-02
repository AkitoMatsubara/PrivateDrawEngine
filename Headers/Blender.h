#pragma once

class Blender {
public:
	enum BLEND_STATE {
		BS_NONE,		// ブレンドなし
		BS_ALPHA,		// 透過
		BS_ADD,			// 加算合成
		BS_SUBTRACT,	// 減算合成
		BS_REPLACE,		//
		BS_MULTIPLY,	// 乗算
		BS_LIGHTEN,		//
		BS_DARKEN,		//
		BS_SCREEN,		// スクリーン
		BS_END			// enum終わり 使用しない
	};

	ID3D11BlendState* states[BS_END];

	// 後に設定するために引数なしコンストラクタを設定
	Blender() {};

	// 生成と同時に設定するためのコンストラクタ
	// Render関数でのみ使用する場合とかに便利？
	Blender(ID3D11Device* device) {
		HRESULT hr                                       = { S_OK };

		D3D11_BLEND_DESC blend_desc{};

		/*----------[BS_NONE] なし----------*/
		blend_desc.AlphaToCoverageEnable                 = FALSE;	                        // マスキングによりくりぬき処理を行ったポリゴンを不透明部分に対してのみ陰面処理に対応しつつレンダリングする手法？を有効にするか
		blend_desc.IndependentBlendEnable                = FALSE;	                        // 複数のRenderTarget[1～]を使用する場合はTrueに
		blend_desc.RenderTarget[0].BlendEnable           = TRUE;	                        // ブレンディングを有効にするかどうか
		blend_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_ONE;	                // 最初のRGBデータソースの指定
		blend_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_ZERO;	            // 2番目のRGBデータソースの指定
		blend_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;	            // RGBの組み合わせ方法を定義	今回はSrcBlendとDestBlendを加算
		blend_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;	                // 最初のアルファデータソースを指定 ONEは(1,1,1,1)の白
		blend_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;	            // 2番目のアルファデータソースを指定 ZEROは(0,0,0,0)の黒
		blend_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;	            // アルファデータソースの組み合わせ方法を指定 加算
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;	// 書き込みマスク 今回はすべてのコンポーネントにデータを保存できるように
		hr = device->CreateBlendState(&blend_desc, &states[BS_NONE]);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		if (FAILED(hr))assert("NONE_BLEND ERROR");

		/*----------[BS_ALPHA] 透過----------*/
		blend_desc.AlphaToCoverageEnable                 = FALSE;
		blend_desc.IndependentBlendEnable                = FALSE;
		blend_desc.RenderTarget[0].BlendEnable           = TRUE;
		blend_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;			// 今回はピクセルシェーダのアルファデータを指定、ブレンディング前の処理は無し
		blend_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_INV_SRC_ALPHA;		// 今回はアルファデータ、ブレンディング前の処理によってデータが反転、１－Aが生成される
		blend_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;				// 今回はSrcBlendとDestBlendを加算
		blend_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;					// ONEは(1,1,1,1)の白
		blend_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;				// ZEROは(0,0,0,0)の黒
		blend_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;				// 加算
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, &states[BS_ALPHA]);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		if (FAILED(hr))assert("NONE_BLEND ERROR");

		/*----------[BS_ADD] 加算----------*/
		blend_desc.AlphaToCoverageEnable                 = FALSE;
		blend_desc.IndependentBlendEnable                = FALSE;
		blend_desc.RenderTarget[0].BlendEnable           = TRUE;
		blend_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, &states[BS_ADD]);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		if (FAILED(hr))assert("NONE_BLEND ERROR");

		/*----------[BS_SUBTRACT] 減算----------*/
		blend_desc.AlphaToCoverageEnable                 = FALSE;
		blend_desc.IndependentBlendEnable                = FALSE;
		blend_desc.RenderTarget[0].BlendEnable           = TRUE;
		blend_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_REV_SUBTRACT;
		blend_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, &states[BS_SUBTRACT]);
		if (FAILED(hr))assert("NONE_BLEND ERROR");

		/*----------[BS_REPLACE] 加算----------*/
		blend_desc.AlphaToCoverageEnable                 = FALSE;
		blend_desc.IndependentBlendEnable                = FALSE;
		blend_desc.RenderTarget[0].BlendEnable           = TRUE;
		blend_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, &states[BS_REPLACE]);
		if (FAILED(hr))assert("NONE_BLEND ERROR");

		blend_desc.AlphaToCoverageEnable                 = FALSE;
		blend_desc.IndependentBlendEnable                = FALSE;
		blend_desc.RenderTarget[0].BlendEnable           = TRUE;
		blend_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_SRC_COLOR;
		blend_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_DEST_ALPHA;
		blend_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, &states[BS_MULTIPLY]);
		if (FAILED(hr))assert("NONE_BLEND ERROR");

		blend_desc.AlphaToCoverageEnable                 = FALSE;
		blend_desc.IndependentBlendEnable                = FALSE;
		blend_desc.RenderTarget[0].BlendEnable           = TRUE;
		blend_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_MAX;
		blend_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_MAX;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, &states[BS_LIGHTEN]);
		if (FAILED(hr))assert("NONE_BLEND ERROR");

		blend_desc.AlphaToCoverageEnable                 = FALSE;
		blend_desc.IndependentBlendEnable                = FALSE;
		blend_desc.RenderTarget[0].BlendEnable           = TRUE;
		blend_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_MIN;
		blend_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_MIN;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, &states[BS_DARKEN]);
		if (FAILED(hr))assert("NONE_BLEND ERROR");

		blend_desc.AlphaToCoverageEnable                 = FALSE;
		blend_desc.IndependentBlendEnable                = FALSE;
		blend_desc.RenderTarget[0].BlendEnable           = TRUE;
		blend_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_INV_SRC_COLOR;
		blend_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_INV_SRC_ALPHA;
		blend_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, &states[BS_SCREEN]);
		if (FAILED(hr))assert("NONE_BLEND ERROR");

		// (ちなみに「Src」はSource(元)の略称らしい)
		// (SrcとDestで元と先を表す変数に使われたり？2要素って感じみたい)
	}

	~Blender() {

	}
	// 好きなタイミングで設定するための関数
	// 設定してないと怒られるしね 中身は上記コンストラクタと同じ
	void setBlender(ID3D11Device* device) {
		HRESULT hr = { S_OK };

		D3D11_BLEND_DESC blend_desc{};

		/*----------[BS_NONE] なし----------*/
		blend_desc.AlphaToCoverageEnable = FALSE;	                        // マスキングによりくりぬき処理を行ったポリゴンを不透明部分に対してのみ陰面処理に対応しつつレンダリングする手法？を有効にするか
		blend_desc.IndependentBlendEnable = FALSE;	                        // 複数のRenderTarget[1～]を使用する場合はTrueに
		blend_desc.RenderTarget[0].BlendEnable = TRUE;	                    // ブレンディングを有効にするかどうか
		blend_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_ONE;	                // 最初のRGBデータソースの指定
		blend_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_ZERO;	            // 2番目のRGBデータソースの指定
		blend_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;	            // RGBの組み合わせ方法を定義	今回はSrcBlendとDestBlendを加算
		blend_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;	                // 最初のアルファデータソースを指定 ONEは(1,1,1,1)の白
		blend_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;                // 2番目のアルファデータソースを指定 ZEROは(0,0,0,0)の黒
		blend_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;              // アルファデータソースの組み合わせ方法を指定 加算
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;	// 書き込みマスク 今回はすべてのコンポーネントにデータを保存できるように
		hr = device->CreateBlendState(&blend_desc, &states[BS_NONE]);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		if (FAILED(hr))assert("NONE_BLEND ERROR");

		/*----------[BS_ALPHA] 透過----------*/
		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;		// 今回はピクセルシェーダのアルファデータを指定、ブレンディング前の処理は無し
		blend_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_INV_SRC_ALPHA;	// 今回はアルファデータ、ブレンディング前の処理によってデータが反転、１－Aが生成される
		blend_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;			// 今回はSrcBlendとDestBlendを加算
		blend_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;				// ONEは(1,1,1,1)の白
		blend_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;			// ZEROは(0,0,0,0)の黒
		blend_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;			// 加算
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, &states[BS_ALPHA]);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		if (FAILED(hr))assert("NONE_BLEND ERROR");

		/*----------[BS_ADD] 加算----------*/
		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, &states[BS_ADD]);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		if (FAILED(hr))assert("NONE_BLEND ERROR");

		/*----------[BS_SUBTRACT] 減算----------*/
		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_REV_SUBTRACT;
		blend_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, &states[BS_SUBTRACT]);
		if (FAILED(hr))assert("NONE_BLEND ERROR");

		/*----------[BS_REPLACE] 加算----------*/
		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, &states[BS_REPLACE]);
		if (FAILED(hr))assert("NONE_BLEND ERROR");

		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_SRC_COLOR;
		blend_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_DEST_ALPHA;
		blend_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, &states[BS_MULTIPLY]);
		if (FAILED(hr))assert("NONE_BLEND ERROR");

		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_MAX;
		blend_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_MAX;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, &states[BS_LIGHTEN]);
		if (FAILED(hr))assert("NONE_BLEND ERROR");

		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_MIN;
		blend_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_MIN;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, &states[BS_DARKEN]);
		if (FAILED(hr))assert("NONE_BLEND ERROR");

		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_INV_SRC_COLOR;
		blend_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_INV_SRC_ALPHA;
		blend_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, &states[BS_SCREEN]);
		if (FAILED(hr))assert("NONE_BLEND ERROR");

		// (ちなみに「Src」はSource(元)の略称らしい)
		// (SrcとDestで元と先を表す変数に使われたり？2要素って感じみたい)
	}
};
