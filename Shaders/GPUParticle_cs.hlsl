//#include "GPUParticle.hlsli"

//// ���͗p�̍\���̒�`
//struct BufInType
//{
//    float3 Pos;
//    float dummy;
//};

//// �o�͗p�̍\���̒�`
//struct BufOutType
//{
//    float3 Pos;
//    float dummy;
//    float3 Size;
//    float dummy2;
//};

//// �ǂݎ���p�̍\�����o�b�t�@�[
//// �A���I�[�_�[�h�A�N�Z�X�o�b�t�@�[�Ƃ��č쐬���ꂽ�o�b�t�@���g�p����
//StructuredBuffer<BufInType> BufferIn : register(t1);

//// �ǂݏ����\�ȍ\�����o�b�t�@�[
//// �A���I�[�_�[�h�A�N�Z�X�o�b�t�@�[�Ƃ��č쐬���ꂽ�o�b�t�@���g�p����
//RWStructuredBuffer<BufOutType> BufferOut : register(u1);

//[numthreads(1, 1, 1)] // ��A�s�A����
//void main(uint3 DTid : SV_DispatchThreadID) // SV_DispatchThreadID�FSV_GroupID * Numthreads �� groupthreadid �̍��v
//{
//    BufferOut[DTid.x].Size = Size;

//}

//// http://maverickproj.web.fc2.com/d3d11_17.html

//// SV_GroupThreadID    : x      , y       , z
//// SV_GroupID          : x'     , y'      , z'
//// SV_DispatchThreadID : x'* X + x , y'* Y+y , z'* Y+ z
//// SV_GroupIndex       : z * X * Y + y * X + x
///
///
#include "GPUParticle.hlsli"


//CPU�ւ̏�������
struct ReturnBuffer
{
    int toCpuNo; //��ԑ����ꏊ
    float3 toCpuPosition; // ���W�l
};


static float3 Acceleration = float3(0.0, -4.8, 0.0); // �����x(m/s^2)

// ���̓o�b�t�@(�\�����o�b�t�@�B�ǂݍ��ݐ�p)
StructuredBuffer<GS_INPUT_GPU2> Input : register(t0);
// �o�̓o�b�t�@(�\�����o�b�t�@�B�ǂݏ����\)
RWStructuredBuffer<GS_INPUT_GPU2> Result : register(u0);
RWStructuredBuffer<ReturnBuffer> Result2 : register(u1);


//�X���b�h�O���[�v�̐��iCPU���Ńf�B�X�p�b�`�������j
//#define THG_X 10

//�X���b�h�̐�
#define TH_X 200
#define TH_Y 1
#define TH_Z 1



// �V�F�[�_�֐�
[numthreads(TH_X, TH_Y, TH_Z)] // �X���b�h �O���[�v�̃X���b�h��
void main(uint3 Gid : SV_GroupID, //�O���[�vID�@�f�B�X�p�b�`���Ŏw��
//	uint3 DTid : SV_DispatchThreadID,
	uint3 GTid : SV_GroupThreadID //�X���b�hID�@�����Ŏw��
//	uint GI : SV_GroupIndex
)
{
	//��
    int THG_X = ParticleNo;

    uint x = GTid.x;
    uint y = Gid.x;
//	uint  CULMS_NO = TH_X;

    uint node = TH_X * y + x;

    float3 myLocation = Input[node].Position; //�����̈ʒu
//	const float length = 0.2f;//�߂��̓_�̔��f�p
    int count = 0;

    float nearest_Length = 9999999.0f;
    float3 nearest_Pos = float3(0, 0, 0);
    float3 Total_Pos = float3(0, 0, 0);
    float3 Total_Direction = float3(0, 0, 0);
    float3 answer;

    float MAX_ACCEL = 0.001f;
    float MAX_SPEED = 0.05f;

	//int count = 10000;// = TH_X * TH_X;
    for (int i = 0; i < THG_X * TH_X; i++)
    {
        float3 near_Location = Input[i].Position;

		//float2  v = near_Location - myLocation;
        float ans = distance(near_Location, myLocation);

//		if (ans < length) //�߂��_�𒊏o����B
		{
            if (ans != 0)
            {
                if (nearest_Length > ans)//��ԋ߂��_���L��
                {
                    nearest_Length = ans;
                    nearest_Pos = Input[i].Position;

                }
            }
            Total_Pos += Input[i].Position;
            Total_Direction += Input[i].Velocity;

            count++;
        }
    }
    if (count != 0)
    {
        float3 centerPos = Total_Pos / (float) count;
        answer = normalize(centerPos - myLocation) * 0.04f;
        answer += normalize(Total_Direction) * 0.02f;
        answer += normalize(nearest_Pos - myLocation) * -0.12f;

    }
    else
    {
        answer = float3(0, 0, 0);
    }

	//�ő�X�s�[�h = 3.0f;
	//�ő�����x = 0.1f;
    float3 F = Input[node].Force;
    F += answer;
    if (length(F) > MAX_ACCEL)
    {
        F = normalize(F);
        F *= MAX_ACCEL;
    }
    float3 V = Input[node].Velocity;
    V += F;
    if (length(V) > MAX_SPEED)
    {
        V = normalize(V);
        V *= MAX_SPEED;
    }
    float3 P = Input[node].Position;
    P += V;

    float val = 0.15f;

    if (P.x < -8)
        F.x = normalize(-myLocation).x * val;
    if (P.y < 2)
        F.y = normalize(-myLocation).y * val;
    if (P.z < -8)
        F.z = normalize(-myLocation).z * val;
    if (P.x > 8)
        F.x = normalize(-myLocation).x * val;
    if (P.y > 3)
        F.y = normalize(-myLocation).y * val;
    if (P.z > 8)
        F.z = normalize(-myLocation).z * val;
    Result[node].Force = F;
    Result[node].Velocity = V;
    Result[node].Position = P;

    if (node == 1)
    {
        Result2[0].toCpuNo = 123; //��ԑ����ꏊ
        Result2[0].toCpuPosition = Total_Pos / (float) count;
		//	Result2[0].toCpuPosition.x = 0;      // ���W�l
		//	Result2[0].toCpuPosition.y = 0;      // ���W�l
		//	Result2[0].toCpuPosition.z = 0;      // ���W�l
    }
}
