#pragma once

class Blender {
public:
	enum BLEND_STATE {
		BS_NONE,		// �u�����h�Ȃ�
		BS_ALPHA,		// ����
		BS_ADD,			// ���Z����
		BS_SUBTRACT,	// ���Z����
		BS_REPLACE,		//
		BS_MULTIPLY,	// ��Z
		BS_LIGHTEN,		//
		BS_DARKEN,		//
		BS_SCREEN,		// �X�N���[��
		BS_END			// enum�I��� �g�p���Ȃ�
	};

	ID3D11BlendState* states[BS_END];

	// ��ɐݒ肷�邽�߂Ɉ����Ȃ��R���X�g���N�^��ݒ�
	Blender() {};

	// �����Ɠ����ɐݒ肷�邽�߂̃R���X�g���N�^
	// Render�֐��ł̂ݎg�p����ꍇ�Ƃ��ɕ֗��H
	Blender(ID3D11Device* device) {
		HRESULT hr                                       = { S_OK };

		D3D11_BLEND_DESC blend_desc{};

		/*----------[BS_NONE] �Ȃ�----------*/
		blend_desc.AlphaToCoverageEnable                 = FALSE;	                        // �}�X�L���O�ɂ�肭��ʂ��������s�����|���S����s���������ɑ΂��Ă̂݉A�ʏ����ɑΉ��������_�����O�����@�H��L���ɂ��邩
		blend_desc.IndependentBlendEnable                = FALSE;	                        // ������RenderTarget[1�`]���g�p����ꍇ��True��
		blend_desc.RenderTarget[0].BlendEnable           = TRUE;	                        // �u�����f�B���O��L���ɂ��邩�ǂ���
		blend_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_ONE;	                // �ŏ���RGB�f�[�^�\�[�X�̎w��
		blend_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_ZERO;	            // 2�Ԗڂ�RGB�f�[�^�\�[�X�̎w��
		blend_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;	            // RGB�̑g�ݍ��킹���@���`	�����SrcBlend��DestBlend�����Z
		blend_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;	                // �ŏ��̃A���t�@�f�[�^�\�[�X���w�� ONE��(1,1,1,1)�̔�
		blend_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;	            // 2�Ԗڂ̃A���t�@�f�[�^�\�[�X���w�� ZERO��(0,0,0,0)�̍�
		blend_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;	            // �A���t�@�f�[�^�\�[�X�̑g�ݍ��킹���@���w�� ���Z
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;	// �������݃}�X�N ����͂��ׂẴR���|�[�l���g�Ƀf�[�^��ۑ��ł���悤��
		hr = device->CreateBlendState(&blend_desc, &states[BS_NONE]);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		if (FAILED(hr))assert("NONE_BLEND ERROR");

		/*----------[BS_ALPHA] ����----------*/
		blend_desc.AlphaToCoverageEnable                 = FALSE;
		blend_desc.IndependentBlendEnable                = FALSE;
		blend_desc.RenderTarget[0].BlendEnable           = TRUE;
		blend_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;			// ����̓s�N�Z���V�F�[�_�̃A���t�@�f�[�^���w��A�u�����f�B���O�O�̏����͖���
		blend_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_INV_SRC_ALPHA;		// ����̓A���t�@�f�[�^�A�u�����f�B���O�O�̏����ɂ���ăf�[�^�����]�A�P�|A�����������
		blend_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;				// �����SrcBlend��DestBlend�����Z
		blend_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;					// ONE��(1,1,1,1)�̔�
		blend_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;				// ZERO��(0,0,0,0)�̍�
		blend_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;				// ���Z
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, &states[BS_ALPHA]);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		if (FAILED(hr))assert("NONE_BLEND ERROR");

