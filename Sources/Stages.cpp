#include "Stages.h"

#include <algorithm>	// std::generate�p

#include "misc.h"	// �G���[�o�͗p
#include "framework.h"


void StageParts::Initialize()
{
	Parameters = std::make_unique<Object3d>();
	Parameters->MaxLife = 5;
	Parameters->CurLife = Parameters->MaxLife;

	Model = std::make_unique<Skinned_Mesh>(".\\Resources\\Stage\\Stage.fbx", 0, false);
	if (!InstanceModel) {	// ��������Ă��Ȃ��ꍇ�ɐ�������
		InstanceModel = std::make_unique<InstanceSkinnedMesh>(".\\Resources\\Stage\\Stage.fbx", draw_amount);
	}
}

void StageParts::Update(bool inst)
{
	// �`�悷�邽�߃��f���ɏ���n��
	if (!inst) {
		Model->getParameters()->CopyParam(Parameters.get());	// �����ē����x�����ς���
	}
	//InstanceModel->getParameters()->CopyParam(Parameters.get());
}

void StageParts::Render(ID3D11DeviceContext* device_context, UINT drawCount, bool inst, Shader* shader)
{
	if (Parameters->Exist) {
		(!inst) ? Model->Render(device_context,shader) : InstanceModel->Render(device_context,drawCount);	// ����inst�ɂ���ă��f���P�̕`�悩�C���X�^���X�`�悩������
	}
}


//bool StageParts::onObject(const Object3d& obj)
//{
//	// �v�Z���׌y���̂���(�ɂȂ邩������Ȃ�����)���苗���𐧌�
//	static constexpr float rideDist = 1.5f;	// ���苗��
//	DirectX::SimpleMath::Vector3 length = Parameters->Position - obj.Position;
//	if (length.Length() > rideDist || !this->Parameters->Exist) return false;	// �����ȏ㗣��Ă��邩�������݂��Ȃ���������
//
//	DirectX::SimpleMath::Vector3 objHead = obj.Position + (obj.Vector * 0.7f);	// �v���C���[����O���ɏ������炵���ʒu�B�������B �����o������������������邽��
//
//	// �܂��X�e�[�W�̎O�p�`���_���Z�o���܂�
//	// ��(12/09)���݂̃��f���ɍ��킹�Ă�̂ŁA�ς������蒼���K�v���邩�炿����Ƃ܂�������
//	static constexpr float ofs_x = 1.0f;	// ���_���獶�E�ւ̃Y��
//	static constexpr float ofs_z = 1.0f;	// ���_�����ւ̃Y��
//	DirectX::SimpleMath::Vector3 triangle[3];
//	triangle[0] = Parameters->Position + (Model->getWorld().Backward() * ofs_x);	// �O���̓_�̂���
//	triangle[1] = Parameters->Position + (Model->getWorld().Forward()  * ofs_x) + (Model->getWorld().Left()  * ofs_z);	// �E���̓_�̂���
//	triangle[2] = Parameters->Position + (Model->getWorld().Forward()  * ofs_x) + (Model->getWorld().Right() * ofs_z);	// �����̓_�̂���
//	/*	  �����Ȃ��Ă����	*/
//	/*		   [0(A)]		�O	*/
//	/*		     *			��	*/
//	/*		    / \			��	*/
//	/*		   /   \		��	*/
//	/*		  /  P  \		��	*/
//	/*		 /       \			*/
//	/*[2(C)]*---------*[1(B)]	*/
//
//	// y������xz���ʂŊO�ς��v�Z �������������g���̂Ń����_���Œ�`
//	constexpr auto cross = [](const DirectX::SimpleMath::Vector3& v1, const DirectX::SimpleMath::Vector3& v2) {return (v1.x * v2.z) - (v1.z * v2.x); };
//	DirectX::SimpleMath::Vector3 A_p, B_p, C_p, A_B, B_C, C_A;	// ���ꂼ��̃x�N�g�����i�[����ϐ�
//	{
//		A_p = objHead - triangle[0];		// A����obj�O���Ɍ������x�N�g��
//		B_p = objHead - triangle[1];		// B����obj�O���Ɍ������x�N�g��
//		C_p = objHead - triangle[2];		// C����obj�O���Ɍ������x�N�g��
//		A_B = triangle[1] - triangle[0];	// A����B�Ɍ������x�N�g��
//		B_C = triangle[2] - triangle[1];	// B����C�Ɍ������x�N�g��
//		C_A = triangle[0] - triangle[2];	// C����A�Ɍ������x�N�g��
//		A_p.y = B_p.y = C_p.y = A_B.y = B_C.y = C_A.y = 0.0f;	// Y���͎g�p���Ȃ��̂�0�ɂ��ď���
//		float c1 = cross(A_B, B_p);
//		float c2 = cross(B_C, C_p);
//		float c3 = cross(C_A, A_p);
//		// �R�̊O�ό��ʂ�������v���Ă���Γ����A�s��v�Ȃ�O��
//		if ((c1 > 0 && c2 > 0 && c3 > 0) || (c1 < 0 && c2 < 0 && c3 < 0)) {
//			return true;	// �����Ł[��
//		}
//	}
//	// ����Ă��{�̂���ɏ���Ă邩�Čv�Z����
//	{
//		A_p = obj.Position - triangle[0];	// A����obj�Ɍ������x�N�g��
//		B_p = obj.Position - triangle[1];	// B����obj�Ɍ������x�N�g��
//		C_p = obj.Position - triangle[2];	// C����obj�Ɍ������x�N�g��
//		A_p.y = B_p.y = C_p.y = A_B.y = B_C.y = C_A.y = 0.0f;	// Y���͎g�p���Ȃ��̂�0�ɂ��ď���
//		float c1 = cross(A_B, B_p);
//		float c2 = cross(B_C, C_p);
//		float c3 = cross(C_A, A_p);
//		if ((c1 > 0 && c2 > 0 && c3 > 0) || (c1 < 0 && c2 < 0 && c3 < 0)) {
//			return true;	// �����Ł[��
//		}
//	}
//	return false;
//}

