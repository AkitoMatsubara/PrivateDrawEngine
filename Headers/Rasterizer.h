#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <assert.h>
using namespace Microsoft::WRL;

class Rasterizer {
public:

	enum RASTERIZER_STATE {
		SOLID_NONE,
		WIRE_NONE,
		SOLID_BACK,
		WIRE_BACK,
		SOLID_BACK_CCW,
		RS_NONE,	// ���X�^���C�U�񋓌^�I��
	};
	ComPtr<ID3D11RasterizerState>	states[RS_NONE];	// 0:�Жʓh��Ԃ�,1:���ʓh��ׂ�,2:�Жʃ��C���[�t���[��,3:���ʃ��C���[�t���[��

public:
	Rasterizer() {}
	Rasterizer(ID3D11Device* device) { SetRasterizer(device); }

	~Rasterizer() {}

	// RasterizerState�̍쐬
	void SetRasterizer(ID3D11Device* device);
};