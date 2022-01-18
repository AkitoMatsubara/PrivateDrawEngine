#include "GPUParticle.h"
#include "framework.h"
#include "UseComputeShader.h"


bool GPUParticle::Init()
{
	Microsoft::WRL::ComPtr<ID3D11Device> device = FRAMEWORK->GetDevice();

	// Compute Shaderセッティング
	{
		// シーンコンスタントバッファの作成と型の設定
		CreateConstantBuffer(ConstantBuffer.GetAddressOf(), sizeof(scene_constants));
		// オブジェクトコンスタントバッファの作成
		CreateConstantBuffer(obj_ConstantBuffer.GetAddressOf(), sizeof(obj_constants));
		// CSコンスタントバッファの作成と型の設定
		CreateConstantBuffer(cs_ConstantBuffer.GetAddressOf(), sizeof(cs_constants));


		// シェーダの作成
		ParticleShader = std::make_unique<ShaderEx>();
		ParticleShader->CreateCS(L"Shaders\\GPUPaticleCS");	// ファイル指定
		ParticleShader->CreateVS(L"Shaders\\GPUParticle_vs");
		ParticleShader->CreatePS(L"Shaders\\GPUParticle_ps");

		// 入力用バッファーに初期値を設定する
		for (int i = 0; i < NUM_ELEMENTS; i++)
		{
			vBufInArray[i].Position = DirectX::SimpleMath::Vector3{ 0.0f, 0.0f, 0.0f };
		}

		// コンピュートシェーダーへの入力時に使用するSRVを作成する
		UseComputeShader::CreateSRVForStructuredBuffer(sizeof(BUFIN_TYPE), NUM_ELEMENTS, &vBufInArray[0], pBufInput.GetAddressOf(), pBufInputSRV.GetAddressOf());

		// コンピュートシェーダーからの出力時に使用するUAVを作成する
		UseComputeShader::CreateUAVForStructuredBuffer(sizeof(BUFOUT_TYPE), NUM_ELEMENTS, NULL, pBufResult.GetAddressOf(), pBufResultUAV.GetAddressOf());
	}
	obj = std::make_unique<Skinned_Mesh>(".\\Resources\\Shots\\Shot.fbx");
	sprite[0] = std::make_unique<sprite_Batch>(L".\\Resources\\particle.png",2048);
	texture = std::make_unique<Texture>();
	texture->Load(L".\\Resources\\particle.png");
	return true;
}

void GPUParticle::Update()
{
	// コンピュートシェーダーを実行する
	{
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context = FRAMEWORK->GetDeviceContext();
		Microsoft::WRL::ComPtr<ID3D11Device> device = FRAMEWORK->GetDevice();
		HRESULT hr = { S_OK };

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		static float theta = 0.0f;
		theta = (theta <= 1.0f) ? theta + 0.01f : 0.0f;	// チカチカすりゅ～！(色が)
#if 0
		D3D11_MAPPED_SUBRESOURCE subRes;	// 別の更新方法 のはず。未完成
		immediate_context->Map(pBufInput.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subRes);
		BUFIN_TYPE* pBufType = (BUFIN_TYPE*)subRes.pData;
		if (GetAsyncKeyState(VK_SPACE) & 1) { pBufType->Position.x += 1.0f; }	// TODO : おためし
https://cafe.kiite.jp/pc		//memcpy(コピー先, pBufType, sizeof(BUFIN_TYPE) * NUM_ELEMENTS);	// 入力前のデータ保持？
		immediate_context->Unmap(pBufInput.Get(), 0);
#else
		// シーンコンスタントバッファ更新
		immediate_context->PSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
		immediate_context->VSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
		// オブジェクトコンスタントバッファ更新
		{
			DirectX::SimpleMath::Matrix world;
			DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(sprite[0]->getSize().x,sprite[0]->getSize().y,1.0f) };	// 拡縮
			DirectX::XMMATRIX R = DirectX::XMMatrixRotationAxis({ 0.0f,0.0f,1.0f }, sprite[0]->getAngle());
			DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(sprite[0]->getPos().x,sprite[0]->getPos().y,0.0f) };	// 平行移動

			XMStoreFloat4x4(&world, S * R * T);	// ワールド変換行列作成
			obj_constants objData{};
			objData.world = world;

			immediate_context->UpdateSubresource(obj_ConstantBuffer.Get(), 0, 0, &objData, 0, 0);
			immediate_context->PSSetConstantBuffers(1, 1, obj_ConstantBuffer.GetAddressOf());
			immediate_context->VSSetConstantBuffers(1, 1, obj_ConstantBuffer.GetAddressOf());

		}// CSコンスタントバッファ更新
		static cs_constants csData{};
		if (GetAsyncKeyState(VK_SPACE) & 1)
		{
			csData.Size.x += 1.0f;
			csData.Size.y += 1.0f;
			csData.Size.z += 1.0f;
		}	// TODO : おためし
		if (GetAsyncKeyState(VK_SHIFT) & 1)
		{
			csData.Size.x -= 1.0f;
			csData.Size.y -= 1.0f;
			csData.Size.z -= 1.0f;
		}	// TODO : おためし
		immediate_context->UpdateSubresource(cs_ConstantBuffer.Get(), 0, 0, &csData, 0, 0);
		immediate_context->CSSetConstantBuffers(3, 1, cs_ConstantBuffer.GetAddressOf());
