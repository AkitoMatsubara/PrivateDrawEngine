
#define BUFFER_SIZE 25

cbuffer CBPerFrame2 : register(b3)
{
    float4 Weight[BUFFER_SIZE];
    float KarnelSize;
    float2 Texcel;
    float dummy;
}

struct VS_IN
{
    float3 position : POSITION; // ÉçÅ[ÉJÉãà íu
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

struct PS_IN
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;

};