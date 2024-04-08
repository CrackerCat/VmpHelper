#pragma once
#include "../Ghidra/pcoderaw.hh"

namespace ghidra
{
    class Funcdata;
}

namespace GhidraHelper
{
	struct TraceResult;
}

class VmpNode;
class VmpArchitecture;
class VmpBlockWalker;
class VmpBasicBlock;

class VmpRegStatus
{
public:
	//vm�ֽ���Ĵ���
	std::string vmCodeReg;
	//vm�����ջ�Ĵ���
	std::string vmStackReg;
	//�Ƿ���ѡ����˼Ĵ���
	bool isSelected;
};

//vmp��������

class VmpBlockBuildContext
{
public:
	enum BUILD_RET
	{
		//����ƥ��
		BUILD_CONTINUE = 0x0,
		//�ϲ�ƥ��
		BUILD_MERGE,
		//�˳�
		BUILD_EXIT,
	};
	enum VM_MATCH_STATUS {
		FIND_VM_INIT = 0x0,
		FINISH_VM_INIT = 0x1,
	};
public:
	VmpRegStatus vmReg;
	VM_MATCH_STATUS status;
	VmpBasicBlock* bBlock;
	BUILD_RET ret;
};

struct VmpHandlerRange
{
    size_t startAddr;
    size_t endAddr;
    std::uint64_t hash;
    VmpHandlerRange(VmpNode& nodeInput);
    bool operator<(const VmpHandlerRange& other) const
    {
        return std::tie(startAddr, endAddr, hash) < std::tie(other.startAddr, other.endAddr, other.hash);
    }
private:
    std::uint64_t NodeHash(VmpNode& nodeInput);
};

class VmpHandlerFactory
{
public:
    VmpHandlerFactory(VmpArchitecture* re);
    ~VmpHandlerFactory() {};
public:
    bool BuildVmpBlock(VmpBlockBuildContext* buildData,VmpBlockWalker& walker);
private:
	void ExecuteVmpPattern(VmpNode& nodeInput);
	bool Execute_FIND_VM_INIT(VmpNode& nodeInput, ghidra::Funcdata* fd);
	bool Execute_FINISH_VM_INIT(VmpNode& nodeInput, ghidra::Funcdata* fd);
	bool tryMatch_vPopReg(std::vector<GhidraHelper::TraceResult>& dstResult, std::vector<GhidraHelper::TraceResult>& srcResult);
private:
	VmpBlockBuildContext* buildContext;
private:
    VmpBlockWalker* walker;
    VmpArchitecture* arch;
};