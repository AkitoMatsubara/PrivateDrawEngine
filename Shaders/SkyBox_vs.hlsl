#include "SkyBox.hlsli"

PS_IN main(VS_IN vin) // �Z�}���e�B�N�X�ƈ�v�����Ēl���󂯎��
{
    PS_IN output;
    output.NDCPosition = float4(vin.NDCPosition, 1.0f);
    output.color = vin.color;

    // NDC���W���烏�[���h���W�ɕϊ�
    float4 p = (float4) 0;

	//	NDC���W����N���b�v��ԍ��W�ւ̕ϊ�
    float4 viewSpacePosition = mul(output.NDCPosition, inverseProjection);
	//	�N���b�v��ԍ��W����J������ԍ��W�ւ̕ϊ�
    viewSpacePosition /= viewSpacePosition.w;
	//	�J������ԍ��W���烏�[���h��ԍ��W�ւ̕ϊ�
    p = mul(viewSpacePosition, inverseView);

    output.worldPosition = p.xyz;

    return output;
}