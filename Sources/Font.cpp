#include "Font.h"
#include "framework.h"

#include <Shlwapi.h>

// �Q�l https://blogoftrueone.wordpress.com/2013/06/02/directx11�ŕ����`��ƃt�H���g�ǂݍ���/



Font::~Font()
{
	for(auto& path:FontList)
	{
		// �V�X�e������t�H���g���폜����
		if (RemoveFontResourceEx(path, FR_PRIVATE, NULL) == 0)
		{
			_ASSERT_EXPR_A(false, "Delete Font Failed.");	// �t�H���g�폜���s
		}
	}
	FontList.clear();
}

void Font::LoadFont(const wchar_t* ttfFontPath, const wchar_t* FontName)
{
	// �V�X�e���Ƀt�H���g��ǉ�����
	if (AddFontResourceEx(ttfFontPath, FR_PRIVATE, NULL) == 0)
	{
		_ASSERT_EXPR_A(false, "Add Font Failed.");	// �t�H���g�ǉ����s
	}
	FontList.push_back(ttfFontPath);	// �ǉ������t�H���g�p�X��ۑ�

	// �t�H���g�̐���
	LONG FontSize = 64;
	LOGFONT lf = {};
	lf.lfHeight = FontSize;	// �����̍���
	lf.lfWidth = 0;			// ������
	lf.lfEscapement = 0;	// ����������X���Ƃ̊p�x
	lf.lfOrientation = 0;	// �e������X���Ƃ̊p�x
	lf.lfWeight = 0;		// ����
	lf.lfItalic = 0;		// �C�^���b�N��
	lf.lfUnderline = 0;		// ����
	lf.lfStrikeOut = 0;		// �ł�������
	lf.lfCharSet = SHIFTJIS_CHARSET;			// �L�����N�^�Z�b�g
	lf.lfOutPrecision = OUT_TT_ONLY_PRECIS;		// �o�͐��x
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;		// �N���b�s���O�̐��x
	lf.lfQuality = PROOF_QUALITY;			// �o�͕i��
	lf.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;	// �s�b�`�ƃt�@�~��
	lstrcpy(lf.lfFaceName, FontName);	// �t�H���g��

	hFont = CreateFontIndirect(&lf);	// �_���t�H���g�����g�̐���
	if ((!hFont)) { MessageBox(0, L"Font Creation Failure", NULL, MB_OK); }
}

