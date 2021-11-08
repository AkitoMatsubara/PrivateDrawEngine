#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>

#include "misc.h"
#include "shaderEx.h"
#include "Object3d.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include <vector>



class Geometric_Primitive {
public:
	std::unique_ptr<Object3d> Parameters;
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;

	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizer_states[3];	// 0:�Жʓh��Ԃ�,1:�Жʃ��C���[�t���[��,2:���ʃ��C���[�t���[��

	std::unique_ptr<ShaderEx> GeometricShader;

protected:
	struct Vertex {
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
	};
	struct Constants {
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4 material_color;
	};

public:
	Geometric_Primitive(const WCHAR* vs_name = L"Shaders\\geometric_primitive_vs", const WCHAR* ps_name = L"Shaders\\geometric_primitive_ps");
	virtual ~Geometric_Primitive() = default;

	void Render(bool wireframe = false);

	// Parameters��ҏW����imgui�E�B���h�E
	void imguiWindow(const char* beginname = "geometric_primitive");

	// �Z�b�^�[
	void setPos    (DirectX::XMFLOAT3 pos)   { Parameters->Position = pos; }
	void setSize   (DirectX::XMFLOAT3 Size)  { Parameters->Scale = Size; }
	void setAngle  (DirectX::XMFLOAT3 angle) { Parameters->Rotate = angle; }
	void setColor  (DirectX::XMFLOAT4 color) { Parameters->Color = color; }

	void setPos   (float posX, float posY,float posZ)       { Parameters->Position   = DirectX::XMFLOAT3(posX, posY,posZ); }
	void setSize  (float sizeX, float sizeY,float sizeZ)    { Parameters->Scale = DirectX::XMFLOAT3(sizeX, sizeY,sizeZ); }
	void setAngle (float angleX, float angleY,float angleZ) { Parameters->Rotate = DirectX::XMFLOAT3(angleX, angleY,angleZ); }
	void setColor (float r, float g, float b, float a)      { Parameters->Color = DirectX::XMFLOAT4(r, g, b, a); }

	// �Q�b�^�[
	DirectX::XMFLOAT3 getPos() { return Parameters->Position; }
	DirectX::XMFLOAT3 getSize() { return Parameters->Scale; }
	DirectX::XMFLOAT3 getAngle() { return Parameters->Rotate; }
	DirectX::XMFLOAT4 getColor() { return Parameters->Color; }

protected:
	void Create_com_buffers(Vertex* vertices, size_t vertex_count, uint32_t* indices, size_t index_count);
};

// ��
class Geometric_Cube :public Geometric_Primitive {
private:
	// ���_���p�񋓌^ �l������̂ł͓ǂ݂ɂ��������̂Ō��t�ŕ\����悤��
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
	// �����̂̐���
	Geometric_Cube();
	// �o�E���e�B�{�b�N�X�p ���_�ʒu���O������w�肷��悤��
	Geometric_Cube(float left,float right,float bottom,float top,float front,float back);
};

// �~��
class Geometric_Cylinder :public Geometric_Primitive {
public:
	//�~���̐���
	// slices : ���p�`��
	Geometric_Cylinder(u_int slices = 20);
};

// ��
class Geometric_Sphere :public Geometric_Primitive {
public:
	//  ���̐���
	// ���a
	FLOAT radian;
	// ���p�`��
	u_int slices;
	// ���̊��炩��
	u_int stacks;

	Geometric_Sphere(u_int slices = 20, u_int stacks = 20);
};
// �J�v�Z��
class Geometric_Capsule :public Geometric_Primitive {
public:
	float TopPos;
	float BottomPos;
	// ���a
	FLOAT Radian;
	// ����
	FLOAT Height;

	// �J�v�Z���̐���
	// slices : ���p�`��
	// stacks : ���̊��炩��
	Geometric_Capsule(FLOAT radian = 0.5f, FLOAT height = 1.0f, u_int slices = 20, u_int stacks = 20);
};