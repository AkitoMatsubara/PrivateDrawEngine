#define _CRT_SECURE_NO_WARNINGS

#include "framework.h"
#include "ShaderEx.h"

#include<memory>

#include<map>
#include <wrl.h>
#include <d3dcompiler.h>
#include <assert.h>
#include <stdlib.h>
#include <string>

using namespace Microsoft::WRL;

D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
{
	//{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT	,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//{ "COLOR"   , 0, DXGI_FORMAT_R32G32B32A32_FLOAT	,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT		,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//{ "NORMAL"  , 0, DXGI_FORMAT_R32G32B32_FLOAT	,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//{ "PARAM"   , 0, DXGI_FORMAT_R32G32B32A32_FLOAT	,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA	, 0 },
	{ "NORMAL"  , 0, DXGI_FORMAT_R32G32B32_FLOAT,	 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA	, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA	, 0 },
	{ "COLOR"   , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA	, 0 },
	{ "PARAM"   , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA	, 0 },

};


HRESULT ShaderEx::create_vs_from_cso(ID3D11Device* device,	// shaderEx.cpp�ł����g�p���Ȃ��̂Ōy�ʉ���}�邽�߈����Ƃ��Ďg�p�Ǝv����
	const char* cso_name, ID3D11VertexShader** vertex_shader, ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements){

	struct set_of_vertex_shader_and_input_layout{
		set_of_vertex_shader_and_input_layout(ID3D11VertexShader* vertex_shader, ID3D11InputLayout* input_layout) : vertex_shader(vertex_shader), input_layout(input_layout) {}
		ComPtr<ID3D11VertexShader> vertex_shader;
		ComPtr<ID3D11InputLayout> input_layout;
	};
	static std::map<std::string, set_of_vertex_shader_and_input_layout> cache;

	auto it = cache.find(cso_name);
	if (it != cache.end())
	{
		*vertex_shader = it->second.vertex_shader.Get();
		(*vertex_shader)->AddRef();
		*input_layout = it->second.input_layout.Get();
		(*input_layout)->AddRef();
		return S_OK;
	}

	// ���_�V�F�[�_�[�I�u�W�F�N�g�̐���

	FILE* fp = nullptr;
	errno_t error;
	error = fopen_s(&fp, cso_name, "rb");	// �t�@�C���|�C���^�A�t�@�C�����Arb�F�ǂݎ���p�̃o�C�i�����[�h
	if (error != 0)assert("CSO File not found");

	fseek(fp, 0, SEEK_END);	// �t�@�C���|�C���^�A�ړ��o�C�g���A�t�@�C���̐擪(_SET)�A���݈ʒu(_CUR)�A�I�[(_END)
	long cso_sz = ftell(fp);	// �t�@�C���̓ǂݏ����ʒu���擾
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);	// unique_ptr��make_unique�Ŏ��̐���
	fread(cso_data.get(), cso_sz, 1, fp);	// �ǂݍ��݃f�[�^�̊i�[��A�ǂݍ��݃f�[�^�̃o�C�g�����A�ǂݍ��݃f�[�^�̐��A�t�@�C���|�C���^
	fclose(fp);

	HRESULT hr = device->CreateVertexShader(cso_data.get(), cso_sz, nullptr, vertex_shader);	// �V�F�[�_�̃|�C���^�[�A�V�F�[�_�[�T�C�Y�Adynamic linkage�Ŏg���|�C���^�A�쐬�����o�b�t�@��ۑ�����|�C���^
	assert(SUCCEEDED(hr));
	//	�����FIL�̍\�����AIL�̗v�f���AVS�̃|�C���^�AVS�̃T�C�Y�A�쐬����IL��ۑ�����|�C���^
	hr = device->CreateInputLayout(input_element_desc, num_elements, cso_data.get(), cso_sz, input_layout);
	assert(SUCCEEDED(hr));

	cache.insert(std::make_pair(cso_name, set_of_vertex_shader_and_input_layout(*vertex_shader, *input_layout)));

	return hr;
}

