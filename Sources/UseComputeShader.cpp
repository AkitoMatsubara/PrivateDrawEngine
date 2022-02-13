#include "UseComputeShader.h"
#include "framework.h"


// �\�����o�b�t�@���쐬����
HRESULT UseComputeShader::CreateStructuredBuffer(UINT uElementSize, UINT uCount, void* pInitData, ID3D11Buffer** ppBuf)
{
	ID3D11Device* pD3DDevice = FRAMEWORK->GetDevice();
	HRESULT hr = E_FAIL;

	// �\�����o�b�t�@�[���쐬����
	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	BufferDesc.BindFlags =	D3D11_BIND_UNORDERED_ACCESS |			// �A���I�[�_�[�h �A�N�Z�X ���\�[�X���o�C���h����
							D3D11_BIND_SHADER_RESOURCE;				// �o�b�t�@�[���V�F�[�_�[ �X�e�[�W�Ƀo�C���h����
	BufferDesc.ByteWidth = uElementSize * uCount;					// �o�b�t�@�T�C�Y
	BufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;	// �\�����o�b�t�@�[�Ƃ��ă��\�[�X���쐬����
	BufferDesc.StructureByteStride = uElementSize;					// �\�����o�b�t�@�[�̃T�C�Y (�o�C�g�P��)

	// �����l��ݒ�
	if (pInitData)
	{
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = pInitData;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;
		hr = pD3DDevice->CreateBuffer(&BufferDesc, &InitData, ppBuf);
	}
	// �����l�Ȃ��ŗ̈�̂݊m�ۂ���
	else
	{
		hr = pD3DDevice->CreateBuffer(&BufferDesc, nullptr, ppBuf);
	}
	if (FAILED(hr))_ASSERT_EXPR_A(false, "FAILED CreateStructuredBuffer");

	return hr;
}

// CS����̓��͎��Ɏg�p����SRV�ƍ\�����o�b�t�@�쐬����
HRESULT UseComputeShader::CreateStructuredBufferAndSRV(UINT uElementSize, UINT uCount, void* pInitData, ID3D11Buffer** ppBuf, ID3D11ShaderResourceView** ppSRVOut) {
	ID3D11Device* pD3DDevice = FRAMEWORK->GetDevice();
	HRESULT hr = E_FAIL;
	*ppBuf = nullptr;
	*ppSRVOut = nullptr;

	// �\�����o�b�t�@�[���쐬����
	hr = UseComputeShader::CreateStructuredBuffer(uElementSize, uCount, pInitData, ppBuf);
	if (FAILED(hr))_ASSERT_EXPR_A(false, "FAILED CreateStructuredBuffer");

	// �\�����o�b�t�@�[����V�F�[�_�[���\�[�X�r���[���쐬����
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;   // SRV�ł��邱�Ƃ��w�肷��
	SRVDesc.BufferEx.FirstElement = 0;
	SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	SRVDesc.BufferEx.NumElements = uCount;                  // ���\�[�X���̗v�f�̐�

	// �\�����o�b�t�@�[�����ƂɃV�F�[�_�[���\�[�X�r���[���쐬����
	hr = pD3DDevice->CreateShaderResourceView(*ppBuf, &SRVDesc, ppSRVOut);
	if (FAILED(hr))	_ASSERT_EXPR_A(false, "FAILED CreateShaderResourceView");

	return hr;
}

