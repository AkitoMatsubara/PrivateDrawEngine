
#include "geometric_primitive.h"

Geometric_Cylinder::Geometric_Cylinder(ID3D11Device* device, u_int slices, const char* vs_cso_name, const char* ps_cso_name) :Geometric_Primitive(device, vs_cso_name, ps_cso_name) {
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	Vertex vertex;
	u_int base_index = 0;

	float r = 0.5f;	// 半径
	float height = 1.0f;	// 円柱の高さ
	float degree = 2.0f * 3.141592653589893f / slices;	// 2 * Pi = 360° これを分割数で割ることによって一つの三角形の角度が求まる

	//- 早い話こんなかんじ -//
	/*          2           */
	/*          /\          */
	/*         / |\         */
	/*        /  | \        */
	/*       /  /0\ \       */
	/*      /  /   \ \      */
	/*   3 *----------* 1   */


	// 上面位置

	// 上面の円
	{
		vertex.position = XMFLOAT3(0.0f, height * 0.5f, 0.0f);
		vertex.normal = XMFLOAT3(0.0f, +1.0f, 0.0f);
		vertices.push_back(vertex);	// 上面の中心点を作成(vertices[0])
		for (u_int i = 0; i < slices; ++i)
		{
			float x = r * cosf(i * degree);		// cos(0)=1,cos(90)=0,cos(60)=0.5
			float z = r * sinf(i * degree);		// sin(0)=0,sin(90)=1,sin(30)=0.5
			float y = height * 0.5f;
			vertex.position = XMFLOAT3(x, y, z);
			vertices.push_back(vertex);	// 各要素に順番に情報を格納していく
		}
		base_index = slices * 0 + 0;	// 下の説明をわかりやすくするための*0+0
		for (u_int i = 0; i < slices - 1; ++i)
		{
			// 時計回りに三角形を組み立てていく
			indices.push_back(base_index + 0);		// 中心から三角形を作るので中心点(vertices[0])は不動なり...!
			indices.push_back(base_index + i + 2);	// 法線方向から見て時計回りに三角形を作る ラスタライザステートのCCWの有無で描画面が決まる的な
			indices.push_back(base_index + i + 1);
		}
		indices.push_back(base_index + 0);			//									   //
		indices.push_back(base_index + 1);			// 最終点から1番目を結んで最後の三角形 //
		indices.push_back(base_index + slices);		//									   //
	}

	// 下面の円
	{
		vertex.position = XMFLOAT3(0.0f, -height * 0.5f, 0.0f);
		vertex.normal = XMFLOAT3(0.0f, -1.0f, 0.0f);
		vertices.push_back(vertex);	// 下面の中心点を作成(vertices[slices])
		for (u_int i = 0; i < slices; ++i)
		{
			float x = r * cosf(i * degree);
			float z = r * sinf(i * degree);
			float y = -height * 0.5f;
			vertex.position = XMFLOAT3(x, y, z);
			vertices.push_back(vertex);	// 各要素に順番に情報を格納していく
		}
		base_index = slices * 1 + 1;	// 上面で生成した三角形の次の番号に下面三角形を生成したいのでslices+1
		for (u_int i = 0; i < slices - 1; ++i)
		{
			// 時計回りに三角形を組み立てていく
			indices.push_back(base_index + 0);		// 中心から三角形を作るので中心点(vertices[slices])は不動なり...!
			indices.push_back(base_index + i + 1);	// 法線方向から見て時計回りに三角形を作る  ラスタライザステートのCCWの有無で描画面が決まる的な
			indices.push_back(base_index + i + 2);
		}
		indices.push_back(base_index + 0);					//									   //
		indices.push_back(base_index + (slices - 1) + 1);	// 最終点から1番目を結んで最後の三角形 //
		indices.push_back(base_index + 1);					//									   //
	}

	// 横面の長方形
	{
		for (u_int i = 0; i < slices; ++i)
		{
			float x = r * cosf(i * degree);
			float z = r * sinf(i * degree);
			// 上の頂点
			vertex.position = XMFLOAT3(x, +0.5f, z);
			vertex.normal = XMFLOAT3(x, 0.0f, z);	// 横向きなのでxz方向を向く法線
			vertices.push_back(vertex);
			// 下の頂点 連番で作る
			vertex.position = XMFLOAT3(x, -0.5f, z);
			vertex.normal = XMFLOAT3(x, 0.0f, z);
			vertices.push_back(vertex);
		}
		base_index = slices * 2 + 2;	// 上下面の次に生成したいので*2(上下)+1(次)+1(の次)
		for (u_int i = 0; i < slices - 1; ++i)
		{
			// 四角形と同じ生成方法
			indices.push_back(base_index + i * 2 + 0);
			indices.push_back(base_index + i * 2 + 2);
			indices.push_back(base_index + i * 2 + 1);

			indices.push_back(base_index + i * 2 + 1);
			indices.push_back(base_index + i * 2 + 2);
			indices.push_back(base_index + i * 2 + 3);
		}
		// 最後の三角形は最初に作成した頂点も使用するので少し式が複雑
		// 最終の辺が最初に作成した頂点を使用するので
		// (0)が最初に作成した頂点を使用する箇所
		/*  0  *------*  2  */
		/*     |   ／ |     */
		/*     | ／   |     */
		/*  1  *------*  3  */

		indices.push_back(base_index + (slices - 1) * 2 + 0);	// 0
		indices.push_back(base_index + (0) * 2 + 0);			// 2
		indices.push_back(base_index + (slices - 1) * 2 + 1);	// 1

		indices.push_back(base_index + (slices - 1) * 2 + 1);	// 1
		indices.push_back(base_index + (0) * 2 + 0);			// 2
		indices.push_back(base_index + (0) * 2 + 1);			// 3
	}

	Create_com_buffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());

}