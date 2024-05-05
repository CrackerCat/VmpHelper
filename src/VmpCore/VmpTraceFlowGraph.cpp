#include "VmpTraceFlowGraph.h"
#include <sstream>
#include "../Manager/DisasmManager.h"
#include "../Manager/VmpVersionManager.h"
#include "../Manager/exceptions.h"

#ifdef DeveloperMode
#pragma optimize("", off) 
#endif

VmpTraceFlowGraph::VmpTraceFlowGraph()
{
#ifdef DEBUG_TRACEFLOW
	std::string logFilePath = R"(C:\Work\VmpConsole\VmpConsole\Release\traceflow\flow.txt)";
    logFile.open(logFilePath, std::ios::out | std::ios::trunc);
#endif
}

VmpTraceFlowGraph::~VmpTraceFlowGraph()
{
#ifdef DEBUG_TRACEFLOW
    logFile.close();
#endif
}

bool isEndIns(cs_insn* curIns)
{
    if (curIns->id == x86_insn::X86_INS_RET) {
        return true;
    }
    if (curIns->id == x86_insn::X86_INS_CALL) {
        return true;
    }
    //��תָ��
    if (curIns->id >= x86_insn::X86_INS_JAE && curIns->id <= X86_INS_JS) {
        return true;
    }
    if (curIns->id == x86_insn::X86_INS_JMP) {
        return true;
    }
    return false;
}

void VmpTraceFlowGraph::executeMerge(VmpTraceFlowNode* fatherNode, VmpTraceFlowNode* childNode)
{
    removeEdge(fatherNode->EndAddr(), childNode->nodeEntry);
    //�ӽڵ��ַ�Ƶ����ڵ�
    int startIdx = fatherNode->addrList.size();
    for (unsigned int n = 0; n < childNode->addrList.size(); ++n) {
        fatherNode->addrList.push_back(childNode->addrList[n]);
		updateInstructionToNodeMap(childNode->addrList[n], fatherNode, startIdx + n);
    }
}

bool VmpTraceFlowGraph::checkCanMerge_Vmp(size_t nodeAddr)
{
    size_t fromAddr = *toEdges[nodeAddr].begin();
    VmpTraceFlowNode* fatherNode = instructionToNodeMap[fromAddr].vmNode;
    std::unique_ptr<RawInstruction> endIns = DisasmManager::Main().DecodeInstruction(fatherNode->EndAddr());
    if (!endIns) {
        return false;
    }
    //retָ��һ���ǲ����кϲ���
    if (endIns->raw->id == X86_INS_RET) {
        return false;
    }
    if (VmpVersionManager::CurrentVmpVersion() == VmpVersionManager::VMP_350) {
        //�����jmp eax����ָ��,�����кϲ�
        if (endIns->raw->id == X86_INS_JMP && endIns->raw->detail->x86.operands[0].type == X86_OP_REG) {
            return false;
        }
    }
    else if (VmpVersionManager::CurrentVmpVersion() == VmpVersionManager::VMP_380) {
        //jmp eax
        if (endIns->raw->id == X86_INS_JMP && endIns->raw->detail->x86.operands[0].type == X86_OP_REG) {
            if (fatherNode->addrList.size() > 2) {
                x86_reg jmpReg = endIns->raw->detail->x86.operands[0].reg;
                std::unique_ptr<RawInstruction> lastIns = DisasmManager::Main().DecodeInstruction(fatherNode->addrList[fatherNode->addrList.size() - 2]);
                if (lastIns->raw->id == X86_INS_ADC || lastIns->raw->id == X86_INS_ADD) {
                    cs_x86_op& op0 = lastIns->raw->detail->x86.operands[0];
                    cs_x86_op& op1 = lastIns->raw->detail->x86.operands[1];
                    if (op0.type == X86_OP_REG && op1.type == X86_OP_IMM && jmpReg == op0.reg) {
                        return true;
                    }
                }
                return false;
            }
        }
    }
    return true;
}


bool VmpTraceFlowGraph::checkCanMerge(size_t nodeAddr)
{
    //����1,ָ���ӽڵ�ı�ֻ��1��
    if (toEdges[nodeAddr].size() != 1) {
        return false;
    }
    //�õ�ָ��ýڵ�ĸ��ڵ�
    size_t fromAddr = *toEdges[nodeAddr].begin();
    VmpTraceFlowNode* fatherNode = instructionToNodeMap[fromAddr].vmNode;
    //����2,���ڵ�ָ��ı�Ҳֻ��1��
    if (fromEdges[fromAddr].size() != 1) {
        return false;
    }
    //����3,�ӽڵ㲻��ָ�򸸽ڵ�
    if (fromEdges[nodeAddr].count(fatherNode->addrList[fatherNode->addrList.size() - 1])) {
        return false;
    }
    return true;
}

