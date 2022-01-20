#include "FrameRateCalculator.h"

void FrameRateCalculator::Init()
{
	//[-------------------------------------------------------------------------------
	// メインループに入る前に精度を取得しておく
	QueryPerformanceFrequency(&timeFreq);	// CPUクロック数(精度)取得
	QueryPerformanceCounter(&timeStart);	// 処理開始前に1度取得しておく(初回計算用)

	//-------------------------------------------------------------------------------]
	frameLimit = 60;	// 最大fps設定
}

void FrameRateCalculator::Update()
{
	QueryPerformanceCounter(&timeEnd); // 処理終了後の現時間を取得

	CalcFrameRate();
	if (Stop())return;
	CalcFps();

}

void FrameRateCalculator::CalcFrameRate()
{
	// 1フレームにかけないといけない処理時間ms
	MIN_FREAM_TIME = 1.0f / static_cast<float>(frameLimit);

	// 経過時間(秒単位) = (今の時間 - 前フレームの時間) / 周波数
	frameTime = static_cast<float>(timeEnd.QuadPart - timeStart.QuadPart) / static_cast<float>(timeFreq.QuadPart);
}

bool FrameRateCalculator::Stop()
{
	if (frameTime < MIN_FREAM_TIME) { // 処理時間が想定時間より短かったら
		// Sleep時間を計算、ミリ秒から変換
		DWORD sleepTime = static_cast<DWORD>((MIN_FREAM_TIME - frameTime) * 1000.0f);
		//float sleepTime = ((MIN_FREAM_TIME - frameTime) * 1000.0f);
		if (static_cast<float>(sleepTime) > 0.0f)
		{
			timeBeginPeriod(1); // 分解能を1msに上げる(こうしないとSleepの精度はガタガタらしい)
			Sleep(sleepTime);   // 寝る
			timeEndPeriod(1);   // 起きたので1msを戻す
			return true;
		}
	}
	return false;
}

void FrameRateCalculator::CalcFps()
{
	// fpsの算出
	fps = 1.0f / frameTime;	// 本来のfps算出

	// 平均fpsの算出
	static float sumFrameTime = 0;	// fps合計変数
	static int count = 0;			// fps加算回数カウンタ
	sumFrameTime += fps;	// 加算していく
	count++;	// 加算回数のカウント
	if (count > frameLimit)	// 加算数がfps上限を超えたら除算して平均算出、合計と加算回数をリセット
	{
		AveFrameTime = sumFrameTime / static_cast<float>(count);
		sumFrameTime = 0;
		count = 0;
	}
	timeStart = timeEnd; // 次フレームの開始時間を現フレームの終了時間に
}