// CS����̓��͎��Ɏg�p����SRV���\�����o�b�t�@����쐬����
HRESULT UseComputeShader::CreateSRVfromStructuredBuffer(ID3D11Buffer* ppBuf, ID3D11ShaderResourceView** ppSRVOut) {
	ID3D11Device* pD3DDevice = FRAMEWORK->GetDevice();
	HRESULT hr = E_FAIL;
	*ppSRVOut = nullptr;

	D3D11_BUFFER_DESC BufDesc;
	ZeroMemory(&BufDesc, sizeof(D3D11_BUFFER_DESC));
	ppBuf->GetDesc(&BufDesc);	// �T�C�Y�̌v�Z�̂��߂ɍ\�����o�b�t�@��Desc���擾

	// �\�����o�b�t�@�[����V�F�[�_�[���\�[�X�r���[���쐬����
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;   // SRV�ł��邱�Ƃ��w�肷��
	SRVDesc.BufferEx.FirstElement = 0;
	SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	SRVDesc.BufferEx.NumElements = BufDesc.ByteWidth / BufDesc.StructureByteStride; // ���\�[�X���̗v�f�̐�  ��������\���̂̃T�C�Y������Ό�����ɓ���

	// �\�����o�b�t�@�[�����ƂɃV�F�[�_�[���\�[�X�r���[���쐬����
	hr = pD3DDevice->CreateShaderResourceView(ppBuf, &SRVDesc, ppSRVOut);
	if (FAILED(hr))	_ASSERT_EXPR_A(false, "FAILED CreateShaderResourceView");

	return hr;
}

