#include "Stages.h"

#include <algorithm>	// std::generate用

#include "misc.h"	// エラー出力用
#include "framework.h"


void StageParts::Initialize()
{
	Parameters = std::make_unique<Object3d>();
	Parameters->MaxLife = 5;
	Parameters->CurLife = Parameters->MaxLife;

	Model = std::make_unique<Skinned_Mesh>(".\\Resources\\Stage\\Stage.fbx", 0, false);
	if (!InstanceModel) {	// 生成されていない場合に生成する
		InstanceModel = std::make_unique<InstanceSkinnedMesh>(".\\Resources\\Stage\\Stage.fbx", draw_amount);
	}
}

void StageParts::Update(bool inst)
{
	// 描画するためモデルに情報を渡す
	if (!inst) {
		Model->getParameters()->CopyParam(Parameters.get());	// いうて透明度しか変わらん
	}
	//InstanceModel->getParameters()->CopyParam(Parameters.get());
}

void StageParts::Render(ID3D11DeviceContext* device_context, UINT drawCount, bool inst, Shader* shader)
{
	if (Parameters->Exist) {
		(!inst) ? Model->Render(device_context,shader) : InstanceModel->Render(device_context,drawCount);	// 引数instによってモデル単体描画かインスタンス描画か分ける
	}
}


//bool StageParts::onObject(const Object3d& obj)
//{
//	// 計算負荷軽減のため(になるか分からないけど)判定距離を制限
//	static constexpr float rideDist = 1.5f;	// 判定距離
//	DirectX::SimpleMath::Vector3 length = Parameters->Position - obj.Position;
//	if (length.Length() > rideDist || !this->Parameters->Exist) return false;	// 距離以上離れているかそも存在しなかったらやめ
//
//	DirectX::SimpleMath::Vector3 objHead = obj.Position + (obj.Vector * 0.7f);	// プレイヤーから前方に少しずらした位置。頭部分。 撃ち出し部分からも判定を取るため
//
//	// まずステージの三角形頂点を算出します
//	// 今(12/09)現在のモデルに合わせてるので、変えたらやり直す必要あるからちょっとまずいかも
//	static constexpr float ofs_x = 1.0f;	// 原点から左右へのズレ
//	static constexpr float ofs_z = 1.0f;	// 原点から上へのズレ
//	DirectX::SimpleMath::Vector3 triangle[3];
//	triangle[0] = Parameters->Position + (Model->getWorld().Backward() * ofs_x);	// 前方の点のつもり
//	triangle[1] = Parameters->Position + (Model->getWorld().Forward()  * ofs_x) + (Model->getWorld().Left()  * ofs_z);	// 右方の点のつもり
//	triangle[2] = Parameters->Position + (Model->getWorld().Forward()  * ofs_x) + (Model->getWorld().Right() * ofs_z);	// 左方の点のつもり
//	/*	  こうなってるつもり	*/
//	/*		   [0(A)]		前	*/
//	/*		     *			↑	*/
//	/*		    / \			↑	*/
//	/*		   /   \		↑	*/
//	/*		  /  P  \		↑	*/
//	/*		 /       \			*/
//	/*[2(C)]*---------*[1(B)]	*/
//
//	// y無視のxz平面で外積を計算 ここだけしか使わんのでラムダ式で定義
//	constexpr auto cross = [](const DirectX::SimpleMath::Vector3& v1, const DirectX::SimpleMath::Vector3& v2) {return (v1.x * v2.z) - (v1.z * v2.x); };
//	DirectX::SimpleMath::Vector3 A_p, B_p, C_p, A_B, B_C, C_A;	// それぞれのベクトルを格納する変数
//	{
//		A_p = objHead - triangle[0];		// Aからobj前方に向かうベクトル
//		B_p = objHead - triangle[1];		// Bからobj前方に向かうベクトル
//		C_p = objHead - triangle[2];		// Cからobj前方に向かうベクトル
//		A_B = triangle[1] - triangle[0];	// AからBに向かうベクトル
//		B_C = triangle[2] - triangle[1];	// BからCに向かうベクトル
//		C_A = triangle[0] - triangle[2];	// CからAに向かうベクトル
//		A_p.y = B_p.y = C_p.y = A_B.y = B_C.y = C_A.y = 0.0f;	// Y軸は使用しないので0にして消す
//		float c1 = cross(A_B, B_p);
//		float c2 = cross(B_C, C_p);
//		float c3 = cross(C_A, A_p);
//		// ３つの外積結果が符号一致していれば内側、不一致なら外側
//		if ((c1 > 0 && c2 > 0 && c3 > 0) || (c1 < 0 && c2 < 0 && c3 < 0)) {
//			return true;	// 内側でーす
//		}
//	}
//	// 違っても本体が上に乗ってるか再計算する
//	{
//		A_p = obj.Position - triangle[0];	// Aからobjに向かうベクトル
//		B_p = obj.Position - triangle[1];	// Bからobjに向かうベクトル
//		C_p = obj.Position - triangle[2];	// Cからobjに向かうベクトル
//		A_p.y = B_p.y = C_p.y = A_B.y = B_C.y = C_A.y = 0.0f;	// Y軸は使用しないので0にして消す
//		float c1 = cross(A_B, B_p);
//		float c2 = cross(B_C, C_p);
//		float c3 = cross(C_A, A_p);
//		if ((c1 > 0 && c2 > 0 && c3 > 0) || (c1 < 0 && c2 < 0 && c3 < 0)) {
//			return true;	// 内側でーす
//		}
//	}
//	return false;
//}

