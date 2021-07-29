#include "geometric_primitive.h"

Geometric_Capsule::Geometric_Capsule(ID3D11Device* device, FLOAT radian, FLOAT height, u_int slices, u_int stacks, const char* vs_cso_name, const char* ps_cso_name) :Geometric_Primitive(device, vs_cso_name, ps_cso_name) {
	// ��{�͋������̉��p
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	u_int base_index = 0;
	u_int vertex_offset = 2;	// �㉺���_���̂���

	float h = height * 0.5f;	// �~���̍���(�̔���)
	float degree = 2.0f * 3.141592653589893f / slices;	// 2 * Pi = 360�� ����𕪊����Ŋ��邱�Ƃɂ���Ĉ�̎O�p�`�̊p�x�����܂�

	// ������
	{
		Vertex top_vertex;		// ��̒��_(�����)
		top_vertex.position = DirectX::XMFLOAT3(0.0f, h + radian, 0.0f);
		top_vertex.normal = DirectX::XMFLOAT3(0.0f, +1.0f, 0.0f);

		Vertex bottom_vertex;	// ���̒��_(������)
		bottom_vertex.position = DirectX::XMFLOAT3(0.0f, -h - radian, 0.0f);
		bottom_vertex.normal = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);

		vertices.push_back(top_vertex);	// ��ɒ��_��[0]�ɓo�^

		float phi_step = DirectX::XM_PI / stacks;			// ��=������H�̃X�e�b�v?
		float theta_step = 2.0f * DirectX::XM_PI / slices;	// �Ƃ̃X�e�b�v?

		// �㑤�̋��ݒ�
		{
			for (u_int i = 1; i <= stacks - 1; ++i)	// ���������쐬	��̒��_�����O���邽�߂�-�P���Ă���
			{
				float phi = i * phi_step;

				// �~�̒��_.
				for (u_int j = 0; j <= slices; ++j)
				{
					float theta = j * theta_step;

					Vertex v;

					// ���ʂ���J���e�V�A��(�������W�n)��
					v.position.x = radian * sinf(phi) * cosf(theta);
					v.position.y = (i >= stacks / 2) ? radian * cosf(phi) + -h : radian * cosf(phi) + h;	// �㔼���܂ł͏�ɁA�����ȍ~�͉��ɔz�u����
					v.position.z = radian * sinf(phi) * sinf(theta);

					DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&v.position);	// position��vector�^��
					DirectX::XMStoreFloat3(&v.normal, DirectX::XMVector3Normalize(p));	// �@���̌v�Z

					vertices.push_back(v);	// �E�����ɂ�����ƍ���Ă�
				}
			}
			vertices.push_back(bottom_vertex);	//��ԉ����_���Z�b�g
		}

		// ��̉~�̒��_(1�i��)����
		{
			for (UINT i = 1; i <= slices; ++i)
			{
				indices.push_back(0);	// ���_
				indices.push_back(i + 1);
				indices.push_back(i);
			}
		}

		// �ŏ��̉~�̍ŏ��̒��_�̃C���f�b�N�X�����炷
		// ����͈�ԏ�̒��_���X�L�b�v���Ă��邾��
		u_int top_offset = 1;
		base_index = 1;
		u_int ring_vertex_count = slices + 1;
		for (u_int step = 0; step < stacks - vertex_offset; ++step)	// �ォ�牺�̏�	�����ݒ� -vertex_offset�͏㉺���_�������O���Ă���	i=�c���� i�i��
		{
			for (u_int j = 0; j < slices; ++j)	// �ォ��O�̏�	j=������ i�ij��
			{
				// �e�ʂ̍����O�p�`
				indices.push_back(top_offset + step * ring_vertex_count + j);			// ��i��
				indices.push_back(top_offset + step * ring_vertex_count + j + 1);		// ��i�E
				indices.push_back(top_offset + (step + 1) * ring_vertex_count + j);		// ���i��

				// �e�ʂ̉E���O�p�`
				indices.push_back(top_offset + (step + 1) * ring_vertex_count + j);		// ���i��
				indices.push_back(top_offset + step * ring_vertex_count + j + 1);		// ��i�E
				indices.push_back(top_offset + (step + 1) * ring_vertex_count + j + 1);	// ���i�E
			}
		}

		// ��ԉ��̒��_���Ō�ɒǉ�

		u_int south_pole_index = (u_int)vertices.size() - 1;

		// �Ō�̉~�̍ŏ��̒��_�̃C���f�b�N�X�����炷
		// ����͈�ԉ��̒��_���X�L�b�v���Ă��邾��

		// ��̉~�̒��_����
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
