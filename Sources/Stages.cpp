#include "Stages.h"

#include <algorithm>	// std::generate用

#include "misc.h"	// エラー出力用

StageParts::StageParts(){}

void StageParts::Initialize()
{
	SkinnedShader = std::make_unique<ShaderEx>();
	SkinnedShader->Create(L"Shaders\\skinned_mesh_vs", L"Shaders\\skinned_mesh_ps");

	Parameters = std::make_unique<Object3d>();
	Parameters->MaxLife = 5;
	Parameters->CurLife = Parameters->MaxLife;
	Parameters->Exist = true;
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
	if (Parameters->Exist) {
		Model->Render(SkinnedShader.get());
	}
}

void StageParts::onObject(const DirectX::SimpleMath::Vector3& objPos)
{
	const static float CHECK_DIST = 1.25f;
	DirectX::SimpleMath::Vector3 length;
	length = Parameters->Position - objPos;
	length.y = 0.0f;	// y軸は無視する
	if (length.Length() > CHECK_DIST)return;

	// まずステージの三角形頂点を算出します
	// モデル変えたらやり直す必要あるからちょっとまずいかも
	float ofs_x = 1.0f;
	float ofs_y = 1.0f;
	DirectX::SimpleMath::Vector3 triangle[3];
	triangle[0] = (Model->getWorld().Backward() * ofs_x);	// 前方の点のつもり
	triangle[1] = (Model->getWorld().Forward() * ofs_x) + (Model->getWorld().Left()  * ofs_y);	// 右方の点のつもり
	triangle[2] = (Model->getWorld().Forward() * ofs_x) + (Model->getWorld().Right() * ofs_y);	// 左方の点のつもり
	/*	こうなってるつもり		*/
	/*		    [0]			前	*/
	/*		     *			↑	*/
	/*		    / \			↑	*/
	/*		   /   \		↑	*/
	/*		  /  P  \		↑	*/
	/*		 /       \			*/
	/*	  [2]*--------*[1]		*/

	DirectX::SimpleMath::Vector3 p_0 = triangle[0] - objPos;
	DirectX::SimpleMath::Vector3 p_1 = triangle[1] - objPos;
	DirectX::SimpleMath::Vector3 p_2 = triangle[2] - objPos;
	p_0.y = p_1.y = p_2.y = 0.0f;	// Y軸は使用しないので0に

	float theta[3] = { 0 };
	// なす角θの計算 cosθ = (dot(a,b)/a.length * b.length)
	theta[0] = acos(p_0.Dot(p_1) / (p_0.Length() * p_1.Length()));
	theta[1] = acos(p_1.Dot(p_2) / (p_1.Length() * p_2.Length()));
	theta[2] = acos(p_0.Dot(p_2) / (p_0.Length() * p_2.Length()));
	// 確認用に3点のなす角を合計
	float sum = 0;
	for (auto& t : theta) { sum += t; }
	// 内包判定
	// なす角の合計値が361°未満であれば判定
	// なぜ未満なのかは誤差で360.001°みたいになっちゃうからその対策
	if (sum < DirectX::XMConvertToRadians(361))
	{		 // 内包する点であれば全て180°以下に収まるのでチェック
		if (theta[0] <= DirectX::XMConvertToRadians(180) && theta[1] <= DirectX::XMConvertToRadians(180) && theta[2] <= DirectX::XMConvertToRadians(180))
		{
			Parameters->CurLife--;	// 体力減らす
			Parameters->Color.w =static_cast<float>(Parameters->CurLife)/ static_cast<float>(Parameters->MaxLife);
		}
	}
	if (Parameters->CurLife <= 0)Parameters->Exist = false;
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
		if(it->Parameters->Exist)
		{
		it->Update();
		}
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
	//Stages[PARTS_SIZE / 2]->onObject(obj);
}
