#include "DepthShadow.hlsli"


//--------------------------------------------
//	�s�N�Z���V�F�[�_�[
//--------------------------------------------
float4 main(PSInputShadow input) : SV_Target
{
	// �[�x�l�Z�o
    float depth = input.Depth.z / input.Depth.w;	// �[�x�l
    float4 shadow = float4(depth, depth*depth, 0, 1);	// �[�x�l�A�[�x�l�̓��
    return shadow;
}


