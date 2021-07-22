#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>

#include "shader.h"
#include "misc.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include <vector>


using namespace DirectX;
using namespace Microsoft::WRL;

class Geometric_Primitive {
public:
	struct Vertex {
		XMFLOAT3 position;
		XMFLOAT3 normal;
	};
	struct Constants {
		XMFLOAT4X4 world;
		XMFLOAT4 material_color;
	};

	bool wireframe;	// ���C���[�t���[���\���̗L��

private:
	ComPtr<ID3D11Buffer> vertex_buffer;
	ComPtr<ID3D11Buffer> index_buffer;

	ComPtr<ID3D11VertexShader> vertex_shader;
	ComPtr<ID3D11PixelShader> pixel_shader;
	ComPtr<ID3D11InputLayout> input_layout;
	ComPtr<ID3D11Buffer> constant_buffer;
	ComPtr<ID3D11RasterizerState>	rasterizer_states[3];	// 0:�Жʓh��Ԃ�,1:�Жʃ��C���[�t���[��,2:���ʃ��C���[�t���[��


	struct primitivParam {
		XMFLOAT3 Pos;		// �`��ʒu
		XMFLOAT3 Size;		// �`��T�C�Y
		XMFLOAT3 Angle;		// ��]�p�x
		XMFLOAT4 Color;		// ���Z�F
	}param;

public:
	Geometric_Primitive(ID3D11Device* device, const char* vs_cso_name = "Shaders\\geometric_primitive_vs.cso", const char* ps_cso_name = "Shaders\\geometric_primitive_ps.cso");
	virtual ~Geometric_Primitive() = default;

	void Render(ID3D11DeviceContext* immediate_context, const XMFLOAT4X4& world, const XMFLOAT4& material_color, bool WireFrame);	// �O�����烏�[���h�s�����肽����
	void Render(ID3D11DeviceContext* immediate_context);							// �����̃��[���h�s����g�p=�ړ��Ȃǂ�����Ŋ��������Ă���

	// param��ҏW����imgui�E�B���h�E
	void imguiWindow(const char* beginname = "geometric_primitive");

	// �Z�b�^�[
	void setPos    (XMFLOAT3 pos)   { param.Pos = pos; }
	void setSize   (XMFLOAT3 Size)  { param.Size = Size; }
	void setAngle  (XMFLOAT3 angle) { param.Angle = angle; }
	void setColor  (XMFLOAT4 color) { param.Color = color; }

	void setPos   (float posX, float posY,float posZ)       { param.Pos   = XMFLOAT3(posX, posY,posZ); }
	void setSize  (float sizeX, float sizeY,float sizeZ)    { param.Size  = XMFLOAT3(sizeX, sizeY,sizeZ); }
	void setAngle (float angleX, float angleY,float angleZ) { param.Angle = XMFLOAT3(angleX, angleY,angleZ); }
	void setColor (float r, float g, float b, float a)      { param.Color = XMFLOAT4(r, g, b, a); }

	// �Q�b�^�[
	XMFLOAT3 getPos() { return param.Pos; }
	XMFLOAT3 getSize() { return param.Size; }
	XMFLOAT3 getAngle() { return param.Angle; }
	XMFLOAT4 getColor() { return param.Color; }

protected:
	void Create_com_buffers(ID3D11Device* device, Vertex* vertices, size_t vertex_count, uint32_t* indices, size_t index_count);

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
	Geometric_Cube(ID3D11Device* device, const char* vs_cso_name = "Shaders\\geometric_primitive_vs.cso", const char* ps_cso_name = "Shaders\\geometric_primitive_ps.cso");
	// �o�E���e�B�{�b�N�X�p ���_�ʒu���O������w�肷��悤��
	Geometric_Cube(ID3D11Device* device, float left,float right,float bottom,float top,float front,float back,
					const char* vs_cso_name = "Shaders\\geometric_primitive_vs.cso", const char* ps_cso_name = "Shaders\\geometric_primitive_ps.cso");
};

// �~��
class Geometric_Cylinder :public Geometric_Primitive {
public:
	//�~���̐���
	// slices : ���p�`��
	Geometric_Cylinder(ID3D11Device* device, u_int slices = 20, const char* vs_cso_name = "Shaders\\geometric_primitive_vs.cso", const char* ps_cso_name = "Shaders\\geometric_primitive_ps.cso");
};

// ��
class Geometric_Sphere :public Geometric_Primitive {
public:
	//  ���̐���
	//  slices : ���p�`��
	//  stacks : ���̊��炩��
	Geometric_Sphere(ID3D11Device* device, u_int slices = 20, u_int stacks = 20, const char* vs_cso_name = "Shaders\\geometric_primitive_vs.cso", const char* ps_cso_name = "Shaders\\geometric_primitive_ps.cso");
};
// �J�v�Z��
class Geometric_Capsule :public Geometric_Primitive {
public:
	//  �J�v�Z���̐���
	//  radian : ���a
	//  height : ����
	//  slices : ���p�`��
	//  stacks : ���̊��炩��
	Geometric_Capsule(ID3D11Device* device, FLOAT radian = 0.5f, FLOAT height = 1.0f, u_int slices = 20, u_int stacks = 20, const char* vs_cso_name = "Shaders\\geometric_primitive_vs.cso", const char* ps_cso_name = "Shaders\\geometric_primitive_ps.cso");
};