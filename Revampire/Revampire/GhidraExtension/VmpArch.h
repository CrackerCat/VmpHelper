#pragma once
#include "../Ghidra/sleigh_arch.hh"

//��ǰ��֧��32λ

class VmpArchitecture :public ghidra::SleighArchitecture
{
public:
	VmpArchitecture();
	~VmpArchitecture();
	bool initVmpArchitecture();
protected:
	void buildLoader(ghidra::DocumentStorage& store) override;
	void resolveArchitecture(void) override;
private:

};