bool StageParts::onObjectSphere(const Object3d& obj,const float& radian)
{
	// �v�Z���׌y���̂���(�ɂȂ邩������Ȃ�����)���苗���𐧌�
	static constexpr float RIDE_DIST = 5.0f;	// ���苗��
	DirectX::SimpleMath::Vector3 length = Parameters->Position - obj.Position;
	if (length.Length() > RIDE_DIST || !this->Parameters->Exist) return false;	// �����ȏ㗣��Ă��邩�������݂��Ȃ���������


	// �܂��X�e�[�W�̎O�p�`���_���Z�o���܂� //
	// ��(05/04)���݂̃��f���ɍ��킹�Ă�̂ŁA�ς������蒼���K�v���邩�炿����Ƃ܂�������
	static constexpr float OFS_X = 1.0f;	// ���_���獶�E�ւ̃Y��
	static constexpr float OFS_Z = 1.0f;	// ���_�����ւ̃Y��
	DirectX::SimpleMath::Vector3 triangle[3];
	triangle[0] = Parameters->Position + (Model->getWorld().Backward() * OFS_X);	// �O���̓_�̂���
	triangle[1] = Parameters->Position + (Model->getWorld().Forward() * OFS_X) + (Model->getWorld().Left()  * OFS_Z);	// �E���̓_�̂���
	triangle[2] = Parameters->Position + (Model->getWorld().Forward() * OFS_X) + (Model->getWorld().Right() * OFS_Z);	// �����̓_�̂���
	/*	  �����Ȃ��Ă����	*/
	/*		   [0(A)]		�O	*/
	/*		     *			��	*/
	/*		    / \			��	*/
	/*		   /   \		��	*/
	/*		  /  P  \		��	*/
	/*		 /       \			*/
	/*[2(C)]*---------*[1(B)]	*/

	// y������xz���ʂŊO�ς��v�Z �������������g���̂Ń����_���Œ�`
	constexpr auto cross = [](const DirectX::SimpleMath::Vector3& v1, const DirectX::SimpleMath::Vector3& v2) {return (v1.x * v2.z) - (v1.z * v2.x); };

	const float rad = (radian) * 0.4f;	// ���a �X�P�[���l�ɂ���Ď����l�͕ϓ�����̂Ōv�Z
	DirectX::SimpleMath::Vector3 A_p, B_p, C_p, A_B, B_C, C_A;	// ���ꂼ��̃x�N�g�����i�[����ϐ�

	A_B = triangle[1] - triangle[0];	// A����B�Ɍ������x�N�g��
	B_C = triangle[2] - triangle[1];	// B����C�Ɍ������x�N�g��
	C_A = triangle[0] - triangle[2];	// C����A�Ɍ������x�N�g��

	A_p = obj.Position - triangle[0];	// A����obj�Ɍ������x�N�g��
	B_p = obj.Position - triangle[1];	// B����obj�Ɍ������x�N�g��
	C_p = obj.Position - triangle[2];	// C����obj�Ɍ������x�N�g��

	A_p.y = B_p.y = C_p.y = A_B.y = B_C.y = C_A.y = 0.0f;	// Y���͎g�p���Ȃ��̂�0�ɂ��ď���
	DirectX::SimpleMath::Vector3 p[3];	// �����Ƌ��̍ŒZ�_
	DirectX::SimpleMath::Vector3 len[3];	// �ŒZ�_�Ƌ��̋���

	DirectX::SimpleMath::Vector3 Normal = (triangle[1] - triangle[0]).Cross(triangle[2] - triangle[0]);	// �O�p�`�̖@�����Z�o
	Normal.Normalize();	// ���K��

	len[0] = p[0] - obj.Position;
	len[1] = p[1] - obj.Position;
	len[2] = p[2] - obj.Position;
	len[0].y = len[1].y = len[2].y = 0.0f;	// y�����O
	float ProjectionLength = 0.0f;	// �ˉe��


	//--------------------- ����蔻��J�n ��ӂƂł��������Ă���Ώ��Ă���Ƃ������ƂȂ̂�true��Ԃ�--------------------- //

	A_B.Normalize();
	ProjectionLength = A_B.Dot(A_p);	// �ˉe�x�N�g���̒���
	p[0] = triangle[0] + (A_B * ProjectionLength);	// ���Ɛ���A_B�̍ŒZ�_
	len[0] = p[0] - obj.Position;
	len[0].y = 0.0f;	// y�����O


	if (len[0].Length() <= rad)	// �ŒZ�_�Ƃ̋��������a��菬������Γ������Ă���\��������̂Ōv�Z�𑱂���
	{
		if (A_B.Dot(A_p) * A_B.Dot(B_p) < 0) { return true; }	// A_p��B_p�̊p�x���s�p�݊p��v���Ȃ���ΐ����͈̔͂ł���̂ł�����
		if (A_p.Length() < rad || B_p.Length() < rad) { return true; }	// �����̖��[�����a�ȉ��ł���Ζ��[���~���Ȃ̂œ�����
	}

	B_C.Normalize();
	ProjectionLength = B_C.Dot(B_p);	// �ˉe�x�N�g���̒���
	p[1] = triangle[1] + (B_C * ProjectionLength);	// ���Ɛ���B_C�̍ŒZ�_
	len[1] = p[1] - obj.Position;
	len[1].y = 0.0f;	// y�����O

	if (len[1].Length() <= rad)	// �ŒZ�_�Ƃ̋��������a��菬������Γ������Ă���\��������̂Ōv�Z�𑱂���
	{
		if (B_C.Dot(B_p) * B_C.Dot(C_p) < 0) { return true; }	// B_p��C_p�̊p�x���s�p�݊p��v���Ȃ���ΐ����͈̔͂ł���̂ł�����
		if (B_p.Length() < rad || C_p.Length() < rad) { return true; }	// �����̖��[�����a�ȉ��ł���Ζ��[���~���Ȃ̂œ�����
	}

	C_A.Normalize();
	ProjectionLength = C_A.Dot(C_p);	// �ˉe�x�N�g���̒���
	p[2] = triangle[2] + (C_A * ProjectionLength);	// ���Ɛ���C_A�̍ŒZ�_
	len[2] = p[2] - obj.Position;
	len[2].y = 0.0f;	// y�����O

	if (len[2].Length() <= rad)	// �ŒZ�_�Ƃ̋��������a��菬������Γ������Ă���\��������̂Ōv�Z�𑱂���
	{
		if (C_A.Dot(C_p) * C_A.Dot(A_p) < 0) { return true; }	// C_p��A_p�̊p�x���s�p�݊p��v���Ȃ���ΐ����͈̔͂ł���̂ł�����
		if (C_p.Length() < rad || A_p.Length() < rad) { return true; }	// �����̖��[�����a�ȉ��ł���Ζ��[���~���Ȃ̂œ�����
	}


	//--------------------- ����蔻��I�� �ŏI�`�F�b�N ---------------------//
	// �����O�p�`�̓����ɂ��邩�ǂ����m�F
	DirectX::SimpleMath::Vector3 crs[3];	// 3�ӂƋ��̊O��
	crs[0] = A_B.Cross(A_p);
	crs[1] = B_C.Cross(B_p);
	crs[2] = C_A.Cross(C_p);
	// ���ς����ׂăv���X�ł���Εӂ���E���ɂ���̂Œ��_�Ƃ̋���������Ă��Ă������ł�
	float dot[3] = { crs[0].Dot(Normal),crs[1].Dot(Normal),crs[2].Dot(Normal) };
	return (dot[0] > 0 && dot[1] > 0 && dot[2] > 0) ? true : false;
}

