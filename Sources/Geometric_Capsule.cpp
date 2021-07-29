#include "geometric_primitive.h"

Geometric_Capsule::Geometric_Capsule(ID3D11Device* device, FLOAT radian, FLOAT height, u_int slices, u_int stacks, const char* vs_cso_name, const char* ps_cso_name) :Geometric_Primitive(device, vs_cso_name, ps_cso_name) {
	// 基本は球生成の応用
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	u_int base_index = 0;
	u_int vertex_offset = 2;	// 上下頂点分のずれ

	float h = height * 0.5f;	// 円柱の高さ(の半分)
	float degree = 2.0f * 3.141592653589893f / slices;	// 2 * Pi = 360° これを分割数で割ることによって一つの三角形の角度が求まる

	// 球部分
	{
		Vertex top_vertex;		// 上の頂点(上向き)
		top_vertex.position = DirectX::XMFLOAT3(0.0f, h + radian, 0.0f);
		top_vertex.normal = DirectX::XMFLOAT3(0.0f, +1.0f, 0.0f);

		Vertex bottom_vertex;	// 下の頂点(下向き)
		bottom_vertex.position = DirectX::XMFLOAT3(0.0f, -h - radian, 0.0f);
		bottom_vertex.normal = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);

		vertices.push_back(top_vertex);	// 上に頂点を[0]に登録

		float phi_step = DirectX::XM_PI / stacks;			// Φ=黄金比？のステップ?
		float theta_step = 2.0f * DirectX::XM_PI / slices;	// θのステップ?

		// 上側の球設定
		{
			for (u_int i = 1; i <= stacks - 1; ++i)	// 半分だけ作成	上の頂点を除外するために-１している
			{
				float phi = i * phi_step;

				// 円の頂点.
				for (u_int j = 0; j <= slices; ++j)
				{
					float theta = j * theta_step;

					Vertex v;

					// 球面からカルテシアン(直交座標系)へ
					v.position.x = radian * sinf(phi) * cosf(theta);
					v.position.y = (i >= stacks / 2) ? radian * cosf(phi) + -h : radian * cosf(phi) + h;	// 上半分までは上に、半分以降は下に配置する
					v.position.z = radian * sinf(phi) * sinf(theta);

					DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&v.position);	// positionをvector型に
					DirectX::XMStoreFloat3(&v.normal, DirectX::XMVector3Normalize(p));	// 法線の計算

					vertices.push_back(v);	// 右方向にぐるっと作ってる
				}
			}
			vertices.push_back(bottom_vertex);	//一番下頂点をセット
		}

		// 上の円の頂点(1段目)生成
		{
			for (UINT i = 1; i <= slices; ++i)
			{
				indices.push_back(0);	// 頂点
				indices.push_back(i + 1);
				indices.push_back(i);
			}
		}

		// 最初の円の最初の頂点のインデックス分ずらす
		// これは一番上の頂点をスキップしているだけ
		u_int top_offset = 1;
		base_index = 1;
		u_int ring_vertex_count = slices + 1;
		for (u_int step = 0; step < stacks - vertex_offset; ++step)	// 上から下の順	半分設定 -vertex_offsetは上下頂点分を除外している	i=縦向き i段目
		{
			for (u_int j = 0; j < slices; ++j)	// 後から前の順	j=横向き i段j個目
			{
				// 各面の左側三角形
				indices.push_back(top_offset + step * ring_vertex_count + j);			// 上段左
				indices.push_back(top_offset + step * ring_vertex_count + j + 1);		// 上段右
				indices.push_back(top_offset + (step + 1) * ring_vertex_count + j);		// 下段左

				// 各面の右側三角形
				indices.push_back(top_offset + (step + 1) * ring_vertex_count + j);		// 下段左
				indices.push_back(top_offset + step * ring_vertex_count + j + 1);		// 上段右
				indices.push_back(top_offset + (step + 1) * ring_vertex_count + j + 1);	// 下段右
			}
		}

		// 一番下の頂点を最後に追加

		u_int south_pole_index = (u_int)vertices.size() - 1;

		// 最後の円の最初の頂点のインデックス分ずらす
		// これは一番下の頂点をスキップしているだけ

		// 底の円の頂点割当
		{
			base_index = south_pole_index - ring_vertex_count;

			for (u_int i = 0; i < slices; ++i)
			{
				indices.push_back(south_pole_index);
				indices.push_back(base_index + i);
				indices.push_back(base_index + i + 1);
			}
		}
	}

	Create_com_buffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());
}
