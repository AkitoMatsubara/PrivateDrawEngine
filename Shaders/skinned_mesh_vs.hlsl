#include "skinned_mesh.hlsli"
VS_OUT main(VS_IN vin)
{
    VS_OUT vout;
    vout.position = mul(vin.position, mul(world, view_projection));
    vout.world_position = mul(vin.position, world); // ���[���h�ϊ�
    vin.normal.w = 0;
    vout.world_normal = normalize(mul(vin.normal, world));  // ���[���h�ϊ�������𐳋K��
    vout.texcoord = vin.texcoord;
    vout.color = material_color;

    return vout;
}