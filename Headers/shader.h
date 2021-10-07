#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

using namespace DirectX;

//HRESULT create_vs_from_cso(const char* cso_name, ID3D11VertexShader** vertex_shader, ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements);
//
//HRESULT create_ps_from_cso(const char* ps_cso_name, ID3D11PixelShader** pixel_shader);
//
//HRESULT create_cs_from_cso(const char* cs_cso_name, ID3D11ComputeShader** compute_shader);

// ���N���X
class Shader {
	// �֐�
public:
protected:
	ComPtr<ID3D11VertexShader>	 VS = nullptr; // ���_�V�F�[�_
	ComPtr<ID3D11PixelShader>	 PS = nullptr; // �s�N�Z���V�F�[�_
	ComPtr<ID3D11GeometryShader> GS = nullptr; // �W�I���g���V�F�[�_
	ComPtr<ID3D11HullShader>	 HS = nullptr; // �n���V�F�[�_
	ComPtr<ID3D11DomainShader>	 DS = nullptr; // �h���C���l�[���V�F�[�_
	ComPtr<ID3D11ComputeShader>	 CS = nullptr; // �R���s���[�g�V�F�[�_

	ComPtr<ID3D11InputLayout> InputLayout = nullptr;	// VS�p
private:
	// �ϐ�
public:
	Shader() {};
	virtual ~Shader() {};

	// VS,PS�̍쐬
	bool Create(WCHAR* filename, LPCSTR VSFunc, LPCSTR PSFunc);
	bool Create(WCHAR* filename, LPCSTR VSName, LPCSTR PSName, LPCSTR GSName);
	bool Create(WCHAR* filename, LPCSTR VSName, LPCSTR PSName, LPCSTR GSName, LPCSTR HSName, LPCSTR DSName);

	void Activate();
	void Inactivate();

protected:
	// �t�@�C���p�X, �G���g���|�C���g, �V�F�[�_���f��, �󂯎��ID3DBlob�|�C���^
	HRESULT Compile(WCHAR* filename, LPCSTR method, LPCSTR shaderModel, ID3DBlob** ppBlobOut);
private:
};

// ���_�t�H�[�}�b�g
struct Vertex {
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT2 texcoord;	// TextuerCoordinate�̗��AUV���W�̎擾�Ȃǂɂ悭�g����l�q
	XMFLOAT4 color;
};
