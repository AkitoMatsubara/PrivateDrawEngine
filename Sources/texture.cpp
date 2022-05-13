
#pragma once

#include <wrl.h>
#include <WICTextureLoader.h>
#include <DirectXTex.h>

#include <string>
#include <map>


#include <memory>

#include "framework.h"
#include "texture.h"
#include "misc.h"


Texture::Texture() :ShaderResourceView(nullptr){}


Texture::~Texture()
{
	//if (ShaderResourceView)	ShaderResourceView->Release();
	//if (RenderTargetView)		RenderTargetView->Release();
	//if (DepthStencilView)		DepthStencilView->Release();
}


bool Texture::Load(const wchar_t* filename)
{
	ID3D11Device* device = FRAMEWORK->GetDevice();


	// �摜�t�@�C���ǂݍ��� DirectXTex
	DirectX::TexMetadata metadata;
	DirectX::ScratchImage image;
	HRESULT hr = DirectX::LoadFromWICFile(filename, DirectX::WIC_FLAGS_NONE, &metadata, image);

	assert(SUCCEEDED(hr));

	// �摜����SRV�쐬
	hr = DirectX::CreateShaderResourceView(device, image.GetImages(), image.GetImageCount(), image.GetMetadata(), ShaderResourceView.GetAddressOf());
	assert(SUCCEEDED(hr));

	//�e�N�X�`���f�[�^�擾
	texture2d_desc.Width  = static_cast<UINT>(metadata.width);
	texture2d_desc.Height = static_cast<UINT>(metadata.height);


	return true;
}


bool Texture::LoadMipMap(const wchar_t* filename)
{
	ID3D11Device* device = FRAMEWORK->GetDevice();
	// �摜�t�@�C���ǂݍ��� DirectXTex
	DirectX::TexMetadata metadata;
	DirectX::ScratchImage image;
	HRESULT hr = LoadFromWICFile(filename, DirectX::WIC_FLAGS_NONE, &metadata, image);
	assert(SUCCEEDED(hr));


	//�~�b�v�}�b�v����
	DirectX::ScratchImage mipChain;
	DirectX::GenerateMipMaps(
		image.GetImages(),		//���摜
		image.GetImageCount(),
		image.GetMetadata(),
		DirectX::TEX_FILTER_DEFAULT,
		0,						//�~�b�v�}�b�v�����ׂč��
		mipChain);

	// �摜����V�F�[�_���\�[�XView
	hr = CreateShaderResourceView(device, mipChain.GetImages(), mipChain.GetImageCount(), mipChain.GetMetadata(), ShaderResourceView.GetAddressOf());

	assert(SUCCEEDED(hr));
	//�e�N�X�`���f�[�^�擾
	texture2d_desc.Width  = static_cast<UINT>(metadata.width);
	texture2d_desc.Height = static_cast<UINT>(metadata.height);

	return true;
}


void Texture::Set(UINT slot, BOOL flg)
{
	ID3D11DeviceContext* device_context = FRAMEWORK->GetDeviceContext();


	if (flg == FALSE) {

		ID3D11ShaderResourceView* rtv[1] = { NULL };
		ID3D11SamplerState* ss[1] = { NULL };
		device_context->PSSetShaderResources(slot, 1, rtv);
		device_context->PSSetSamplers(slot, 1, ss);
		device_context->DSSetShaderResources(slot, 1, rtv);
		device_context->DSSetSamplers(slot, 1, ss);
		return;
	}
	if (ShaderResourceView) {
		device_context->PSSetShaderResources(slot, 1, ShaderResourceView.GetAddressOf());
		device_context->DSSetShaderResources(slot, 1, ShaderResourceView.GetAddressOf());
	}
}

bool Texture::Create(u_int width, u_int height, DXGI_FORMAT format)
{
	ID3D11Device* device = FRAMEWORK->GetDevice();

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
	HRESULT hr = S_OK;
	//	�e�N�X�`���쐬
	ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
	texture2d_desc.Width = width;
	texture2d_desc.Height = height;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = format;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	hr = device->CreateTexture2D(&texture2d_desc, NULL, &texture2d);
	assert(SUCCEEDED(hr));

	//	�����_�[�^�[�Q�b�g�r���[�쐬
	D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	ZeroMemory(&rtvd, sizeof(rtvd));
	rtvd.Format = format;
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvd.Texture2D.MipSlice = 0;
	hr = device->CreateRenderTargetView(texture2d.Get(), &rtvd, RenderTargetView.GetAddressOf());
	assert(SUCCEEDED(hr));

	//	�V�F�[�_�[���\�[�X�r���[�쐬
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(texture2d.Get(), &srvd, ShaderResourceView.GetAddressOf());

	assert(SUCCEEDED(hr));

	return true;
}

