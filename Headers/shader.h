#pragma once

#include <d3d11.h>
#include <SimpleMath.h>
#include <d3dcompiler.h>

#include <wrl.h>

// ���N���X
class Shader {
	// �֐�
public:
protected:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>	 VS = nullptr; // ���_�V�F�[�_
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	 PS = nullptr; // �s�N�Z���V�F�[�_
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> GS = nullptr; // �W�I���g���V�F�[�_
	Microsoft::WRL::ComPtr<ID3D11HullShader>	 HS = nullptr; // �n���V�F�[�_
	Microsoft::WRL::ComPtr<ID3D11DomainShader>	 DS = nullptr; // �h���C���l�[���V�F�[�_
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>	 CS = nullptr; // �R���s���[�g�V�F�[�_

	Microsoft::WRL::ComPtr<ID3D11InputLayout> InputLayout = nullptr;	// VS�p
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
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector3 normal;
	DirectX::XMFLOAT2 texcoord;	// TextuerCoordinate�̗��AUV���W�̎擾�Ȃǂɂ悭�g����l�q
	DirectX::SimpleMath::Vector4 color;
};
