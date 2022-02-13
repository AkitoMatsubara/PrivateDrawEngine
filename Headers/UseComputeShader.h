#pragma once
#include "shaderEx.h"

#include <memory>
#include <d3d11.h>

namespace UseComputeShader
{
	HRESULT CreateStructuredBuffer(UINT uElementSize, UINT uCount, void* pInitData, ID3D11Buffer** ppBuf);

	// CS����̓��͎��Ɏg�p����SRV�ƍ\�����o�b�t�@���쐬����
	HRESULT CreateStructuredBufferAndSRV(UINT uElementSize, UINT uCount, void* pInitData, ID3D11Buffer** ppBuf, ID3D11ShaderResourceView** ppSRVOut);
	// CS����̓��͎��Ɏg�p����SRV���\�����o�b�t�@����쐬����
	HRESULT CreateSRVfromStructuredBuffer(ID3D11Buffer* ppStructuerdBuf, ID3D11ShaderResourceView** ppSRVOut);

	// CS����̏o�͎��Ɏg�p����UAV�ƍ\�����o�b�t�@���쐬����
	HRESULT CreateStructuredBufferAndUAV(UINT uElementSize, UINT uCount, void* pInitData, ID3D11Buffer** ppBuf, ID3D11UnorderedAccessView** ppUAVOut);
	// CS����̏o�͎��Ɏg�p����URV���\�����o�b�t�@����쐬����
	HRESULT CreateUAVfromStructuredBuffer(ID3D11Buffer* ppStructuerdBuf, ID3D11UnorderedAccessView** ppUAVOut);

	// �A���I�[�_�[�h�A�N�Z�X�r���[�̃o�b�t�@�̓��e�� CPU ����ǂݍ��݉\�ȃo�b�t�@�փR�s�[����
	void CreateAndCopyToBuffer(ID3D11Buffer* pSrcBuf,ID3D11Buffer** ppDstBuf);

	// CS�����s����
	void RunComputeShader(ID3D11ComputeShader* pComputeShader, ID3D11ShaderResourceView* pSrcSRV,UINT SRVslot, ID3D11UnorderedAccessView* pDstUAV,UINT UAVslot, UINT X, UINT Y, UINT Z);
};
