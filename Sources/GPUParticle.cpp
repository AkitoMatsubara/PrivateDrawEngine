#include "GPUParticle.h"
#include "framework.h"
#include "UseComputeShader.h"


bool GPUParticle::Init()
{
	Microsoft::WRL::ComPtr<ID3D11Device> device = FRAMEWORK->GetDevice();

	{
		//コンピュート計算のワークをセット

		DispathNo = 10;  /*ディスパッチ数*/
		PerticleAmount = 2000;        // パーティクルの数
		chainA = 0;
		chainB = 1;
		HRESULT hr = { S_OK };

		// 描画用リソース・ビュー
		{
			// 頂点バッファの定義　（書き込みにも対応）
			D3D11_BUFFER_DESC vBufferDesc;
			vBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			vBufferDesc.ByteWidth = sizeof(VBuffer) * PerticleAmount;
			vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // 頂点バッファとストリーム出力として使う
			vBufferDesc.CPUAccessFlags = 0;
			vBufferDesc.MiscFlags = 0;
			vBufferDesc.StructureByteStride = 0;

			// 頂点バッファのサブリソースの初期値(頂点座標)
			struct VBuffer* vBuf = new VBuffer[PerticleAmount];

			// 頂点バッファのサブリソースの定義(バッファーリソースでも一つだけ持てる）
			D3D11_SUBRESOURCE_DATA vSubData;//初期化用データを作成
			vSubData.pSysMem = vBuf;  // バッファ・データの初期値
			vSubData.SysMemPitch = 0;
			vSubData.SysMemSlicePitch = 0;

			// 描画用頂点入力バッファの作成　　初期化用データを送っている
			device->CreateBuffer(&vBufferDesc, &vSubData, VerticesBuffer.GetAddressOf());
			//delete[] vBuf;

			// 動的な定数バッファの定義
			CreateConstantBuffer(DynamicCBuffer.GetAddressOf(), sizeof(cbCBuffer), true);

			//初期化用データ
			//struct VBuffer* vBuf = new VBuffer[PerticleAmount];
			float fx = 0;
			float fy = 0;
			float fz = 0;
			float theta = 0;
			float x = 0;
			float y = 0;
			for (int i = 0; i < PerticleAmount; ++i) {

				fx = ((i % DispathNo) - (DispathNo / 2.0f))/* / (float)DispathNo * 0.005f*/;
				fz = ((i / DispathNo) - (DispathNo / 2.0f))/* / (float)DispathNo * 0.005f*/;
				fy = 0.1f;
				vBuf[i].Position = DirectX::XMFLOAT3(fx, fy, fz);


				theta = 0.1f;
 				x = fx * cosf(theta) - fy * sinf(theta);
				y = fx * sinf(theta) + fy * cosf(theta);
				//x = fx * cosf(theta) - fy * sinf(theta) + (((float)rand() / RAND_MAX) - 0.5)*20;
				//y = fx * sinf(theta) + fy * cosf(theta) + (((float)rand() / RAND_MAX) - 0.5)*20;
				vBuf[i].Velocity = DirectX::XMFLOAT3(x / 10000.0f, y / 10000.0f, 0);//速度
				vBuf[i].Force = DirectX::XMFLOAT3(0, 0, 0);//加速度
			}

			// リソースの設定
			// 最初の入力リソース
			UseComputeShader::CreateStructuredBuffer(sizeof(VBuffer), PerticleAmount, vBuf, InBuffer.GetAddressOf());
			// SRVの作成
			UseComputeShader::CreateSRVfromStructuredBuffer(InBuffer.Get(), g_pSRV[0].GetAddressOf());
			// UAVの作成
			UseComputeShader::CreateUAVfromStructuredBuffer(InBuffer.Get(), g_pUAV[0].GetAddressOf());


			// 最初の出力リソース
			UseComputeShader::CreateStructuredBuffer(sizeof(VBuffer), PerticleAmount, nullptr, OutBuffer.GetAddressOf());
			// SRVの作成
			UseComputeShader::CreateSRVfromStructuredBuffer(OutBuffer.Get(), g_pSRV[1].GetAddressOf());
			// UAVの作成
			UseComputeShader::CreateUAVfromStructuredBuffer(OutBuffer.Get(), g_pUAV[1].GetAddressOf());

			delete[] vBuf;

			// CPUへのデータ書き込み用バッファとCPU転送用UAVの設定
			UseComputeShader::CreateStructuredBufferAndUAV(sizeof(ReturnBuffer), 1, nullptr, ToCpuBuffer.GetAddressOf(), ToCpuUAV.GetAddressOf());


			// リードバック用バッファ リソースの作成
			UseComputeShader::CreateAndCopyToBuffer(ToCpuBuffer.Get(), CPUReadBackBuffer.GetAddressOf());
		}
		//アルファブレンド用ブレンドステート作成
		//pngファイル内にアルファ情報がある。アルファにより透過するよう指定している
		D3D11_BLEND_DESC blend_desc;
		ZeroMemory(&blend_desc, sizeof(D3D11_BLEND_DESC));
		blend_desc.IndependentBlendEnable = false;
		blend_desc.AlphaToCoverageEnable = false;
		blend_desc.RenderTarget[0].BlendEnable = true;
		blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;


		//hr = device->CreateBlendState(&bd, &pBlendState.p);
		hr = device->CreateBlendState(&blend_desc, bd_states.GetAddressOf());



	}
	// Compute Shaderセッティング
	{
		// シーンコンスタントバッファの作成と型の設定
		CreateConstantBuffer(ConstantBuffer.GetAddressOf(), sizeof(scene_constants));
		// オブジェクトコンスタントバッファの作成
		CreateConstantBuffer(obj_ConstantBuffer.GetAddressOf(), sizeof(obj_constants));
		// CSコンスタントバッファの作成と型の設定
		CreateConstantBuffer(cs_ConstantBuffer.GetAddressOf(), sizeof(cs_constants));
		// Samplerの設定
		sample = std::make_shared<Sampler>(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);

		//描画用シェーダ
		// 入力要素
		static constexpr UINT IL_NUM = 3;
		D3D11_INPUT_ELEMENT_DESC layout[IL_NUM] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "FORCE",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// シェーダの作成
		ParticleShader = std::make_unique<ShaderEx>();
		ParticleShader->CreateCS(L"Shaders\\GPUParticle_cs");	// ファイル指定
		ParticleShader->CreateVS(L"Shaders\\GPUParticle_vs", IL_NUM, layout);
		ParticleShader->CreateGS(L"Shaders\\GPUParticle_gs");
		ParticleShader->CreatePS(L"Shaders\\GPUParticle_ps");

		// 入力用バッファーに初期値を設定する
		for (int i = 0; i < NUM_ELEMENTS; i++)
		{
			vBufInArray[i].Position = DirectX::SimpleMath::Vector3{ 0.0f, 0.0f, 0.0f };
		}

		// コンピュートシェーダーへの入力時に使用するSRVを作成する
		UseComputeShader::CreateStructuredBufferAndSRV(sizeof(BUFIN_TYPE), NUM_ELEMENTS, &vBufInArray[0], pBufInput.GetAddressOf(), pBufInputSRV.GetAddressOf());

		// コンピュートシェーダーからの出力時に使用するUAVを作成する
		UseComputeShader::CreateStructuredBufferAndUAV(sizeof(BUFOUT_TYPE), NUM_ELEMENTS, NULL, pBufResult.GetAddressOf(), pBufResultUAV.GetAddressOf());
	}
	texture = std::make_shared<Texture>();
	texture->Load(L".\\Resources\\particle.png");
	return true;
}

