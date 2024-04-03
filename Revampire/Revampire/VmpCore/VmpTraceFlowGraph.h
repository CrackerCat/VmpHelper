#pragma once
#include <vector>
#include <map>
#include <sstream>
#include <set>
#include <unordered_set>
#include "../Helper/UnicornHelper.h"

struct VmpTraceFlowNode
{
public:
    //���������
    size_t nodeEntry;
    //ָ���б�
    std::vector<size_t> addrList;
    VmpTraceFlowNode() {
        nodeEntry = 0x0;
    }
public:
    size_t TryGetAddr(size_t index) {
        if (index >= addrList.size()) {
            return 0x0;
        }
        return addrList[index];
    }
    size_t EndAddr() {
        return addrList[addrList.size() - 1];
    }
};

struct VmpTraceFlowNodeIndex
{
    VmpTraceFlowNode* vmNode;
    int index;
    VmpTraceFlowNodeIndex() {
        vmNode = nullptr;
        index = -1;
    }
    VmpTraceFlowNodeIndex(VmpTraceFlowNode* node, int idx) {
        vmNode = node;
        index = idx;
    }
};

class VmpTraceFlowGraph
{
public:
    VmpTraceFlowGraph();
    ~VmpTraceFlowGraph();
public:
    void AddTraceFlow(const std::vector<reg_context>& traceList);
    void DumpGraph(std::ostream& ss, bool bCompress);
    //�Խڵ���кϲ��Ż�
    void MergeAllNodes();
private:
    void updateInstructionToNodeMap(size_t addr, VmpTraceFlowNode* updateNode, int index);
    bool addLink(size_t fromAddr, size_t toAddr);
    bool addNormalLink(size_t fromAddr, size_t toAddr);
    bool addJmpLink(size_t fromAddr, size_t toAddr);
    void removeEdge(size_t from, size_t to);
    void linkEdge(size_t from, size_t to);
    VmpTraceFlowNode* createNode(size_t start);
    VmpTraceFlowNode* splitBlock(VmpTraceFlowNode* toNode, size_t splitAddr);
    //�Ƿ���Ժϲ�
    bool checkCanMerge(size_t nodeAddr);
    bool checkMerge_Vmp300(size_t nodeAddr);
    //ִ�кϲ��߼�
    void executeMerge(VmpTraceFlowNode* fatherNode, VmpTraceFlowNode* childNode);
public:
    //key��block����ʼ��ַ,value��BasicBlock
    std::map<size_t, VmpTraceFlowNode> nodeMap;
    //key��ָ��ĵ�ַ,value��ָ��Block������Block������
    std::map<size_t, VmpTraceFlowNodeIndex> instructionToNodeMap;
    //key������ָ���ַ,value�Ǳ�����ָ���ַ
    std::map<size_t, std::unordered_set<size_t>> fromEdges;
    std::map<size_t, std::unordered_set<size_t>> toEdges;
};