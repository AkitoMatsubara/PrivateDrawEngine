//// http://maverickproj.web.fc2.com/d3d11_17.html

//// SV_GroupThreadID    : x      , y       , z      //�X���b�hID�@��������numthread�ԍ�
//// SV_GroupID          : x'     , y'      , z'     //�O���[�vID�@��������Dispatch�ԍ�
//// SV_DispatchThreadID : x' * X + x , y' * Y+y , z' * Y+ z   // Dispatch*numthread+GTid
//// SV_GroupIndex       : z * X * Y + y * X + x

#include "GPUParticle.hlsli"



static float3 Acceleration = float3(0.0, -4.8, 0.0); // �����x(m/s^2)

// ���̓o�b�t�@(�\�����o�b�t�@�B�ǂݍ��ݐ�p)
StructuredBuffer<VS_INPUT_GPU> InBuffer : register(t0);
// �o�̓o�b�t�@(�\�����o�b�t�@�B�ǂݏ����\)
RWStructuredBuffer<VS_INPUT_GPU> Result : register(u0);


// float�̒l���󂯎���ė������� -range~range�̊Ԃ̐��𐶐�
float rand(float theta, float range)
{
	return (frac(sin(theta) * 100000.0) * 2.0 - 1.0) * range;
}
// float2�̒l���󂯎���ė�������  -range~range�̊Ԃ̐��𐶐�
float rand(float2 theta, float range)
{
	return (frac(sin(dot(theta.xy, float2(12.9898, 78.233))) * 10000.0) * 2.0 - 1.0) * range;
}
// float��2�l���󂯎���ė�������
float3 rand3(float2 seed, float range)
{
	return 2.0 * (float3(rand(seed * 1, range), rand(seed * 2, range), rand(seed * 3, range)) - 0.5);
}

// �p�[�e�B�N���Đݒ�
VS_INPUT_GPU Reset(uint3 dTid, float seed1)
{
    uint node = dTid.x;

    float2 seed = float2(dTid.x + 1, dTid.y + 2) * InBuffer[node].Position.xz; // �����pseed
    float3 pos = rand3(seed, 10.0); // ��������o�����ʒu
    pos.y = rand(seed * pos.xy, 5.0); // y������߂ɒ���
    
    float3 vel = rand3(seed, 50.0) * 0.0005;
    vel.y = rand(seed, 5.0) * 0.0005; // �����������x��y������߂ɒ���
    
    float3 force = rand3(vel.xz, 50.0) * 0.0000001;

    VS_INPUT_GPU p = InBuffer[node];
    p.Position.xyz = pos;
    p.Velocity = vel;
    p.Force = force;
    //p.Color = float4(InBuffer[node].Color.rgb,0.0f);
    p.Color = float4(rand3(pos.rg,1.0), 0.0f);  // �F�����_��
	p.Life = uint(rand(pos.z, 5.0f)); // unsigned
    p.Active = true;
    return p;
}

//�X���b�h�̐�
#define TH_X 500
#define TH_Y 1
#define TH_Z 1
// �V�F�[�_�֐�
[numthreads(TH_X, TH_Y, TH_Z)]
void main(uint3 Gid : SV_GroupID, uint3 GTid : SV_GroupThreadID, uint3 DTid : SV_DispatchThreadID)
{
	int THG_X = ParticleNo; // dispatch��

	uint x = GTid.x;
	uint y = Gid.x;

	//uint node = TH_X * Gid.x + GTid.x;
	uint node = DTid.x;

		// �������̂ݏ����X�V����
    if (InBuffer[node].Active)
    {
		// ���₷���悤��(�v�Z���邽�߂�)��x�ϐ���
        float4 pos = InBuffer[node].Position;
        float3 vel = InBuffer[node].Velocity;
        float3 force = InBuffer[node].Force;
        float life = InBuffer[node].Life;
		float alpha = sin((life / pos.w) * 3.15f) * 1.5f; // ���݃��C�t���ő僉�C�t�ŏ��Z�API��������Ɣ����̑傫���̃T�C���g���ł���@�����Ɋ|���Z����ƒl�̑��������܂�
		life -= 0.01f;
        pos.xyz += vel; // �ʒu�X�V
        vel += force; // ���x�X�V
		
        // CPU�ɕԂ�
		Result[node].Position = pos;
        Result[node].Velocity = vel;
		Result[node].Force = force;
        Result[node].Color = InBuffer[node].Color;
		Result[node].Color.w = alpha;
        Result[node].Life = life;
		  // ���C�t�v�Z�A���񂾂�񐶑���
		Result[node].Active = (Result[node].Life <= 0.0f) ? false : true;
    }
    // �񐶑��Ȃ�Đݒ���s���Đ���������
    else
    {
		Result[node] = Reset(DTid, InBuffer[node].Position.x);
	}
}