void GPUParticle::Update()
{
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context = FRAMEWORK->GetDeviceContext();
	Microsoft::WRL::ComPtr<ID3D11Device> device = FRAMEWORK->GetDevice();
	HRESULT hr = { S_OK };
	//	// コンピュートシェーダーを実行する
	//	{
	//
	//		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//		static float theta = 0.0f;
	//		theta = (theta <= 1.0f) ? theta + 0.01f : 0.0f;	// チカチカすりゅ～！(色が)
	//#if 0
	//		D3D11_MAPPED_SUBRESOURCE subRes;	// 別の更新方法 のはず。未完成
	//		immediate_context->Map(pBufInput.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subRes);
	//		BUFIN_TYPE* pBufType = (BUFIN_TYPE*)subRes.pData;
	//		if (GetAsyncKeyState(VK_SPACE) & 1) { pBufType->Position.x += 1.0f; }	// TODO : おためし
	//		//memcpy(コピー先, pBufType, sizeof(BUFIN_TYPE) * NUM_ELEMENTS);	// 入力前のデータ保持？
	//		immediate_context->Unmap(pBufInput.Get(), 0);
	//#else
	//		// シーンコンスタントバッファ更新
	//		immediate_context->PSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
	//		immediate_context->VSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
	//		// オブジェクトコンスタントバッファ更新
	//		{
	//			DirectX::SimpleMath::Matrix world;
	//			DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(sprite[0]->getSize().x,sprite[0]->getSize().y,1.0f) };	// 拡縮
	//			DirectX::XMMATRIX R = DirectX::XMMatrixRotationAxis({ 0.0f,0.0f,1.0f }, sprite[0]->getAngle());
	//			DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(sprite[0]->getPos().x,sprite[0]->getPos().y,0.0f) };	// 平行移動
	//
	//			XMStoreFloat4x4(&world, S * R * T);	// ワールド変換行列作成
	//			obj_constants objData{};
	//			objData.world = world;
	//
	//			immediate_context->UpdateSubresource(obj_ConstantBuffer.Get(), 0, 0, &objData, 0, 0);
	//			immediate_context->PSSetConstantBuffers(1, 1, obj_ConstantBuffer.GetAddressOf());
	//			immediate_context->VSSetConstantBuffers(1, 1, obj_ConstantBuffer.GetAddressOf());
	//
	//		}// CSコンスタントバッファ更新
	//		static cs_constants csData{};
	//		//if (GetAsyncKeyState(VK_SPACE) & 1)
	//		//{
	//		//	csData.Size.x += 1.0f;
	//		//	csData.Size.y += 1.0f;
	//		//	csData.Size.z += 1.0f;
	//		//}	// TODO : おためし
	//		//if (GetAsyncKeyState(VK_SHIFT) & 1)
	//		//{
	//		//	csData.Size.x -= 1.0f;
	//		//	csData.Size.y -= 1.0f;
	//		//	csData.Size.z -= 1.0f;
	//		//}	// TODO : おためし
	//		immediate_context->UpdateSubresource(cs_ConstantBuffer.Get(), 0, 0, &csData, 0, 0);
	//		immediate_context->CSSetConstantBuffers(3, 1, cs_ConstantBuffer.GetAddressOf());
	//#endif
	//		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//
	//		// いろいろ設定、Dispatch数を指定してCSを実行
	//		UseComputeShader::RunComputeShader(ParticleShader->GetCS(), pBufInputSRV.Get(), 1, pBufResultUAV.Get(), 1, 1, 1, 1);
	//
	//		// アンオーダードアクセスビューのバッファの内容を CPU から読み込み可能なバッファへコピーする
	//		UseComputeShader::CreateAndCopyToBuffer(pBufResult.Get(), ReadBackBuffer.GetAddressOf());
	//
	//		D3D11_MAPPED_SUBRESOURCE MappedResource = { 0 };
	//		hr = immediate_context->Map(ReadBackBuffer.Get(), 0, D3D11_MAP_READ, 0, &MappedResource);	// 読み取り専用でマップ
	//		{	// マッピング中の処理
	//			BUFOUT_TYPE* p;	// 受け取る型の変数を用意する
	//			// "p,配列要素数"とウォッチ式に入力すると値が見れる これ便利
	//			p = (BUFOUT_TYPE*)MappedResource.pData;	// 型指定して代入
	//			obj->setScale(p->Size);
	//		}
	//		immediate_context->Unmap(ReadBackBuffer.Get(), 0);	// マップ解除
	//		ReadBackBuffer.Reset();	// 解放 メモリリーク回避のため
	//
	//	}

	// サンプルコピー-------------------------------------
	// **********************************************************
	// コンピュート・シェーダを使った演算
	// **********************************************************
	// シェーダを設定
	g_cbCBuffer.No = DispathNo;

	// ***************************************
	// 定数バッファのマップ取得

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	immediate_context->Map(DynamicCBuffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	 // 動的コンスタントバッファの値にコピーする
	CopyMemory(MappedResource.pData, &g_cbCBuffer, sizeof(cbCBuffer));
	// マップ解除
	immediate_context->Unmap(DynamicCBuffer.Get(), NULL);

	// PSに定数バッファを設定
	immediate_context->CSSetConstantBuffers(0, 1, DynamicCBuffer.GetAddressOf());
	immediate_context->CSSetShader(ParticleShader->GetCS(), 0, 0);

	//// 現在のSRVを解除
	//ID3D11ShaderResourceView* SRVNULL = NULL;
	//immediate_context->CSSetShaderResources(0, 1, &SRVNULL);
	// SRVの設定
	immediate_context->CSSetShaderResources(0, 1, g_pSRV[chainA].GetAddressOf());

	// UAVの設定
	immediate_context->CSSetUnorderedAccessViews(0, 1, g_pUAV[chainB].GetAddressOf(), 0);
	immediate_context->CSSetUnorderedAccessViews(1, 1, ToCpuUAV.GetAddressOf(), 0);
	// CSの実行
	immediate_context->Dispatch(DispathNo, 1, 1);

	immediate_context->CopyResource(VerticesBuffer.Get(), InBuffer.Get());
	immediate_context->CopyResource(CPUReadBackBuffer.Get(), ToCpuBuffer.Get());

	// 結果をCPUから読み込む
	hr = immediate_context->Map(CPUReadBackBuffer.Get(), 0, D3D11_MAP_READ, 0, &MappedResource);
	ReturnBuffer* pt = (ReturnBuffer*)MappedResource.pData;
	// 取り出したデータをコピー
	memcpy(&CpuGpuBuffer, pt, sizeof(ReturnBuffer));
	// マップ解除
	immediate_context->Unmap(CPUReadBackBuffer.Get(), 0);

	chainA = chainA ? 0 : 1; // バッファーの切り替え
	chainB = chainB ? 0 : 1; // バッファーの切り替え
	//----------------------------------------------------
}

void GPUParticle::Draw(Camera* camera)
{
	//ParticleShader->Activate();
	//texture->Set(0);
	//FRAMEWORK->GetDeviceContext()->Draw(4, 0);
	//ParticleShader->Inactivate();

	//----------------------------------------------------
	ID3D11DeviceContext* immadiate_context = FRAMEWORK->GetDeviceContext();


	g_cbCBuffer.Projection = DirectX::XMMatrixTranspose(camera->GetProjection());
	g_cbCBuffer.View = DirectX::XMMatrixTranspose(camera->GetView());	// 行列をシェーダに渡すには転置行列にする
	g_cbCBuffer.ParticleSize.x = 0.08f;
	g_cbCBuffer.ParticleSize.y = 0.08f;
	g_cbCBuffer.EyePos = DirectX::SimpleMath::Vector4(camera->GetPos().x, camera->GetPos().y, camera->GetPos().z, 1.0f);
	//g_cbCBuffer.FogNear = 10.0f;	//霧の始まる位置
	//g_cbCBuffer.FogFar = 60.0f;		//霧の終わる位置
	g_cbCBuffer.FogColor = DirectX::SimpleMath::Vector4{ 0.0f,0.0f,0.0f,1.0f };	//霧の終わる位置

	immadiate_context->UpdateSubresource(DynamicCBuffer.Get(), 0, 0, &g_cbCBuffer, 0, 0);
	D3D11_BUFFER_DESC desc;
	DynamicCBuffer->GetDesc(&desc);
	// ***************************************
	// VSに定数バッファを設定
	immadiate_context->VSSetConstantBuffers(0, 1, DynamicCBuffer.GetAddressOf());
	// PSに定数バッファを設定
	immadiate_context->PSSetConstantBuffers(0, 1, DynamicCBuffer.GetAddressOf());
	immadiate_context->GSSetConstantBuffers(0, 1, DynamicCBuffer.GetAddressOf());

	// ***************************************

	// IAに頂点バッファを設定
	UINT strides[1] = { sizeof(VBuffer) };
	UINT offsets[1] = { 0 };
	immadiate_context->IASetVertexBuffers(0, 1, VerticesBuffer.GetAddressOf(), strides, offsets);
	// IAにプリミティブの種類を設定
	//immadiate_context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	// 各シェーダのセット
	ParticleShader->Activate();

	texture->Set(0);
	sample->Set(0);

	UINT mask = 0xffffffff;
	immadiate_context->OMSetBlendState(FRAMEWORK->GetBlendState(FRAMEWORK->BS_ALPHA), nullptr, mask);
	immadiate_context->RSSetState(FRAMEWORK->GetRasterizerState(FRAMEWORK->RS_SOLID_NONE));

	// ***************************************
	// 描画する
	immadiate_context->Draw(PerticleAmount, 0);
	// シェーダの無効化
	ParticleShader->Inactivate();
	//----------------------------------------------------
}

void GPUParticle::SetSceneConstantBuffer(const ID3D11Buffer* cbBuf)
{
	memcpy(ConstantBuffer.Get(), cbBuf, sizeof(ID3D11Buffer));
}

void GPUParticle::CreateConstantBuffer(ID3D11Buffer** dstBuf, size_t size, bool dynamicFlg)
{
	Microsoft::WRL::ComPtr<ID3D11Device> device = FRAMEWORK->GetDevice();

	D3D11_BUFFER_DESC buffer_desc{};
	ZeroMemory(&buffer_desc, sizeof(D3D11_BUFFER_DESC));	// 一旦初期化
	buffer_desc.ByteWidth = static_cast<UINT>(size);	// 型指定
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	if (dynamicFlg)
	{
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		buffer_desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	}
	else
	{
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.CPUAccessFlags = 0;
	}
	HRESULT hr = device->CreateBuffer(&buffer_desc, nullptr, dstBuf);	// バッファ作成
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

}
