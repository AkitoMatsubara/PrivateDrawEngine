#include "GPUParticle.hlsli"

//--------------------------------------------
//	�t�H�O
//--------------------------------------------
//color		:���݂̐F
//viewPos	:���_
//Pos		:�s�N�Z���̈ʒu
//fogColor	:�t�H�O�̐F
//Near		:�t�H�O�̋ߋ���
//Far		:�t�H�O�̉�����
float3 Fog(float3 color, float3 viewPos, float3 Pos, float3 fogColor, float Near, float Far)
{
    float Len = length(viewPos - Pos);
    float fogalpha = saturate((Len - Near) / (Far - Near));
    return color.rgb * (1.0 - fogalpha) + fogColor * fogalpha;
}

// �s�N�Z���E�V�F�[�_�̊֐�
float4 main(PS_INPUT Input) : SV_Target
{
    float4 color = Tex2D.Sample(smpWrap, Input.Tex);
    //color.xyz = Fog(color.xyz, EyePos.xyz, Input.wPosition, FogColor.xyz, FogNear, FogFar);

    return color;
//				float4 vec1 = normalize(Input.Pos);
//				return float4(vec1.x, vec1.y, vec1.z, 1);
}