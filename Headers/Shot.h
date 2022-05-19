#pragma once

#include "Object3d.h"
#include "skinned_mesh.h"
#include "geometric_primitive.h"

#include <memory>

// �e�N���X
class Shot
{
	// �ϐ�
private:
	std::unique_ptr<Skinned_Mesh> Model;	// ���̂܂܃��f��
	std::unique_ptr<Geometric_Sphere> Sphere;	// �e�X�g���f�� �܂�

	// �f�t�H���g�̃V�F�[�_�[
	//std::unique_ptr<ShaderEx> SkinnedShader = nullptr;

	float LifeTimer = 0;	// �ˏo���Ԃۗ̕L
	bool Exist = false;		// ���ˍς݃t���O
public:
	std::unique_ptr<Object3d> Parameters;
	bool HitConfirmation = false;	// �����蔻��

	// �֐�
private:
public:

	Shot() {};
	Shot(Shot& shot)
	{
		this->Model = std::move(shot.Model);
		this->Sphere = std::move(shot.Sphere);
		this->LifeTimer = shot.LifeTimer;
	};
	~Shot() {};

	void Initialize();
	void Update();
	void Render(ID3D11DeviceContext* device_context);

	void set(const Object3d* parent);	// �I�u�W�F�N�g�͑S��Object3d�N���X��ۗL���Ă���O��Ŋe��p�����[�^���R�s�[
	void setExist(bool flg) { Exist = flg; }
	bool getExist() { return Exist; }
};

class ShotManager
{
private:
	std::vector<std::unique_ptr<Shot>> Shots;

public:
	// �����肪�N��
	enum class MASTER
	{
		PLAYER,
		ENEMY,
	};
	MASTER Master;

	void Initialize(MASTER ms) { Shots.clear(); Master = ms; }
	void Update();	// ���݂��Ă��Ȃ��e�͓����ō폜���Ă���
	void Render(ID3D11DeviceContext* device_context);

	// �����A�i�[�n
	void newSet(const Object3d* initData);
	void push(std::unique_ptr<Shot> shot) { Shots.emplace_back(std::move(shot)); };	// �z��Ɋi�[����

	size_t getSize() { return Shots.size(); };
	std::vector<std::unique_ptr<Shot>>* getShots() { return &Shots; }	// �i�[�R���e�i��Ԃ� �O������Q�Ƃ������Ƃ���

	bool isHit(const Object3d* capcule,const Shot* shots);
	bool isHits(const Object3d* sphere,const Shot* shots);
};