#include "static_mesh.hlsli"

VS_OUT main(float4 position : POSITION, float4 normal : NORMAL, float2 texcoord : TEXCOORD)	// cpp���̃Z�}���e�B�N�X�Ɠ����ɂ��邱�Ƃł��̒l���g�p�ł���
{
    VS_OUT vout;    // �o�͗p���̐錾
    vout.position = mul(position, mul(world, view_projection)); // mul = �|���Z world * �l = ���[�J�����烏�[���h�ɂȂ�

    vout.world_position = mul(position, world);
    normal.w = 0;
    vout.world_normal = normalize(mul(normal, world));

    vout.color = material_color;

    //float4 Normal = normalize(mul(normal, world)); // �l * world = ���[���h���烍�[�J���ɂȂ�
    //float4 Light = normalize(-light_direction);
    //vout.color.rgb = material_color.rgb * max(0, dot(Light, Normal));
    //vout.color.a = material_color.a;

    vout.texcoord = texcoord;
    return vout;
}