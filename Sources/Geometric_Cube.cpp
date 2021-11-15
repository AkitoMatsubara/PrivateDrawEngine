
#include "framework.h"
#include "geometric_primitive.h"

Geometric_Cube::Geometric_Cube(){
	ID3D11Device* device = FRAMEWORK->GetDevice();

	Vertex vertices[24]{};	// ���_���z��ivertices�j�ɂ��ׂĒ��_�̈ʒu�E�@�������i�[����B
							// �T�C�Y��1.0�̐������̃f�[�^���쐬����i�d�S�����_�ɂ���j�B�������̂̃R���g���[���|�C���g���� 8 �A
							// 1�̃R���g���[���|�C���g�̈ʒu�ɂ͖@���̌���(x,y,z)���Ⴄ���_��3����̂Œ��_���̑����� 8���_x3��=24��

	uint32_t indeces[36]{};	// �`�揇�ɎO�p�`�̒��_���(vertices�̔z��ԍ�)������
							// �����̎Ⴂ������n�܂�A���v��肪�\�ʂɂȂ�悤�Ɋi�[

	/*     LEFT_TOP  *------*  RIGHT_TOP     */
	/*               |   �^ |                */
	/*               | �^   |                */
	/*  LEFT_BOTTOM  *------*  RIGHT_BOTTOM  */

	// ���
	int face = FACE::TOP_FACE;
	vertices[face * 4 + LEFT_TOP].position = DirectX::SimpleMath::Vector3(LEFT, TOP, BACK);
	vertices[face * 4 + RIGHT_TOP].position = DirectX::SimpleMath::Vector3(RIGHT, TOP, BACK);
	vertices[face * 4 + LEFT_BOTTOM].position = DirectX::SimpleMath::Vector3(LEFT, TOP, FRONT);
	vertices[face * 4 + RIGHT_BOTTOM].position = DirectX::SimpleMath::Vector3(RIGHT, TOP, FRONT);
	vertices[face * 4 + LEFT_TOP].normal = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
	vertices[face * 4 + RIGHT_TOP].normal = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
	vertices[face * 4 + LEFT_BOTTOM].normal = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
	vertices[face * 4 + RIGHT_BOTTOM].normal = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);

	indeces[face * 6 + 0] = face * 4 + LEFT_TOP;
	indeces[face * 6 + 1] = face * 4 + RIGHT_TOP;
	indeces[face * 6 + 2] = face * 4 + LEFT_BOTTOM;

	indeces[face * 6 + 3] = face * 4 + RIGHT_TOP;
	indeces[face * 6 + 4] = face * 4 + RIGHT_BOTTOM;
	indeces[face * 6 + 5] = face * 4 + LEFT_BOTTOM;

	// ����
	face = FACE::BOTTOM_FACE;
	vertices[face * 4 + LEFT_TOP].position = DirectX::SimpleMath::Vector3(LEFT, BOTTOM, BACK);
	vertices[face * 4 + RIGHT_TOP].position = DirectX::SimpleMath::Vector3(RIGHT, BOTTOM, BACK);
	vertices[face * 4 + LEFT_BOTTOM].position = DirectX::SimpleMath::Vector3(LEFT, BOTTOM, FRONT);
	vertices[face * 4 + RIGHT_BOTTOM].position = DirectX::SimpleMath::Vector3(RIGHT, BOTTOM, FRONT);
	vertices[face * 4 + LEFT_TOP].normal = DirectX::SimpleMath::Vector3(0.0f, -1.0f, 0.0f);
	vertices[face * 4 + RIGHT_TOP].normal = DirectX::SimpleMath::Vector3(0.0f, -1.0f, 0.0f);
	vertices[face * 4 + LEFT_BOTTOM].normal = DirectX::SimpleMath::Vector3(0.0f, -1.0f, 0.0f);
	vertices[face * 4 + RIGHT_BOTTOM].normal = DirectX::SimpleMath::Vector3(0.0f, -1.0f, 0.0f);

	indeces[face * 6 + 0] = face * 4 + LEFT_TOP;
	indeces[face * 6 + 1] = face * 4 + LEFT_BOTTOM;
	indeces[face * 6 + 2] = face * 4 + RIGHT_TOP;

	indeces[face * 6 + 3] = face * 4 + RIGHT_TOP;
	indeces[face * 6 + 4] = face * 4 + LEFT_BOTTOM;
	indeces[face * 6 + 5] = face * 4 + RIGHT_BOTTOM;

	// �O��
	face = FACE::FRONT_FACE;
	vertices[face * 4 + LEFT_TOP].position = DirectX::SimpleMath::Vector3(LEFT, TOP, FRONT);
	vertices[face * 4 + RIGHT_TOP].position = DirectX::SimpleMath::Vector3(RIGHT, TOP, FRONT);
	vertices[face * 4 + LEFT_BOTTOM].position = DirectX::SimpleMath::Vector3(LEFT, BOTTOM, FRONT);
	vertices[face * 4 + RIGHT_BOTTOM].position = DirectX::SimpleMath::Vector3(RIGHT, BOTTOM, FRONT);
	vertices[face * 4 + LEFT_TOP].normal = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f);
	vertices[face * 4 + RIGHT_TOP].normal = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f);
	vertices[face * 4 + LEFT_BOTTOM].normal = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f);
	vertices[face * 4 + RIGHT_BOTTOM].normal = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f);

	indeces[face * 6 + 0] = face * 4 + LEFT_TOP;
	indeces[face * 6 + 1] = face * 4 + RIGHT_TOP;
	indeces[face * 6 + 2] = face * 4 + LEFT_BOTTOM;

	indeces[face * 6 + 3] = face * 4 + RIGHT_TOP;
	indeces[face * 6 + 4] = face * 4 + RIGHT_BOTTOM;
	indeces[face * 6 + 5] = face * 4 + LEFT_BOTTOM;

	// ���
	face = FACE::BACK_FACE;
	vertices[face * 4 + LEFT_TOP].position = DirectX::SimpleMath::Vector3(LEFT, TOP, BACK);
	vertices[face * 4 + RIGHT_TOP].position = DirectX::SimpleMath::Vector3(RIGHT, TOP, BACK);
	vertices[face * 4 + LEFT_BOTTOM].position = DirectX::SimpleMath::Vector3(LEFT, BOTTOM, BACK);
	vertices[face * 4 + RIGHT_BOTTOM].position = DirectX::SimpleMath::Vector3(RIGHT, BOTTOM, BACK);
	vertices[face * 4 + LEFT_TOP].normal = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f);
	vertices[face * 4 + RIGHT_TOP].normal = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f);
	vertices[face * 4 + LEFT_BOTTOM].normal = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f);
	vertices[face * 4 + RIGHT_BOTTOM].normal = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f);

	indeces[face * 6 + 0] = face * 4 + LEFT_TOP;
	indeces[face * 6 + 1] = face * 4 + LEFT_BOTTOM;
	indeces[face * 6 + 2] = face * 4 + RIGHT_TOP;

	indeces[face * 6 + 3] = face * 4 + RIGHT_TOP;
	indeces[face * 6 + 4] = face * 4 + LEFT_BOTTOM;
	indeces[face * 6 + 5] = face * 4 + RIGHT_BOTTOM;

	// �E��
	face = FACE::RIGHT_FACE;
	vertices[face * 4 + LEFT_TOP].position = DirectX::SimpleMath::Vector3(RIGHT, TOP, FRONT);
	vertices[face * 4 + RIGHT_TOP].position = DirectX::SimpleMath::Vector3(RIGHT, TOP, BACK);
	vertices[face * 4 + LEFT_BOTTOM].position = DirectX::SimpleMath::Vector3(RIGHT, BOTTOM, FRONT);
	vertices[face * 4 + RIGHT_BOTTOM].position = DirectX::SimpleMath::Vector3(RIGHT, BOTTOM, BACK);
	vertices[face * 4 + LEFT_TOP].normal = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
	vertices[face * 4 + RIGHT_TOP].normal = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
	vertices[face * 4 + LEFT_BOTTOM].normal = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
	vertices[face * 4 + RIGHT_BOTTOM].normal = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);

	indeces[face * 6 + 0] = face * 4 + LEFT_TOP;
	indeces[face * 6 + 1] = face * 4 + RIGHT_TOP;
	indeces[face * 6 + 2] = face * 4 + LEFT_BOTTOM;

	indeces[face * 6 + 3] = face * 4 + RIGHT_TOP;
	indeces[face * 6 + 4] = face * 4 + RIGHT_BOTTOM;
	indeces[face * 6 + 5] = face * 4 + LEFT_BOTTOM;

	// ����
	face = FACE::LEFT_FACE;
	vertices[face * 4 + LEFT_TOP].position = DirectX::SimpleMath::Vector3(LEFT, TOP, FRONT);
	vertices[face * 4 + RIGHT_TOP].position = DirectX::SimpleMath::Vector3(LEFT, TOP, BACK);
	vertices[face * 4 + LEFT_BOTTOM].position = DirectX::SimpleMath::Vector3(LEFT, BOTTOM, FRONT);
	vertices[face * 4 + RIGHT_BOTTOM].position = DirectX::SimpleMath::Vector3(LEFT, BOTTOM, BACK);
	vertices[face * 4 + LEFT_TOP].normal = DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 0.0f);
	vertices[face * 4 + RIGHT_TOP].normal = DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 0.0f);
	vertices[face * 4 + LEFT_BOTTOM].normal = DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 0.0f);
	vertices[face * 4 + RIGHT_BOTTOM].normal = DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 0.0f);

	indeces[face * 6 + 0] = face * 4 + LEFT_TOP;
	indeces[face * 6 + 1] = face * 4 + LEFT_BOTTOM;
	indeces[face * 6 + 2] = face * 4 + RIGHT_TOP;

	indeces[face * 6 + 3] = face * 4 + RIGHT_TOP;
	indeces[face * 6 + 4] = face * 4 + LEFT_BOTTOM;
	indeces[face * 6 + 5] = face * 4 + RIGHT_BOTTOM;

	Create_com_buffers(vertices, 24, indeces, 36);
}

