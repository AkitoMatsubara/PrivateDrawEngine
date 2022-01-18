#include "GPUParticle.hlsli"

PS_IN main(VS_IN vin) // セマンティクスと一致させて値を受け取る
{
    PS_IN vout;
    vout.position = float4(vin.position, 1); // 現状は処理を行わないため、そのままピクセルシェーダに流す
    vout.color = vin.color;
    vout.texcoord = vin.texcoord;
    {
        PS_IN vout;
        vout.position = mul(float4(vin.position, 0), mul(world, view_projection));
        vout.world_position = mul(vin.position, world); // ワールド変換
        vout.world_normal = normalize(mul(vin.normal, world)); // ワールド変換したやつを正規化
        vout.texcoord = vin.texcoord;
        vout.color = vin.color;

    }
    return vout;
}