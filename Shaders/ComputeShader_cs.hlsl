//[numthreads(1, 1, 1)]
//void main( uint3 DTid : SV_DispatchThreadID )
//{
//}

// ���͗p�̍\���̒�`
struct BufInType
{
    int i;
    float f;
};

// �o�͗p�̍\���̒�`
struct BufOutType
{
    int i;
};

// �ǂݎ���p�̍\�����o�b�t�@�[
// �A���I�[�_�[�h�A�N�Z�X�o�b�t�@�[�Ƃ��č쐬���ꂽ�o�b�t�@���g�p����
StructuredBuffer<BufInType> BufferIn : register(t0);

// �ǂݏ����\�ȍ\�����o�b�t�@�[
// �A���I�[�_�[�h�A�N�Z�X�o�b�t�@�[�Ƃ��č쐬���ꂽ�o�b�t�@���g�p����
RWStructuredBuffer<BufOutType> BufferOut : register(u0);

[numthreads(2, 1, 1)]   // ��A�s�A����
void main(uint3 DTid : SV_DispatchThreadID) // SV_DispatchThreadID�FSV_GroupID * Numthreads �� groupthreadid �̍��v
{
    //BufferOut[DTid.x].i = 10;
    BufferOut[DTid.x].i = BufferIn[DTid.x].i + (int) BufferIn[DTid.x].f;
    //BufferOut[DTid.y].i = BufferIn[DTid.y].i + (int) BufferIn[DTid.y].f;
}

// http://maverickproj.web.fc2.com/d3d11_17.html

// SV_GroupThreadID    : x      , y       , z
// SV_GroupID          : x'     , y'      , z'
// SV_DispatchThreadID : x'* X + x , y'* Y+y , z'* Y+ z
// SV_GroupIndex       : z * X * Y + y * X + x