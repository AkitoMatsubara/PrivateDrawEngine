#include "RenderShadow.hlsli"
#include "ShadowFunc.hlsli"

//--------------------------------------------
//	���_�V�F�[�_�[
//--------------------------------------------
PSInput main(VSInput input)
{
	PSInput output = (PSInput)0;
    float4 P = input.Position; // float3��Position��float4�ɕϊ�
	row_major float4x4 matWVP = mul(world, view_projection);	// WVP�Z�o

    output.Position = mul(P, matWVP); // �J��������̈ʒu���Z�o

	float3 wPos = mul(P,world).xyz;	// ���[���h���W�Z�o
	output.wPosition = wPos;

	float3 wN = mul(world, input.Normal).xyz;	// ���[���h�@��
	wN = normalize(wN);	// ���K��
	output.wNormal = wN;

	output.Tex = input.Tex;
	output.Color = material_color;

	// UV���W���Z�o����
    output.vShadow = GetShadowTex(LightViewProjection, wPos);

	return output;
}

