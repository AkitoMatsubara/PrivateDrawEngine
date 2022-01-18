#pragma once
#include <d3d11.h>
#include <SimpleMath.h>

#include <wrl.h>

#include "shaderEx.h"
#include "Object3d.h"
#include "Rasterizer.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"


class Geometric_Primitive {

public:
	std::unique_ptr<Object3d> Parameters;
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;

	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;

	static std::unique_ptr<ShaderEx> GeometricShader;
	DirectX::SimpleMath::Matrix world;
	Rasterizer rasterizer;

	//std::unique_ptr<ShaderEx> GeometricShader;

protected:
	struct Vertex {
		DirectX::SimpleMath::Vector3 position;
		DirectX::SimpleMath::Vector3 normal;
	};
	struct Constants {
		DirectX::SimpleMath::Matrix world;
		DirectX::SimpleMath::Vector4 material_color;
	};

public:
	Geometric_Primitive(const WCHAR* vs_name = L"Shaders\\geometric_primitive_vs", const WCHAR* ps_name = L"Shaders\\geometric_primitive_ps");
	virtual ~Geometric_Primitive() = default;

	void Render(bool wireframe = false);

	// Parametersを編集するimguiウィンドウ
	void imguiWindow(const char* beginname = "geometric_primitive");

	// セッター
	void setPos    (DirectX::SimpleMath::Vector3 pos)   { Parameters->Position = pos; }
	void setSize   (DirectX::SimpleMath::Vector3 Size)  { Parameters->Scale = Size; }
	void setAngle  (DirectX::SimpleMath::Quaternion orientation) { Parameters->Orientation = orientation; }
	void setColor  (DirectX::SimpleMath::Vector4 color) { Parameters->Color = color; }

	void setPos   (float posX, float posY,float posZ)       { Parameters->Position   = DirectX::SimpleMath::Vector3(posX, posY,posZ); }
	void setSize  (float sizeX, float sizeY,float sizeZ)    { Parameters->Scale = DirectX::SimpleMath::Vector3(sizeX, sizeY,sizeZ); }
	//void setAngle (float angleX, float angleY,float angleZ) { Parameters->Rotate = DirectX::SimpleMath::Vector3(angleX, angleY,angleZ); }
	void setAngle (float angleX, float angleY,float angleZ) { Parameters->Orientation = DirectX::SimpleMath::Quaternion(angleX, angleY,angleZ,Parameters->Orientation.w); }
	void setColor (float r, float g, float b, float a)      { Parameters->Color = DirectX::SimpleMath::Vector4(r, g, b, a); }

	// ゲッター
	DirectX::SimpleMath::Vector3 getPos() { return Parameters->Position; }
	DirectX::SimpleMath::Vector3 getSize() { return Parameters->Scale; }
	//DirectX::SimpleMath::Vector3 getAngle() { return Parameters->Rotate; }
	DirectX::SimpleMath::Quaternion getAngle() { return Parameters->Orientation; }
	DirectX::SimpleMath::Vector4 getColor() { return Parameters->Color; }
	DirectX::SimpleMath::Matrix getWorld() { return world; }

protected:
	void Create_com_buffers(Vertex* vertices, size_t vertex_count, uint32_t* indices, size_t index_count);
};

// 箱
class Geometric_Cube :public Geometric_Primitive {
private:
	// 頂点情報用列挙型 値を入れるのでは読みにくかったので言葉で表せるように
	enum FACE {
		TOP_FACE,
		BOTTOM_FACE,
		FRONT_FACE,
		BACK_FACE,
		RIGHT_FACE,
		LEFT_FACE,
	};
	enum VERTEX_POSITION{
		LEFT_TOP,
		RIGHT_TOP,
		LEFT_BOTTOM,
		RIGHT_BOTTOM,
	};
	const float SIZE   = 1.0f;
	const float LEFT   = -SIZE * 0.5f;
	const float RIGHT  = +SIZE * 0.5f;
	const float BOTTOM = -SIZE * 0.5f;
	const float TOP    = +SIZE * 0.5f;
	const float FRONT  = -SIZE * 0.5f;
	const float BACK   = +SIZE * 0.5f;

public:
	// 立方体の生成
	Geometric_Cube();
	// バウンティボックス用 頂点位置を外部から指定するように
	Geometric_Cube(float left,float right,float bottom,float top,float front,float back);
};

// 円柱
class Geometric_Cylinder :public Geometric_Primitive {
public:
	//円柱の生成
	// slices : 何角形か
	Geometric_Cylinder(u_int slices = 20);
};

// 球
class Geometric_Sphere :public Geometric_Primitive {
public:
	//  球の生成
	// 半径
	FLOAT Radian;
	// 何角形か
	u_int slices;
	// 球の滑らかさ
	u_int stacks;

	Geometric_Sphere(u_int slices = 20, u_int stacks = 20);
};
// カプセル
class Geometric_Capsule :public Geometric_Primitive {
public:
	float TopPos;
	float BottomPos;
	// 半径
	FLOAT Radian;
	// 高さ
	FLOAT Height;

	// カプセルの生成
	// slices : 何角形か
	// stacks : 球の滑らかさ
	Geometric_Capsule(FLOAT height = 1.0f, u_int slices = 20, u_int stacks = 20);
};