Geometric_Cube::Geometric_Cube(float left, float right, float bottom, float top,float front,float back) {
	ID3D11Device* device = FRAMEWORK->GetDevice();

	Vertex vertices[24]{};	// ���_���z��ivertices�j�ɂ��ׂĒ��_�̈ʒu�E�@�������i�[����B
							// �T�C�Y��1.0�̐������̃f�[�^���쐬����i�d�S�����_�ɂ���j�B�������̂̃R���g���[���|�C���g���� 8 �A
							// 1�̃R���g���[���|�C���g�̈ʒu�ɂ͖@���̌���(x,y,z)���Ⴄ���_��3����̂Œ��_���̑����� 8���_x3��=24��

	uint32_t indeces[36]{};	// �`�揇�ɎO�p�`�̒��_���(vertices�̔z��ԍ�)������
							// �����̎Ⴂ������n�܂�A���v��肪�\�ʂɂȂ�悤�Ɋi�[

	/*     LEFT_TOP  *------*  RIGHT_TOP     */
	/*               |   �^ |                */
	/*               | �^   |                */
	/*  LEFT_BOTTOM  *------*  RIGHT_BOTTOM  */

	// ���
	int face = FACE::TOP_FACE;
	vertices[face * 4 + LEFT_TOP].position = DirectX::SimpleMath::Vector3(left, top, back);
	vertices[face * 4 + RIGHT_TOP].position = DirectX::SimpleMath::Vector3(right, top, back);
	vertices[face * 4 + LEFT_BOTTOM].position = DirectX::SimpleMath::Vector3(left, top, front);
	vertices[face * 4 + RIGHT_BOTTOM].position = DirectX::SimpleMath::Vector3(right, top, front);
	vertices[face * 4 + LEFT_TOP].normal = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
	vertices[face * 4 + RIGHT_TOP].normal = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
	vertices[face * 4 + LEFT_BOTTOM].normal = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
	vertices[face * 4 + RIGHT_BOTTOM].normal = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);

	indeces[face * 6 + 0] = face * 4 + LEFT_TOP;
	indeces[face * 6 + 1] = face * 4 + RIGHT_TOP;
	indeces[face * 6 + 2] = face * 4 + LEFT_BOTTOM;

	indeces[face * 6 + 3] = face * 4 + RIGHT_TOP;
	indeces[face * 6 + 4] = face * 4 + RIGHT_BOTTOM;
	indeces[face * 6 + 5] = face * 4 + LEFT_BOTTOM;

	// ����
	face = FACE::BOTTOM_FACE;
	vertices[face * 4 + LEFT_TOP].position = DirectX::SimpleMath::Vector3(left, bottom, back);
	vertices[face * 4 + RIGHT_TOP].position = DirectX::SimpleMath::Vector3(right, bottom, back);
	vertices[face * 4 + LEFT_BOTTOM].position = DirectX::SimpleMath::Vector3(left, bottom, front);
	vertices[face * 4 + RIGHT_BOTTOM].position = DirectX::SimpleMath::Vector3(right, bottom, front);
	vertices[face * 4 + LEFT_TOP].normal = DirectX::SimpleMath::Vector3(0.0f, -1.0f, 0.0f);
	vertices[face * 4 + RIGHT_TOP].normal = DirectX::SimpleMath::Vector3(0.0f, -1.0f, 0.0f);
	vertices[face * 4 + LEFT_BOTTOM].normal = DirectX::SimpleMath::Vector3(0.0f, -1.0f, 0.0f);
	vertices[face * 4 + RIGHT_BOTTOM].normal = DirectX::SimpleMath::Vector3(0.0f, -1.0f, 0.0f);

	indeces[face * 6 + 0] = face * 4 + LEFT_TOP;
	indeces[face * 6 + 1] = face * 4 + LEFT_BOTTOM;
	indeces[face * 6 + 2] = face * 4 + RIGHT_TOP;

	indeces[face * 6 + 3] = face * 4 + RIGHT_TOP;
	indeces[face * 6 + 4] = face * 4 + LEFT_BOTTOM;
	indeces[face * 6 + 5] = face * 4 + RIGHT_BOTTOM;

	// �O��
	face = FACE::FRONT_FACE;
	vertices[face * 4 + LEFT_TOP].position = DirectX::SimpleMath::Vector3(left, top, front);
	vertices[face * 4 + RIGHT_TOP].position = DirectX::SimpleMath::Vector3(right, top, front);
	vertices[face * 4 + LEFT_BOTTOM].position = DirectX::SimpleMath::Vector3(left, bottom, front);
	vertices[face * 4 + RIGHT_BOTTOM].position = DirectX::SimpleMath::Vector3(right, bottom, front);
	vertices[face * 4 + LEFT_TOP].normal = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f);
	vertices[face * 4 + RIGHT_TOP].normal = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f);
	vertices[face * 4 + LEFT_BOTTOM].normal = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f);
	vertices[face * 4 + RIGHT_BOTTOM].normal = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f);

	indeces[face * 6 + 0] = face * 4 + LEFT_TOP;
	indeces[face * 6 + 1] = face * 4 + RIGHT_TOP;
	indeces[face * 6 + 2] = face * 4 + LEFT_BOTTOM;

	indeces[face * 6 + 3] = face * 4 + RIGHT_TOP;
	indeces[face * 6 + 4] = face * 4 + RIGHT_BOTTOM;
	indeces[face * 6 + 5] = face * 4 + LEFT_BOTTOM;

	// ���
	face = FACE::BACK_FACE;
	vertices[face * 4 + LEFT_TOP].position = DirectX::SimpleMath::Vector3(left, top, back);
	vertices[face * 4 + RIGHT_TOP].position = DirectX::SimpleMath::Vector3(right, top, back);
	vertices[face * 4 + LEFT_BOTTOM].position = DirectX::SimpleMath::Vector3(left, bottom, back);
	vertices[face * 4 + RIGHT_BOTTOM].position = DirectX::SimpleMath::Vector3(right, bottom, back);
	vertices[face * 4 + LEFT_TOP].normal = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f);
	vertices[face * 4 + RIGHT_TOP].normal = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f);
	vertices[face * 4 + LEFT_BOTTOM].normal = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f);
	vertices[face * 4 + RIGHT_BOTTOM].normal = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f);

	indeces[face * 6 + 0] = face * 4 + LEFT_TOP;
	indeces[face * 6 + 1] = face * 4 + LEFT_BOTTOM;
	indeces[face * 6 + 2] = face * 4 + RIGHT_TOP;

	indeces[face * 6 + 3] = face * 4 + RIGHT_TOP;
	indeces[face * 6 + 4] = face * 4 + LEFT_BOTTOM;
	indeces[face * 6 + 5] = face * 4 + RIGHT_BOTTOM;

	// �E��
	face = FACE::RIGHT_FACE;
	vertices[face * 4 + LEFT_TOP].position = DirectX::SimpleMath::Vector3(right, top, front);
	vertices[face * 4 + RIGHT_TOP].position = DirectX::SimpleMath::Vector3(right, top, back);
	vertices[face * 4 + LEFT_BOTTOM].position = DirectX::SimpleMath::Vector3(right, bottom, front);
	vertices[face * 4 + RIGHT_BOTTOM].position = DirectX::SimpleMath::Vector3(right, bottom, back);
	vertices[face * 4 + LEFT_TOP].normal = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
	vertices[face * 4 + RIGHT_TOP].normal = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
	vertices[face * 4 + LEFT_BOTTOM].normal = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
	vertices[face * 4 + RIGHT_BOTTOM].normal = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);

	indeces[face * 6 + 0] = face * 4 + LEFT_TOP;
	indeces[face * 6 + 1] = face * 4 + RIGHT_TOP;
	indeces[face * 6 + 2] = face * 4 + LEFT_BOTTOM;

	indeces[face * 6 + 3] = face * 4 + RIGHT_TOP;
	indeces[face * 6 + 4] = face * 4 + RIGHT_BOTTOM;
	indeces[face * 6 + 5] = face * 4 + LEFT_BOTTOM;

	// ����
	face = FACE::LEFT_FACE;
	vertices[face * 4 + LEFT_TOP].position = DirectX::SimpleMath::Vector3(left, top, front);
	vertices[face * 4 + RIGHT_TOP].position = DirectX::SimpleMath::Vector3(left, top, back);
	vertices[face * 4 + LEFT_BOTTOM].position = DirectX::SimpleMath::Vector3(left, bottom, front);
	vertices[face * 4 + RIGHT_BOTTOM].position = DirectX::SimpleMath::Vector3(left, bottom, back);
	vertices[face * 4 + LEFT_TOP].normal = DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 0.0f);
	vertices[face * 4 + RIGHT_TOP].normal = DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 0.0f);
	vertices[face * 4 + LEFT_BOTTOM].normal = DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 0.0f);
	vertices[face * 4 + RIGHT_BOTTOM].normal = DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 0.0f);

	indeces[face * 6 + 0] = face * 4 + LEFT_TOP;
	indeces[face * 6 + 1] = face * 4 + LEFT_BOTTOM;
	indeces[face * 6 + 2] = face * 4 + RIGHT_TOP;

	indeces[face * 6 + 3] = face * 4 + RIGHT_TOP;
	indeces[face * 6 + 4] = face * 4 + LEFT_BOTTOM;
	indeces[face * 6 + 5] = face * 4 + RIGHT_BOTTOM;

	Create_com_buffers(vertices, 24, indeces, 36);
}
