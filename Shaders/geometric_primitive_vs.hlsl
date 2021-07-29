#include "geometric_primitive.hlsli"

VS_OUT main(float4 position : POSITION, float4 normal : NORMAL)	// cpp側のセマンティクスと同名にすることでその値を使用できる
{
    VS_OUT vout;
    vout.poosition = mul(position, mul(world, view_projection));
    normal.w = 0;
    float4 Normal = normalize(mul(normal, world));
    float4 Light = normalize(-light_direction);

    vout.color.rgb = material_color.rgb * max(0, dot(Light,Normal));
    vout.color.a = material_color.a;
    return vout;
}