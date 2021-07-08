
#pragma once

// UNIT.04
#include <d3d11.h>

HRESULT load_texture_from_file(ID3D11Device* device, const wchar_t* filename, ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc);

void rerease_all_textures();