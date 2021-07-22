#include "static_mesh.hlsli"

VS_OUT main(float4 position : POSITION, float4 normal : NORMAL, float2 texcoord : TEXCOORD)	// cpp側のセマンティクスと同名にすることでその値を使用できる
{
    VS_OUT vout;    // 出力用実体宣言
    vout.position = mul(position, mul(world, view_projection)); // mul = 掛け算 world * 値 = ローカルからワールドになる

    vout.world_position = mul(position, world);
    normal.w = 0;
    vout.world_normal = normalize(mul(normal, world));

    vout.color = material_color;

    //float4 Normal = normalize(mul(normal, world)); // 値 * world = ワールドからローカルになる
    //float4 Light = normalize(-light_direction);
    //vout.color.rgb = material_color.rgb * max(0, dot(Light, Normal));
    //vout.color.a = material_color.a;

    vout.texcoord = texcoord;
    return vout;
}