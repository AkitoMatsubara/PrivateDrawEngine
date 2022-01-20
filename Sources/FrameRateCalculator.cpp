#include "FrameRateCalculator.h"

void FrameRateCalculator::Init()
{
	//[-------------------------------------------------------------------------------
	// ���C�����[�v�ɓ���O�ɐ��x���擾���Ă���
	QueryPerformanceFrequency(&timeFreq);	// CPU�N���b�N��(���x)�擾
	QueryPerformanceCounter(&timeStart);	// �����J�n�O��1�x�擾���Ă���(����v�Z�p)

	//-------------------------------------------------------------------------------]
	frameLimit = 60;	// �ő�fps�ݒ�
}

void FrameRateCalculator::Update()
{
	QueryPerformanceCounter(&timeEnd); // �����I����̌����Ԃ��擾

	CalcFrameRate();
	if (Stop())return;
	CalcFps();

}

void FrameRateCalculator::CalcFrameRate()
{
	// 1�t���[���ɂ����Ȃ��Ƃ����Ȃ���������ms
	MIN_FREAM_TIME = 1.0f / static_cast<float>(frameLimit);

	// �o�ߎ���(�b�P��) = (���̎��� - �O�t���[���̎���) / ���g��
	frameTime = static_cast<float>(timeEnd.QuadPart - timeStart.QuadPart) / static_cast<float>(timeFreq.QuadPart);
}

bool FrameRateCalculator::Stop()
{
	if (frameTime < MIN_FREAM_TIME) { // �������Ԃ��z�莞�Ԃ��Z��������
		// Sleep���Ԃ��v�Z�A�~���b����ϊ�
		DWORD sleepTime = static_cast<DWORD>((MIN_FREAM_TIME - frameTime) * 1000.0f);
		//float sleepTime = ((MIN_FREAM_TIME - frameTime) * 1000.0f);
		if (static_cast<float>(sleepTime) > 0.0f)
		{
			timeBeginPeriod(1); // ����\��1ms�ɏグ��(�������Ȃ���Sleep�̐��x�̓K�^�K�^�炵��)
			Sleep(sleepTime);   // �Q��
			timeEndPeriod(1);   // �N�����̂�1ms��߂�
			return true;
		}
	}
	return false;
}

void FrameRateCalculator::CalcFps()
{
	// fps�̎Z�o
	fps = 1.0f / frameTime;	// �{����fps�Z�o

	// ����fps�̎Z�o
	static float sumFrameTime = 0;	// fps���v�ϐ�
	static int count = 0;			// fps���Z�񐔃J�E���^
	sumFrameTime += fps;	// ���Z���Ă���
	count++;	// ���Z�񐔂̃J�E���g
	if (count > frameLimit)	// ���Z����fps����𒴂����珜�Z���ĕ��ώZ�o�A���v�Ɖ��Z�񐔂����Z�b�g
	{
		AveFrameTime = sumFrameTime / static_cast<float>(count);
		sumFrameTime = 0;
		count = 0;
	}
	timeStart = timeEnd; // ���t���[���̊J�n���Ԃ����t���[���̏I�����Ԃ�
}