#pragma once

#include <d3d11.h>

// fps�Z�o�p�N���X
class FrameRateCalculator
{
	//-------------------------------------------------------------------
	// fps�Z�o�n
	int frameLimit;	// �ő�fps
	float MIN_FREAM_TIME;	// ��{�ςł͂Ȃ���Imgui���ŕύX�ł���悤�ɕϐ���`
	float frameTime;	// 1�t���[���̏����ɂ�����������
	LARGE_INTEGER timeStart;
	LARGE_INTEGER timeEnd;
	LARGE_INTEGER timeFreq;
	//-------------------------------------------------------------------

	void CalcFrameRate();	// �t���[�����[�g�Z�o
	void CalcFps();			// ���σt���[�����[�g�Z�o
	bool Stop();			// ������~

public:
	// fps���擾����Ȃ�0�ŏ��������Ȃ��ƃS�~���������ă}�C�i�X����n�܂����肷��(�����m��Ȃ�)
	float fps;
	// �t���[�����[�g�֘A �K���ɍ����������̂ł���ȂƂ����
	float AveFrameTime;

	void Init();
	void Update();

	int GetLimit() { return frameLimit; }
	void SetLimit(int limit) { frameLimit = limit; }


	static FrameRateCalculator& getInstance()
	{
		static FrameRateCalculator instance;
		return instance;
	}
};
