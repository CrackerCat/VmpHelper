#pragma once
#include "../Ghidra/pcoderaw.hh"
#include "../GhidraExtension/VmpInstruction.h"

namespace ghidra
{
    class Funcdata;
	class PcodeOp;
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
	void ClearStatus();
	//vm�ֽ���Ĵ���
	std::string reg_code;
	//vm�����ջ�Ĵ���
	std::string reg_stack;
	//�Ƿ���ѡ����˼Ĵ���
	bool isSelected = false;
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
	VmpBlockBuildContext();
	bool PushVmpOp(std::unique_ptr<VmpInstruction> inst);
public:
	VmpRegStatus vmreg;
	VM_MATCH_STATUS status;
	//�����ɵ�block
	VmpBasicBlock* newBlock;
	//��¼��һ��block
	VmAddress from_addr;
	BUILD_RET build_ret;
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
	bool tryMatch_vPopReg(ghidra::PcodeOp* opStore, VmpNode& nodeInput, std::vector<GhidraHelper::TraceResult>& dstResult, std::vector<GhidraHelper::TraceResult>& srcResult);
	bool tryMatch_vPushImm(ghidra::Funcdata* fd, VmpNode& nodeInput, std::vector<GhidraHelper::TraceResult>& dstResult, std::vector<GhidraHelper::TraceResult>& srcResult);
	bool tryMatch_vPushReg(ghidra::Funcdata* fd, VmpNode& nodeInput, std::vector<GhidraHelper::TraceResult>& dstResult, std::vector<GhidraHelper::TraceResult>& srcResult);
	bool tryMatch_vLogicalOp(ghidra::Funcdata* fd, VmpNode& nodeInput, std::vector<GhidraHelper::TraceResult>& dstResult, std::vector<GhidraHelper::TraceResult>& srcResult);
	bool tryMatch_vCheckEsp(ghidra::Funcdata* fd, VmpNode& nodeInput);
	bool tryMatch_vJmp(ghidra::Funcdata* fd, VmpNode& nodeInput);
private:
	VmpBlockBuildContext* buildContext;
private:
    VmpBlockWalker* walker;
    VmpArchitecture* arch;
};