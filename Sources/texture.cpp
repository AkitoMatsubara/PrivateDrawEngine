
#pragma once

#include <WICTextureLoader.h>

#include <wrl.h>

#include <string>
#include <map>


#include <memory>

#include "framework.h"
#include "texture.h"
#include "misc.h"
static std::map <std::wstring, Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>> resources;


HRESULT load_texture_from_file(const wchar_t* filename, ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc) {
	ID3D11Device* device = FRAMEWORK->GetDevice();

	HRESULT hr{ S_OK };
	Microsoft::WRL::ComPtr<ID3D11Resource> resource{};

	// �摜�t�@�C���̃��[�h��SRV�I�u�W�F�N�g�̐���
	auto it = resources.find(filename);	// �N���X�̃e���v���[�g�p�����[�^key_type�^�̃L�[���󂯎���Č�������B������Ȃ�������end()��Ԃ�
	if (it != resources.end()) {		// end()����Ȃ���filename������������
		*shader_resource_view = it->second.Get();	// second��y�I�u�W�F�N�g�A�����ł�SRV�B���Ȃ݂�x�I�u�W�F�N�g��first
		(*shader_resource_view)->AddRef();			// �J�v�Z�������ꂽ�C���^�[�t�F�C�X�|�C���^���Ăяo��
		(*shader_resource_view)->GetResource(resource.GetAddressOf());
	}
	else {
		hr = DirectX::CreateWICTextureFromFile(device, filename, resource.GetAddressOf(), shader_resource_view);	// resource��srv���쐬�����
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		resources.insert(std::make_pair(filename, *shader_resource_view));	// insert�Fmap�R���e�i�̊g��
	}

	// �e�N�X�`�����̎擾
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d{};
	hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());	// ����̃C���^�[�t�F�C�X���T�|�[�g���Ă��邩�𔻕ʂ���
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	texture2d->GetDesc(texture2d_desc);

	return hr;
}

// �_�~�[�e�N�X�`���̍쐬
HRESULT make_dummy_texture(ID3D11ShaderResourceView** shader_resource_view, DWORD value/*0xAABBGGRR*/, UINT dimension) {
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

void rerease_all_textures() {
	resources.clear();
}