VmpTraceFlowNode* VmpTraceFlowGraph::splitBlock(VmpTraceFlowNode* toNode, size_t splitAddr)
{
    VmpTraceFlowNode* newNode = createNode(splitAddr);
    unsigned int index = 1;
    while (true) {
        if (index >= toNode->addrList.size()) {
            throw VmpTraceException("splitBlock error");
        }
        if (toNode->addrList[index] == splitAddr) {
            linkEdge(toNode->addrList[index - 1], splitAddr);
            break;
        }
        index++;
    }
    //��������ָ���
    int insIndex = 1;
    for (unsigned int n = index + 1; n < toNode->addrList.size(); ++n) {
        newNode->addrList.push_back(toNode->addrList[n]);
        this->updateInstructionToNodeMap(toNode->addrList[n], newNode, insIndex++);
    }
    //��ȡ�ϰ��Block
    toNode->addrList.resize(index);
    return newNode;
}

VmpTraceFlowNode* VmpTraceFlowGraph::createNode(size_t start)
{
    VmpTraceFlowNode* newNode = &nodeMap[start];
    newNode->nodeEntry = start;
    newNode->addrList.push_back(start);
    this->updateInstructionToNodeMap(start, newNode, 0x0);
    return newNode;
}

void VmpTraceFlowGraph::removeEdge(size_t from, size_t to)
{
    fromEdges[from].erase(to);
    toEdges[to].erase(from);
}

void VmpTraceFlowGraph::linkEdge(size_t from, size_t to)
{
    fromEdges[from].insert(to);
    toEdges[to].insert(from);
}

void VmpTraceFlowGraph::updateInstructionToNodeMap(size_t addr, VmpTraceFlowNode* updateNode, int index)
{
    this->instructionToNodeMap[addr] = VmpTraceFlowNodeIndex(updateNode, index);
}

bool VmpTraceFlowGraph::addJmpLink(size_t fromAddr, size_t toAddr)
{
#ifdef DEBUG_TRACEFLOW
	logFile << "Jmp from " << std::hex << fromAddr << " - " << toAddr << std::endl;
    if (toAddr == 0x451cef) {
        int a = 0;
    }
#endif
    linkEdge(fromAddr, toAddr);
    //��ȷ��������������
    VmpTraceFlowNodeIndex* curNodeIndex = &instructionToNodeMap[fromAddr];
    if (curNodeIndex->vmNode == nullptr) {
        curNodeIndex->vmNode = createNode(fromAddr);
        curNodeIndex->index = 0x0;
    }
    VmpTraceFlowNodeIndex* nextNodeIndex = &instructionToNodeMap[toAddr];
    if (nextNodeIndex->vmNode == nullptr) {
        nextNodeIndex->vmNode = createNode(toAddr);
        nextNodeIndex->index = 0x0;
    }
    //ֻ�е�from������β��ַ��toλ�������׵�ַ�Ų��÷ֿ�
    if (curNodeIndex->index == curNodeIndex->vmNode->addrList.size() - 1 && nextNodeIndex->index == 0x0) {
        return true;
    }
    //ȷ����Ҫ�ֿ�
    //�����Ѿ���ͬһ����������
    if (curNodeIndex->vmNode == nextNodeIndex->vmNode) {
        splitBlock(nextNodeIndex->vmNode, toAddr);
        return true;
    }
    //�����ڲ�ͬ������
    if (curNodeIndex->index != curNodeIndex->vmNode->addrList.size() - 1) {
        splitBlock(curNodeIndex->vmNode, fromAddr);
    }
    if (nextNodeIndex->index != 0x0) {
        splitBlock(nextNodeIndex->vmNode, toAddr);
    }
    return true;
}

