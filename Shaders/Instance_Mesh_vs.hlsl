#include "Instance_Mesh.hlsli"

struct InBuffer {	// �C���X�^���X�`��p�p�����[�^
    row_major float4x4 world; // ���[���h�s�� �I�u�W�F�N�g�ɂ���Ĉʒu���]���Ⴄ�̂ŕ����K�v
    float4 color;	// ����͐F�����ꂼ��Ⴄ�̂Ŏ󂯎��
};

// ���̓o�b�t�@(�\�����o�b�t�@�B�ǂݍ��ݐ�p)
StructuredBuffer<InBuffer> InputBuffer : register(t0);

VS_OUT main(VS_IN vin)
{
	VS_OUT vout = (VS_OUT)0;
    float4 pos = vin.position;
	float4x4 World = InputBuffer[vin.instanceID].world;
    float4 color = InputBuffer[vin.instanceID].color;

	vout.position = mul(pos, mul(World, view_projection));
	vout.world_position = mul(pos, World); // ���[���h�ϊ�
	vin.normal.w = 0;
	vout.world_normal = normalize(mul(vin.normal, World)); // ���[���h�ϊ�������𐳋K��
	vout.texcoord = vin.texcoord;
	vout.color = color;

	float3 wPos = mul(pos, World).xyz; // ���[���h���W�Z�o

	// UV���W���Z�o����
	vout.vShadow = GetShadowTex(LightViewProjection, wPos);
	return vout;
}