#pragma once
#include "shaderEx.h"

#include <memory>
#include <d3d11.h>

namespace UseComputeShader
{
	HRESULT CreateStructuredBuffer(UINT uElementSize, UINT uCount, void* pInitData, ID3D11Buffer** ppBuf);

	// SRVと構造化バッファを作成する
	HRESULT CreateStructuredBufferAndSRV(UINT uElementSize, UINT uCount, void* pInitData, ID3D11Buffer** ppBuf, ID3D11ShaderResourceView** ppSRVOut);
	// SRVと書き込み可能な構造化バッファを作成する
	HRESULT CreateWritableStructuredBufferAndSRV(UINT uElementSize, UINT uCount, void* pInitData, ID3D11Buffer** ppBuf, ID3D11ShaderResourceView** ppSRVOut);
	// SRVを構造化バッファから作成する
	HRESULT CreateSRVfromStructuredBuffer(ID3D11Buffer* ppStructuerdBuf, ID3D11ShaderResourceView** ppSRVOut);

	// UAVと構造化バッファを作成する
	HRESULT CreateStructuredBufferAndUAV(UINT uElementSize, UINT uCount, void* pInitData, ID3D11Buffer** ppBuf, ID3D11UnorderedAccessView** ppUAVOut);
	// URVを構造化バッファから作成する
	HRESULT CreateUAVfromStructuredBuffer(ID3D11Buffer* ppStructuerdBuf, ID3D11UnorderedAccessView** ppUAVOut);

	// アンオーダードアクセスビューのバッファの内容を CPU から読み込み可能なバッファへコピーする
	void CreateAndCopyToBuffer(ID3D11Buffer* pSrcBuf,ID3D11Buffer** ppDstBuf);

	// CSを実行する
	void RunComputeShader(ID3D11ComputeShader* pComputeShader, ID3D11ShaderResourceView* pSrcSRV,UINT SRVslot, ID3D11UnorderedAccessView* pDstUAV,UINT UAVslot, UINT X, UINT Y, UINT Z);
};
