#include "Stage.h"

using namespace std;

Stage::Stage(const char* fbx_filename, int cstNo, const bool triangulate)
{
	Model = make_unique<Skinned_Mesh>(fbx_filename, cstNo, triangulate);
}

void Stage::Initialize()
{
	SkinnedShader = std::make_unique<ShaderEx>();
	SkinnedShader->Create(L"Shaders\\skinned_mesh_vs", L"Shaders\\skinned_mesh_ps");


	Model->setPos(0.0f, -5.0f, 0.0f);
	Model->setAngle(0.0f, 0.0f, 0.0f);
	Model->setSize(1.0f, 1.0f, 1.0f);
	Model->setColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void Stage::Update()
{

}

void Stage::Render()
{
	Model->Render(SkinnedShader.get());
}

void Stage::setStageFbx(const char* fbx_filename, int cstNo, const bool triangulate)
{
	if (Model)Model = nullptr;	// Ç¢Ç¡ÇΩÇÒè¡Ç∑
	Model = make_unique<Skinned_Mesh>(fbx_filename, cstNo, triangulate);	// çƒê∂ê¨
}