		/*----------[BS_ADD] ���Z----------*/
		blend_desc.AlphaToCoverageEnable                 = FALSE;
		blend_desc.IndependentBlendEnable                = FALSE;
		blend_desc.RenderTarget[0].BlendEnable           = TRUE;
		blend_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, &states[BS_ADD]);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		if (FAILED(hr))assert("NONE_BLEND ERROR");

		/*----------[BS_SUBTRACT] ���Z----------*/
		blend_desc.AlphaToCoverageEnable                 = FALSE;
		blend_desc.IndependentBlendEnable                = FALSE;
		blend_desc.RenderTarget[0].BlendEnable           = TRUE;
		blend_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_REV_SUBTRACT;
		blend_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, &states[BS_SUBTRACT]);
		if (FAILED(hr))assert("NONE_BLEND ERROR");

		/*----------[BS_REPLACE] ���Z----------*/
		blend_desc.AlphaToCoverageEnable                 = FALSE;
		blend_desc.IndependentBlendEnable                = FALSE;
		blend_desc.RenderTarget[0].BlendEnable           = TRUE;
		blend_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, &states[BS_REPLACE]);
		if (FAILED(hr))assert("NONE_BLEND ERROR");

		blend_desc.AlphaToCoverageEnable                 = FALSE;
		blend_desc.IndependentBlendEnable                = FALSE;
		blend_desc.RenderTarget[0].BlendEnable           = TRUE;
		blend_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_SRC_COLOR;
		blend_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_DEST_ALPHA;
		blend_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, &states[BS_MULTIPLY]);
		if (FAILED(hr))assert("NONE_BLEND ERROR");

		blend_desc.AlphaToCoverageEnable                 = FALSE;
		blend_desc.IndependentBlendEnable                = FALSE;
		blend_desc.RenderTarget[0].BlendEnable           = TRUE;
		blend_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_MAX;
		blend_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_MAX;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, &states[BS_LIGHTEN]);
		if (FAILED(hr))assert("NONE_BLEND ERROR");

		blend_desc.AlphaToCoverageEnable                 = FALSE;
		blend_desc.IndependentBlendEnable                = FALSE;
		blend_desc.RenderTarget[0].BlendEnable           = TRUE;
		blend_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_MIN;
		blend_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_MIN;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, &states[BS_DARKEN]);
		if (FAILED(hr))assert("NONE_BLEND ERROR");

		blend_desc.AlphaToCoverageEnable                 = FALSE;
		blend_desc.IndependentBlendEnable                = FALSE;
		blend_desc.RenderTarget[0].BlendEnable           = TRUE;
		blend_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_INV_SRC_COLOR;
		blend_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_INV_SRC_ALPHA;
		blend_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, &states[BS_SCREEN]);
		if (FAILED(hr))assert("NONE_BLEND ERROR");

		// (���Ȃ݂ɁuSrc�v��Source(��)�̗��̂炵��)
		// (Src��Dest�Ō��Ɛ��\���ϐ��Ɏg��ꂽ��H2�v�f���Ċ����݂���)
	}

	~Blender() {

	}
	// �D���ȃ^�C�~���O�Őݒ肷�邽�߂̊֐�
	// �ݒ肵�ĂȂ��Ɠ{���邵�� ���g�͏�L�R���X�g���N�^�Ɠ���
	void setBlender(ID3D11Device* device) {
		HRESULT hr = { S_OK };

		D3D11_BLEND_DESC blend_desc{};

		/*----------[BS_NONE] �Ȃ�----------*/
		blend_desc.AlphaToCoverageEnable = FALSE;	                        // �}�X�L���O�ɂ�肭��ʂ��������s�����|���S����s���������ɑ΂��Ă̂݉A�ʏ����ɑΉ��������_�����O�����@�H��L���ɂ��邩
		blend_desc.IndependentBlendEnable = FALSE;	                        // ������RenderTarget[1�`]���g�p����ꍇ��True��
		blend_desc.RenderTarget[0].BlendEnable = TRUE;	                    // �u�����f�B���O��L���ɂ��邩�ǂ���
		blend_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_ONE;	                // �ŏ���RGB�f�[�^�\�[�X�̎w��
		blend_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_ZERO;	            // 2�Ԗڂ�RGB�f�[�^�\�[�X�̎w��
		blend_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;	            // RGB�̑g�ݍ��킹���@���`	�����SrcBlend��DestBlend�����Z
		blend_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;	                // �ŏ��̃A���t�@�f�[�^�\�[�X���w�� ONE��(1,1,1,1)�̔�
		blend_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;                // 2�Ԗڂ̃A���t�@�f�[�^�\�[�X���w�� ZERO��(0,0,0,0)�̍�
		blend_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;              // �A���t�@�f�[�^�\�[�X�̑g�ݍ��킹���@���w�� ���Z
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;	// �������݃}�X�N ����͂��ׂẴR���|�[�l���g�Ƀf�[�^��ۑ��ł���悤��
		hr = device->CreateBlendState(&blend_desc, &states[BS_NONE]);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		if (FAILED(hr))assert("NONE_BLEND ERROR");

		/*----------[BS_ALPHA] ����----------*/
		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;		// ����̓s�N�Z���V�F�[�_�̃A���t�@�f�[�^���w��A�u�����f�B���O�O�̏����͖���
		blend_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_INV_SRC_ALPHA;	// ����̓A���t�@�f�[�^�A�u�����f�B���O�O�̏����ɂ���ăf�[�^�����]�A�P�|A�����������
		blend_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;			// �����SrcBlend��DestBlend�����Z
		blend_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;				// ONE��(1,1,1,1)�̔�
		blend_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;			// ZERO��(0,0,0,0)�̍�
		blend_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;			// ���Z
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, &states[BS_ALPHA]);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		if (FAILED(hr))assert("NONE_BLEND ERROR");

		/*----------[BS_ADD] ���Z----------*/
		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, &states[BS_ADD]);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		if (FAILED(hr))assert("NONE_BLEND ERROR");

		/*----------[BS_SUBTRACT] ���Z----------*/
		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_REV_SUBTRACT;
		blend_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, &states[BS_SUBTRACT]);
		if (FAILED(hr))assert("NONE_BLEND ERROR");

		/*----------[BS_REPLACE] ���Z----------*/
		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, &states[BS_REPLACE]);
		if (FAILED(hr))assert("NONE_BLEND ERROR");

		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_SRC_COLOR;
		blend_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_DEST_ALPHA;
		blend_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, &states[BS_MULTIPLY]);
		if (FAILED(hr))assert("NONE_BLEND ERROR");

		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_MAX;
		blend_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_MAX;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, &states[BS_LIGHTEN]);
		if (FAILED(hr))assert("NONE_BLEND ERROR");

		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_MIN;
		blend_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_MIN;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, &states[BS_DARKEN]);
		if (FAILED(hr))assert("NONE_BLEND ERROR");

		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_INV_SRC_COLOR;
		blend_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_INV_SRC_ALPHA;
		blend_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, &states[BS_SCREEN]);
		if (FAILED(hr))assert("NONE_BLEND ERROR");

		// (���Ȃ݂ɁuSrc�v��Source(��)�̗��̂炵��)
		// (Src��Dest�Ō��Ɛ��\���ϐ��Ɏg��ꂽ��H2�v�f���Ċ����݂���)
	}
};