void StageParts::Damage()
{
	Parameters->CurLife--;	// �̗͂����炷
	Parameters->Color.w = static_cast<float>(Parameters->CurLife) / static_cast<float>(Parameters->MaxLife);

	// �����̗͂�0�ȉ��ł���Α��݃t���O���ւ��܂�B����ȊO�͑��݂��Ă܂�
	Parameters->Exist = (Parameters->CurLife <= 0) ? false : true;
}




//---------------------------------------------------------------------------------------------------------------------------------------------------------
StageManager::StageManager()
{
	if ((ROW_PARTS % 2 == 0) || (COL_PARTS % 2 == 0))	// ���������Őݒ肵������Ă���Ȃ�
	{
		_ASSERT_EXPR_A(false, "_StageParts is not an odd number._");	// �X�e�[�W�p�[�c����ł͗L��܂���G���[ �ꉞ��
	}
}

void StageManager::Initialize()
{
	// �X�e�[�W����(����)
	//---------------------------------------------------------
	static const int CENTER = PARTS_SIZE / 2;	// �c������̂Ƃ��A/2+1�Ő^�񒆂̐��l������c�񂾂��ǁA0�X�^�[�g�̂Ƃ��͕��ʂɔ����̒l

	// ���܂�����make_unique���g�����ɒT���Ă��Ƃ���A���̂悤�ȕ��@���������̂ō̗p�B1�s�ōςނ��X�}�[�g����
	// �쐬 begin~end�܂�{}�����s���� �R���X�g���N�^�̈����͕`�搔�Ƃ��Ďg�p
	std::generate(std::begin(Stages), std::end(Stages), []() { return std::make_unique<StageParts>(PARTS_SIZE); });

	// �^�񒆂�0�Ƃ��邽�߁A�^�񒆂���̍������J�n�n�_�A�I�_�n�_�ɂ����Ă���B�u1,2,3�v�Ō�����2��^�񒆁A1��3���n�_�I�_�ɂ���B
	// �(��L3)�������Ɉ�x�ϊ�����̂�-1���Ă���B
	for (int i = static_cast<int>(-((COL_PARTS - 1) * 0.5f)); i <= static_cast<int>(((COL_PARTS - 1) * 0.5f)); i++)
	{
		for (int j = static_cast<int>(-((ROW_PARTS - 1) * 0.5f)); j <= static_cast<int>(((ROW_PARTS - 1) * 0.5f)); j++)
		{
			int row = CENTER + j;		// �\�ߗv�f�ԍ��̌v�Z���s���Ă���
			int col = COL_PARTS * i;
			int node = row + col;	// ���ݎQ�Ƃ��Ă���v�f�ԍ�
			if (node < 0) { _ASSERT_EXPR_A(false, "Initialize ArraySize UnderFlow"); }	// �v�f�ԍ����}�C�i�X�w��ɂȂ����������G���[
			Stages.at(node)->Initialize();
			// �^�񒆂̗v�f(CENTER)�ɍs(j)�����Z���A��(COL_PARTS)��i�񕪏�Z���Č��Z����
			Stages.at(node)->Parameters->Position = DirectX::SimpleMath::Vector3(j * 1.0f, -0.5f, i * -2.0f);

			bool inversion = false;	// �p�x���]�t���O
			inversion = (j % 2) ? true : false;	// �������A��s�ڂȂ�p�x���]�t���O��true�ɂ���
			if (i % 2)inversion = !inversion;	// �������A���ڂȂ�p�x���]�t���O�́u�����v�𔽓]������
			if (inversion)	// 180����]������
			{
				Stages.at(node)->Parameters->Orientation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(180.0f), 0.0f, 0.0);
				Stages.at(node)->Parameters->Color = DirectX::SimpleMath::Vector4(1.0f, 0.4f, 0.4f, 1.0f);
				//Stages.at(node)->Parameters->Color = DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f);
			}
			else
			{
				Stages.at(node)->Parameters->Orientation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0);
				Stages.at(node)->Parameters->Color = DirectX::SimpleMath::Vector4(0.4f, 0.4f, 1.0f, 1.0f);
				//Stages.at(node)->Parameters->Color = DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f);
			}

			// �^�񒆂��킩��₷���F�ς��Ă�
			if (node == CENTER) { Stages.at(CENTER)->Parameters->Color = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 0.0f, 1.0f); }
			Stages.at(node)->Model->getParameters()->CopyParam(Stages.at(node)->Parameters.get());
			// �C���X�^���X�p�f�[�^�����낦��
			// �f�[�^���X�g�b�N
			Stages.at(0)->InstanceModel->getParameters()->CopyParam(Stages.at(node)->Parameters.get());
			Stages.at(0)->InstanceModel->addWorldData();
		}
	}
}

