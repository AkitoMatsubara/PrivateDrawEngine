#include "GPUParticle.hlsli"

PS_IN main(VS_IN vin) // �Z�}���e�B�N�X�ƈ�v�����Ēl���󂯎��
{
    PS_IN vout;
    vout.position = float4(vin.position, 1); // ����͏������s��Ȃ����߁A���̂܂܃s�N�Z���V�F�[�_�ɗ���
    vout.color = vin.color;
    vout.texcoord = vin.texcoord;
    {
        PS_IN vout;
        vout.position = mul(float4(vin.position, 0), mul(world, view_projection));
        vout.world_position = mul(vin.position, world); // ���[���h�ϊ�
        vout.world_normal = normalize(mul(vin.normal, world)); // ���[���h�ϊ�������𐳋K��
        vout.texcoord = vin.texcoord;
        vout.color = vin.color;

    }
    return vout;
}