#endif
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// いろいろ設定、Dispatch数を指定してCSを実行
		UseComputeShader::RunComputeShader(ParticleShader->GetCS(), pBufInputSRV.Get(), 1, pBufResultUAV.Get(), 1, 1, 1, 1);

		// アンオーダードアクセスビューのバッファの内容を CPU から読み込み可能なバッファへコピーする
		UseComputeShader::CreateAndCopyToBuffer(pBufResult.Get(), ReadBackBuffer.GetAddressOf());

		D3D11_MAPPED_SUBRESOURCE MappedResource = { 0 };
		hr = immediate_context->Map(ReadBackBuffer.Get(), 0, D3D11_MAP_READ, 0, &MappedResource);	// 読み取り専用でマップ
		{	// マッピング中の処理
			BUFOUT_TYPE* p;	// 受け取る型の変数を用意する
			// "p,配列要素数"とウォッチ式に入力すると値が見れる これ便利
			p = (BUFOUT_TYPE*)MappedResource.pData;	// 型指定して代入
			obj->setScale(p->Size);
		}
		immediate_context->Unmap(ReadBackBuffer.Get(), 0);	// マップ解除
		ReadBackBuffer.Reset();	// 解放 メモリリーク回避のため

	}
}

void GPUParticle::Draw()
{
	obj->Render();
	{
		sprite[0]->begin();
		for (int i = 0; i < 1092; i++)
		{
			sprite[0]->setPos((i * sprite[0]->getTexSize().x), (i - ((i / sprite[0]->getTexSize().y))));
			sprite[0]->Render();
		}
		sprite[0]->end(ParticleShader.get());
	}
	//ParticleShader->Activate();
	//texture->Set(0);
	//FRAMEWORK->GetDeviceContext()->Draw(4, 0);
	//ParticleShader->Inactivate();

}

void GPUParticle::SetSceneConstantBuffer(const ID3D11Buffer* cbBuf)
{
	memcpy(ConstantBuffer.Get(), cbBuf, sizeof(ID3D11Buffer));
}

void GPUParticle::CreateConstantBuffer(ID3D11Buffer** dstBuf, size_t size)
{
	Microsoft::WRL::ComPtr<ID3D11Device> device = FRAMEWORK->GetDevice();

	D3D11_BUFFER_DESC buffer_desc{};
	ZeroMemory(&buffer_desc, sizeof(D3D11_BUFFER_DESC));	// 一旦初期化
	buffer_desc.ByteWidth = size;	// 型指定
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	HRESULT hr = device->CreateBuffer(&buffer_desc, nullptr, dstBuf);	// バッファ作成
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

}
