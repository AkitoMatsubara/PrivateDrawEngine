#pragma once

#include <windows.h>

class high_resolution_timer
{
public:
	high_resolution_timer()
	{
		LONGLONG counts_per_sec;
		QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&counts_per_sec));
		seconds_per_count = 1.0 / static_cast<double>(counts_per_sec);

		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&this_time));
		base_time = this_time;
		last_time = this_time;
	}
	~high_resolution_timer() = default;
	high_resolution_timer(const high_resolution_timer&) = delete;
	high_resolution_timer& operator=(const high_resolution_timer&) = delete;
	high_resolution_timer(high_resolution_timer&&) noexcept = delete;
	high_resolution_timer& operator=(high_resolution_timer&&) noexcept = delete;

	// Reset()���Ă΂�Ă���̌o�ߎ��Ԃ̍��v��Ԃ��܂��B
	// ���v����~���Ă��鎞�ԁB
	float time_stamp() const  // in seconds
	{
		// ��~���Ă���ꍇ�́A��~���Ă���̌o�ߎ��Ԃ𐔂��Ȃ��B
		// ����ɁA�ȑO�ɂ��łɈꎞ��~���Ă����ꍇ�A���̋���
		// stop_time - base_time �ɂ͈ꎞ��~���Ԃ��܂܂�Ă���A����̓J�E���g�������Ȃ��B
		// ������C������ɂ́AmStopTime ����ꎞ��~���Ԃ����������΂悢�B
		//                     |<--paused_time-->|
		// ----*---------------*-----------------*------------*------------*------> time
		//  base_time       stop_time        start_time     stop_time    this_time

		if (stopped)
		{
			return static_cast<float>(((stop_time - paused_time) - base_time) * seconds_per_count);
		}

		// this_time - mBaseTime�̋����ɂ́A�J�E���g�������Ȃ��ꎞ��~���Ԃ��܂܂�Ă��܂��B
		// ������C������ɂ́Athis_time����ꎞ��~�������Ԃ����������΂悢�B
		// (this_time - paused_time) - base_time
		//
		//                     |<--paused_time-->|
		// ----*---------------*-----------------*------------*------> time
		//  base_time       stop_time        start_time     this_time
		else
		{
			return static_cast<float>(((this_time - paused_time) - base_time) * seconds_per_count);
		}
	}

	float time_interval() const  // in seconds
	{
		return static_cast<float>(delta_time);
	}

	void reset() // ���b�Z�[�W���[�v�̑O�ɌĂяo���B
	{
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&this_time));
		base_time = this_time;
		last_time = this_time;

		stop_time = 0;
		stopped = false;
	}

	void start() // Call when unpaused.
	{
		LONGLONG start_time;
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&start_time));

		// ��~�ƊJ�n�̃y�A�̊Ԃ̌o�ߎ��Ԃ�ώZ����B
		//
		//                     |<-------d------->|
		// ----*---------------*-----------------*------------> time
		//  base_time       stop_time        start_time
		if (stopped)
		{
			paused_time += (start_time - stop_time);
			last_time = start_time;
			stop_time = 0;
			stopped = false;
		}
	}

	void stop() // �ꎞ��~���ɌĂяo���B
	{
		if (!stopped)
		{
			QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&stop_time));
			stopped = true;
		}
	}

	void tick() // �t���[�����ɌĂяo���B
	{
		if (stopped)
		{
			delta_time = 0.0;
			return;
		}

		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&this_time));
		// ���̃t���[���ƑO�̃t���[���Ƃ̎��ԍ��B
		delta_time = (this_time - last_time) * seconds_per_count;

		// ���̃t���[������������D
		last_time = this_time;

		// �����I�ɔ񕉂ɂ���B DXSDK��CDXUTTimer�ł́A�ȉ��̏ꍇ�Ɍ��y����Ă��܂��B
		// �v���Z�b�T���p���[�Z�[�u���[�h�ɓ��邩�A���̃v���Z�b�T�ɃV���b�t�������B
		// mDeltaTime �͕��ɂȂ�\��������܂��B
		if (delta_time < 0.0)
		{
			delta_time = 0.0;
		}
	}

private:
	double seconds_per_count{ 0.0 };
	double delta_time{ 0.0 };

	LONGLONG base_time{ 0LL };
	LONGLONG paused_time{ 0LL };
	LONGLONG stop_time{ 0LL };
	LONGLONG last_time{ 0LL };
	LONGLONG this_time{ 0LL };

	bool stopped{ false };
};
