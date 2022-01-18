#pragma once
#include "shaderEx.h"

#include <memory>
#include <d3d11.h>

namespace UseComputeShader
{
	// �Ă���-------------------------------------------------------------------------
	HRESULT CreateStructuredBuffer(UINT uElementSize, UINT uCount, void* pInitData, ID3D11Buffer** ppBuf);

	// �R���s���[�g�V�F�[�_�[����̓��͎��Ɏg�p����V�F�[�_�[���\�[�X�r���[���쐬����
	HRESULT CreateSRVForStructuredBuffer(UINT uElementSize, UINT uCount, void* pInitData, ID3D11Buffer** ppBuf, ID3D11ShaderResourceView** ppSRVOut);

	// �R���s���[�g�V�F�[�_�[����̏o�͎��Ɏg�p����A���I�[�_�[�h�A�N�Z�X�r���[���쐬����
	HRESULT CreateUAVForStructuredBuffer(UINT uElementSize, UINT uCount, void* pInitData, ID3D11Buffer** ppBuf, ID3D11UnorderedAccessView** ppUAVOut);

	// �A���I�[�_�[�h�A�N�Z�X�r���[�̃o�b�t�@�̓��e�� CPU ����ǂݍ��݉\�ȃo�b�t�@�փR�s�[����
	void CreateAndCopyToBuffer(ID3D11Buffer* pSrcBuf,ID3D11Buffer** ppDstBuf);

	// �R���s���[�g�V�F�[�_�[�����s����
	void RunComputeShader(ID3D11ComputeShader* pComputeShader, ID3D11ShaderResourceView* pSrcSRV,UINT SRVslot, ID3D11UnorderedAccessView* pDstUAV,UINT UAVslot, UINT X, UINT Y, UINT Z);
	// �Ă��Ƃ���---------------------------------------------------------------------
};
