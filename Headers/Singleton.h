#pragma once

template<class T>

class singleton
{
public:
	static T& getInstance()
	{
		static T instance;
		return instance;
	}
protected:
	singleton(){}	// �O���ł̃C���X�^���X�쐬�͋֎~
	virtual ~singleton(){}

private:
	// ������R�s�[�֎~
	void operator=(const singleton& obj) {}
	singleton(const singleton& obj) {}
};