bool VmpTraceFlowGraph::addNormalLink(size_t fromAddr, size_t toAddr)
{
#ifdef DEBUG_TRACEFLOW
    logFile << "link from " << std::hex << fromAddr << " - " << toAddr << std::endl;
    if (fromAddr == 0x451CEF) {
        int a = 0;
    }
#endif
    VmpTraceFlowNodeIndex* curNodeIndex = &instructionToNodeMap[fromAddr];
    if (curNodeIndex->vmNode == nullptr) {
        curNodeIndex->vmNode = createNode(fromAddr);
        curNodeIndex->index = 0x0;
    }
    VmpTraceFlowNodeIndex* nextNodeIndex = &instructionToNodeMap[toAddr];
    if (nextNodeIndex->vmNode == nullptr) {
        curNodeIndex->vmNode->addrList.push_back(toAddr);
        nextNodeIndex->vmNode = curNodeIndex->vmNode;
        nextNodeIndex->index = curNodeIndex->index + 1;
    }
    //������ͬ�������ҷ�����ת˳��
    if (curNodeIndex->vmNode == nextNodeIndex->vmNode) {
        if (curNodeIndex->index + 1 == nextNodeIndex->index) {
            return true;
        }
        //����������ǲ����ܵ�
        throw VmpTraceException("addNormalLink error");
    }
    //˵��������ָ��ָ���A->B,���Ծ�����
    return true;
}

bool VmpTraceFlowGraph::addLink(size_t fromAddr, size_t toAddr)
{
    std::unique_ptr<RawInstruction> tmpIns = DisasmManager::Main().DecodeInstruction(fromAddr);
    if (!tmpIns) {
        return false;
    }
    //�������Ϊ�������
    //��һ���Ǵ�Aִ�е�B,�ڶ����Ǵ�A����B
    if (isEndIns(tmpIns->raw)) {
        return addJmpLink(fromAddr, toAddr);
    }
    else {
        return addNormalLink(fromAddr, toAddr);
    }
    return true;
}

void VmpTraceFlowGraph::AddTraceFlow(const std::vector<reg_context>& traceList)
{
    if (traceList.size() <= 1) {
        return;
    }
    for (unsigned int n = 0; n < traceList.size() - 1; n++) {
        if (!addLink(traceList[n].EIP, traceList[n + 1].EIP)) {
            return;
        }
    }
}

void VmpTraceFlowGraph::MergeAllNodes()
{
    //��ȷ���޷��ϲ��Ľڵ�
    std::set<size_t> badNodeList;
    bool bUpdateNode;
    do
    {
        bUpdateNode = false;
        std::map<size_t, VmpTraceFlowNode>::iterator it = nodeMap.begin();
        while (it != nodeMap.end()) {
            size_t nodeAddr = it->first;
            if (badNodeList.count(nodeAddr)) {
                it++;
                continue;
            }
            if (checkCanMerge(nodeAddr)) {
                size_t fromAddr = *toEdges[nodeAddr].begin();
                VmpTraceFlowNode* fatherNode = instructionToNodeMap[fromAddr].vmNode;
                if (checkCanMerge_Vmp(nodeAddr)) {
                    executeMerge(fatherNode, &it->second);
                    bUpdateNode = true;
                    it = nodeMap.erase(it);
                    continue;
                }
            }
            badNodeList.insert(nodeAddr);
            it++;
        }
    } while (bUpdateNode);
}

void VmpTraceFlowGraph::DumpGraph(std::ostream& ss, bool bCompress)
{
    ss << "strict digraph \"hello world\"{\n";
    for (std::map<size_t, VmpTraceFlowNode>::iterator it = nodeMap.begin(); it != nodeMap.end(); ++it) {
        VmpTraceFlowNode& node = it->second;
        ss << "\"" << std::hex << it->first << "\"[label=\"";
        for (unsigned int n = 0; n < node.addrList.size(); ++n) {
            if (bCompress) {
                if (n > 20 && (n != node.addrList.size() - 1)) {
                    continue;
                }
            }
            std::unique_ptr<RawInstruction> tmpIns = DisasmManager::Main().DecodeInstruction(node.addrList[n]);
            if (tmpIns) {
                ss << std::hex << node.addrList[n] << "\t" << tmpIns->raw->mnemonic << " " << tmpIns->raw->op_str << "\\n";
            }
            else {
                ss << std::hex << node.addrList[n] << "\t" << "invalid instruction" << "\\n";
            }
        }
        ss << "\"];\n";
    }
    for (std::map<size_t, std::unordered_set<size_t>>::iterator it = fromEdges.begin(); it != fromEdges.end(); ++it) {
        std::unordered_set<size_t>& edgeList = it->second;
        for (std::unordered_set<size_t>::iterator edegIt = edgeList.begin(); edegIt != edgeList.end(); ++edegIt) {
            VmpTraceFlowNode* fromBlock = instructionToNodeMap[it->first].vmNode;
            ss << "\"" << std::hex << fromBlock->nodeEntry << "\" -> ";
            ss << "\"" << std::hex << *edegIt << "\";\n";
        }
    }
    ss << "\n}";
    return;
}

#ifdef DeveloperMode
#pragma optimize("", on) 
#endif