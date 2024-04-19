#pragma once
#include "../Ghidra/action.hh"

namespace ghidra
{
    //�����ǽ�ֻ���ڴ����ֱ��ת��Ϊ����
    //����a = *[0x401000] => a = 0x40000
    class RuleVmpLoadConst : public Rule
    {
    public:
        RuleVmpLoadConst(const string& g) : Rule(g, 0, "vmploadconst") {}	///< Constructor
        virtual Rule* clone(const ActionGroupList& grouplist) const {
            if (!grouplist.contains(getGroup())) return (Rule*)0;
            return new RuleVmpLoadConst(getGroup());
        }
        virtual void getOpList(vector<uint4>& oplist) const;
        virtual int4 applyOp(PcodeOp* op, Funcdata& data);
    };

    //���Ƴ���ջ
    class RuleVmpEarlyRemoval : public Rule {
    public:
        RuleVmpEarlyRemoval(const string& g, AddrSpace* ss) : Rule(g, 0, "vmpearlyremoval") { stackspace = ss; }	///< Constructor
        virtual Rule* clone(const ActionGroupList& grouplist) const {
            if (!grouplist.contains(getGroup())) return (Rule*)0;
            return new RuleVmpEarlyRemoval(getGroup(), stackspace);
        }
        // This rule applies to all ops
        virtual int4 applyOp(PcodeOp* op, Funcdata& data);
    private:
        AddrSpace* stackspace;
    };
}