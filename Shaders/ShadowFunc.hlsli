
Texture2D MappingTexture : register(t0);    // 深度テクスチャ

Texture2D ShadowTexture : register(t3);     // マッピング、ぼかし後のテクスチャ

cbuffer ShadowConstant : register(b3)
{
    float4x4 LightViewProjection;   // ライトからのVP
    float4 LightPosition;           // ライトの位置
    float4 ShadowColor;             // 影の色
};

cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 view_projection; // カメラのVP
    float4 light_direction;             // ライトの向き
    float4 camera_position;             // カメラの位置
}

//ワールド座標をライト空間座標に変換してUV座標と深度値を算出
float3 GetShadowTex(float4x4 lvp, float3 wPos)
{
	// 正規化デバイス座標系
    float4 wvpPos;
    wvpPos.xyz = wPos;
    wvpPos.w = 1.0f;
	// ライトから見たNDC座標を算出
    wvpPos = mul(lvp, wvpPos);
	// NDC座標からUV座標を算出する
    wvpPos /= wvpPos.w;
	// テクスチャ座標系
    wvpPos.y = -wvpPos.y;
	wvpPos.xy = wvpPos.xy * 0.5f + 0.5f;
    return wvpPos.xyz;
}


//シャドーマップからシャドー空間座標に変換とZ値比較
float3 GetShadow(Texture2D st, SamplerState ss, float3 sTex, float3 Scolor, float Bias)
{
#if 1   // 拡散シャドウマップ
    // カンテリの不等式(チェビシェフの不等式)
    // P(x>=t)<=Pmax(t)=(sigma*sigma)/(siguma*siguma)+(t-E(x))*(t-E(x))
    // P(x>=t)はtがxを下回る確率    Ａ
    // Pmax(t)はtが最大になる確率   Ｂ
    // ＡはＢよりも小さくなるが、Ａは求めるのが難しいので求めやすいＢを求めて代わりに使おうとのこと
    // t=(sigma*sigma)/(siguma*siguma)+(t-E(x))*(t-E(x))かな？
    // Pmax(t)=影の濃さとしてあつかう

    // まずは深度値の取り出し
    float2 depth = st.Sample(ss, sTex.xy).xy;

	// t = 描画ピクセルの光源への距離≒深度値
    float t = sTex.z;
    // E(x) = 深度値をぼかした後の深度値
    float Ex = depth.x;
    // E(x^2) = 深度値の二乗をぼかした後の深度値
    float E_x2 = depth.y;
	// E(x)^2 = 深度値をぼかした後の深度値の二乗
	float Ex2 = Ex * Ex;
	// sigma^2 = 深度値の二乗をぼかした後の深度値 - 深度値をぼかした後の深度値の二乗 他ではVariance(分散率)という役割として使われている
    float sigma = E_x2 - Ex2;
    // 確率上の最大値の算出 = 影の濃さ
    float Pmax = sigma / sigma + (t - Ex) * (t - Ex);
    // カンテリの不等式の有効チェック
    float b = t > Ex;
    Pmax = max(b, Pmax);
    return Pmax;

#else   // 深度シャドウマップ
	float d = st.Sample(ss, sTex.xy).x;
	// シャドウマップの深度値とカメラからの深度の比較 シャドウアクネという問題解決用にバイアスを使用している
    Scolor = (sTex.z - d > Bias) ? Scolor : float3(1, 1, 1);
    return Scolor;
#endif
}
