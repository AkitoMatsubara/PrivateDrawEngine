#include "GPUParticle.h"
#include "framework.h"
#include "UseComputeShader.h"

// TODO 適当 書き込み可能構造化バッファを作成する
HRESULT CreateWriteStructuredBuffer(UINT uElementSize, UINT uCount, ID3D11Buffer** ppBuf)
{
	ID3D11Device* pD3DDevice = FRAMEWORK->GetDevice();
	HRESULT hr = E_FAIL;
	*ppBuf = nullptr;

	// 構造化バッファーを作成する
	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));

	BufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	BufferDesc.ByteWidth = uElementSize * uCount;					// バッファサイズ
	BufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	BufferDesc.StructureByteStride = uElementSize;					// 構造化バッファーのサイズ (バイト単位)
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = pD3DDevice->CreateBuffer(&BufferDesc, nullptr, ppBuf);

	if (FAILED(hr))_ASSERT_EXPR_A(false, "FAILED CreateStructuredBuffer");

	return hr;
}

bool GPUParticle::Init()
{
	Microsoft::WRL::ComPtr<ID3D11Device> device = FRAMEWORK->GetDevice();

	{
		//コンピュート計算のワークをセット

		DispathNo = 500;  // ディスパッチ数
		const int CSnumthreadNo = 500;	// CSの実行数
		ParticleAmount = DispathNo * CSnumthreadNo;        // パーティクルの数 (DispatchNo * CSnumthread)
		HRESULT hr = { S_OK };

		// 描画用リソース・ビュー
		{
			VerticesBuffer = nullptr;	// バッファを作るので中身があっては大変、消します

			// 頂点バッファの定義　（書き込みにも対応）
			D3D11_BUFFER_DESC vBufferDesc;
			vBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			vBufferDesc.ByteWidth = sizeof(VBuffer) * ParticleAmount;
			vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // 頂点バッファとストリーム出力として使う
			vBufferDesc.CPUAccessFlags = 0;
			vBufferDesc.MiscFlags = 0;
			vBufferDesc.StructureByteStride = 0;

			// 頂点バッファのサブリソースを指定するためのポインタ すぐ消す
			VBuffer* vBuffer = new VBuffer[ParticleAmount];

			// 頂点バッファのサブリソースの定義(バッファーリソースでも一つだけ持てる）
			D3D11_SUBRESOURCE_DATA vSubData;//初期化用データを作成
			vSubData.pSysMem = vBuffer;  // バッファ・データの初期値
			vSubData.SysMemPitch = 0;
			vSubData.SysMemSlicePitch = 0;
			// 描画用頂点入力バッファの作成
			device->CreateBuffer(&vBufferDesc, &vSubData, VerticesBuffer.GetAddressOf());
			delete[] vBuffer;	// お片付け

			// 初期値の設定
			for (int i = 0; i < ParticleAmount; ++i) {
				VBuffer data;
				// 初期位置の設定
				data.Position.x = ((i % DispathNo) - (DispathNo / 2.0f)) / (float)DispathNo * 0.005f;
				data.Position.z = ((i / DispathNo) - (DispathNo / 2.0f)) / (float)DispathNo * 0.005f;
				data.Position.y = 0.1f;

				// 初期速度の設定
				data.Velocity.x = ((rand() % 2000) - 1000) * 0.00005f;
				data.Velocity.y = ((rand() % 2000) - 1000) * 0.00005f;
				data.Velocity.z = ((rand() % 2000) - 1000) * 0.00005f;

				data.Force = DirectX::XMFLOAT3(0, -0.0005f, 0);//加速度

				// その他
				data.Color = DirectX::XMFLOAT4(testColor[0], testColor[1], testColor[2], testColor[3]);
				data.Active = true;
				testLife = 50;
				data.Life = testLife * (rand() % 10 + 1);

				vVecBuf.emplace_back(data);	// 格納
			}
			// 動的な定数バッファの定義
			CreateConstantBuffer(DynamicCBuffer.GetAddressOf(), sizeof(cbCBuffer), true);

			// リソースの設定
			// 入力リソース
			// SRVの作成
			UseComputeShader::CreateWritableStructuredBufferAndSRV(sizeof(VBuffer), ParticleAmount, vVecBuf.data(), InputBuffer.GetAddressOf(), g_pSRV.GetAddressOf());

			// 出力リソース
			// UAVの作成
			UseComputeShader::CreateStructuredBufferAndUAV(sizeof(VBuffer), ParticleAmount, nullptr,OutputBuffer.GetAddressOf(), g_pUAV.GetAddressOf());

			// リードバック用バッファ リソースの作成
			UseComputeShader::CreateAndCopyToBuffer(OutputBuffer.Get(), CPUReadBackBuffer.GetAddressOf());
		}
	}
	// Compute Shaderセッティング
	{
		// シーンコンスタントバッファの作成と型の設定
		CreateConstantBuffer(ConstantBuffer.GetAddressOf(), sizeof(scene_constants));
		// Samplerの設定
		if (sample == nullptr)
		{
			sample = std::make_shared<Sampler>(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);
		}
		//描画用シェーダ
		if (ParticleShader == nullptr)
		{
			// 入力要素
			D3D11_INPUT_ELEMENT_DESC layout[] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
					D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
					D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "FORCE",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
					D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
					D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "ACTIVE",    0, DXGI_FORMAT_R32_UINT, 0,
					D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "LIFE",    0, DXGI_FORMAT_R32_FLOAT, 0,
					D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
			static constexpr UINT IL_NUM = ARRAYSIZE(layout);	// ILの配列サイズ取得
			// シェーダの作成
			ParticleShader = std::make_unique<ShaderEx>();
			ParticleShader->CreateCS(L"Shaders\\GPUParticle_cs");	// ファイル指定
			ParticleShader->CreateVS(L"Shaders\\GPUParticle_vs", IL_NUM, layout);
			ParticleShader->CreateGS(L"Shaders\\GPUParticle_gs");
			ParticleShader->CreatePS(L"Shaders\\GPUParticle_ps");
		}
	}
	if (texture == nullptr)
	{
		texture = std::make_shared<Texture>();
		texture->Load(L".\\Resources\\particle_W.png");
	}
	return true;
}

