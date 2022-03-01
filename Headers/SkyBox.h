#pragma once

#include "sprite.h"
#include "shaderEx.h"
#include "Camera.h"
#include  "Sampler.h"

class SkyBox
{
private:
	std::unique_ptr<Sprite> SkyImage;	// �X�J�C�{�b�N�X�̉摜
	static inline std::unique_ptr<ShaderEx> SkyShader;	// �V�F�[�_

	struct SkyConstantBuffer
	{
		DirectX::SimpleMath::Vector4 CameraPos;	        // �J�����ʒu
		DirectX::SimpleMath::Matrix InverseView;	    // �t�����s��
		DirectX::SimpleMath::Matrix InverseProjection;	// �t�ˉe�s��
	}SkycBuffer;

	Microsoft::WRL::ComPtr<ID3D11Buffer> cBuffer;
	std::unique_ptr<Sampler> sample;
public:
	SkyBox(const wchar_t* filename);
	~SkyBox();

	void Render(Camera* camera);
};
