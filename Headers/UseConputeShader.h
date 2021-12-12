#pragma once
#include "shaderEx.h"

#include <memory>
#include <d3d11.h>

namespace UseConputeShader
{
	// �Ă���-------------------------------------------------------------------------
	HRESULT CreateStructuredBuffer(UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBuf);

	HRESULT CreateSRVForStructuredBuffer(UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBuf, ID3D11ShaderResourceView** ppSRVOut);
	// �R���s���[�g�V�F�[�_�[����̏o�͎��Ɏg�p����A���I�[�_�[�h�A�N�Z�X�r���[���쐬����
	HRESULT CreateUAVForStructuredBuffer(UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBuf, ID3D11UnorderedAccessView** ppUAVOut);
	// �A���I�[�_�[�h�A�N�Z�X�r���[�̃o�b�t�@�̓��e�� CPU ����ǂݍ��݉\�ȃo�b�t�@�փR�s�[����
	ID3D11Buffer* CreateAndCopyToBuffer(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DDeviceContext, ID3D11Buffer* pBuffer);
	// �R���s���[�g�V�F�[�_�[�����s����
	void RunComputeShader(ID3D11ComputeShader* pComputeShader, ID3D11ShaderResourceView* pRscSRV, ID3D11UnorderedAccessView* pDstUAV, UINT X, UINT Y, UINT Z);
	// �Ă��Ƃ���---------------------------------------------------------------------
};
