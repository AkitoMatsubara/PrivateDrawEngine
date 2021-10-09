#include <d3d11.h>
#include "framework.h"
#include "Sampler.h"

Sampler::Sampler(D3D11_SAMPLER_DESC sd) {
	ID3D11Device* device = FRAMEWORK->GetDevice();

	ZeroMemory(&sd, sizeof(sd));
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;	// �g�k���̐F�̎擾���@ https://msdn.microsoft.com/ja-jp/library/ee416129(v=vs.85).aspx
	sd.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;		// UV���W��0�`1�͈̔͊O�̏ꍇ�̐F�̎擾���@
	sd.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
	//FLOAT color[4] = { r,g,b,a };
	//sd.BorderColor = color;

	sd.MinLOD = 0;
	sd.MaxLOD = D3D11_FLOAT32_MAX;

	HRESULT hr = device->CreateSamplerState(&sd, sampler_state.GetAddressOf());
	assert(SUCCEEDED(hr));

}

Sampler::Sampler(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE Address_mode, FLOAT color[4]) {
	ID3D11Device* device = FRAMEWORK->GetDevice();
	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Filter = filter;			// �g�k���̐F�̎擾���@ https://msdn.microsoft.com/ja-jp/library/ee416129(v=vs.85).aspx
	sd.AddressU = Address_mode;	// UV���W��0�`1�͈̔͊O�̏ꍇ�̐F�̎擾���@
	sd.AddressV = Address_mode;
	sd.AddressW = Address_mode;
	sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
	// color���ݒ肵�Ă����
	if (color) {
		memcpy(sd.BorderColor, color, sizeof(FLOAT) * 4);
	}

	sd.MinLOD = 0;					// �A�N�Z�X�\�ȃ~�j�}�b�v�̉����l
	sd.MaxLOD = D3D11_FLOAT32_MAX;	// �A�N�Z�X�\�ȃ~�j�}�b�v�̏���l

	HRESULT hr = device->CreateSamplerState(&sd, sampler_state.GetAddressOf());
	assert(SUCCEEDED(hr));

	//----------------------------------------------------------------
	//D3D11_SAMPLER_DESC sampler_desc;
	//sampler_desc.MipLODBias = 0;							// �v�Z���ꂽ�~�b�v�}�b�v ���x������̃o�C�A�X
	//sampler_desc.MaxAnisotropy = 16;						// �T���v�����O�Ɉٕ�����Ԃ��g�p���Ă���ꍇ�̌��E�l�B�L���Ȓl�� 1 �` 16
	//sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;	// ��r�I�v�V����
	//sampler_desc.BorderColor[0] = 0;
	//sampler_desc.BorderColor[1] = 0;
	//sampler_desc.BorderColor[2] = 0;
	//sampler_desc.BorderColor[3] = 0;
	//----------------------------------------------------------------
}

void Sampler::Set(UINT Slot)
{
	ID3D11DeviceContext* device_context = FRAMEWORK->GetDeviceContext();

	if (sampler_state) {
		device_context->PSSetSamplers(Slot, 1, sampler_state.GetAddressOf());
		device_context->DSSetSamplers(Slot, 1, sampler_state.GetAddressOf());
	}
}

