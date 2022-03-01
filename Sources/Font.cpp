#include "Font.h"
#include "framework.h"

#include <Shlwapi.h>

// 参考 https://blogoftrueone.wordpress.com/2013/06/02/directx11で文字描画とフォント読み込み/



Font::~Font()
{
	for(auto& path:FontList)
	{
		// システムからフォントを削除する
		if (RemoveFontResourceEx(path, FR_PRIVATE, NULL) == 0)
		{
			_ASSERT_EXPR_A(false, "Delete Font Failed.");	// フォント削除失敗
		}
	}
	FontList.clear();
}

void Font::LoadFont(const wchar_t* ttfFontPath, const wchar_t* FontName)
{
	// システムにフォントを追加する
	if (AddFontResourceEx(ttfFontPath, FR_PRIVATE, NULL) == 0)
	{
		_ASSERT_EXPR_A(false, "Add Font Failed.");	// フォント追加失敗
	}
	FontList.push_back(ttfFontPath);	// 追加したフォントパスを保存

	// フォントの生成
	LONG FontSize = 64;
	LOGFONT lf = {};
	lf.lfHeight = FontSize;	// 文字の高さ
	lf.lfWidth = 0;			// 文字幅
	lf.lfEscapement = 0;	// 文字方向とX軸との角度
	lf.lfOrientation = 0;	// 各文字とX軸との角度
	lf.lfWeight = 0;		// 太さ
	lf.lfItalic = 0;		// イタリック体
	lf.lfUnderline = 0;		// 下線
	lf.lfStrikeOut = 0;		// 打ち消し線
	lf.lfCharSet = SHIFTJIS_CHARSET;			// キャラクタセット
	lf.lfOutPrecision = OUT_TT_ONLY_PRECIS;		// 出力精度
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;		// クリッピングの精度
	lf.lfQuality = PROOF_QUALITY;			// 出力品質
	lf.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;	// ピッチとファミリ
	lstrcpy(lf.lfFaceName, FontName);	// フォント名

	hFont = CreateFontIndirect(&lf);	// 論理フォント文字枠の生成
	if ((!hFont)) { MessageBox(0, L"Font Creation Failure", NULL, MB_OK); }
}

void Font::CreateFontTexture(const WCHAR* FontChar)
{
	ID3D11Device* Dev = FRAMEWORK->GetDevice();
	ID3D11DeviceContext* DevCon = FRAMEWORK->GetDeviceContext();
	HRESULT hr;

	HDC hdc = GetDC(nullptr); // デバイスコンテキストハンドルの取得
	HFONT oldFont = static_cast<HFONT>(SelectObject(hdc, hFont));
	UINT code;     // テクスチャに書き込む文字の文字コード

#if _UNICODE
	code = (UINT)*FontChar;
#else
	// マルチバイト文字かを確認して処理B
	// 2バイト文字のコードは[文字コード]*256 + [先導コード]らしい
	if (IsDBCSLeadByte(*fontChar)) { code = (BYTE)fontChar[0] << 8 | (BYTE)fontChar[1]; }
	else { code = fontChar[0]; }

#endif
	// フォントビットマップ取得
	TEXTMETRIC TM;	// 使用文字フォントの大きさ,幅,高さ,太さなどをもつ構造体らしい
	GetTextMetrics(hdc, &TM);
	GLYPHMETRICS GM;	// 単一のグリフの情報を表す (グリフ):1つ,または複数の文字の視覚的な表現
	CONST MAT2 Mat = {
		{0,1},
		{0,0},
		{0,0},
		{0,1} };
	DWORD size = GetGlyphOutline(hdc, code, GGO_GRAY4_BITMAP, &GM, 0, nullptr, &Mat);	// 必要なバッファサイズを返してくる
	BYTE* ptr = new BYTE[size];	// ビットマップ情報を受け取るバッファへのポインタ
	GetGlyphOutline(hdc, code, GGO_GRAY4_BITMAP, &GM, size, ptr, &Mat);		// ビットマップ情報を取得

	// デバイスコンテキストとフォントハンドルの解放
	SelectObject(hdc, oldFont);
	//DeleteObject(hFont);
	ReleaseDC(nullptr, hdc);


	//// CPUで書き込みができるテクスチャを作成 ////
	ZeroMemory(&texture2d_desc, sizeof(D3D11_TEXTURE2D_DESC));
	texture2d_desc.Width = GM.gmCellIncX;
	texture2d_desc.Height = TM.tmHeight;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;		// RGBA(255,255,255,255)タイプ
	texture2d_desc.Usage = D3D11_USAGE_DYNAMIC;				// 動的 CPU書き込み可能
	texture2d_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;	// シェーダリソース
	texture2d_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// CPUから書き込みアクセス可
	texture2d_desc.SampleDesc.Count = 1;						// サンプリングは1ピクセルのみ

	hr = Dev->CreateTexture2D(&texture2d_desc, NULL, &tex2D);
	if (FAILED(hr)) { MessageBox(0, L"Font Texture Create Failure", NULL, MB_OK); }

	// フォント情報をテクスチャに書き込む
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	hr = DevCon->Map(tex2D.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	if (FAILED(hr)) { MessageBox(0, L"Font Texture Write Failure", NULL, MB_OK); }

	// ここに書き込む
	BYTE* pBits = (BYTE*)MappedResource.pData;
	// フォント情報の書き込み
	//  書き出し位置(左上)
	int iOfs_x = GM.gmptGlyphOrigin.x;
	int iOfs_y = TM.tmAscent - GM.gmptGlyphOrigin.y;
	// フォントビットマップの幅高
	// ビットマップの1行のピクセル数は4の倍数でなければならない
	int iBmp_w = GM.gmBlackBoxX + ((4 - (GM.gmBlackBoxX % 4)) % 4);
	int iBmp_h = GM.gmBlackBoxY;
	// Level : α値の段階 (GGO_GRAY4_BITMAPなので17段階)
	int Level = 17;

	DWORD Alpha, Color;
	memset(pBits, 0, MappedResource.RowPitch * TM.tmHeight);
	// フォント情報の書き込み
	for (int y = iOfs_y; y < iOfs_y + iBmp_h; y++) {
		for (int x = iOfs_x; x < iOfs_x + iBmp_w; x++) {
			Alpha = (255 * ptr[x - iOfs_x + iBmp_w * (y - iOfs_y)]) / (Level - 1);
			Color = 0x00ffffff | (Alpha << 24);
			memcpy((BYTE*)pBits + MappedResource.RowPitch * y + 4 * x, &Color, sizeof(DWORD));	// おそらくここでピクセルに書き込んでる
		}
	}
	DevCon->Unmap(tex2D.Get(), 0);
	delete[] ptr;	// newで確保したので後片付け

	//	シェーダーリソースビュー作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = texture2d_desc.MipLevels;
	hr = Dev->CreateShaderResourceView(tex2D.Get(), &srvd, shader_resource_view.GetAddressOf());	// SRVの作成 これでシェーダに渡せる
	if (FAILED(hr)) { MessageBox(0, L"SRV Create Failure", NULL, MB_OK); }

	param->Size    = DirectX::SimpleMath::Vector2(static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
	param->TexSize = DirectX::SimpleMath::Vector2(static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));


	//// シェーダ用にサンプラを作成する
	//D3D11_SAMPLER_DESC samDesc;
	//ZeroMemory(&samDesc, sizeof(samDesc));
	//samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	//samDesc.AddressU = samDesc.AddressV = samDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	//samDesc.MaxAnisotropy = 1;
	//samDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	//samDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//hr = Dev->CreateSamplerState(&samDesc, &samplerState);


}