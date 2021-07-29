
#include "geometric_primitive.h"

Geometric_Cylinder::Geometric_Cylinder(ID3D11Device* device, u_int slices, const char* vs_cso_name, const char* ps_cso_name) :Geometric_Primitive(device, vs_cso_name, ps_cso_name) {
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	Vertex vertex;
	u_int base_index = 0;

	float r = 0.5f;	// ���a
	float height = 1.0f;	// �~���̍���
	float degree = 2.0f * 3.141592653589893f / slices;	// 2 * Pi = 360�� ����𕪊����Ŋ��邱�Ƃɂ���Ĉ�̎O�p�`�̊p�x�����܂�

	//- �����b����Ȃ��� -//
	/*          2           */
	/*          /\          */
	/*         / |\         */
	/*        /  | \        */
	/*       /  /0\ \       */
	/*      /  /   \ \      */
	/*   3 *----------* 1   */


	// ��ʈʒu

	// ��ʂ̉~
	{
		vertex.position = XMFLOAT3(0.0f, height * 0.5f, 0.0f);
		vertex.normal = XMFLOAT3(0.0f, +1.0f, 0.0f);
		vertices.push_back(vertex);	// ��ʂ̒��S�_���쐬(vertices[0])
		for (u_int i = 0; i < slices; ++i)
		{
			float x = r * cosf(i * degree);		// cos(0)=1,cos(90)=0,cos(60)=0.5
			float z = r * sinf(i * degree);		// sin(0)=0,sin(90)=1,sin(30)=0.5
			float y = height * 0.5f;
			vertex.position = XMFLOAT3(x, y, z);
			vertices.push_back(vertex);	// �e�v�f�ɏ��Ԃɏ����i�[���Ă���
		}
		base_index = slices * 0 + 0;	// ���̐������킩��₷�����邽�߂�*0+0
		for (u_int i = 0; i < slices - 1; ++i)
		{
			// ���v���ɎO�p�`��g�ݗ��ĂĂ���
			indices.push_back(base_index + 0);		// ���S����O�p�`�����̂Œ��S�_(vertices[0])�͕s���Ȃ�...!
			indices.push_back(base_index + i + 2);	// �@���������猩�Ď��v���ɎO�p�`����� ���X�^���C�U�X�e�[�g��CCW�̗L���ŕ`��ʂ����܂�I��
			indices.push_back(base_index + i + 1);
		}
		indices.push_back(base_index + 0);			//									   //
		indices.push_back(base_index + 1);			// �ŏI�_����1�Ԗڂ�����ōŌ�̎O�p�` //
		indices.push_back(base_index + slices);		//									   //
	}

	// ���ʂ̉~
	{
		vertex.position = XMFLOAT3(0.0f, -height * 0.5f, 0.0f);
		vertex.normal = XMFLOAT3(0.0f, -1.0f, 0.0f);
		vertices.push_back(vertex);	// ���ʂ̒��S�_���쐬(vertices[slices])
		for (u_int i = 0; i < slices; ++i)
		{
			float x = r * cosf(i * degree);
			float z = r * sinf(i * degree);
			float y = -height * 0.5f;
			vertex.position = XMFLOAT3(x, y, z);
			vertices.push_back(vertex);	// �e�v�f�ɏ��Ԃɏ����i�[���Ă���
		}
		base_index = slices * 1 + 1;	// ��ʂŐ��������O�p�`�̎��̔ԍ��ɉ��ʎO�p�`�𐶐��������̂�slices+1
		for (u_int i = 0; i < slices - 1; ++i)
		{
			// ���v���ɎO�p�`��g�ݗ��ĂĂ���
			indices.push_back(base_index + 0);		// ���S����O�p�`�����̂Œ��S�_(vertices[slices])�͕s���Ȃ�...!
			indices.push_back(base_index + i + 1);	// �@���������猩�Ď��v���ɎO�p�`�����  ���X�^���C�U�X�e�[�g��CCW�̗L���ŕ`��ʂ����܂�I��
			indices.push_back(base_index + i + 2);
		}
		indices.push_back(base_index + 0);					//									   //
		indices.push_back(base_index + (slices - 1) + 1);	// �ŏI�_����1�Ԗڂ�����ōŌ�̎O�p�` //
		indices.push_back(base_index + 1);					//									   //
	}

	// ���ʂ̒����`
	{
		for (u_int i = 0; i < slices; ++i)
		{
			float x = r * cosf(i * degree);
			float z = r * sinf(i * degree);
			// ��̒��_
			vertex.position = XMFLOAT3(x, +0.5f, z);
			vertex.normal = XMFLOAT3(x, 0.0f, z);	// �������Ȃ̂�xz�����������@��
			vertices.push_back(vertex);
			// ���̒��_ �A�Ԃō��
			vertex.position = XMFLOAT3(x, -0.5f, z);
			vertex.normal = XMFLOAT3(x, 0.0f, z);
			vertices.push_back(vertex);
		}
		base_index = slices * 2 + 2;	// �㉺�ʂ̎��ɐ����������̂�*2(�㉺)+1(��)+1(�̎�)
		for (u_int i = 0; i < slices - 1; ++i)
		{
			// �l�p�`�Ɠ����������@
			indices.push_back(base_index + i * 2 + 0);
			indices.push_back(base_index + i * 2 + 2);
			indices.push_back(base_index + i * 2 + 1);

			indices.push_back(base_index + i * 2 + 1);
			indices.push_back(base_index + i * 2 + 2);
			indices.push_back(base_index + i * 2 + 3);
		}
		// �Ō�̎O�p�`�͍ŏ��ɍ쐬�������_���g�p����̂ŏ����������G
		// �ŏI�̕ӂ��ŏ��ɍ쐬�������_���g�p����̂�
		// (0)���ŏ��ɍ쐬�������_���g�p����ӏ�
		/*  0  *------*  2  */
		/*     |   �^ |     */
		/*     | �^   |     */
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