void StageManager::Update()
{
	if (inst) {
		for (auto& it : Stages)
		{
			if (it->Parameters->Exist)
			{
				it->Update(inst);
			}
		}
	}
}

void StageManager::Render(ID3D11DeviceContext* device_context, Shader* shader)
{
	if (GetAsyncKeyState('I') & 1) inst = !inst;	// �C���X�^���X�`��؂�ւ�
	if (!inst) {
		for (auto& it : Stages)
		{
			it->Render(device_context, PARTS_SIZE, inst, shader);	// ��ʕ`��p�X
		}
	}
	else {
		Stages.at(0)->Render(device_context, PARTS_SIZE, inst);	// �C���X�^���X�`�� ����͂O�Ԗڂ����Ƀh���[�R�[����ʂ��A�C���X�^���X�`��
	}
}

void StageManager::Check(const Object3d& obj, const float& radian)
{
	int index = 0;
	for (auto& it : Stages)
	{
		// �I�u�W�F�N�g�ƃX�e�[�W�̈ʒu����ŏ�ɏ���Ă�����X�e�[�W�̃_���[�W�������Ăяo��
		if(it->onObjectSphere(obj,radian))
		{
			it->Damage();
			Stages.at(0)->InstanceModel->setColor(index, it->Parameters->Color);	// �_���[�W���󂯂邽�тɓ����x���ω�����̂Őݒ肵�Ă���
		}
		index++;	// ���Ԗڂ��̃J�E���g�p
	}
}

bool StageManager::RideParts(Object3d& obj,const float& radian)
{
	for (auto& it : Stages)
	{
		// �I�u�W�F�N�g�ƃX�e�[�W�̈ʒu����ŏ�ɏ���Ă�����y�l���Œ肷��
		if (it->onObjectSphere(obj,radian))
		{
			return true;
		}
	}
	return false;
}
