#pragma once

#include <wrl.h>	// ComPtr�g�p�ɕK�v
using namespace Microsoft::WRL;

class Sampler
{
protected:
	ComPtr<ID3D11SamplerState> sampler_state = nullptr;

public:
	// �R���X�g���N�^1
	Sampler(D3D11_SAMPLER_DESC sd);

	// filter:
	//	D3D11_FILTER_MIN_MAG_MIP_LINEAR -���`���
	//	D3D11_FILTER_ANISOTROPIC	    -�ٕ������
	//	D3D11_FILTER_MIN_MAG_MIP_POINT  -�|�C���g�T���v�����O
	// Address_mode:
	//	D3D11_TEXTURE_ADDRESS_WRAP   -�J��Ԃ�
	//	D3D11_TEXTURE_ADDRESS_CLAMP  -�����L�΂�
	//	D3D11_TEXTURE_ADDRESS_MIRROR -�����킹
	//	D3D11_TEXTURE_ADDRESS_BORDER -color[4]�̐F�œh��Ԃ�
	Sampler(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE Address_mode, FLOAT color[4] = { 0 });

	void Set(UINT Slot = 0);

	ID3D11SamplerState* GetSamplerState() { return sampler_state.Get(); }
};