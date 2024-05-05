#pragma once
#include "../Common/Public.h"

//����vmp�İ汾
//����Ŀǰ��˵�������дͨ�õ�ȥvmp���ű�,�������vmp�İ汾��д�ض��ű������

class VmpVersionManager
{
public:
	enum VmpVersion {
		VMP_UNKNOWN = 0x0,
		VMP_350,
		VMP_380,
	};
	static void SetVmpVersion(VmpVersion v) { ver = v; };
	static VmpVersion CurrentVmpVersion() { return ver; };
public:
	static VmpVersion ver;
};