#include "sprite.hlsli"

VS_OUT main(float4 position : POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD) // �Z�}���e�B�N�X�ƈ�v�����Ēl���󂯎��
{
    VS_OUT vout;
    vout.positon  = position;    // ����͏������s��Ȃ����߁A���̂܂܃s�N�Z���V�F�[�_�ɗ���
    vout.color    = color;
    vout.texcoord = texcoord;
    return vout;
}