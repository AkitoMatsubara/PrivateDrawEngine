#pragma once

#include <d3d11.h>

class Score
{
private:
	int KillCount;	// “|‚µ‚½”
	float GameTime;	// §ŒÀŠÔ

private:
	void Initialize();
	void Update();
	void Render();

	void Kill() { KillCount++; }	// “G‚ğ“|‚µ‚½ƒJƒEƒ“ƒg
};