HRESULT ShaderEx::create_ps_from_cso(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader)
{
	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11PixelShader>> cache;
	auto it = cache.find(cso_name);
	if (it != cache.end())
	{
		//it->second.Attach(*pixel_shader);
		*pixel_shader = it->second.Get();
		(*pixel_shader)->AddRef();
		return S_OK;
	}

	FILE* fp = nullptr;
	errno_t error;

	error = fopen_s(&fp, cso_name, "rb");
	if (error != 0) assert("CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	HRESULT hr = device->CreatePixelShader(cso_data.get(), cso_sz, nullptr, pixel_shader);
	assert(SUCCEEDED(hr));

	cache.insert(std::make_pair(cso_name, *pixel_shader));

	return hr;
}

HRESULT ShaderEx::create_hs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11HullShader** hull_shader)
{
	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11HullShader>> cache;
	auto it = cache.find(cso_name);
	if (it != cache.end())
	{
		*hull_shader = it->second.Get();
		(*hull_shader)->AddRef();
		return S_OK;
	}

	FILE* fp = nullptr;
	errno_t error;

	error = fopen_s(&fp, cso_name, "rb");
	assert("CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	HRESULT hr = device->CreateHullShader(cso_data.get(), cso_sz, nullptr, hull_shader);
	assert(SUCCEEDED(hr));

	cache.insert(std::make_pair(cso_name, *hull_shader));

	return hr;
}

HRESULT ShaderEx::create_ds_from_cso(ID3D11Device* device, const char* cso_name, ID3D11DomainShader** domain_shader)
{
	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11DomainShader>> cache;
	auto it = cache.find(cso_name);
	if (it != cache.end())
	{
		*domain_shader = it->second.Get();
		(*domain_shader)->AddRef();
		return S_OK;
	}

	FILE* fp = nullptr;
	errno_t error;

	error = fopen_s(&fp, cso_name, "rb");
	assert("CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	HRESULT hr = device->CreateDomainShader(cso_data.get(), cso_sz, nullptr, domain_shader);
	assert(SUCCEEDED(hr));

	cache.insert(std::make_pair(cso_name, *domain_shader));

	return hr;
}

HRESULT ShaderEx::create_gs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11GeometryShader** geometry_shader)
{
	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11GeometryShader>> cache;
	auto it = cache.find(cso_name);
	if (it != cache.end())
	{
		*geometry_shader = it->second.Get();
		(*geometry_shader)->AddRef();
		return S_OK;
	}

	FILE* fp = nullptr;
	errno_t error;

	error = fopen_s(&fp, cso_name, "rb");
	assert("CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	HRESULT hr = device->CreateGeometryShader(cso_data.get(), cso_sz, nullptr, geometry_shader);
	assert(SUCCEEDED(hr));

	cache.insert(std::make_pair(cso_name, *geometry_shader));

	return hr;
}

HRESULT ShaderEx::create_cs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11ComputeShader** compute_shader)
{
	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11ComputeShader>> cache;
	auto it = cache.find(cso_name);
	if (it != cache.end())
	{
		*compute_shader = it->second.Get();
		(*compute_shader)->AddRef();
		return S_OK;
	}

	FILE* fp = nullptr;
	errno_t error;

	error = fopen_s(&fp, cso_name, "rb");
	assert("CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	HRESULT hr = device->CreateComputeShader(cso_data.get(), cso_sz, nullptr, compute_shader);
	assert(SUCCEEDED(hr));

	cache.insert(std::make_pair(cso_name, *compute_shader));

	return hr;
}


bool ShaderEx::Create(const WCHAR* vsfilename, const WCHAR* psfilename)
{
	ID3D11Device* device = FRAMEWORK->GetDevice();
	ID3D11DeviceContext* device_context = FRAMEWORK->GetDeviceContext();
	UINT numElements = sizeof(input_element_desc) / sizeof(input_element_desc[0]);
	//���C�h��������}���`�o�C�g�����֕ϊ�
	char fullname[256];
	memset(fullname, NULL, sizeof(fullname));
	wcstombs(fullname, vsfilename, wcslen(vsfilename));
	sprintf(fullname, "%s%s", fullname, ".cso");
	create_vs_from_cso(device, fullname, VS.GetAddressOf(), InputLayout.GetAddressOf(), input_element_desc, numElements);

	memset(fullname, NULL, sizeof(fullname));
	wcstombs(fullname, psfilename, wcslen(psfilename));
	sprintf(fullname, "%s%s", fullname, ".cso");
	create_ps_from_cso(device, fullname, PS.GetAddressOf());

	// ���̓��C�A�E�g�ݒ�
	device_context->IASetInputLayout(InputLayout.Get());


	return false;
}

bool ShaderEx::Create(const WCHAR* vsfilename, const WCHAR* gsfilename, const WCHAR* psfilename)
{
	ID3D11Device* device = FRAMEWORK->GetDevice();
	ID3D11DeviceContext* device_context = FRAMEWORK->GetDeviceContext();
	UINT numElements = sizeof(input_element_desc) / sizeof(input_element_desc[0]);
	//���C�h��������}���`�o�C�g�����֕ϊ�
	char fullname[256];
	memset(fullname, NULL, sizeof(fullname));
	wcstombs(fullname, vsfilename, wcslen(vsfilename));
	sprintf(fullname, "%s%s", fullname, ".cso");
	create_vs_from_cso(device, fullname, VS.GetAddressOf(), InputLayout.GetAddressOf(), input_element_desc, numElements);

	memset(fullname, NULL, sizeof(fullname));
	wcstombs(fullname, gsfilename, wcslen(gsfilename));
	sprintf(fullname, "%s%s", fullname, ".cso");
	create_gs_from_cso(device, fullname, GS.GetAddressOf());

	memset(fullname, NULL, sizeof(fullname));
	wcstombs(fullname, psfilename, wcslen(psfilename));
	sprintf(fullname, "%s%s", fullname, ".cso");
	create_ps_from_cso(device, fullname, PS.GetAddressOf());

	// ���̓��C�A�E�g�ݒ�
	device_context->IASetInputLayout(InputLayout.Get());


	return false;
}

bool ShaderEx::Create(const WCHAR* vsfilename, const WCHAR* hsfilename, const WCHAR* dsfilename, const WCHAR* psfilename)
{
	ID3D11Device* device = FRAMEWORK->GetDevice();
	ID3D11DeviceContext* device_context = FRAMEWORK->GetDeviceContext();
	UINT numElements = sizeof(input_element_desc) / sizeof(input_element_desc[0]);

	//���C�h��������}���`�o�C�g�����֕ϊ�
	char fullname[256];
	memset(fullname, NULL, sizeof(fullname));
	wcstombs(fullname, vsfilename, wcslen(vsfilename));
	sprintf(fullname, "%s%s", fullname, ".cso");
	create_vs_from_cso(device, fullname, VS.GetAddressOf(), InputLayout.GetAddressOf(), input_element_desc, numElements);

	memset(fullname, NULL, sizeof(fullname));
	wcstombs(fullname, hsfilename, wcslen(hsfilename));
	sprintf(fullname, "%s%s", fullname, ".cso");
	create_hs_from_cso(device, fullname, HS.GetAddressOf());

	memset(fullname, NULL, sizeof(fullname));
	wcstombs(fullname, dsfilename, wcslen(dsfilename));
	sprintf(fullname, "%s%s", fullname, ".cso");
	create_ds_from_cso(device, fullname, DS.GetAddressOf());

	memset(fullname, NULL, sizeof(fullname));
	wcstombs(fullname, psfilename, wcslen(psfilename));
	sprintf(fullname, "%s%s", fullname, ".cso");
	create_ps_from_cso(device, fullname, PS.GetAddressOf());


	// ���̓��C�A�E�g�ݒ�
	device_context->IASetInputLayout(InputLayout.Get());

	return false;
}

bool ShaderEx::Create(const WCHAR* vsfilename, const WCHAR* hsfilename, const WCHAR* dsfilename, const WCHAR* gsfilename, const WCHAR* psfilename)
{
	ID3D11Device* device = FRAMEWORK->GetDevice();
	ID3D11DeviceContext* device_context = FRAMEWORK->GetDeviceContext();
	UINT numElements = sizeof(input_element_desc) / sizeof(input_element_desc[0]);

	//���C�h��������}���`�o�C�g�����֕ϊ�
	char fullname[256];
	//memset(fullname, NULL, sizeof(fullname));
	//wcstombs(fullname, vsfilename, wcslen(vsfilename));
	//sprintf(fullname, "%s%s", fullname, ".cso");
	create_vs_from_cso(device, "sprite_vs.cso", VS.GetAddressOf(), InputLayout.GetAddressOf(), input_element_desc, numElements);

	memset(fullname, NULL, sizeof(fullname));
	wcstombs(fullname, hsfilename, wcslen(hsfilename));
	sprintf(fullname, "%s%s", fullname, ".cso");
	create_hs_from_cso(device, fullname, HS.GetAddressOf());

	memset(fullname, NULL, sizeof(fullname));
	wcstombs(fullname, dsfilename, wcslen(dsfilename));
	sprintf(fullname, "%s%s", fullname, ".cso");
	create_ds_from_cso(device, fullname, DS.GetAddressOf());

	memset(fullname, NULL, sizeof(fullname));
	wcstombs(fullname, gsfilename, wcslen(gsfilename));
	sprintf(fullname, "%s%s", fullname, ".cso");
	create_gs_from_cso(device, fullname, GS.GetAddressOf());

	memset(fullname, NULL, sizeof(fullname));
	wcstombs(fullname, psfilename, wcslen(psfilename));
	sprintf(fullname, "%s%s", fullname, ".cso");
	create_ps_from_cso(device, fullname, PS.GetAddressOf());

	// ���̓��C�A�E�g�ݒ�
	device_context->IASetInputLayout(InputLayout.Get());

	return false;
}

bool ShaderEx::Create(const WCHAR* csfilename)
{
	ID3D11Device* device = FRAMEWORK->GetDevice();
	ID3D11DeviceContext* device_context = FRAMEWORK->GetDeviceContext();
	UINT numElements = sizeof(input_element_desc) / sizeof(input_element_desc[0]);

	//���C�h��������}���`�o�C�g�����֕ϊ�
	char fullname[256];
	memset(fullname, NULL, sizeof(fullname));
	wcstombs(fullname, csfilename, wcslen(csfilename));
	sprintf(fullname, "%s%s", fullname, ".cso");
	create_cs_from_cso(device, fullname, CS.GetAddressOf());

	return false;
}
