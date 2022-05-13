#include "GPUParticle.hlsli"


[maxvertexcount(4)]
void main(point GS_INPUT In[1], inout TriangleStream<PS_INPUT> Stream)
{
    float4 pos = In[0].Pos;
    pos.w = 1.0f;   // 最大ライフを消す
	// 座標変換　ワールド座標系　→　ビュー座標系
    pos = mul(pos, View);

    float4 pPos = pos;

	// 点を面にする
    float4 posLT = pPos + float4(-ParticleSize.x,  ParticleSize.y, 0.0, 0.0) * pPos.w;
    float4 posLB = pPos + float4(-ParticleSize.x, -ParticleSize.y, 0.0, 0.0) * pPos.w;
    float4 posRT = pPos + float4( ParticleSize.x,  ParticleSize.y, 0.0, 0.0) * pPos.w;
    float4 posRB = pPos + float4( ParticleSize.x, -ParticleSize.y, 0.0, 0.0) * pPos.w;

    PS_INPUT Out;
    Out.Color = In[0].Color;
    Out.Life = In[0].Life;

	// 左上の点の位置を計算して出力
    Out.Pos = mul(posLT, Projection);
    Out.Tex = float2(0.0, 0.0);
    float3 wPos = posLT.xyz;
    Out.wPosition = wPos;
    Stream.Append(Out);

	// 右上の点の位置を計算して出力
    Out.Pos = mul(posLB, Projection);
    Out.Tex = float2(1.0, 0.0);
    wPos = posLB.xyz;
    Out.wPosition = wPos;
	Stream.Append(Out);

	// 左下の点の位置を計算して出力
    Out.Pos = mul(posRT, Projection);
    Out.Tex = float2(0.0, 1.0);
    wPos = posRT.xyz;
    Out.wPosition = wPos;
    Stream.Append(Out);

	// 右下の点の位置を計算して出力
    Out.Pos = mul(posRB, Projection);
    Out.Tex = float2(1.0, 1.0);
    wPos = posRB.xyz;
    Out.wPosition = wPos;
    Stream.Append(Out);

	Stream.RestartStrip();  // ストリームリセット
}