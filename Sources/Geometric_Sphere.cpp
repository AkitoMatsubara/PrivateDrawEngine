#include "geometric_primitive.h"

Geometric_Sphere::Geometric_Sphere(ID3D11Device* device, u_int slices, u_int stacks, const char* vs_cso_name, const char* ps_cso_name) :Geometric_Primitive(device, vs_cso_name, ps_cso_name) {
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	float r = 0.5f;	// ���a
	float degree = 2.0f * 3.14159265358989f / slices;	// �*Pi/������=�O�p �����ł���Ε��s�l�ӌ`�ɂȂ�(???)

	//
	// ��ԏ�̒��_���珇�ɒ��_���v�Z���Ă����D
	//

	// �㉺���_�F�}�b�s���O���ɍŏ㉺���_�Ɋ��蓖�Ă�e�N�X�`���}�b�v��̈�ӂ̓_���Ȃ����߁A�e�N�X�`�����W�̘c�݂��������邱�Ƃɒ���
	// �����`�̃e�N�X�`�������̂Ƀ}�b�s���O����ہA�ŏ㉺���_�Ɋ��蓖�Ă�e�N�X�`���}�b�v��̈�ӂ̓_���Ȃ����߁A�e�N�X�`�����W�̘c�݂������邱�Ƃɒ���
	// �����`�̃e�N�X�`�������̂Ƀ}�b�s���O����ꍇ�A�ŏ㉺���_�Ɋ��蓖�Ă�e�N�X�`���}�b�v��̈�ӂ̓_���Ȃ����߁A�e�N�X�`�����W�̘c�݂������邱�Ƃɒ���

	Vertex top_vertex;		// ��̒��_(�����)
	top_vertex.position = DirectX::XMFLOAT3(0.0f, +r, 0.0f);
	top_vertex.normal = DirectX::XMFLOAT3(0.0f, +1.0f, 0.0f);

	Vertex bottom_vertex;	// ���̒��_(������)
	bottom_vertex.position = DirectX::XMFLOAT3(0.0f, -r, 0.0f);
	bottom_vertex.normal = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);

	vertices.push_back(top_vertex);	// ��ɒ��_��[0]�ɓo�^

	float phi_step = DirectX::XM_PI / stacks;			// ��=������H�̃X�e�b�v?
	float theta_step = 2.0f * DirectX::XM_PI / slices;	// �Ƃ̃X�e�b�v?

	// �e�X�^�b�N�~�̒��_���v�Z����
	for (u_int i = 1; i <= stacks - 1; ++i)
	{
		float phi = i * phi_step;

		// �~�̒��_.
		for (u_int j = 0; j <= slices; ++j)
		{
			float theta = j * theta_step;

			Vertex v;

			// ���ʂ���J���e�V�A��(�������W�n)��
			v.position.x = r * sinf(phi) * cosf(theta);
			v.position.y = r * cosf(phi);
			v.position.z = r * sinf(phi) * sinf(theta);

			DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&v.position);	// position��vector�^��
			DirectX::XMStoreFloat3(&v.normal, DirectX::XMVector3Normalize(p));	// �@���̌v�Z

			vertices.push_back(v);	// ���ԂɊi�[
		}
	}
	vertices.push_back(bottom_vertex);

	// ��̉~�̒��_����
	{
		for (UINT i = 1; i <= slices; ++i)
		{
			indices.push_back(0);
			indices.push_back(i + 1);
			indices.push_back(i);
		}
	}

	//
	// �����̃X�^�b�N�i�ŏ㉺���_�ɐڑ�����Ă��Ȃ��j�̃C���f�b�N�X���v�Z���܂��B
	//

	// �ŏ��̉~�̍ŏ��̒��_�̃C���f�b�N�X�����炷
	// ����͈�ԏ�̒��_���X�L�b�v���Ă��邾��
	u_int base_index = 1;
	u_int ring_vertex_count = slices + 1;
	for (u_int i = 0; i < stacks - 2; ++i)	// �ォ�牺�̏�
	{
		for (u_int j = 0; j < slices; ++j)	// �ォ��O�̏�
		{
			// �e�ʂ̍����O�p�`
			indices.push_back(base_index + i * ring_vertex_count + j);
			indices.push_back(base_index + i * ring_vertex_count + j + 1);
			indices.push_back(base_index + (i + 1) * ring_vertex_count + j);

			// �e�ʂ̉E���O�p�`
			indices.push_back(base_index + (i + 1) * ring_vertex_count + j);
			indices.push_back(base_index + i * ring_vertex_count + j + 1);
			indices.push_back(base_index + (i + 1) * ring_vertex_count + j + 1);
		}
	}

	//
	// ���̃X�^�b�N�̃C���f�b�N�X���v�Z���� ���̃X�^�b�N�͒��_�o�b�t�@�ɍŌ�ɏ������܂ꂽ����
	// �����Ĉ�ԉ��̒��_�Ɖ��̉~��ڑ�����
	//

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

	Create_com_buffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());
}