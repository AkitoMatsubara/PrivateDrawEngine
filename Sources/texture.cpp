
#pragma once
#include "texture.h"

#include <WICTextureLoader.h>
using namespace DirectX;

#include <wrl.h>
using namespace Microsoft::WRL;

#include <string>
#include <map>
using namespace std;


#include "misc.h"

static map < wstring, ComPtr <ID3D11ShaderResourceView>> resources;


HRESULT load_texture_from_file(ID3D11Device* device, const wchar_t* filename, ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc) {
	HRESULT hr{ S_OK };
	ComPtr<ID3D11Resource> resource{};

	// �摜�t�@�C���̃��[�h��SRV�I�u�W�F�N�g�̐���
	auto it = resources.find(filename);	// �N���X�̃e���v���[�g�p�����[�^key_type�^�̃L�[���󂯎���Č�������B������Ȃ�������end()��Ԃ�
	if (it != resources.end()) {	// end()����Ȃ���filename������������
		*shader_resource_view = it->second.Get();	// second��y�I�u�W�F�N�g�A�����ł�SRV�B���Ȃ݂�x�I�u�W�F�N�g��first
		(*shader_resource_view)->AddRef();	// �J�v�Z�������ꂽ�C���^�[�t�F�C�X�|�C���^���Ăяo��
		(*shader_resource_view)->GetResource(resource.GetAddressOf());
	}
	else {
		hr = CreateWICTextureFromFile(device, filename, resource.GetAddressOf(), shader_resource_view);	// resource��srv���쐬�����
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		resources.insert(make_pair(filename, *shader_resource_view));	// insert�Fmap�R���e�i�̊g��
	}

	// �e�N�X�`�����̎擾
	ComPtr<ID3D11Texture2D> texture2d{};
	hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());	// ����̃C���^�[�t�F�C�X���T�|�[�g���Ă��邩�𔻕ʂ���
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	texture2d->GetDesc(texture2d_desc);

	return hr;
}

void rerease_all_textures() {
	resources.clear();
}