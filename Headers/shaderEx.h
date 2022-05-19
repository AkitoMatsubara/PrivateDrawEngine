#pragma once
#include "shader.h"

#include <d3d11.h>
#include  <mutex>



// Shaderägí£ÉNÉâÉX
class ShaderEx :public Shader
{
private:
	HRESULT create_vs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader, ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements);
	HRESULT create_ps_from_cso(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader);
	HRESULT create_ds_from_cso(ID3D11Device* device, const char* cso_name, ID3D11DomainShader** domain_shader);
	HRESULT create_hs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11HullShader** hull_shader);
	HRESULT create_gs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11GeometryShader** geometry_shader);
	HRESULT create_cs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11ComputeShader** compute_shader);

	//std::mutex mutex;
	//std::mutex& GetMutex() { return mutex; }
public:
	ShaderEx() {}
	virtual ~ShaderEx() {}

	//bool CreateVS(const WCHAR* vsfilename, ID3D11InputLayout* ia = nullptr);
	bool CreateVS(const WCHAR* vsfilename, const UINT IL_NUM = 0 , D3D11_INPUT_ELEMENT_DESC ia_desc[] = nullptr);
	bool CreatePS(const WCHAR* psfilename);
	bool CreateDS(const WCHAR* dsfilename);
	bool CreateHS(const WCHAR* hsfilename);
	bool CreateGS(const WCHAR* gsfilename);
	bool CreateCS(const WCHAR* csfilename);

	ID3D11VertexShader*		GetVS() { return VS.Get(); }
	ID3D11PixelShader*		GetPS() { return PS.Get(); }
	ID3D11InputLayout*		GetIL() { return InputLayout.Get(); }
	ID3D11ComputeShader*	GetCS() { return CS.Get(); }
};