#include "GaussianBlur.hlsli"

PS_IN main(VS_IN vin)
{
    PS_IN vout;
    vout.position = float4(vin.position, 1); // ���̂܂܃s�N�Z���V�F�[�_�ɗ���
    vout.color = vin.color;
    vout.texcoord = vin.texcoord;
    return vout;
}