bool Texture::CreateDepth(u_int width, u_int height, DXGI_FORMAT format)
{
	ID3D11Device* device = FRAMEWORK->GetDevice();

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
	HRESULT hr = S_OK;
	//	�e�N�X�`���쐬
	ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
	texture2d_desc.Width = width;
	texture2d_desc.Height = height;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = format;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.SampleDesc.Quality = 0;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.CPUAccessFlags = 0;
	texture2d_desc.MiscFlags = 0;
	texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	hr = device->CreateTexture2D(&texture2d_desc, NULL, texture2d.GetAddressOf());
	assert(SUCCEEDED(hr));

	// �[�x�X�e���V���r���[�ݒ�
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0;
	hr = device->CreateDepthStencilView(texture2d.Get(), &dsvd, DepthStencilView.GetAddressOf());
	assert(SUCCEEDED(hr));

	//	�V�F�[�_�[���\�[�X�r���[�쐬
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(texture2d.Get(), &srvd, ShaderResourceView.GetAddressOf());

	assert(SUCCEEDED(hr));

	return true;
}


bool Texture::CreateShadowDepth(u_int width, u_int height)
{
	ID3D11Device* device = FRAMEWORK->GetDevice();

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
	HRESULT hr = S_OK;
	//	�e�N�X�`���쐬
	ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
	texture2d_desc.Width = width;
	texture2d_desc.Height = height;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = DXGI_FORMAT_R32_TYPELESS;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.SampleDesc.Quality = 0;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.CPUAccessFlags = 0;
	texture2d_desc.MiscFlags = 0;
	texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	hr = device->CreateTexture2D(&texture2d_desc, NULL, &texture2d);
	assert(SUCCEEDED(hr));

	// �[�x�X�e���V���r���[�ݒ�
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvd.Format = DXGI_FORMAT_D32_FLOAT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0;
	hr = device->CreateDepthStencilView(texture2d.Get(), &dsvd, DepthStencilView.GetAddressOf());
	assert(SUCCEEDED(hr));

	//	�V�F�[�_�[���\�[�X�r���[�쐬
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = DXGI_FORMAT_R32_FLOAT;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(texture2d.Get(), &srvd, ShaderResourceView.GetAddressOf());

	assert(SUCCEEDED(hr));

	return true;
}

bool Texture::CreateMipMap(u_int width, u_int height, DXGI_FORMAT format)
{
	ID3D11Device* device = FRAMEWORK->GetDevice();

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
	HRESULT hr = S_OK;
	//	�e�N�X�`���쐬
	ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
	texture2d_desc.Width = width;
	texture2d_desc.Height = height;
	texture2d_desc.MipLevels = 9;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = format;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	hr = device->CreateTexture2D(&texture2d_desc, NULL, &texture2d);
	assert(SUCCEEDED(hr));

	//	�����_�[�^�[�Q�b�g�r���[�쐬
	D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	ZeroMemory(&rtvd, sizeof(rtvd));
	rtvd.Format = format;
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvd.Texture2D.MipSlice = 0;
	hr = device->CreateRenderTargetView(texture2d.Get(), &rtvd, RenderTargetView.GetAddressOf());
	assert(SUCCEEDED(hr));

	//	�V�F�[�_�[���\�[�X�r���[�쐬
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = 9;
	hr = device->CreateShaderResourceView(texture2d.Get(), &srvd, ShaderResourceView.GetAddressOf());

	assert(SUCCEEDED(hr));



	return true;
}

