#pragma once

#include <d3d11.h>

class Score
{
private:
	int KillCount;	// �|������
	float GameTime;	// ��������

private:
	void Initialize();
	void Update();
	void Render();

	void Kill() { KillCount++; }	// �G��|�����J�E���g
};
