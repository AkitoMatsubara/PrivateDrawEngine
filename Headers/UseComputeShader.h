#pragma once
#include "shaderEx.h"

#include <memory>
#include <d3d11.h>

namespace UseComputeShader
{
	// てすと-------------------------------------------------------------------------
	HRESULT CreateStructuredBuffer(UINT uElementSize, UINT uCount, void* pInitData, ID3D11Buffer** ppBuf);

	// コンピュートシェーダーからの入力時に使用するシェーダーリソースビューを作成する
	HRESULT CreateSRVForStructuredBuffer(UINT uElementSize, UINT uCount, void* pInitData, ID3D11Buffer** ppBuf, ID3D11ShaderResourceView** ppSRVOut);

	// コンピュートシェーダーからの出力時に使用するアンオーダードアクセスビューを作成する
	HRESULT CreateUAVForStructuredBuffer(UINT uElementSize, UINT uCount, void* pInitData, ID3D11Buffer** ppBuf, ID3D11UnorderedAccessView** ppUAVOut);

	// アンオーダードアクセスビューのバッファの内容を CPU から読み込み可能なバッファへコピーする
	void CreateAndCopyToBuffer(ID3D11Buffer* pSrcBuf,ID3D11Buffer** ppDstBuf);

	// コンピュートシェーダーを実行する
	void RunComputeShader(ID3D11ComputeShader* pComputeShader, ID3D11ShaderResourceView* pSrcSRV,UINT SRVslot, ID3D11UnorderedAccessView* pDstUAV,UINT UAVslot, UINT X, UINT Y, UINT Z);
	// てすとしめ---------------------------------------------------------------------
};
