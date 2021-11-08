#include "Shot.h"
#include "XMFLOAT_Helper.h"

//std::unique_ptr<Skinned_Mesh> Shot::Model = std::make_unique<Skinned_Mesh>(".\\Resources\\Player\\Player.fbx");

void Shot::Initialize()
{
	//if(!Model)	Model = std::make_unique<Skinned_Mesh>(".\\Resources\\Player\\Player.fbx");
	test = std::make_unique<Geometric_Sphere>();
	SkinnedShader = std::make_unique<ShaderEx>();
	SkinnedShader->Create(L"Shaders\\static_mesh_vs", L"Shaders\\static_mesh_ps");

	Parameters = std::make_unique<Object3d>();

	Timer = 0;
	Exist = false;
}

void Shot::Update()
{
	// ‚Æ‚è‚ ‚¦‚¸‘O‚Éi‚Þ“®‚«‚ðì‚é
	static DirectX::XMFLOAT3 vel{ 0,0,0 };
	DirectX::XMFLOAT3 pos = Parameters->Position + Parameters->Vector * 0.1;
	Parameters->Position = (pos);

	// Ž©‘RÁ–ÅŽÀ‘•­Õ(ˆêu)
	if (Exist) {
		Timer += 0.1f;
	}
	if (Timer >= 50) {
		Timer = 0.0f;
		Exist = false;
	}

	// ƒ‚ƒfƒ‹‚É•`‰æŒnƒpƒ‰ƒ[ƒ^[‚ð“n‚·
	test->Parameters->CopyParam(Parameters.get());
	//test->Parameters->CopyParam(Model->getParameters());
	//Model->getParameters()->CopyParam(Parameters.get());
}

void Shot::Render()
{
	if (Exist) {
		//Model->Render(SkinnedShader.get());
		test->Render();
	}
}

void Shot::set(const Object3d* parent)
{
	Parameters->CopyParam(parent);
	Exist = true;
}

//std::unique_ptr<Shot> Shot::clone() const
//{
//	return std::make_unique<Shot>(*this);
//}