#pragma once

#include "geometric_primitive.h"
#include "shaderEx.h"

#include <memory>

class Stage
{
	// 変数
private:
	std::unique_ptr<Geometric_Cylinder> Model;	// そのままモデル
	// デフォルトのシェーダー
	std::unique_ptr<ShaderEx> GeomtricShader;

public:
	//関数
private:
	Stage();
	~Stage() {};

	void  Initialize();
	void Update();
	void Render();


};