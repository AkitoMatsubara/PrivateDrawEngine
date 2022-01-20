#pragma once

#include <d3d11.h>

// fps算出用クラス
class FrameRateCalculator
{
	//-------------------------------------------------------------------
	// fps算出系
	int frameLimit;	// 最大fps
	float MIN_FREAM_TIME;	// 基本可変ではないがImgui等で変更できるように変数定義
	float frameTime;	// 1フレームの処理にかかった時間
	LARGE_INTEGER timeStart;
	LARGE_INTEGER timeEnd;
	LARGE_INTEGER timeFreq;
	//-------------------------------------------------------------------

	void CalcFrameRate();	// フレームレート算出
	void CalcFps();			// 平均フレームレート算出
	bool Stop();			// 処理停止

public:
	// fpsを取得するなら0で初期化しないとゴミが混ざってマイナスから始まったりする(かも知れない)
	float fps;
	// フレームレート関連 適当に作っちゃったのでこんなところに
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
