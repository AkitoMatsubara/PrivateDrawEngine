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
	singleton(){}	// 外部でのインスタンス作成は禁止
	virtual ~singleton(){}

private:
	// 代入＆コピー禁止
	void operator=(const singleton& obj) {}
	singleton(const singleton& obj) {}
};