void GPUParticle::Update(Camera* camera)
{
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context = FRAMEWORK->GetDeviceContext();
	Microsoft::WRL::ComPtr<ID3D11Device> device = FRAMEWORK->GetDevice();
	HRESULT hr = { S_OK };

	D3D11_MAPPED_SUBRESOURCE subRes;
	immediate_context->Map(InputBuffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &subRes);	// subResにInputBufferをマップ
	{
		memcpy(subRes.pData, vVecBuf.data(), sizeof(VBuffer) * ParticleAmount);	// SRVのバッファに初期化情報をコピー
	}
	immediate_context->Unmap(InputBuffer.Get(), 0);

	// **********************************************************
	// コンピュート・シェーダを使った演算
	// **********************************************************
	// シェーダを設定
	g_cbCBuffer.No = DispathNo;

	g_cbCBuffer.Projection	= DirectX::XMMatrixTranspose(camera->GetProjection());
	g_cbCBuffer.View		= DirectX::XMMatrixTranspose(camera->GetView());	// 行列をシェーダに渡すには転置行列にする
	g_cbCBuffer.ParticleSize.x = 0.02f;
	g_cbCBuffer.ParticleSize.y = 0.02f;
	g_cbCBuffer.EyePos = DirectX::SimpleMath::Vector4(camera->GetPos().x, camera->GetPos().y, camera->GetPos().z, 1.0f);

	// ***************************************
	// 定数バッファのマップ取得

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	// D3D11_MAP_WRITE_DISCARD : もとのデータを無効にして書き込む
	immediate_context->Map(DynamicCBuffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	{
		// 動的コンスタントバッファの値にコピーする
		CopyMemory(MappedResource.pData, &g_cbCBuffer, sizeof(cbCBuffer));
	}
	// マップ解除
	immediate_context->Unmap(DynamicCBuffer.Get(), NULL);

	// PSに定数バッファを設定
	immediate_context->CSSetConstantBuffers(0, 1, DynamicCBuffer.GetAddressOf());
	immediate_context->CSSetShader(ParticleShader->GetCS(), 0, 0);

	// CPUを介さずやり取りを行うため、２つのバッファで位置等を交互に出し入れしている
	// 0の情報をもとに計算、結果を1に → 1の情報をもとに計算、結果を0に…  というように

	// SRVの設定
	immediate_context->CSSetShaderResources(0, 1, g_pSRV.GetAddressOf());

	// UAVの設定
	immediate_context->CSSetUnorderedAccessViews(0, 1, g_pUAV.GetAddressOf(), 0);
	// CSの実行
	if(runCS) immediate_context->Dispatch(DispathNo, 1, 1);

	immediate_context->CopyResource(VerticesBuffer.Get(), InputBuffer.Get());	// 描画前に計算した値を頂点バッファに入れる

	// 結果をCPUから読み込む
	immediate_context->CopyResource(CPUReadBackBuffer.Get(), OutputBuffer.Get());
	// 結果を頂点バッファ情報へコピーする
	hr = immediate_context->Map(CPUReadBackBuffer.Get(), 0, D3D11_MAP_READ, 0, &MappedResource);
	{
		VBuffer* pt = static_cast<VBuffer*>(MappedResource.pData);
		memcpy(vVecBuf.data(), pt, sizeof(VBuffer) * ParticleAmount);
	}
	immediate_context->Unmap(CPUReadBackBuffer.Get(), 0);
	//----------------------------------------------------
}

void GPUParticle::Draw()
{
	ID3D11DeviceContext* immediate_context = FRAMEWORK->GetDeviceContext();

	// ***************************************
	// VSに定数バッファを設定
	immediate_context->VSSetConstantBuffers(0, 1, DynamicCBuffer.GetAddressOf());
	// PSに定数バッファを設定
	immediate_context->PSSetConstantBuffers(0, 1, DynamicCBuffer.GetAddressOf());
	immediate_context->GSSetConstantBuffers(0, 1, DynamicCBuffer.GetAddressOf());

	// ***************************************

	// IAに頂点バッファを設定
	UINT strides[1] = { sizeof(VBuffer) };
	UINT offsets[1] = { 0 };
	immediate_context->IASetVertexBuffers(0, 1, VerticesBuffer.GetAddressOf(), strides, offsets);

	// IAにプリミティブの種類を設定
	immediate_context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	// 各シェーダのセット
	ParticleShader->Activate();

	texture->Set(0);
	sample->Set(0);

	const UINT MASK = 0xffffffff;
	immediate_context->OMSetBlendState(FRAMEWORK->GetBlendState(FRAMEWORK->BS_ALPHA), nullptr, MASK);
	immediate_context->RSSetState(FRAMEWORK->GetRasterizerState(FRAMEWORK->RS_SOLID_NONE));
	sample->Set(0);
	// ***************************************
	// 描画する
	immediate_context->Draw(ParticleAmount, 0);
	// シェーダの無効化
	ParticleShader->Inactivate();
	//----------------------------------------------------
}

void GPUParticle::SetParticle()
{

	//// forで回してActiveでなければ消す処理…なんだけどあまりにもforが多すぎて動作しない
	//for (std::vector<VBuffer>::iterator itr = vVecBuf.begin(); itr != vVecBuf.end();)
	//{
	//	if(itr->Active)
	//	{
	//		++itr;
	//	}
	//	else
	//	{
	//		itr = vVecBuf.erase(itr);
	//	}
	//}

	// TODO パーティクル一括削除。応急処置なだけ
	vVecBuf.clear();

	for (int i = 0; i < ParticleAmount; ++i) {
		VBuffer data;
		// 初期位置の設定
		data.Position.x = ((i % DispathNo) - (DispathNo / 2.0f)) / (float)DispathNo * 0.005f;
		data.Position.z = ((i / DispathNo) - (DispathNo / 2.0f)) / (float)DispathNo * 0.005f;
		data.Position.y = 0.1f;

		// 初期速度の設定
		data.Velocity.x = ((rand() % 2000) - 1000) * 0.00005f;
		data.Velocity.y = ((rand() % 2000) - 1000) * 0.00005f;
		data.Velocity.z = ((rand() % 2000) - 1000) * 0.00005f;

		data.Force = DirectX::XMFLOAT3(0, -0.0005f, 0);//加速度

		// その他
		data.Color = DirectX::XMFLOAT4(testColor[0], testColor[1], testColor[2], testColor[3]);
		data.Active = true;
		testLife = 50;
		data.Life = testLife * (rand() % 10 + 1);
		vVecBuf.emplace_back(data);	// 格納
	}
}


void GPUParticle::SetSceneConstantBuffer(const ID3D11Buffer* cbBuf)
{
	memcpy(ConstantBuffer.Get(), cbBuf, sizeof(ID3D11Buffer));
}

// GPUParticleに限らないと思うので今回はメンバ関数として定義
void GPUParticle::CreateConstantBuffer(ID3D11Buffer** dstBuf, size_t size, bool dynamicFlg)
{
	Microsoft::WRL::ComPtr<ID3D11Device> device = FRAMEWORK->GetDevice();

	*dstBuf = nullptr;
	D3D11_BUFFER_DESC buffer_desc{};
	ZeroMemory(&buffer_desc, sizeof(D3D11_BUFFER_DESC));	// 一旦初期化
	buffer_desc.ByteWidth = static_cast<UINT>(size);	// 型指定
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	if (dynamicFlg)
	{
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.CPUAccessFlags = 0;
	}
	HRESULT hr = device->CreateBuffer(&buffer_desc, nullptr, dstBuf);	// バッファ作成
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

}

void GPUParticle::ImguiParticles()
{
#ifdef USE_IMGUI
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.0f, 0.6f, 0.1f, 1.0f));	// これ一つ呼ぶとImGui::PopStyleColorを書かないといけないらしい
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.0f, 0.2f, 0.0f, 1.0f));

	// ライト調整等グローバル設定
	ImGui::Begin("Particles");

	ImGui::SliderFloat("Life", &testLife, 0.0f, 1.0f);
	ImGui::ColorEdit4("Color", (float*)testColor);
	ImGui::Checkbox("RunGPUParticle", &runCS);
	if (ImGui::Button("Particle Set")) { SetParticle(); }

	ImGui::Separator();
	ImGui::Text("DispatchNo: %d", DispathNo);
	ImGui::Text("ParticleAmount: %d", ParticleAmount);

	ImGui::PopStyleColor(2);

	ImGui::End();
#endif

}