void Font::CreateFontTexture(const WCHAR* FontChar)
{
	ID3D11Device* Dev = FRAMEWORK->GetDevice();
	ID3D11DeviceContext* DevCon = FRAMEWORK->GetDeviceContext();
	HRESULT hr;

	HDC hdc = GetDC(nullptr); // �f�o�C�X�R���e�L�X�g�n���h���̎擾
	HFONT oldFont = static_cast<HFONT>(SelectObject(hdc, hFont));
	UINT code;     // �e�N�X�`���ɏ������ޕ����̕����R�[�h

#if _UNICODE
	code = (UINT)*FontChar;
#else
	// �}���`�o�C�g���������m�F���ď���B
	// 2�o�C�g�����̃R�[�h��[�����R�[�h]*256 + [�擱�R�[�h]�炵��
	if (IsDBCSLeadByte(*fontChar)) { code = (BYTE)fontChar[0] << 8 | (BYTE)fontChar[1]; }
	else { code = fontChar[0]; }

#endif
	// �t�H���g�r�b�g�}�b�v�擾
	TEXTMETRIC TM;	// �g�p�����t�H���g�̑傫��,��,����,�����Ȃǂ����\���̂炵��
	GetTextMetrics(hdc, &TM);
	GLYPHMETRICS GM;	// �P��̃O���t�̏���\�� (�O���t):1��,�܂��͕����̕����̎��o�I�ȕ\��
	CONST MAT2 Mat = {
		{0,1},
		{0,0},
		{0,0},
		{0,1} };
	DWORD size = GetGlyphOutline(hdc, code, GGO_GRAY4_BITMAP, &GM, 0, nullptr, &Mat);	// �K�v�ȃo�b�t�@�T�C�Y��Ԃ��Ă���
	BYTE* ptr = new BYTE[size];	// �r�b�g�}�b�v�����󂯎��o�b�t�@�ւ̃|�C���^
	GetGlyphOutline(hdc, code, GGO_GRAY4_BITMAP, &GM, size, ptr, &Mat);		// �r�b�g�}�b�v�����擾

	// �f�o�C�X�R���e�L�X�g�ƃt�H���g�n���h���̉��
	SelectObject(hdc, oldFont);
	//DeleteObject(hFont);
	ReleaseDC(nullptr, hdc);


	//// CPU�ŏ������݂��ł���e�N�X�`�����쐬 ////
	ZeroMemory(&texture2d_desc, sizeof(D3D11_TEXTURE2D_DESC));
	texture2d_desc.Width = GM.gmCellIncX;
	texture2d_desc.Height = TM.tmHeight;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;		// RGBA(255,255,255,255)�^�C�v
	texture2d_desc.Usage = D3D11_USAGE_DYNAMIC;				// ���I CPU�������݉\
	texture2d_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;	// �V�F�[�_���\�[�X
	texture2d_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// CPU���珑�����݃A�N�Z�X��
	texture2d_desc.SampleDesc.Count = 1;						// �T���v�����O��1�s�N�Z���̂�

	hr = Dev->CreateTexture2D(&texture2d_desc, NULL, &tex2D);
	if (FAILED(hr)) { MessageBox(0, L"Font Texture Create Failure", NULL, MB_OK); }

	// �t�H���g�����e�N�X�`���ɏ�������
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	hr = DevCon->Map(tex2D.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	if (FAILED(hr)) { MessageBox(0, L"Font Texture Write Failure", NULL, MB_OK); }

	// �����ɏ�������
	BYTE* pBits = (BYTE*)MappedResource.pData;
	// �t�H���g���̏�������
	//  �����o���ʒu(����)
	int iOfs_x = GM.gmptGlyphOrigin.x;
	int iOfs_y = TM.tmAscent - GM.gmptGlyphOrigin.y;
	// �t�H���g�r�b�g�}�b�v�̕���
	// �r�b�g�}�b�v��1�s�̃s�N�Z������4�̔{���łȂ���΂Ȃ�Ȃ�
	int iBmp_w = GM.gmBlackBoxX + ((4 - (GM.gmBlackBoxX % 4)) % 4);
	int iBmp_h = GM.gmBlackBoxY;
	// Level : ���l�̒i�K (GGO_GRAY4_BITMAP�Ȃ̂�17�i�K)
	int Level = 17;

	DWORD Alpha, Color;
	memset(pBits, 0, MappedResource.RowPitch * TM.tmHeight);
	// �t�H���g���̏�������
	for (int y = iOfs_y; y < iOfs_y + iBmp_h; y++) {
		for (int x = iOfs_x; x < iOfs_x + iBmp_w; x++) {
			Alpha = (255 * ptr[x - iOfs_x + iBmp_w * (y - iOfs_y)]) / (Level - 1);
			Color = 0x00ffffff | (Alpha << 24);
			memcpy((BYTE*)pBits + MappedResource.RowPitch * y + 4 * x, &Color, sizeof(DWORD));	// �����炭�����Ńs�N�Z���ɏ�������ł�
		}
	}
	DevCon->Unmap(tex2D.Get(), 0);
	delete[] ptr;	// new�Ŋm�ۂ����̂Ō�Еt��

	//	�V�F�[�_�[���\�[�X�r���[�쐬
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = texture2d_desc.MipLevels;
	hr = Dev->CreateShaderResourceView(tex2D.Get(), &srvd, shader_resource_view.GetAddressOf());	// SRV�̍쐬 ����ŃV�F�[�_�ɓn����
	if (FAILED(hr)) { MessageBox(0, L"SRV Create Failure", NULL, MB_OK); }

	param->Size    = DirectX::SimpleMath::Vector2(static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
	param->TexSize = DirectX::SimpleMath::Vector2(static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));


	//// �V�F�[�_�p�ɃT���v�����쐬����
	//D3D11_SAMPLER_DESC samDesc;
	//ZeroMemory(&samDesc, sizeof(samDesc));
	//samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	//samDesc.AddressU = samDesc.AddressV = samDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	//samDesc.MaxAnisotropy = 1;
	//samDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	//samDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//hr = Dev->CreateSamplerState(&samDesc, &samplerState);


}