bool StageParts::onObjectSphere(const Object3d& obj,const float& radian)
{
	// 計算負荷軽減のため(になるか分からないけど)判定距離を制限
	static constexpr float RIDE_DIST = 5.0f;	// 判定距離
	DirectX::SimpleMath::Vector3 length = Parameters->Position - obj.Position;
	if (length.Length() > RIDE_DIST || !this->Parameters->Exist) return false;	// 距離以上離れているかそも存在しなかったらやめ


	// まずステージの三角形頂点を算出します //
	// 今(05/04)現在のモデルに合わせてるので、変えたらやり直す必要あるからちょっとまずいかも
	static constexpr float OFS_X = 1.0f;	// 原点から左右へのズレ
	static constexpr float OFS_Z = 1.0f;	// 原点から上へのズレ
	DirectX::SimpleMath::Vector3 triangle[3];
	triangle[0] = Parameters->Position + (Model->getWorld().Backward() * OFS_X);	// 前方の点のつもり
	triangle[1] = Parameters->Position + (Model->getWorld().Forward() * OFS_X) + (Model->getWorld().Left()  * OFS_Z);	// 右方の点のつもり
	triangle[2] = Parameters->Position + (Model->getWorld().Forward() * OFS_X) + (Model->getWorld().Right() * OFS_Z);	// 左方の点のつもり
	/*	  こうなってるつもり	*/
	/*		   [0(A)]		前	*/
	/*		     *			↑	*/
	/*		    / \			↑	*/
	/*		   /   \		↑	*/
	/*		  /  P  \		↑	*/
	/*		 /       \			*/
	/*[2(C)]*---------*[1(B)]	*/

	// y無視のxz平面で外積を計算 ここだけしか使わんのでラムダ式で定義
	constexpr auto cross = [](const DirectX::SimpleMath::Vector3& v1, const DirectX::SimpleMath::Vector3& v2) {return (v1.x * v2.z) - (v1.z * v2.x); };

	const float rad = (radian) * 0.4f;	// 半径 スケール値によって実数値は変動するので計算
	DirectX::SimpleMath::Vector3 A_p, B_p, C_p, A_B, B_C, C_A;	// それぞれのベクトルを格納する変数

	A_B = triangle[1] - triangle[0];	// AからBに向かうベクトル
	B_C = triangle[2] - triangle[1];	// BからCに向かうベクトル
	C_A = triangle[0] - triangle[2];	// CからAに向かうベクトル

	A_p = obj.Position - triangle[0];	// Aからobjに向かうベクトル
	B_p = obj.Position - triangle[1];	// Bからobjに向かうベクトル
	C_p = obj.Position - triangle[2];	// Cからobjに向かうベクトル

	A_p.y = B_p.y = C_p.y = A_B.y = B_C.y = C_A.y = 0.0f;	// Y軸は使用しないので0にして消す
	DirectX::SimpleMath::Vector3 p[3];	// 線分と球の最短点
	DirectX::SimpleMath::Vector3 len[3];	// 最短点と球の距離

	DirectX::SimpleMath::Vector3 Normal = (triangle[1] - triangle[0]).Cross(triangle[2] - triangle[0]);	// 三角形の法線を算出
	Normal.Normalize();	// 正規化

	len[0] = p[0] - obj.Position;
	len[1] = p[1] - obj.Position;
	len[2] = p[2] - obj.Position;
	len[0].y = len[1].y = len[2].y = 0.0f;	// y軸除外
	float ProjectionLength = 0.0f;	// 射影長


	//--------------------- 交わり判定開始 一辺とでも当たっていれば乗れているということなのでtrueを返す--------------------- //

	A_B.Normalize();
	ProjectionLength = A_B.Dot(A_p);	// 射影ベクトルの長さ
	p[0] = triangle[0] + (A_B * ProjectionLength);	// 球と線分A_Bの最短点
	len[0] = p[0] - obj.Position;
	len[0].y = 0.0f;	// y軸除外


	if (len[0].Length() <= rad)	// 最短点との距離が半径より小さければ当たっている可能性があるので計算を続ける
	{
		if (A_B.Dot(A_p) * A_B.Dot(B_p) < 0) { return true; }	// A_pとB_pの角度が鋭角鈍角一致しなければ線分の範囲であるのであたり
		if (A_p.Length() < rad || B_p.Length() < rad) { return true; }	// 線分の末端が半径以下であれば末端が円内なので当たり
	}

	B_C.Normalize();
	ProjectionLength = B_C.Dot(B_p);	// 射影ベクトルの長さ
	p[1] = triangle[1] + (B_C * ProjectionLength);	// 球と線分B_Cの最短点
	len[1] = p[1] - obj.Position;
	len[1].y = 0.0f;	// y軸除外

	if (len[1].Length() <= rad)	// 最短点との距離が半径より小さければ当たっている可能性があるので計算を続ける
	{
		if (B_C.Dot(B_p) * B_C.Dot(C_p) < 0) { return true; }	// B_pとC_pの角度が鋭角鈍角一致しなければ線分の範囲であるのであたり
		if (B_p.Length() < rad || C_p.Length() < rad) { return true; }	// 線分の末端が半径以下であれば末端が円内なので当たり
	}

	C_A.Normalize();
	ProjectionLength = C_A.Dot(C_p);	// 射影ベクトルの長さ
	p[2] = triangle[2] + (C_A * ProjectionLength);	// 球と線分C_Aの最短点
	len[2] = p[2] - obj.Position;
	len[2].y = 0.0f;	// y軸除外

	if (len[2].Length() <= rad)	// 最短点との距離が半径より小さければ当たっている可能性があるので計算を続ける
	{
		if (C_A.Dot(C_p) * C_A.Dot(A_p) < 0) { return true; }	// C_pとA_pの角度が鋭角鈍角一致しなければ線分の範囲であるのであたり
		if (C_p.Length() < rad || A_p.Length() < rad) { return true; }	// 線分の末端が半径以下であれば末端が円内なので当たり
	}


	//--------------------- 交わり判定終了 最終チェック ---------------------//
	// 球が三角形の内側にあるかどうか確認
	DirectX::SimpleMath::Vector3 crs[3];	// 3辺と球の外積
	crs[0] = A_B.Cross(A_p);
	crs[1] = B_C.Cross(B_p);
	crs[2] = C_A.Cross(C_p);
	// 内積がすべてプラスであれば辺から右側にあるので頂点との距離が離れていても内側です
	float dot[3] = { crs[0].Dot(Normal),crs[1].Dot(Normal),crs[2].Dot(Normal) };
	return (dot[0] > 0 && dot[1] > 0 && dot[2] > 0) ? true : false;
}

