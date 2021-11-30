#include "Stages.h"

#include <algorithm>	// std::generate用

#include "misc.h"	// エラー出力用

StageParts::StageParts()
{
}

void StageParts::Initialize()
{
	SkinnedShader = std::make_unique<ShaderEx>();
	SkinnedShader->Create(L"Shaders\\skinned_mesh_vs", L"Shaders\\skinned_mesh_ps");

	Parameters = std::make_unique<Object3d>();

	Model = std::make_unique<Skinned_Mesh>(".\\Resources\\Stage\\Stage.fbx", 0, false);
}

void StageParts::Update()
{
	// ギミック組み込む
	// 弾を撃ったobjectを検知、そいつが立っているステージをライフ削る
	// ライフがなくなれば床が抜け落ちるって感じ

	// 描画するためモデルに情報を渡す
	Model->getParameters()->CopyParam(Parameters.get());
}

void StageParts::Render()
{
	Model->Render(SkinnedShader.get());
}

void StageParts::onObject(const DirectX::SimpleMath::Vector3& obj)
{
	// まずステージの三角形頂点を算出します
	// モデル変えたらやり直す必要あるからちょっとまずいかも
	DirectX::SimpleMath::Vector3 triangle[3];
	triangle[0] = (Model->getWorld().Backward() * 1.0f);	// 前方の点のつもり
	triangle[1] = (Model->getWorld().Forward() * 1.0f) + (Model->getWorld().Left() * 0.5f);	// 右方の点のつもり
	triangle[2] = (Model->getWorld().Forward() * 1.0f) + (Model->getWorld().Right() * 0.5f);	// 左方の点のつもり
	/*	こうなってるつもり		*/
	/*		     *[0]			*/
	/*		    / \				*/
	/*		   /   \			*/
	/*		  /     \			*/
	/*		 /       \			*/
	/*	  [2]*--------*[1]		*/



}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
StageManager::StageManager()
{
	if ((ROW_PARTS % 2 == 0) || (COL_PARTS % 2 == 0))	// もし奇数なら
	{
		_ASSERT_EXPR_A(false, "_StageParts is not an odd number._");	// ステージパーツが奇数では有りませんエラー
	}
}

void StageManager::Initialize()
{
	// ステージ生成(整列)
	//---------------------------------------------------------
	static const int CENTER = PARTS_SIZE / 2;	// 縦横が奇数のとき、/2+1で真ん中の数値が取れる…んだけど、0スタートのときは普通に半分の値

	// うまいことmake_uniqueが使えずに探してたところ、このような方法を見つけたので採用。1行で済むしスマートかと
	// 作成 begin~endまで{}を実行する
	std::generate(std::begin(Stages), std::end(Stages), []() { return std::make_unique<StageParts>(); });

	// 真ん中を0とするため、真ん中からの差分を開始地点、終点地点においている。「1,2,3」で言うと2を真ん中、1と3を始点終点にする。
	// 奇数(上記3)を偶数に一度変換するので-1している。
	for (int i = static_cast<int>(-((COL_PARTS - 1) * 0.5f)); i <= static_cast<int>(((COL_PARTS - 1) * 0.5f)); i++)
	{
		for (int j = static_cast<int>(-((ROW_PARTS - 1) * 0.5f)); j <= static_cast<int>(((ROW_PARTS - 1) * 0.5f)); j++)
		{
			int row = CENTER + j;		// 予め要素番号の計算を行っておく
			int col = COL_PARTS * i;	// キャストして警告を消すためこのまま入れちゃうと可読性が下がってしまうため
			if (row + col < 0) { _ASSERT_EXPR_A(false, "Initialize ArraySize UnderFlow"); }	// 要素番号がマイナス指定になっちゃったよエラー

			Stages[row + col]->Initialize();
			// 真ん中の要素(CENTER)に行(j)を加算し、列数(COL_PARTS)をi列分乗算して減算する
			Stages[row + col]->Parameters->Position = DirectX::SimpleMath::Vector3(j * 1.0f, -0.5f, -2.0f * i);

			bool inversion = false;	// 角度反転フラグ
			inversion = (j % 2) ? true : false;	// もしも、奇数行目なら角度反転フラグをtrueにする
			if (i % 2)inversion = !inversion;	// しかし、奇数列目なら角度反転フラグの「条件」を反転させる
			if (inversion)	// 180°回転させる
			{
				Stages[row + col]->Parameters->Rotate = DirectX::SimpleMath::Vector3(0.0f, 180.0f, 0.0);
				Stages[row + col]->Parameters->Color = DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f);
			}
			else
			{
				Stages[row + col]->Parameters->Rotate = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
				Stages[row + col]->Parameters->Color = DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f);
			}
			// 一応前方算出
			Stages[row + col]->Parameters->calcForward();
		}
	}
	// 真ん中をわかりやすく色変えてる
	Stages[CENTER]->Parameters->Color = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 0.0f, 1.0f);
	////---------------------------------------------------------
}

void StageManager::Update()
{
	for (auto& it : Stages)
	{
		it->Update();
	}
}

void StageManager::Render()
{
	for (auto& it : Stages)
	{
		it->Render();
	}
}

void StageManager::Check(const DirectX::SimpleMath::Vector3& obj)
{
	for (auto& it : Stages)
	{
		it->onObject(obj);
	}

}
