#include "GPUParticle.hlsli"

// ���͗p�̍\���̒�`
struct BufInType
{
    float3 Pos;
    float dummy;
};

// �o�͗p�̍\���̒�`
struct BufOutType
{
    float3 Pos;
    float dummy;
    float3 Size;
    float dummy2;
};

// �ǂݎ���p�̍\�����o�b�t�@�[
// �A���I�[�_�[�h�A�N�Z�X�o�b�t�@�[�Ƃ��č쐬���ꂽ�o�b�t�@���g�p����
StructuredBuffer<BufInType> BufferIn : register(t1);

// �ǂݏ����\�ȍ\�����o�b�t�@�[
// �A���I�[�_�[�h�A�N�Z�X�o�b�t�@�[�Ƃ��č쐬���ꂽ�o�b�t�@���g�p����
RWStructuredBuffer<BufOutType> BufferOut : register(u1);

[numthreads(1, 1, 1)] // ��A�s�A����
void main(uint3 DTid : SV_DispatchThreadID) // SV_DispatchThreadID�FSV_GroupID * Numthreads �� groupthreadid �̍��v
{
    BufferOut[DTid.x].Size = Size;

}

// http://maverickproj.web.fc2.com/d3d11_17.html

// SV_GroupThreadID    : x      , y       , z
// SV_GroupID          : x'     , y'      , z'
// SV_DispatchThreadID : x'* X + x , y'* Y+y , z'* Y+ z
// SV_GroupIndex       : z * X * Y + y * X + x