void StageParts::Damage()
{
	Parameters->CurLife--;	// 体力を減らす
	Parameters->Color.w = static_cast<float>(Parameters->CurLife) / static_cast<float>(Parameters->MaxLife);

	// もし体力が0以下であれば存在フラグをへし折る。それ以外は存在してます
	Parameters->Exist = (Parameters->CurLife <= 0) ? false : true;
}




//---------------------------------------------------------------------------------------------------------------------------------------------------------
StageManager::StageManager()
{
	if ((ROW_PARTS % 2 == 0) || (COL_PARTS % 2 == 0))	// もし偶数で設定しちゃってたらなら
	{
		_ASSERT_EXPR_A(false, "_StageParts is not an odd number._");	// ステージパーツが奇数では有りませんエラー 一応ね
	}
}

void StageManager::Initialize()
{
	// ステージ生成(整列)
	//---------------------------------------------------------
	static const int CENTER = PARTS_SIZE / 2;	// 縦横が奇数のとき、/2+1で真ん中の数値が取れる…んだけど、0スタートのときは普通に半分の値

	// うまいことmake_uniqueが使えずに探してたところ、このような方法を見つけたので採用。1行で済むしスマートかと
	// 作成 begin~endまで{}を実行する コンストラクタの引数は描画数として使用
	std::generate(std::begin(Stages), std::end(Stages), []() { return std::make_unique<StageParts>(PARTS_SIZE); });

	// 真ん中を0とするため、真ん中からの差分を開始地点、終点地点においている。「1,2,3」で言うと2を真ん中、1と3を始点終点にする。
	// 奇数(上記3)を偶数に一度変換するので-1している。
	for (int i = static_cast<int>(-((COL_PARTS - 1) * 0.5f)); i <= static_cast<int>(((COL_PARTS - 1) * 0.5f)); i++)
	{
		for (int j = static_cast<int>(-((ROW_PARTS - 1) * 0.5f)); j <= static_cast<int>(((ROW_PARTS - 1) * 0.5f)); j++)
		{
			int row = CENTER + j;		// 予め要素番号の計算を行っておく
			int col = COL_PARTS * i;
			int node = row + col;	// 現在参照している要素番号
			if (node < 0) { _ASSERT_EXPR_A(false, "Initialize ArraySize UnderFlow"); }	// 要素番号がマイナス指定になっちゃったよエラー
			Stages.at(node)->Initialize();
			// 真ん中の要素(CENTER)に行(j)を加算し、列数(COL_PARTS)をi列分乗算して減算する
			Stages.at(node)->Parameters->Position = DirectX::SimpleMath::Vector3(j * 1.0f, -0.5f, i * -2.0f);

			bool inversion = false;	// 角度反転フラグ
			inversion = (j % 2) ? true : false;	// もしも、奇数行目なら角度反転フラグをtrueにする
			if (i % 2)inversion = !inversion;	// しかし、奇数列目なら角度反転フラグの「条件」を反転させる
			if (inversion)	// 180°回転させる
			{
				Stages.at(node)->Parameters->Orientation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(180.0f), 0.0f, 0.0);
				Stages.at(node)->Parameters->Color = DirectX::SimpleMath::Vector4(1.0f, 0.4f, 0.4f, 1.0f);
				//Stages.at(node)->Parameters->Color = DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f);
			}
			else
			{
				Stages.at(node)->Parameters->Orientation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0);
				Stages.at(node)->Parameters->Color = DirectX::SimpleMath::Vector4(0.4f, 0.4f, 1.0f, 1.0f);
				//Stages.at(node)->Parameters->Color = DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f);
			}

			// 真ん中をわかりやすく色変えてる
			if (node == CENTER) { Stages.at(CENTER)->Parameters->Color = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 0.0f, 1.0f); }
			Stages.at(node)->Model->getParameters()->CopyParam(Stages.at(node)->Parameters.get());
			// インスタンス用データをそろえる
			// データをストック
			Stages.at(0)->InstanceModel->getParameters()->CopyParam(Stages.at(node)->Parameters.get());
			Stages.at(0)->InstanceModel->addWorldData();
		}
	}
}

