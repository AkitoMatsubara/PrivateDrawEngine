////-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//#include <d3d11.h>
//#include <stdio.h>
//#include <tchar.h>
//#include <d3dcompiler.h>
//
//#define SAFE_RELEASE(x) if( x != NULL ){ x->Release(); x = NULL; }
//
//const UINT NUM_ELEMENTS = 128;
//
//struct BUFIN_TYPE
//{
//	int i;
//	float f;
//};
//
//struct BUFOUT_TYPE
//{
//	int i;
//};
//
//// ������
//HRESULT CreateComputeDevice(ID3D11Device** ppD3DDevice, ID3D11DeviceContext** ppD3DDeviceContext, ID3D11ComputeShader** ppComputeShader)
//{
//	HRESULT hr = E_FAIL;
//
//	// �R���s���[�g�V�F�[�_�[�� SM 4 �ł����삷��炵�����T���v���ł͑Ή����Ȃ�
//	D3D_FEATURE_LEVEL FeatureLevel[] = {
//										   D3D_FEATURE_LEVEL_11_0,  // Direct3D 11.0 SM 5
//	};
//	UINT FeatureLevelLength = _countof(FeatureLevel);
//
//#if defined(DEBUG) || defined(_DEBUG)
//	// �f�o�b�O�R���p�C���̏ꍇ�A�f�o�b�O���C���[��L���ɂ���B
//	UINT createDeviceFlag = D3D11_CREATE_DEVICE_DEBUG;
//#else
//	UINT createDeviceFlag = 0;
//#endif
//
//	// �f�o�C�X���쐬����B
//	hr = D3D11CreateDevice(
//		NULL,                       // �g�p����A�_�v�^�[��ݒ�BNULL�̏ꍇ�̓f�t�H���g�̃A�_�v�^�[�B
//		D3D_DRIVER_TYPE_HARDWARE,   // D3D_DRIVER_TYPE�̂����ꂩ�B�h���C�o�[�̎�ށB
//		NULL,                       // �\�t�g�E�F�A���X�^���C�U����������DLL�ւ̃n���h���B
//		createDeviceFlag,           // D3D11_CREATE_DEVICE_FLAG�̑g�ݍ��킹�B�f�o�C�X���쐬���Ɏg�p�����p�����[�^�B
//		FeatureLevel,               // D3D_FEATURE_LEVEL�̃|�C���^
//		FeatureLevelLength,         // D3D_FEATURE_LEVEL�z��̗v�f���B
//		D3D11_SDK_VERSION,          // DirectX SDK�̃o�[�W�����B���̒l�͌Œ�B
//		ppD3DDevice,                // ���������ꂽ�f�o�C�X
//		NULL,                       // �̗p���ꂽ�t�B�[�`���[���x���B�g�p���Ȃ��̂� NULL
//		ppD3DDeviceContext          // ���������ꂽ�f�o�C�X�R���e�L�X�g
//	);
//	if (FAILED(hr))
//		goto EXIT;
//
//	ID3D10Blob* pBlob = NULL;
//
//	UINT Flag1 = D3D10_SHADER_ENABLE_STRICTNESS;
//#if defined(DEBUG) || defined(_DEBUG)
//	Flag1 |= D3D10_SHADER_OPTIMIZATION_LEVEL0;
//#else
//	Flag1 |= D3D10_SHADER_OPTIMIZATION_LEVEL3;
//#endif
//
//	// �t�@�C�������ɃG�t�F�N�g���R���p�C������
//	hr = D3DCompileFromFile(_T("../../USER/HLSL/ComputeShader01.hlsl"), NULL, NULL, "CS_Main", "cs_5_0", Flag1, 0, &pBlob, NULL);
//	if (FAILED(hr))
//		goto EXIT;
//
//	// �R���p�C���ς݃V�F�[�_�[����A�R���s���[�g�V�F�[�_�[ �I�u�W�F�N�g���쐬����
//	hr = (*ppD3DDevice)->CreateComputeShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, ppComputeShader);
//	if (FAILED(hr))
//		goto EXIT;
//
//	hr = S_OK;
//
//EXIT:
//	//SAFE_RELEASE(pBlob);
//	return hr;
//}
//
//// �R���s���[�g�V�F�[�_�[�ւ̓��͎��Ɏg�p����V�F�[�_�[���\�[�X�r���[���쐬����
//HRESULT CreateSRVForStructuredBuffer(ID3D11Device* pD3DDevice, UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBuf, ID3D11ShaderResourceView** ppSRVOut) {
//	HRESULT hr = E_FAIL;
//
//	*ppBuf = NULL;
//	*ppSRVOut = NULL;
//
//	// �\�����o�b�t�@�[���쐬����
//
//	D3D11_BUFFER_DESC BufferDesc;
//	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
//	BufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS |		// �A���I�[�_�[�h �A�N�Z�X ���\�[�X���o�C���h����
//		D3D11_BIND_SHADER_RESOURCE;								// �o�b�t�@�[���V�F�[�_�[ �X�e�[�W�Ƀo�C���h����
//	BufferDesc.ByteWidth = uElementSize * uCount;					// �o�b�t�@�T�C�Y
//	BufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;	// �\�����o�b�t�@�[�Ƃ��ă��\�[�X���쐬����
//	BufferDesc.StructureByteStride = uElementSize;					// �\�����o�b�t�@�[�̃T�C�Y (�o�C�g�P��)
//
//	// �����l��ݒ�
//	if (pInitData)
//	{
//		D3D11_SUBRESOURCE_DATA InitData;
//		InitData.pSysMem = pInitData;
//		hr = pD3DDevice->CreateBuffer(&BufferDesc, &InitData, ppBuf);
//		if (FAILED(hr))
//			goto EXIT;
//	}
//	// �����l�Ȃ��ŗ̈�̂݊m�ۂ���
//	else
//	{
//		hr = pD3DDevice->CreateBuffer(&BufferDesc, NULL, ppBuf);
//		if (FAILED(hr))
//			goto EXIT;
//	}
//
//	// �\�����o�b�t�@�[����V�F�[�_�[���\�[�X�r���[���쐬����
//
//	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
//	ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
//	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;   // �g�����ꂽ�o�b�t�@�[�ł��邱�Ƃ��w�肷��
//	SRVDesc.BufferEx.FirstElement = 0;
//	SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
//	SRVDesc.BufferEx.NumElements = uCount;                  // ���\�[�X���̗v�f�̐�
//
//	// �\�����o�b�t�@�[�����ƂɃV�F�[�_�[���\�[�X�r���[���쐬����
//	hr = pD3DDevice->CreateShaderResourceView(*ppBuf, &SRVDesc, ppSRVOut);
//	if (FAILED(hr))
//		goto EXIT;
//
//	hr = S_OK;
//EXIT:
//	return hr;
//}
//
//// �R���s���[�g�V�F�[�_�[����̏o�͎��Ɏg�p����A���I�[�_�[�h�A�N�Z�X�r���[���쐬����
//HRESULT CreateUAVForStructuredBuffer(ID3D11Device* pD3DDevice, UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBuf, ID3D11UnorderedAccessView** ppUAVOut) {
//	HRESULT hr = E_FAIL;
//
//	*ppBuf = NULL;
//	*ppUAVOut = NULL;
//
//	// �\�����o�b�t�@�[���쐬����
//
//	D3D11_BUFFER_DESC BufferDesc;
//	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
//	BufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS |          // �A���I�[�_�[�h �A�N�Z�X ���\�[�X���o�C���h����
//		D3D11_BIND_SHADER_RESOURCE;            // �o�b�t�@�[���V�F�[�_�[ �X�e�[�W�Ƀo�C���h����
//	BufferDesc.ByteWidth = uElementSize * uCount;                  // �o�b�t�@�T�C�Y
//	BufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;  // �\�����o�b�t�@�[�Ƃ��ă��\�[�X���쐬����
//	BufferDesc.StructureByteStride = uElementSize;                 // �\�����o�b�t�@�[�̃T�C�Y (�o�C�g�P��)
//
//	// �����l��ݒ�
//	if (pInitData)
//	{
//		D3D11_SUBRESOURCE_DATA InitData;
//		InitData.pSysMem = pInitData;
//		hr = pD3DDevice->CreateBuffer(&BufferDesc, &InitData, ppBuf);
//		if (FAILED(hr))
//			goto EXIT;
//	}
//	// �����l�Ȃ��ŗ̈�̂݊m�ۂ���
//	else
//	{
//		hr = pD3DDevice->CreateBuffer(&BufferDesc, NULL, ppBuf);
//		if (FAILED(hr))
//			goto EXIT;
//	}
//
//	// �\�����o�b�t�@�[����A���I�[�_�[�h�A�N�Z�X�r���[���쐬����
//
//	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
//	ZeroMemory(&UAVDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
//	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;   // �o�b�t�@�[�ł��邱�Ƃ��w�肷��
//	UAVDesc.Buffer.FirstElement = 0;
//	UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
//	UAVDesc.Buffer.NumElements = uCount;                  // ���\�[�X���̗v�f�̐�
//
//	// �\�����o�b�t�@�[�����ƂɃA���I�[�_�[�h �A�N�Z�X �r���[���쐬����
//	hr = pD3DDevice->CreateUnorderedAccessView(*ppBuf, &UAVDesc, ppUAVOut);
//	if (FAILED(hr))
//		goto EXIT;
//
//	hr = S_OK;
//EXIT:
//	return hr;
//}
//
//// �R���s���[�g�V�F�[�_�[�����s����
//void RunComputeShader(ID3D11DeviceContext* pD3DDeviceContext, ID3D11ComputeShader* pComputeShader
//	, ID3D11ShaderResourceView* pBufSRV         // ���͗p
//	, ID3D11UnorderedAccessView* pBufResultUAV  // �o�͗p
//	, UINT X, UINT Y, UINT Z) {
//	pD3DDeviceContext->CSSetShader(pComputeShader, NULL, 0);
//
//	// �V�F�[�_�[���\�[�X�r���[���R���s���[�g�V�F�[�_�[�ɐݒ�
//	pD3DDeviceContext->CSSetShaderResources(0, 1, &pBufSRV);
//
//	// �A���I�[�_�[�h�A�N�Z�X�r���[���R���s���[�g�V�F�[�_�[�ɐݒ�
//	pD3DDeviceContext->CSSetUnorderedAccessViews(0, 1, &pBufResultUAV, NULL);
//
//	// �R���s���[�g�V�F�[�_�[�����s����B���܂����킩���ĂȂ�
//	pD3DDeviceContext->Dispatch(X, Y, Z);
//
//	// �R���s���[�g�V�F�[�_�̐ݒ����
//	pD3DDeviceContext->CSSetShader(NULL, NULL, 0);
//
//	ID3D11UnorderedAccessView* ppUAViewNULL[1] = { NULL };
//	pD3DDeviceContext->CSSetUnorderedAccessViews(0, 1, ppUAViewNULL, NULL);
//
//	ID3D11ShaderResourceView* ppSRVNULL[2] = { NULL, NULL };
//	pD3DDeviceContext->CSSetShaderResources(0, 2, ppSRVNULL);
//
//	ID3D11Buffer* ppCBNULL[1] = { NULL };
//	pD3DDeviceContext->CSSetConstantBuffers(0, 1, ppCBNULL);
//}
//
//// �A���I�[�_�[�h�A�N�Z�X�r���[�̃o�b�t�@�̓��e�� CPU ����ǂݍ��݉\�ȃo�b�t�@�փR�s�[����
//ID3D11Buffer* CreateAndCopyToDebugBuf(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DDeviceContext, ID3D11Buffer* pBuffer)
//{
//	ID3D11Buffer* debugbuf = NULL;
//
//	D3D11_BUFFER_DESC BufferDesc;
//	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
//	pBuffer->GetDesc(&BufferDesc);
//	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;  // CPU ����ǂݍ��݂ł���悤�ɐݒ肷��
//	BufferDesc.Usage = D3D11_USAGE_STAGING;             // GPU ���� CPU �ւ̃f�[�^�]�� (�R�s�[) ���T�|�[�g���郊�\�[�X
//	BufferDesc.BindFlags = 0;
//	BufferDesc.MiscFlags = 0;
//	if (FAILED(pD3DDevice->CreateBuffer(&BufferDesc, NULL, &debugbuf)))
//		goto EXIT;
//
//	pD3DDeviceContext->CopyResource(debugbuf, pBuffer);
//
//EXIT:
//	return debugbuf;
//}
//
//int __cdecl mainTest() {
//	// �ϐ��p��
//	HRESULT hr = E_FAIL;
//
//	ID3D11Device* pD3DDevice = NULL;					// Direct3D11 �f�o�C�X
//	ID3D11DeviceContext* pD3DDeviceContext = NULL;      // Direct3D11 �f�o�C�X�R���e�L�X�g
//	ID3D11ComputeShader* pComputeShader = NULL;         // �R���s���[�g�V�F�[�_�[ �C���^�[�t�F�[�X
//
//	ID3D11Buffer* pBuf = NULL;                          // ���͗p�̍\�����o�b�t�@�[
//	ID3D11Buffer* pBufResult = NULL;                    // �o�͗p�̍\�����o�b�t�@�[
//
//	ID3D11ShaderResourceView* pBufSRV = NULL;           // ���͗p�̍\�����o�b�t�@�[����쐬�����V�F�[�_�[���\�[�X�r���[
//	ID3D11UnorderedAccessView* pBufResultUAV = NULL;    // �o�͗p�̍\�����o�b�t�@�[����쐬�����A���I�[�_�[�h �A�N�Z�X �r���[
//
//	BUFIN_TYPE vBufInArray[NUM_ELEMENTS];               // ���͗p�o�b�t�@�[�̔z���錾
//
//	// --Initialize--
//
//	// Direct3D�f�o�C�X�A����уR���s���[�g�V�F�[�_�[���쐬����
//	hr = CreateComputeDevice(&pD3DDevice, &pD3DDeviceContext, &pComputeShader);
//	if (FAILED(hr))
//		goto EXIT;
//
//	// ���͗p�o�b�t�@�[�ɏ����l��ݒ肷��
//	for (int i = 0; i < NUM_ELEMENTS; i++)
//	{
//		vBufInArray[i].i = i;
//		vBufInArray[i].f = static_cast<float>(NUM_ELEMENTS - 1 - i);
//	}
//
//	// �R���s���[�g�V�F�[�_�[�ւ̓��͎��Ɏg�p����V�F�[�_�[���\�[�X�r���[���쐬����
//	hr = CreateSRVForStructuredBuffer(pD3DDevice, sizeof(BUFIN_TYPE), NUM_ELEMENTS, &vBufInArray[0], &pBuf, &pBufSRV);
//	if (FAILED(hr))
//		goto EXIT;
//
//	// �R���s���[�g�V�F�[�_�[����̏o�͎��Ɏg�p����A���I�[�_�[�h�A�N�Z�X�r���[���쐬����
//	hr = CreateUAVForStructuredBuffer(pD3DDevice, sizeof(BUFOUT_TYPE), NUM_ELEMENTS, NULL, &pBufResult, &pBufResultUAV);
//	if (FAILED(hr))
//		goto EXIT;
//
//	// --Update--
//
//	// �R���s���[�g�V�F�[�_�[�����s����
//	RunComputeShader(pD3DDeviceContext, pComputeShader, pBufSRV, pBufResultUAV, NUM_ELEMENTS / 2, 1, 1);
//
//	// �A���I�[�_�[�h�A�N�Z�X�r���[�̃o�b�t�@�̓��e�� CPU ����ǂݍ��݉\�ȃo�b�t�@�փR�s�[����
//	ID3D11Buffer* debugbuf = CreateAndCopyToDebugBuf(pD3DDevice, pD3DDeviceContext, pBufResult);
//	D3D11_MAPPED_SUBRESOURCE MappedResource;
//	pD3DDeviceContext->Map(debugbuf, 0, D3D11_MAP_READ, 0, &MappedResource);
//	BUFOUT_TYPE* p = reinterpret_cast<BUFOUT_TYPE*>(MappedResource.pData);
//
//	//// �R���s���[�g�V�F�[�_�[�̌��ʂ��o��
//	//for (int i = 0; i < NUM_ELEMENTS; i++)
//	//{
//	//	printf("%d + %d = %d\n", i, NUM_ELEMENTS - 1 - i, p[i]);
//	//}
//
//	pD3DDeviceContext->Unmap(debugbuf, 0);
//	SAFE_RELEASE(debugbuf);
//
//	char str[6];
//	scanf_s("%5s", str, 6);
//
//	hr = S_OK;
//
//EXIT:
//	SAFE_RELEASE(pBufSRV);
//	SAFE_RELEASE(pBufResultUAV);
//	SAFE_RELEASE(pBufResult);
//	SAFE_RELEASE(pBuf);
//
//	SAFE_RELEASE(pComputeShader);
//	SAFE_RELEASE(pD3DDeviceContext);
//	SAFE_RELEASE(pD3DDevice);
//
//	return 0;
//}