// CS����̏o�͎��Ɏg�p����UAV�ƍ\�����o�b�t�@�쐬����
HRESULT UseComputeShader::CreateStructuredBufferAndUAV(UINT uElementSize, UINT uCount, void* pInitData, ID3D11Buffer** ppBuf, ID3D11UnorderedAccessView** ppUAVOut) {
	ID3D11Device* pD3DDevice = FRAMEWORK->GetDevice();
	HRESULT hr = E_FAIL;
	*ppBuf = nullptr;
	*ppUAVOut = nullptr;

	// �\�����o�b�t�@�[���쐬����
	hr = UseComputeShader::CreateStructuredBuffer(uElementSize, uCount, pInitData, ppBuf);
	if (FAILED(hr))	_ASSERT_EXPR_A(false, "FAILED CreateStructuredBuffer");

	// �\�����o�b�t�@�[����A���I�[�_�[�h�A�N�Z�X�r���[���쐬����
	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
	ZeroMemory(&UAVDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;   // UAV�ł��邱�Ƃ��w�肷��
	UAVDesc.Buffer.FirstElement = 0;
	UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	UAVDesc.Buffer.NumElements = uCount;                  // ���\�[�X���̗v�f�̐�

	// �\�����o�b�t�@�[�����ƂɃA���I�[�_�[�h �A�N�Z�X �r���[���쐬����
	hr = pD3DDevice->CreateUnorderedAccessView(*ppBuf, &UAVDesc, ppUAVOut);
	if (FAILED(hr))	_ASSERT_EXPR_A(false, "FAILED CreateUnorderedAccessView");

	return hr;
}

// CS����̏o�͎��Ɏg�p����UAV�ƍ\�����o�b�t�@�쐬����
HRESULT UseComputeShader::CreateUAVfromStructuredBuffer(ID3D11Buffer* ppBuf, ID3D11UnorderedAccessView** ppUAVOut) {
	ID3D11Device* pD3DDevice = FRAMEWORK->GetDevice();
	HRESULT hr = E_FAIL;
	*ppUAVOut = nullptr;

	D3D11_BUFFER_DESC BufDesc;
	ZeroMemory(&BufDesc, sizeof(D3D11_BUFFER_DESC));
	ppBuf->GetDesc(&BufDesc);	// �T�C�Y�̌v�Z�̂��߂ɍ\�����o�b�t�@��Desc���擾

	// �\�����o�b�t�@�[����A���I�[�_�[�h�A�N�Z�X�r���[���쐬����
	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
	ZeroMemory(&UAVDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;   // UAV�ł��邱�Ƃ��w�肷��
	UAVDesc.Buffer.FirstElement = 0;
	UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	UAVDesc.Buffer.NumElements = BufDesc.ByteWidth / BufDesc.StructureByteStride; // ���\�[�X���̗v�f�̐�  ��������\���̂̃T�C�Y������Ό�����ɓ���

	// �\�����o�b�t�@�[�����ƂɃA���I�[�_�[�h �A�N�Z�X �r���[���쐬����
	hr = pD3DDevice->CreateUnorderedAccessView(ppBuf, &UAVDesc, ppUAVOut);
	if (FAILED(hr))	_ASSERT_EXPR_A(false, "FAILED CreateUnorderedAccessView");

	return hr;
}

// �A���I�[�_�[�h�A�N�Z�X�r���[�̃o�b�t�@(GPU)�̓��e��CPU����ǂݍ��݉\�ȃo�b�t�@���쐬���ăR�s�[����
void UseComputeShader::CreateAndCopyToBuffer(ID3D11Buffer* pSrcBuf,ID3D11Buffer** ppDstBuf)
{
	ID3D11Device* device = FRAMEWORK->GetDevice();
	ID3D11DeviceContext* immediate_context = FRAMEWORK->GetDeviceContext();
	HRESULT hr = E_FAIL;

	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	pSrcBuf->GetDesc(&BufferDesc);	// ������BUFFER_DESC���擾
	// ���g��CPU�œǂݍ��ނ߂�悤�ɂ�DESC���Đݒ�
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;  // CPU ����ǂݍ��݂ł���悤�ɐݒ肷��
	BufferDesc.Usage = D3D11_USAGE_STAGING;             // GPU ���� CPU �ւ̃f�[�^�]�� (�R�s�[) ���T�|�[�g���郊�\�[�X CPU�ǂݏ�����p
	BufferDesc.BindFlags = 0;
	BufferDesc.MiscFlags = 0;

	// �Đݒ肵��DESC��buffer���쐬
	hr = device->CreateBuffer(&BufferDesc, nullptr, ppDstBuf);
	if (FAILED(hr))	_ASSERT_EXPR_A(false, "FAILED CreateReadBackBuffer. Not Set Shader?");

	// ����buffer�̃f�[�^���쐬����buffer�ɃR�s�[
	immediate_context->CopyResource(*ppDstBuf, pSrcBuf);

}

void UseComputeShader::RunComputeShader(ID3D11ComputeShader* pComputeShader, ID3D11ShaderResourceView* pSrcSRV, UINT SRVslot, ID3D11UnorderedAccessView* pDstUAV, UINT UAVslot, UINT X, UINT Y, UINT Z) {
	ID3D11DeviceContext* immediate_context = FRAMEWORK->GetDeviceContext();

	// �g��CS��ݒ�
	immediate_context->CSSetShader(pComputeShader, nullptr, 0);

	// �V�F�[�_�[���\�[�X�r���[���R���s���[�g�V�F�[�_�[�ɐݒ�
	immediate_context->CSSetShaderResources(SRVslot, 1, &pSrcSRV);

	// �A���I�[�_�[�h�A�N�Z�X�r���[���R���s���[�g�V�F�[�_�[�ɐݒ�
	immediate_context->CSSetUnorderedAccessViews(UAVslot, 1, &pDstUAV, nullptr);

	// �R���s���[�g�V�F�[�_�[�����s����B�X���b�h���Ƃ��悤�킩��񂪂���
	immediate_context->Dispatch(X, Y, Z);

	// �R���s���[�g�V�F�[�_�̐ݒ����
	immediate_context->CSSetShader(nullptr, nullptr, 0);

	ID3D11UnorderedAccessView* ppUAViewNULL[1] = { nullptr };
	immediate_context->CSSetUnorderedAccessViews(0, 1, ppUAViewNULL, nullptr);

	ID3D11ShaderResourceView* ppSRVNULL[2] = { nullptr, nullptr };
	immediate_context->CSSetShaderResources(0, 2, ppSRVNULL);

	ID3D11Buffer* ppCBNULL[1] = { nullptr };
	immediate_context->CSSetConstantBuffers(0, 1, ppCBNULL);
}