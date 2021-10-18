#include "Stage.h"

using namespace std;

Stage::Stage()
{

}

void Stage::Initialize()
{
	Model = make_unique<Geometric_Cylinder>(3);
	GeomtricShader = std::make_unique<ShaderEx>();
	GeomtricShader->Create(L"Shaders\\skinned_mesh_vs", L"Shaders\\skinned_mesh_ps");


	Model->setPos(0.0f, 0.0f, 0.0f);
	Model->setAngle(0.0f, 0.0f, 0.0f);
	Model->setSize(1.0f, 1.0f, 1.0f);
	Model->setColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void Stage::Update()
{

}

void Stage::Render()
{
	Model->Render(GeomtricShader.get());
}