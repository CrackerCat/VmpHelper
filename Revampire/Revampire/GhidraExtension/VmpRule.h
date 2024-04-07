#pragma once
#include "../Ghidra/action.hh"

namespace ghidra
{
    //�����ǽ��ڴ����ֱ��ת��Ϊ��������
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


}