#include "GPUParticle.hlsli"

// 頂点シェーダの関数
GS_INPUT main(VS_INPUT_GPU In)
{
    GS_INPUT Out1;
    Out1.Pos.x = In.Position.x;
    Out1.Pos.y = In.Position.y;
    Out1.Pos.z = In.Position.z;
    Out1.Pos.w = 1;


    Out1.Tex.x = 0;
    Out1.Tex.y = 0;

    Out1.Color = In.Color;
    Out1.Life = In.Life;

    return Out1;

//	return In;
}