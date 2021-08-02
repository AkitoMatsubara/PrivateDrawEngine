#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <assert.h>
using namespace Microsoft::WRL;

class Rasterizer {
public:

	enum RASTERIZER_STATE{
		SOLID_NONE,
		WIRE_NONE,
		SOLID_BACK,
		WIRE_BACK,
		RS_NONE,	// ���X�^���C�U�I��
	};
	ComPtr<ID3D11RasterizerState>	states[RS_NONE];	// 0:�Жʓh��Ԃ�,1:���ʓh��ׂ�,2:�Жʃ��C���[�t���[��,3:���ʃ��C���[�t���[��

public:
	Rasterizer() {}
	Rasterizer(ID3D11Device* device) { SetRasterizer(device); }

	~Rasterizer() {}

	// RasterizerState�̍쐬
	void SetRasterizer(ID3D11Device* device) {
		HRESULT hr = { S_OK };

		// ���X�^���C�U�I�u�W�F�N�g�̐���
		D3D11_RASTERIZER_DESC rasterizer_desc{};
		rasterizer_desc.FrontCounterClockwise = FALSE;	// �O�p�`���O�ʂ��w�ʂ������肷�� TRUE�̎��A���_�����Ύ��肾�ƑO�����Ƃ݂Ȃ����
		rasterizer_desc.DepthBias = 0;					// �[�x�o�C�A�X ����[�x��ɕ\������Ƃ��ɗD��x�����߂�̂Ɏg�p�����肷��
		rasterizer_desc.DepthBiasClamp = 0;			    // ��L���l     �s�N�Z���̍ő�[�x�o�C�A�X
		rasterizer_desc.SlopeScaledDepthBias = 0;		// ��L���l     ����̃s�N�Z���̌X���̃X�J���[
		rasterizer_desc.DepthClipEnable = TRUE;		    // �����Ɋ�Â��ăN���b�s���O��L���ɂ��邩
		rasterizer_desc.ScissorEnable = FALSE;			// �V�U�[��`�J�����O���g�p���邩 �V�U�[��`�F�`��̈�̎w��ɂ悭�g����
		rasterizer_desc.MultisampleEnable = FALSE;		// �}���`�T���v�����O�A���`�G�C���A�X(MSAA)��RTV���g�p���Ă��鎞�Ature�Ŏl�ӌ`���C���A���`�G�C���A�X�Afalse�ŃA���t�@���C���A���`�G�C���A�X���g�p
														// MSAA���g�p����ɂ̓��\�[�X��������DX11_SAMPLE_DESC::Count��1����̒l��ݒ肷��K�v������
		/*-----�h��ׂ� ���ʕ`��-----*/
		rasterizer_desc.FillMode = D3D11_FILL_SOLID;
		rasterizer_desc.CullMode = D3D11_CULL_NONE;
		rasterizer_desc.AntialiasedLineEnable = TRUE;
		hr = device->CreateRasterizerState(&rasterizer_desc, states[SOLID_NONE].GetAddressOf());
		/*-----���C���[�t���[�� ���ʕ`��-----*/
		rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizer_desc.CullMode = D3D11_CULL_NONE;
		rasterizer_desc.AntialiasedLineEnable = TRUE;
		hr = device->CreateRasterizerState(&rasterizer_desc, states[WIRE_NONE].GetAddressOf());
		/*-----�h��Ԃ� �O�ʕ`��-----*/
		rasterizer_desc.FillMode = D3D11_FILL_SOLID;	// �����_�����O�Ɏg���h��Ԃ����[�h D3D11_FILL_SOLID|D3D11_FILL_WIREFRAME
		rasterizer_desc.CullMode = D3D11_CULL_BACK;	    // �`�悷��@������ D3D11_CULL_NONE(���ʕ`��)|D3D11_CULL_FRONT(��ʕ`��)|D3D11_CULL_BACK(�O�ʕ`��)
		rasterizer_desc.AntialiasedLineEnable = FALSE;	// MSAA��RTV���g�p���Ă��鎞�A�����`���MultisampleEnable��false�̎��ɃA���`�G�C���A�X��L���ɂ���
		hr = device->CreateRasterizerState(&rasterizer_desc, states[SOLID_BACK].GetAddressOf());
		/*-----���C���[�t���[�� �O�ʕ`��-----*/
		rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizer_desc.CullMode = D3D11_CULL_BACK;
		rasterizer_desc.AntialiasedLineEnable = TRUE;
		hr = device->CreateRasterizerState(&rasterizer_desc, states[WIRE_BACK].GetAddressOf());

		if (SUCCEEDED(hr)) {
			assert("Create RasterizerState Failde.");
		}
	}
};