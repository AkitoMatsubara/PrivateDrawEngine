#pragma once

#include "sprite.h"
#include "shaderEx.h"
#include "Camera.h"
#include  "Sampler.h"

class SkyBox
{
private:
	std::unique_ptr<Sprite> SkyImage;	// スカイボックスの画像
	static inline std::unique_ptr<ShaderEx> SkyShader;	// シェーダ

	struct SkyConstantBuffer
	{
		DirectX::SimpleMath::Vector4 CameraPos;	        // カメラ位置
		DirectX::SimpleMath::Matrix InverseView;	    // 逆視線行列
		DirectX::SimpleMath::Matrix InverseProjection;	// 逆射影行列
	}SkycBuffer;

	Microsoft::WRL::ComPtr<ID3D11Buffer> cBuffer;
	std::unique_ptr<Sampler> sample;
public:
	SkyBox(const wchar_t* filename);
	~SkyBox();

	void Render(Camera* camera);
};
