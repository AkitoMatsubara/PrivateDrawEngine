#include "static_mesh.hlsli"

Texture2D color_map : register(t0);
Texture2D normal_map : register(t1);
SamplerState point_sampler_state : register(s0);
SamplerState linear_sampler_state : register(s1);
SamplerState anisotropic_sampler_state : register(s2);

// ほうせんまっぴんぐのps pdf.3のないようなのでそこまで来たらこれコメントアウトしようね
float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = color_map.Sample(anisotropic_sampler_state, pin.texcoord); // テクスチャの取得
    float alpha = color.a;
    float3 Normal = normalize(pin.world_normal.xyz); // ワールド座標系の法線を正規化

    float3 T = float3(1.0001, 0, 0);
    float3 B = normalize(cross(Normal, T));
    T = normalize(cross(B, Normal));
    float4 normal = normal_map.Sample(linear_sampler_state, pin.texcoord);
    normal = (normal * 2.0) - 1.0;
    normal.w = 0;
    Normal = normalize((normal.x * T) + (normal.y * B) + (normal.z * Normal));

    // 拡散反射
    float3 Light = normalize(-light_direction.xyz); // ライトの反対方向で正規化
    float3 diffuse = color.rgb * max(0, dot(Normal, Light)); //Diffuse = 拡散反射
    // 鏡面反射
    float3 V = normalize(camera_position.xyz - pin.world_position.xyz); // ワールド座標系位置からカメラへのベクトル
    float3 specular = pow(max(0, dot(Normal, normalize(V + Light))), 128); // Speqular = 反射  累乗する値が大きいほどメリハリができる

    return float4(diffuse + specular, alpha) * pin.color;
}