#include "UseComputeShader.h"
#include "GPUParticle.h"
#include "Camera.h"
#include "framework.h"

bool GPUParticle::Init()
{
	Microsoft::WRL::ComPtr<ID3D11Device> device = FRAMEWORK->GetDevice();

	{
		//コンピュート計算のワークをセット

		DispathNo = 100;  // ディスパッチ数
		const static int CS_NUMTHREAD_NO = 500;	// CSの実行数
		ParticleAmount = DispathNo * CS_NUMTHREAD_NO;        // パーティクルの数 (DispatchNo * CS_numthread)
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
			D3D11_SUBRESOURCE_DATA vSubData;//頂点バッファ用初期化用データを作成
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
				data.Position.x = rand() % 41 - 20;
				data.Position.z = rand() % 41 - 20;
				data.Position.y = 0.1f;
				data.Position.w = ((rand() % 101) + 50) * 0.01f;	// ここは最大ライフを格納する
				// 初期速度の設定
				data.Velocity.x = ((rand() % 2000) - 1000) * 0.00005f;
				data.Velocity.y = ((rand() % 2000) - 1000) * 0.00005f;
				data.Velocity.z = ((rand() % 2000) - 1000) * 0.00005f;

				data.Force = DirectX::XMFLOAT3(0, GRABITY, 0);//加速度

				// その他
				data.Color = DirectX::XMFLOAT4(testColor[0], testColor[1], testColor[2], testColor[3]);
				data.Life = ((rand() % 101) + 50) * 0.01f;
				data.Active = false;

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
		// Samplerの設定
		if (!sample)
		{
			sample = std::make_shared<Sampler>(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);
		}
		//描画用シェーダ
		if (ParticleShader == nullptr)
		{
			// 入力要素
			D3D11_INPUT_ELEMENT_DESC layout[] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
					D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
					D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "FORCE",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
					D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
					D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "LIFE",    0, DXGI_FORMAT_R32_FLOAT, 0,
					D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "ACTIVE",    0, DXGI_FORMAT_R32_UINT, 0,
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
		texture->Load(L".\\Resources\\ParticleImage\\particle_W.png");
	}
	return true;
}

void GPUParticle::Update()
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

	g_cbCBuffer.projection	= DirectX::XMMatrixTranspose(Camera::getInstance().GetProjection());
	g_cbCBuffer.view		= DirectX::XMMatrixTranspose(Camera::getInstance().GetView());	// 行列をシェーダに渡すには転置行列にする
	g_cbCBuffer.ParticleSize.x = 0.03f;
	g_cbCBuffer.ParticleSize.y = 0.03f;
	g_cbCBuffer.EyePos = DirectX::SimpleMath::Vector4(Camera::getInstance().GetPos().x, Camera::getInstance().GetPos().y, Camera::getInstance().GetPos().z, 1.0f);

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
	immediate_context->CSSetShader(ParticleShader->GetCS(), 0, 0);
	immediate_context->CSSetConstantBuffers(0, 1, DynamicCBuffer.GetAddressOf());

	// CPUを介さずやり取りを行うため、２つのバッファで位置等を交互に出し入れしていた(過去
	// 0の情報をもとに計算、結果を1に → 1の情報をもとに計算、結果を0に…  というように
	// TODO:今は亡き手法、後でコメント消そう

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

	static const UINT MASK = 0xffffffff;
	(blendNone) ? 
		immediate_context->OMSetBlendState(FRAMEWORK->GetBlendState(FRAMEWORK->BS_NONE), nullptr, MASK)
	  : immediate_context->OMSetBlendState(FRAMEWORK->GetBlendState(FRAMEWORK->BS_ADD), nullptr, MASK);

	immediate_context->RSSetState(FRAMEWORK->GetRasterizerState(FRAMEWORK->RS_SOLID_BACK_CCW));
	// ***************************************
	// 描画する
	immediate_context->Draw(ParticleAmount, 0);
	// シェーダの無効化
	ParticleShader->Inactivate();
	texture->Set(0, false);
	//----------------------------------------------------
}

void GPUParticle::Play(){
	Update();
	Draw();
}

