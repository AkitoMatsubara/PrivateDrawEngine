#pragma once
#include "shaderEx.h"

#include <memory>
#include <d3d11.h>

namespace UseComputeShader
{
	HRESULT CreateStructuredBuffer(UINT uElementSize, UINT uCount, void* pInitData, ID3D11Buffer** ppBuf);

	// CSからの入力時に使用するSRVと構造化バッファを作成する
	HRESULT CreateStructuredBufferAndSRV(UINT uElementSize, UINT uCount, void* pInitData, ID3D11Buffer** ppBuf, ID3D11ShaderResourceView** ppSRVOut);
	// CSからの入力時に使用するSRVを構造化バッファから作成する
	HRESULT CreateSRVfromStructuredBuffer(ID3D11Buffer* ppStructuerdBuf, ID3D11ShaderResourceView** ppSRVOut);

	// CSからの出力時に使用するUAVと構造化バッファを作成する
	HRESULT CreateStructuredBufferAndUAV(UINT uElementSize, UINT uCount, void* pInitData, ID3D11Buffer** ppBuf, ID3D11UnorderedAccessView** ppUAVOut);
	// CSからの出力時に使用するURVを構造化バッファから作成する
	HRESULT CreateUAVfromStructuredBuffer(ID3D11Buffer* ppStructuerdBuf, ID3D11UnorderedAccessView** ppUAVOut);

	// アンオーダードアクセスビューのバッファの内容を CPU から読み込み可能なバッファへコピーする
	void CreateAndCopyToBuffer(ID3D11Buffer* pSrcBuf,ID3D11Buffer** ppDstBuf);

	// CSを実行する
	void RunComputeShader(ID3D11ComputeShader* pComputeShader, ID3D11ShaderResourceView* pSrcSRV,UINT SRVslot, ID3D11UnorderedAccessView* pDstUAV,UINT UAVslot, UINT X, UINT Y, UINT Z);
};
