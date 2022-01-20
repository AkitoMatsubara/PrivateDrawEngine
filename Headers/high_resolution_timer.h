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

	// Reset()が呼ばれてからの経過時間の合計を返します。
	// 時計が停止している時間。
	float time_stamp() const  // in seconds
	{
		// 停止している場合は、停止してからの経過時間を数えない。
		// さらに、以前にすでに一時停止していた場合、その距離
		// stop_time - base_time には一時停止時間が含まれており、これはカウントしたくない。
		// これを修正するには、mStopTime から一時停止時間を差し引けばよい。
		//                     |<--paused_time-->|
		// ----*---------------*-----------------*------------*------------*------> time
		//  base_time       stop_time        start_time     stop_time    this_time

		if (stopped)
		{
			return static_cast<float>(((stop_time - paused_time) - base_time) * seconds_per_count);
		}

		// this_time - mBaseTimeの距離には、カウントしたくない一時停止時間が含まれています。
		// これを修正するには、this_timeから一時停止した時間を差し引けばよい。
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

	void reset() // メッセージループの前に呼び出す。
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

		// 停止と開始のペアの間の経過時間を積算する。
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

	void stop() // 一時停止中に呼び出す。
	{
		if (!stopped)
		{
			QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&stop_time));
			stopped = true;
		}
	}

	void tick() // フレーム毎に呼び出す。
	{
		if (stopped)
		{
			delta_time = 0.0;
			return;
		}

		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&this_time));
		// このフレームと前のフレームとの時間差。
		delta_time = (this_time - last_time) * seconds_per_count;

		// 次のフレームを準備する．
		last_time = this_time;

		// 強制的に非負にする。 DXSDKのCDXUTTimerでは、以下の場合に言及されています。
		// プロセッサがパワーセーブモードに入るか、他のプロセッサにシャッフルされる。
		// mDeltaTime は負になる可能性があります。
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
