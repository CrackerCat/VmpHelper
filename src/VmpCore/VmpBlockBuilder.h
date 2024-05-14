#pragma once
#include "../GhidraExtension/VmpInstruction.h"
#include "../Helper/UnicornHelper.h"
#include "../VmpCore/VmpUnicorn.h"

namespace GhidraHelper
{
	struct TraceResult;
}

class VmpControlFlowBuilder;
class VmpFlowBuildContext;
class VmpNode;
class VmpBasicBlock;
class VmpTraceFlowGraph;
class VmpOpJmp;
class VmpOpJmpConst;

class VmpBlockWalker
{
public:
	VmpBlockWalker(VmpTraceFlowGraph& t) :tfg(t) {};
	~VmpBlockWalker() {};
public:
	void StartWalk(VmpUnicornContext& startCtx, size_t walkSize);
	const std::vector<reg_context>& GetTraceList();
	bool IsWalkToEnd();
	VmpNode GetNextNode();
	void MoveToNext();
	size_t CurrentIndex();
private:
	VmpUnicorn unicorn;
	VmpTraceFlowGraph& tfg;
	//��ǰִ�е�ָ��˳��
	size_t idx = 0x0;
	//��ǰ�ڵ��С
	size_t curNodeSize = 0x0;
};



//������vmp������

class VmpBlockBuilder
{
public:
	VmpBlockBuilder(VmpControlFlowBuilder& cfg);
	~VmpBlockBuilder() {};
public:
	bool BuildVmpBlock(VmpFlowBuildContext* task);
private:
	bool Execute_FIND_VM_INIT();
	bool Execute_FINISH_VM_INIT();
	std::unique_ptr<VmpInstruction> AnaVmpPattern(ghidra::Funcdata* fd,VmpNode& nodeInput);
	std::unique_ptr<VmpInstruction> tryMatch_vPopReg(ghidra::Funcdata* fd, VmpNode& nodeInput);
	std::unique_ptr<VmpInstruction> tryMatch_vCpuid(ghidra::Funcdata* fd, VmpNode& nodeInput);
	std::unique_ptr<VmpInstruction> tryMatch_vRdtsc(ghidra::Funcdata* fd, VmpNode& nodeInput);
	std::unique_ptr<VmpInstruction> tryMatch_vPushImm(ghidra::Funcdata* fd, VmpNode& nodeInput);
	std::unique_ptr<VmpInstruction> tryMatch_vPushReg(ghidra::Funcdata* fd, VmpNode& nodeInput);
	std::unique_ptr<VmpInstruction> tryMatch_vLogicalOp(ghidra::Funcdata* fd, VmpNode& nodeInput);
	std::unique_ptr<VmpInstruction> tryMatch_vLogicalOp2(ghidra::Funcdata* fd, VmpNode& nodeInput);
	std::unique_ptr<VmpInstruction> tryMatch_vMemAccess(ghidra::Funcdata* fd, VmpNode& nodeInput);
	std::unique_ptr<VmpInstruction> tryMatch_vJmp(ghidra::Funcdata* fd, VmpNode& nodeInput);
	std::unique_ptr<VmpInstruction> tryMatch_Mul(ghidra::Funcdata* fd, VmpNode& nodeInput);
	std::unique_ptr<VmpInstruction> tryMatch_Div(ghidra::Funcdata* fd, VmpNode& nodeInput);
	std::unique_ptr<VmpInstruction> tryMatch_vPushVsp(ghidra::Funcdata* fd, VmpNode& nodeInput);
	std::unique_ptr<VmpInstruction> tryMatch_vWriteVsp(ghidra::Funcdata* fd, VmpNode& nodeInput);
	std::unique_ptr<VmpInstruction> tryMatch_vCopyStack(ghidra::Funcdata* fd, VmpNode& nodeInput);
	std::unique_ptr<VmpInstruction> tryMatch_vJmpConst(ghidra::Funcdata* fd, VmpNode& nodeInput);
	bool tryMatch_vCheckEsp(ghidra::Funcdata* fd, VmpNode& nodeInput);
	bool tryMatch_vJunkCode(ghidra::Funcdata* fd, VmpNode& nodeInput);
	bool tryMatch_vPopfd(ghidra::Funcdata* fd, VmpNode& nodeInput);
	bool tryMatch_vExit(ghidra::Funcdata* fd, VmpNode& nodeInput);
private:
	bool updateVmRegOffset(ghidra::Funcdata* fd);
	//ִ��ÿ��opָ��
	bool executeVmpOp(VmpNode& nodeInput, std::unique_ptr<VmpInstruction> inst);
	bool executeVmJmp(VmpNode& nodeInput, VmpOpJmp* inst);
	bool executeVmJmpConst(VmpNode& nodeInput, VmpOpJmpConst* inst);
	bool updateVmReg(VmpNode& nodeInput, VmpInstruction* inst);
	bool executeVmInit(VmpNode& nodeInput, VmpOpInit* inst);
	bool executeVmExit(VmpNode& nodeInput, VmpInstruction* inst);
public:
	static int vmRegStartAddr;
private:
	VmpControlFlowBuilder& flow;
	VmpBasicBlock* curBlock;
	VmpFlowBuildContext* buildCtx;
	VmpBlockWalker walker;
};