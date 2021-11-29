#include "Stage.h"

#include "misc.h"	// エラー出力用

Stage::Stage(const char* fbx_filename, int cstNo, const bool triangulate)
{
	if ((ROW_PARTS % 2 == 0) || (COL_PARTS % 2 == 0))	// もし奇数なら
	{
		_ASSERT_EXPR_A(false, "StageParts is not an odd number.");	// ステージパーツが奇数では有りませんエラー
	}
	for (int i = 0; i < PARTS_SIZE; i++)
	{
		Model[i] = std::make_unique<Skinned_Mesh>(fbx_filename, cstNo, triangulate);
		Model[i]->setColor(0.0f, 1.0f, 0.0f, 1.0f);
	}
	Parameters = std::make_unique<Object3d>();
}

void Stage::Initialize()
{
	SkinnedShader = std::make_unique<ShaderEx>();
	SkinnedShader->Create(L"Shaders\\skinned_mesh_vs", L"Shaders\\skinned_mesh_ps");

	// ステージ生成(整列)のとこ
	//---------------------------------------------------------
	static const int CENTER = PARTS_SIZE / 2;	// 縦横が奇数のとき、/2+1で真ん中の数値が取れる…んだけど、0スタートのときは普通に半分の値

	// 真ん中を0とするため、真ん中からの差分を開始地点、終点地点においている。「1,2,3」で言うと2を真ん中、1と3を始点終点にする。
	// 奇数(上記3)を偶数に一度変換するので-1している。
	for (int i = static_cast<int>(-((COL_PARTS - 1) * 0.5f)); i <= static_cast<int>(((COL_PARTS - 1) * 0.5f)); i++)
	{
		for (int j = static_cast<int>(-((ROW_PARTS - 1) * 0.5f)); j <= static_cast<int>(((ROW_PARTS - 1) * 0.5f)); j++)
		{
			// 真ん中の要素(CENTER)に行(j)を加算し、列数(COL_PARTS)をi列分乗算して減算する
			Model[CENTER + j + (COL_PARTS * i)]->setPos(j * 1.0f, -1.0f, -2.0f * i);

			bool inversion = false;	// 角度反転フラグ
			inversion = (j % 2) ? true : false;	// もし奇数行目なら角度反転フラグをtrueにする
			if (i % 2)inversion = !inversion;	// が、奇数列目なら角度反転フラグの「条件」を反転させる
			if (inversion)	// 180°回転させる
			{
				Model[CENTER + j + (COL_PARTS * i)]->setAngle(0.0f, 180.0f, 0.0);
				Model[CENTER + j + (COL_PARTS * i)]->setColor(1.0f, 0.0f, 0.0f, 1.0f);
			}
			else
			{
				Model[CENTER + j + (COL_PARTS * i)]->setAngle(0.0f, 0.0f, 0.0f);
				Model[CENTER + j + (COL_PARTS * i)]->setColor(0.0f, 0.0f, 1.0f, 1.0f);
			}
		}
	}
	// 真ん中をわかりやすく色変えてる
	Model[CENTER]->setColor(1.0f, 1.0f, 0.0f, 1.0f);
	////---------------------------------------------------------
}

void Stage::Update()
{
	// ギミック組み込む
	// 弾を撃ったobjectを検知、そいつが立っているステージをライフ削る
	// ライフがなくなれば床が抜け落ちるって感じ
}

void Stage::Render()
{
	for (int i = 0; i < PARTS_SIZE; i++)
	{
		Model[i]->Render(SkinnedShader.get());
	}

	//Model->Render(SkinnedShader.get());
}

void Stage::setStageFbx(const char* fbx_filename, int cstNo, const bool triangulate)
{
	for (int i = 0; i < PARTS_SIZE; i++)
	{
		Model[i] = std::make_unique<Skinned_Mesh>(fbx_filename, cstNo, triangulate);
	}
}

int Stage::getStageObj()
{
	return 0;
}
