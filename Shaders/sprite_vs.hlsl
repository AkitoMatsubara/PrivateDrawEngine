#include "sprite.hlsli"

PS_IN main(VS_IN vin) // �Z�}���e�B�N�X�ƈ�v�����Ēl���󂯎��
{
    PS_IN vout;
    vout.position = float4(vin.position, 1); // ����͏������s��Ȃ����߁A���̂܂܃s�N�Z���V�F�[�_�ɗ���
    vout.color    = vin.color;
    vout.texcoord = vin.texcoord;
    return vout;
}