void GPUParticle::SetParticle()
{

	// TODO パーティクル一括削除。応急処置なだけ
	//vVecBuf.clear();
	const static int STAGE_AREA = 51;	// パーティクルを発生させる範囲

	//for (int i = 0; i < ParticleAmount; ++i) {
	for (int i = 0; i < vVecBuf.size(); ++i) {
		VBuffer data;
		// 初期位置の設定
		data.Position.x = rand() % STAGE_AREA - (STAGE_AREA*0.5f);
		data.Position.z = rand() % STAGE_AREA - (STAGE_AREA*0.5f);
		data.Position.y = 0.1f;
		data.Position.w = ((rand() % 101) + 50) * 0.01f;	// ここは最大ライフを格納する

		// 初期速度の設定
		data.Velocity.x = ((rand() % 2000) - 1000) * 0.00005f;
		data.Velocity.y = ((rand() % 2000) - 1000) * 0.00005f;
		data.Velocity.z = ((rand() % 2000) - 1000) * 0.00005f;
		data.Force = DirectX::XMFLOAT3(0, GRABITY, 0); //加速度

		// その他
		data.Color = DirectX::XMFLOAT4(testColor[0], testColor[1], testColor[2], testColor[3]);
		data.Life = (testLife)?testLife: ((rand() % 101) + 50) * 0.01f;
		data.Active = true;
		vVecBuf.at(i) = data;	// 格納
	}
}
void GPUParticle::SetFirstPos(DirectX::SimpleMath::Vector3 pos) {
	// TODO パーティクル一括削除。応急処置なだけ
	vVecBuf.clear();

	for (int i = 0; i < ParticleAmount; ++i) {
		VBuffer data;
		// 初期位置の設定
		data.Position.x = pos.x;
		data.Position.z = pos.z;
		data.Position.y = pos.y;
		data.Position.w = ((rand() % 101) + 50) * 0.01f;	// ここは最大ライフを格納する

		// 初期速度の設定
		data.Velocity.x = ((rand() % 2000) - 1000) * 0.00005f;
		data.Velocity.y = ((rand() % 2000) - 1000) * 0.00005f;
		data.Velocity.z = ((rand() % 2000) - 1000) * 0.00005f;

		data.Force = DirectX::XMFLOAT3(0, -0.0005f, 0);//加速度

		// その他
		data.Color = DirectX::XMFLOAT4(testColor[0], testColor[1], testColor[2], testColor[3]);
		data.Life = ((rand() % 51) + 50) * 0.01f;
		data.Active = true;
		vVecBuf.emplace_back(data);	// 格納
	}

}

void GPUParticle::SpaceEffect() {
	static const float STAGE_AREA = 50.0f;	// パーティクルを発生させる範囲

	for (int i = 0; i < ParticleAmount; ++i) {
		VBuffer data;
		// 初期位置の設定
		data.Position.x = rand() % (static_cast<int>(STAGE_AREA)*2)-STAGE_AREA;
		data.Position.z = rand() % (static_cast<int>(STAGE_AREA)*2)-STAGE_AREA;
		data.Position.y = 0.1f;
		data.Position.w = ((rand() % 101) + 50) * 0.01f;	// ここは最大ライフを格納する

		// 初期速度の設定
		data.Velocity.x = ((rand() % 2000) - 1000) * 0.00005f;
		data.Velocity.y = ((rand() % 2000) - 1000) * 0.00005f;
		data.Velocity.z = ((rand() % 2000) - 1000) * 0.00005f;
		const static float GRABITY = -0.005;
		data.Force = DirectX::XMFLOAT3(0, GRABITY, 0); //加速度

		// その他
		data.Color = DirectX::XMFLOAT4(testColor[0], testColor[1], testColor[2], testColor[3]);
		data.Active = true;
		data.Life = ((rand() % 51) + 50) * 0.01f;
		vVecBuf.emplace_back(data);	// 格納
	}

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

	ImGui::SliderFloat("Life", &testLife, 0.0f, 50.0f);
	ImGui::ColorEdit4("Color", (float*)testColor);
	ImGui::Checkbox("RunGPUParticle", &runCS);
	if (ImGui::Button("Particle Set")) { SetParticle(); }
	ImGui::Checkbox("BS_NONE", &blendNone);

	ImGui::Separator();
	ImGui::Text("DispatchNo: %d", DispathNo);
	ImGui::Text("ParticleAmount: %d", ParticleAmount);

	ImGui::PopStyleColor(2);

	ImGui::End();
#endif

}
