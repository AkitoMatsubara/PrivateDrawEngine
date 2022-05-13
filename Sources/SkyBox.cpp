#include "SkyBox.h"
#include "framework.h"

SkyBox::SkyBox(const wchar_t* filename)
{
	const Microsoft::WRL::ComPtr<ID3D11Device> device = FRAMEWORK->GetDevice();

	SkyImage = std::make_unique<Sprite>();
	SkyImage->LoadImages(filename);	// 画像読み込み
	SkyImage->setSize(SCREEN_WIDTH, SCREEN_HEIGHT);

	SkyShader = std::make_unique<ShaderEx>();
	SkyShader->CreateVS(L"Shaders\\SkyBox_vs");
	SkyShader->CreatePS(L"Shaders\\SkyBox_ps");


	D3D11_BUFFER_DESC buffer_desc{};
	ZeroMemory(&buffer_desc, sizeof(D3D11_BUFFER_DESC));
	buffer_desc.ByteWidth = static_cast<UINT>(sizeof(SkyConstantBuffer));	// 型指定
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	buffer_desc.CPUAccessFlags = 0;

	HRESULT hr = device->CreateBuffer(&buffer_desc, nullptr, cBuffer.GetAddressOf());	// バッファ作成
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// サンプラステート
	{
		D3D11_SAMPLER_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.MipLODBias = 0.0f;
		desc.MaxAnisotropy = 1;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = -FLT_MAX;
		desc.MaxLOD = FLT_MAX;
		desc.BorderColor[0] = 1.0f;
		desc.BorderColor[1] = 1.0f;
		desc.BorderColor[2] = 1.0f;
		desc.BorderColor[3] = 1.0f;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

		sample = std::make_unique<Sampler>(D3D11_FILTER_MIN_MAG_MIP_LINEAR,D3D11_TEXTURE_ADDRESS_WRAP);
	}

}

SkyBox::~SkyBox() {	}

void SkyBox::Render(Camera* camera)
{
	const Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context = FRAMEWORK->GetDeviceContext();

	SkycBuffer.CameraPos = DirectX::SimpleMath::Vector4{ camera->GetPos().x,camera->GetPos().y,camera->GetPos().z,1.0f };
	SkycBuffer.InverseView = DirectX::XMMatrixInverse(nullptr, camera->GetView());	// 逆行列を計算
	SkycBuffer.InverseProjection = DirectX::XMMatrixInverse(nullptr, camera->GetProjection());	// 逆行列を計算

	immediate_context->VSSetConstantBuffers(0, 1, cBuffer.GetAddressOf());
	immediate_context->PSSetConstantBuffers(0, 1, cBuffer.GetAddressOf());
	immediate_context->UpdateSubresource(cBuffer.Get(), 0, 0, &SkycBuffer, 0, 0);

	immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(FRAMEWORK->DS_FALSE),0);
	sample->Set(0);
	SkyImage->Render(nullptr, SkyShader.get());
	immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(FRAMEWORK->DS_TRUE), 0);
}
