//�\����
#include "GPUParticle.hlsli"

// �W�I���g���E�V�F�[�_�̊֐�
[maxvertexcount(4)]
void main(point GS_INPUT In[1], inout TriangleStream<PS_INPUT> Stream)
{
    float4 pos = In[0].Pos;
    pos.w = 1.0f;
	// ���W�ϊ��@���[���h���W�n�@���@�r���[���W�n
    pos = mul(pos, View);

    float4 pPos = pos;

	// �_��ʂɂ���
    float4 posLT = pPos + float4(-ParticleSize.x,  ParticleSize.y, 0.0, 0.0) * pPos.w;
    float4 posLB = pPos + float4(-ParticleSize.x, -ParticleSize.y, 0.0, 0.0) * pPos.w;
    float4 posRT = pPos + float4( ParticleSize.x,  ParticleSize.y, 0.0, 0.0) * pPos.w;
    float4 posRB = pPos + float4( ParticleSize.x, -ParticleSize.y, 0.0, 0.0) * pPos.w;

	// ����̓_�̈ʒu(�ˉe���W�n)���v�Z���ďo��
    PS_INPUT Out;
    Out.Color = In[0].Color;
    Out.Life = In[0].Life;  // ���C�t�͎����ɑS����

    Out.Pos = mul(posLT, Projection);
    Out.Tex = float2(0.0, 0.0);
    float3 wPos = posLT.xyz;
    Out.wPosition = wPos;


    Stream.Append(Out);

	// �E��̓_�̈ʒu(�ˉe���W�n)���v�Z���ďo��

    Out.Pos = mul(posLB, Projection);
    Out.Tex = float2(1.0, 0.0);
    wPos = posLB.xyz;
    Out.wPosition = wPos;

    Stream.Append(Out);

	// �����̓_�̈ʒu(�ˉe���W�n)���v�Z���ďo��
    Out.Pos = mul(posRT, Projection);
    Out.Tex = float2(0.0, 1.0);
    wPos = posRT.xyz;
    Out.wPosition = wPos;
    Stream.Append(Out);

	// �E���̓_�̈ʒu(�ˉe���W�n)���v�Z���ďo��
    Out.Pos = mul(posRB, Projection);
    Out.Tex = float2(1.0, 1.0);
    wPos = posRB.xyz;
    Out.wPosition = wPos;
    Stream.Append(Out);
    Stream.RestartStrip();
}