bool Texture::CreateFontImage(u_int width, u_int height, ID3D11Texture2D* texture2d) {
	ID3D11Device* device = FRAMEWORK->GetDevice();
	ID3D11DeviceContext* immediate_context = FRAMEWORK->GetDeviceContext();

	HRESULT hr = S_OK;
	
	//// CPU�ŏ������݂��ł���e�N�X�`�����쐬 ////
	ZeroMemory(&texture2d_desc, sizeof(D3D11_TEXTURE2D_DESC));
	texture2d_desc.Width  = width;
	texture2d_desc.Height = height;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;		// RGBA(255,255,255,255)�^�C�v
	texture2d_desc.Usage = D3D11_USAGE_DYNAMIC;				// ���I CPU�������݉\
	texture2d_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;	// �V�F�[�_���\�[�X
	texture2d_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// CPU���珑�����݃A�N�Z�X��
	texture2d_desc.SampleDesc.Count = 1;					// �T���v�����O��1�s�N�Z���̂�

	hr = device->CreateTexture2D(&texture2d_desc, NULL, &texture2d);
	if (FAILED(hr)) { MessageBox(0, L"Font Texture Create Failure", NULL, MB_OK); }

	//	�V�F�[�_�[���\�[�X�r���[�쐬
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = texture2d_desc.MipLevels;
	hr = device->CreateShaderResourceView(texture2d, &srvd, ShaderResourceView.GetAddressOf());	// SRV�̍쐬 ����ŃV�F�[�_�ɓn����悤�ɂȂ�

	assert(SUCCEEDED(hr));

	return true;

}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
HRESULT Texture::LoadTextureFromFile(const wchar_t* filename, ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc) {
	ID3D11Device* device = FRAMEWORK->GetDevice();

	HRESULT hr{ S_OK };

	static std::map <std::wstring, Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>> cashe;

	Microsoft::WRL::ComPtr<ID3D11Resource> resource{};

	// �摜�t�@�C���̃��[�h��SRV�I�u�W�F�N�g�̐���
	auto it = cashe.find(filename);	// �N���X�̃e���v���[�g�p�����[�^key_type�^�̃L�[���󂯎���Č�������B������Ȃ�������end()��Ԃ�
	if (it != cashe.end()) {			// end()����Ȃ���filename������������
		*shader_resource_view = it->second.Get();	// second��y�I�u�W�F�N�g�A�����ł�SRV�B���Ȃ݂�x�I�u�W�F�N�g��first
		(*shader_resource_view)->AddRef();			// �J�v�Z�������ꂽ�C���^�[�t�F�C�X�|�C���^���Ăяo��
		(*shader_resource_view)->GetResource(resource.GetAddressOf());
	}
	else {
		hr = DirectX::CreateWICTextureFromFile(device, filename, resource.GetAddressOf(), shader_resource_view);	// resource��srv���쐬�����
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		cashe.insert(std::make_pair(filename, *shader_resource_view));	// insert�Fmap�R���e�i�̊g��
	}

	// �e�N�X�`�����̎擾
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d{};
	hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());	// ����̃C���^�[�t�F�C�X���T�|�[�g���Ă��邩�𔻕ʂ���
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	texture2d->GetDesc(texture2d_desc);

	return hr;
}

// �_�~�[�e�N�X�`���̍쐬
HRESULT Texture::MakeDummyTexture(ID3D11ShaderResourceView** shader_resource_view, DWORD value/*0xAABBGGRR*/, UINT dimension) {
	ID3D11Device* device = FRAMEWORK->GetDevice();

	HRESULT hr{ S_OK };

	D3D11_TEXTURE2D_DESC texture2d_desc{};
	texture2d_desc.Width = dimension;
	texture2d_desc.Height= dimension;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize= 1;
	texture2d_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.SampleDesc.Quality = 0;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	size_t texels = dimension * dimension;
	std::unique_ptr<DWORD[]>system{std::make_unique<DWORD[]>(texels) };
	for (size_t i = 0; i < texels; i++) {
		system[i] = value;
	}

	D3D11_SUBRESOURCE_DATA subresource_data{};
	subresource_data.pSysMem = system.get();
	subresource_data.SysMemPitch = sizeof(DWORD) * dimension;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
	hr = device->CreateTexture2D(&texture2d_desc, &subresource_data, &texture2d);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{};
	shader_resource_view_desc.Format = texture2d_desc.Format;
	shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shader_resource_view_desc.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(texture2d.Get(), &shader_resource_view_desc, shader_resource_view);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	return hr;

}

