#pragma once
#include "shaderEx.h"

#include <memory>
#include <d3d11.h>

namespace UseConputeShader
{
	// てすと-------------------------------------------------------------------------
	HRESULT CreateStructuredBuffer(UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBuf);

	HRESULT CreateSRVForStructuredBuffer(UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBuf, ID3D11ShaderResourceView** ppSRVOut);
	// コンピュートシェーダーからの出力時に使用するアンオーダードアクセスビューを作成する
	HRESULT CreateUAVForStructuredBuffer(UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBuf, ID3D11UnorderedAccessView** ppUAVOut);
	// アンオーダードアクセスビューのバッファの内容を CPU から読み込み可能なバッファへコピーする
	ID3D11Buffer* CreateAndCopyToBuffer(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DDeviceContext, ID3D11Buffer* pBuffer);
	// コンピュートシェーダーを実行する
	void RunComputeShader(ID3D11ComputeShader* pComputeShader, ID3D11ShaderResourceView* pRscSRV, ID3D11UnorderedAccessView* pDstUAV, UINT X, UINT Y, UINT Z);
	// てすとしめ---------------------------------------------------------------------
};
