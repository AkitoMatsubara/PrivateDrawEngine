
Texture2D MappingTexture : register(t0);    // �[�x�e�N�X�`��

Texture2D ShadowTexture : register(t3);     // �}�b�s���O�A�ڂ�����̃e�N�X�`��

cbuffer ShadowConstant : register(b3)
{
    float4x4 LightViewProjection;   // ���C�g�����VP
    float4 LightPosition;           // ���C�g�̈ʒu
    float4 ShadowColor;             // �e�̐F
};

cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 view_projection; // �J������VP
    float4 light_direction;             // ���C�g�̌���
    float4 camera_position;             // �J�����̈ʒu
}

//���[���h���W�����C�g��ԍ��W�ɕϊ�����UV���W�Ɛ[�x�l���Z�o
float3 GetShadowTex(float4x4 lvp, float3 wPos)
{
	// ���K���f�o�C�X���W�n
    float4 wvpPos;
    wvpPos.xyz = wPos;
    wvpPos.w = 1.0f;
	// ���C�g���猩��NDC���W���Z�o
    wvpPos = mul(lvp, wvpPos);
	// NDC���W����UV���W���Z�o����
    wvpPos /= wvpPos.w;
	// �e�N�X�`�����W�n
    wvpPos.y = -wvpPos.y;
	wvpPos.xy = wvpPos.xy * 0.5f + 0.5f;
    return wvpPos.xyz;
}


//�V���h�[�}�b�v����V���h�[��ԍ��W�ɕϊ���Z�l��r
float3 GetShadow(Texture2D st, SamplerState ss, float3 sTex, float3 Scolor, float Bias)
{
#if 1   // �g�U�V���h�E�}�b�v
    // �J���e���̕s����(�`�F�r�V�F�t�̕s����)
    // P(x>=t)<=Pmax(t)=(sigma*sigma)/(siguma*siguma)+(t-E(x))*(t-E(x))
    // P(x>=t)��t��x�������m��    �`
    // Pmax(t)��t���ő�ɂȂ�m��   �a
    // �`�͂a�����������Ȃ邪�A�`�͋��߂�̂�����̂ŋ��߂₷���a�����߂đ���Ɏg�����Ƃ̂���
    // t=(sigma*sigma)/(siguma*siguma)+(t-E(x))*(t-E(x))���ȁH
    // Pmax(t)=�e�̔Z���Ƃ��Ă�����

    // �܂��͐[�x�l�̎��o��
    float2 depth = st.Sample(ss, sTex.xy).xy;

	// t = �`��s�N�Z���̌����ւ̋������[�x�l
    float t = sTex.z;
    // E(x) = �[�x�l���ڂ�������̐[�x�l
    float Ex = depth.x;
    // E(x^2) = �[�x�l�̓����ڂ�������̐[�x�l
    float E_x2 = depth.y;
	// E(x)^2 = �[�x�l���ڂ�������̐[�x�l�̓��
	float Ex2 = Ex * Ex;
	// sigma^2 = �[�x�l�̓����ڂ�������̐[�x�l - �[�x�l���ڂ�������̐[�x�l�̓�� ���ł�Variance(���U��)�Ƃ��������Ƃ��Ďg���Ă���
    float sigma = E_x2 - Ex2;
    // �m����̍ő�l�̎Z�o = �e�̔Z��
    float Pmax = sigma / sigma + (t - Ex) * (t - Ex);
    // �J���e���̕s�����̗L���`�F�b�N
    float b = t > Ex;
    Pmax = max(b, Pmax);
    return Pmax;

#else   // �[�x�V���h�E�}�b�v
	float d = st.Sample(ss, sTex.xy).x;
	// �V���h�E�}�b�v�̐[�x�l�ƃJ��������̐[�x�̔�r �V���h�E�A�N�l�Ƃ����������p�Ƀo�C�A�X���g�p���Ă���
    Scolor = (sTex.z - d > Bias) ? Scolor : float3(1, 1, 1);
    return Scolor;
#endif
}