void StageManager::Update()
{
	if (inst) {
		for (auto& it : Stages)
		{
			if (it->Parameters->Exist)
			{
				it->Update(inst);
			}
		}
	}
}

void StageManager::Render(ID3D11DeviceContext* device_context, Shader* shader)
{
	if (GetAsyncKeyState('I') & 1) inst = !inst;	// インスタンス描画切り替え
	if (!inst) {
		for (auto& it : Stages)
		{
			it->Render(device_context, PARTS_SIZE, inst, shader);	// 大量描画パス
		}
	}
	else {
		Stages.at(0)->Render(device_context, PARTS_SIZE, inst);	// インスタンス描画 今回は０番目だけにドローコールを通す、インスタンス描画
	}
}

void StageManager::Check(const Object3d& obj, const float& radian)
{
	int index = 0;
	for (auto& it : Stages)
	{
		// オブジェクトとステージの位置判定で上に乗っていたらステージのダメージ処理を呼び出す
		if(it->onObjectSphere(obj,radian))
		{
			it->Damage();
			Stages.at(0)->InstanceModel->setColor(index, it->Parameters->Color);	// ダメージを受けるたびに透明度が変化するので設定している
		}
		index++;	// 何番目かのカウント用
	}
}

bool StageManager::RideParts(Object3d& obj,const float& radian)
{
	for (auto& it : Stages)
	{
		// オブジェクトとステージの位置判定で上に乗っていたらy値を固定する
		if (it->onObjectSphere(obj,radian))
		{
			return true;
		}
	}
	return false;
}
