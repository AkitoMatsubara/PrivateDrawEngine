
#include "Geometric_Primitive.h"

Geometric_Cube::Geometric_Cube(ID3D11Device* device, const char* vs_cso_name, const char* ps_cso_name ) :Geometric_Primitive(device, vs_cso_name, ps_cso_name) {
	Vertex vertices[24]{};	// 頂点情報配列（vertices）にすべて頂点の位置・法線情報を格納する。
							// サイズが1.0の正立方体データを作成する（重心を原点にする）。正立方体のコントロールポイント数は 8 個、
							// 1つのコントロールポイントの位置には法線の向き(x,y,z)が違う頂点が3個あるので頂点情報の総数は 8頂点x3軸=24個

	uint32_t indeces[36]{};	// 描画順に三角形の頂点情報(verticesの配列番号)を入れる
							// 数字の若い所から始まり、時計回りが表面になるように格納

	/*     LEFT_TOP  *------*  RIGHT_TOP     */
	/*               |   ／ |                */
	/*               | ／   |                */
	/*  LEFT_BOTTOM  *------*  RIGHT_BOTTOM  */

	// 上面
	int face = FACE::TOP_FACE;
	vertices[face * 4 + LEFT_TOP].position = XMFLOAT3(LEFT, TOP, BACK);
	vertices[face * 4 + RIGHT_TOP].position = XMFLOAT3(RIGHT, TOP, BACK);
	vertices[face * 4 + LEFT_BOTTOM].position = XMFLOAT3(LEFT, TOP, FRONT);
	vertices[face * 4 + RIGHT_BOTTOM].position = XMFLOAT3(RIGHT, TOP, FRONT);
	vertices[face * 4 + LEFT_TOP].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertices[face * 4 + RIGHT_TOP].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertices[face * 4 + LEFT_BOTTOM].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertices[face * 4 + RIGHT_BOTTOM].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

	indeces[face * 6 + 0] = face * 4 + LEFT_TOP;
	indeces[face * 6 + 1] = face * 4 + RIGHT_TOP;
	indeces[face * 6 + 2] = face * 4 + LEFT_BOTTOM;

	indeces[face * 6 + 3] = face * 4 + RIGHT_TOP;
	indeces[face * 6 + 4] = face * 4 + RIGHT_BOTTOM;
	indeces[face * 6 + 5] = face * 4 + LEFT_BOTTOM;

	// 下面
	face = FACE::BOTTOM_FACE;
	vertices[face * 4 + LEFT_TOP].position = XMFLOAT3(LEFT, BOTTOM, BACK);
	vertices[face * 4 + RIGHT_TOP].position = XMFLOAT3(RIGHT, BOTTOM, BACK);
	vertices[face * 4 + LEFT_BOTTOM].position = XMFLOAT3(LEFT, BOTTOM, FRONT);
	vertices[face * 4 + RIGHT_BOTTOM].position = XMFLOAT3(RIGHT, BOTTOM, FRONT);
	vertices[face * 4 + LEFT_TOP].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);
	vertices[face * 4 + RIGHT_TOP].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);
	vertices[face * 4 + LEFT_BOTTOM].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);
	vertices[face * 4 + RIGHT_BOTTOM].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);

	indeces[face * 6 + 0] = face * 4 + LEFT_TOP;
	indeces[face * 6 + 1] = face * 4 + LEFT_BOTTOM;
	indeces[face * 6 + 2] = face * 4 + RIGHT_TOP;

	indeces[face * 6 + 3] = face * 4 + RIGHT_TOP;
	indeces[face * 6 + 4] = face * 4 + LEFT_BOTTOM;
	indeces[face * 6 + 5] = face * 4 + RIGHT_BOTTOM;

	// 前面
	face = FACE::FRONT_FACE;
	vertices[face * 4 + LEFT_TOP].position = XMFLOAT3(LEFT, TOP, FRONT);
	vertices[face * 4 + RIGHT_TOP].position = XMFLOAT3(RIGHT, TOP, FRONT);
	vertices[face * 4 + LEFT_BOTTOM].position = XMFLOAT3(LEFT, BOTTOM, FRONT);
	vertices[face * 4 + RIGHT_BOTTOM].position = XMFLOAT3(RIGHT, BOTTOM, FRONT);
	vertices[face * 4 + LEFT_TOP].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertices[face * 4 + RIGHT_TOP].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertices[face * 4 + LEFT_BOTTOM].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertices[face * 4 + RIGHT_BOTTOM].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	indeces[face * 6 + 0] = face * 4 + LEFT_TOP;
	indeces[face * 6 + 1] = face * 4 + RIGHT_TOP;
	indeces[face * 6 + 2] = face * 4 + LEFT_BOTTOM;

	indeces[face * 6 + 3] = face * 4 + RIGHT_TOP;
	indeces[face * 6 + 4] = face * 4 + RIGHT_BOTTOM;
	indeces[face * 6 + 5] = face * 4 + LEFT_BOTTOM;

	// 後面
	face = FACE::BACK_FACE;
	vertices[face * 4 + LEFT_TOP].position = XMFLOAT3(LEFT, TOP, BACK);
	vertices[face * 4 + RIGHT_TOP].position = XMFLOAT3(RIGHT, TOP, BACK);
	vertices[face * 4 + LEFT_BOTTOM].position = XMFLOAT3(LEFT, BOTTOM, BACK);
	vertices[face * 4 + RIGHT_BOTTOM].position = XMFLOAT3(RIGHT, BOTTOM, BACK);
	vertices[face * 4 + LEFT_TOP].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);
	vertices[face * 4 + RIGHT_TOP].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);
	vertices[face * 4 + LEFT_BOTTOM].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);
	vertices[face * 4 + RIGHT_BOTTOM].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);

	indeces[face * 6 + 0] = face * 4 + LEFT_TOP;
	indeces[face * 6 + 1] = face * 4 + LEFT_BOTTOM;
	indeces[face * 6 + 2] = face * 4 + RIGHT_TOP;

	indeces[face * 6 + 3] = face * 4 + RIGHT_TOP;
	indeces[face * 6 + 4] = face * 4 + LEFT_BOTTOM;
	indeces[face * 6 + 5] = face * 4 + RIGHT_BOTTOM;

	// 右面
	face = FACE::RIGHT_FACE;
	vertices[face * 4 + LEFT_TOP].position = XMFLOAT3(RIGHT, TOP, FRONT);
	vertices[face * 4 + RIGHT_TOP].position = XMFLOAT3(RIGHT, TOP, BACK);
	vertices[face * 4 + LEFT_BOTTOM].position = XMFLOAT3(RIGHT, BOTTOM, FRONT);
	vertices[face * 4 + RIGHT_BOTTOM].position = XMFLOAT3(RIGHT, BOTTOM, BACK);
	vertices[face * 4 + LEFT_TOP].normal = XMFLOAT3(1.0f, 0.0f, 0.0f);
	vertices[face * 4 + RIGHT_TOP].normal = XMFLOAT3(1.0f, 0.0f, 0.0f);
	vertices[face * 4 + LEFT_BOTTOM].normal = XMFLOAT3(1.0f, 0.0f, 0.0f);
	vertices[face * 4 + RIGHT_BOTTOM].normal = XMFLOAT3(1.0f, 0.0f, 0.0f);

	indeces[face * 6 + 0] = face * 4 + LEFT_TOP;
	indeces[face * 6 + 1] = face * 4 + RIGHT_TOP;
	indeces[face * 6 + 2] = face * 4 + LEFT_BOTTOM;

	indeces[face * 6 + 3] = face * 4 + RIGHT_TOP;
	indeces[face * 6 + 4] = face * 4 + RIGHT_BOTTOM;
	indeces[face * 6 + 5] = face * 4 + LEFT_BOTTOM;

	// 左面
	face = FACE::LEFT_FACE;
	vertices[face * 4 + LEFT_TOP].position = XMFLOAT3(LEFT, TOP, FRONT);
	vertices[face * 4 + RIGHT_TOP].position = XMFLOAT3(LEFT, TOP, BACK);
	vertices[face * 4 + LEFT_BOTTOM].position = XMFLOAT3(LEFT, BOTTOM, FRONT);
	vertices[face * 4 + RIGHT_BOTTOM].position = XMFLOAT3(LEFT, BOTTOM, BACK);
	vertices[face * 4 + LEFT_TOP].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	vertices[face * 4 + RIGHT_TOP].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	vertices[face * 4 + LEFT_BOTTOM].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	vertices[face * 4 + RIGHT_BOTTOM].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);

	indeces[face * 6 + 0] = face * 4 + LEFT_TOP;
	indeces[face * 6 + 1] = face * 4 + LEFT_BOTTOM;
	indeces[face * 6 + 2] = face * 4 + RIGHT_TOP;

	indeces[face * 6 + 3] = face * 4 + RIGHT_TOP;
	indeces[face * 6 + 4] = face * 4 + LEFT_BOTTOM;
	indeces[face * 6 + 5] = face * 4 + RIGHT_BOTTOM;

	Create_com_buffers(device, vertices, 24, indeces, 36);
}
