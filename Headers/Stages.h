#pragma once


#include "skinned_mesh.h"
#include "Object3d.h"
#include "shaderEx.h"

#include <memory>
#include <array>
#include <SimpleMath.h>

class Enemy;

class StageParts
{
	// �ϐ�
private:
	// �f�t�H���g�̃V�F�[�_�[
	static inline std::unique_ptr<ShaderEx> SkinnedShader;

public:
	std::unique_ptr<Object3d> Parameters;
	std::unique_ptr<Skinned_Mesh> Model;	// ���̂܂܃��f��

	//�֐�
private:
public:
	StageParts();
	~StageParts() {};

	void Initialize();
	void Update();
	void Render();

	void onObject(const DirectX::SimpleMath::Vector3& objPos);
};
// �X�e�[�W�p�[�c�̑��Ǘ��N���X �Ƃ������p�[�c�S�������Ă�
class StageManager
{
private:
	const static int ROW_PARTS = 15;	// �s���B��ł̐ݒ肨�肢���܂�
	const static int COL_PARTS = 15;	// �񐔁B��ł̐ݒ肨�肢���܂�
	const static int PARTS_SIZE = ROW_PARTS * COL_PARTS;	// �c*���̑S�Ẵp�[�c��

	std::array<std::unique_ptr<StageParts>, PARTS_SIZE> Stages;	// ���̂܂܃��f��

public:
	StageManager();
	void Initialize();
	void Update();
	void Render();

	static StageManager& getInstance()
	{
		static StageManager instance;
		return instance;
	}
	// StageParts��onObject()�ɓn������
	void Check(const DirectX::SimpleMath::Vector3& obj);

	// �擾�A�i�[�n
	size_t getSize() { return Stages.size(); };
};