#include "sprite.hlsli"

//#define JAPAN // ���̊ە`��

float4 main(VS_OUT pin) : SV_TARGET
{
#ifdef JAPAN
    const float2 center = float2(1280 / 2, 720 / 2);    // �E�B���h�E�T�C�Y(1280,720)�̒��S�_
    float distance = length(center - pin.positon.xy);   // ���S����xy�̋���
    if (distance > 200) // ���S����200�ȏ㗣��Ă�����
    {
        return 1;       // ���ŕ`�悷��
    }
    else
        return float4(1, 0, 0, 1);  // �Ԃŕ`�悷��
#else
    return pin.color; // ���_���̐F��n��
#endif
}