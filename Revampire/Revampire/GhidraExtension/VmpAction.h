#pragma once
#include "../Ghidra/action.hh"

namespace ghidra
{
    //���handler���Ƶ�����������
    class ActionVmpHandlerDeadCode : public Action {
    public:
        ActionVmpHandlerDeadCode(const string& g) : Action(0, "handlerdeadcode", g) {}	///< Constructor
        virtual Action* clone(const ActionGroupList& grouplist) const {
            if (!grouplist.contains(getGroup())) return (Action*)0;
            return new ActionVmpHandlerDeadCode(getGroup());
        }
        virtual int4 apply(Funcdata& data);
    };
}