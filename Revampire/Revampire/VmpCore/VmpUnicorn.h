#pragma once
#include <vector>
#include <memory>
#include <unicorn/unicorn.h>
#include "../Helper/UnicornHelper.h"

class VmpUnicorn
{
public:
    VmpUnicorn();
    ~VmpUnicorn();
public:
    //����unicorn
    bool reset(const VmpUnicornContext& ctx);
    //�������е�unicorn��Դ
    void clear();
    //��ʼ��unicorn��Դ
    bool init();
    //����ڿ�ʼ,���ڽ���,��ȡһ��������vmp����
    std::vector<reg_context> StartVmpTrace(const VmpUnicornContext& ctx, size_t count);
    bool ContinueVmpTrace(const VmpUnicornContext& ctx, size_t count);
    std::unique_ptr<VmpUnicornContext> CopyCurrentUnicornContext();
private:
    //�����޸���ջ
    bool fixStack();
    //��ȡ��һ��ָ���ַ
    size_t getNextInsAddr(size_t addr);
    bool fillStack(const VmpUnicornContext& ctx);
    bool fillMemoryMap();
    bool fillRegister(const VmpUnicornContext& ctx);
protected:
    static void cb_hook_code(uc_engine* uc, uint64_t address, uint32_t size, void* user_data);
    static void cb_hook_mem(uc_engine* uc, uc_mem_type type, uint64_t address, int size, int64_t value, void* user_data);
private:
    uc_struct* uc = nullptr;
    uc_hook hook_code = 0x0;
    uc_hook hook_mem = 0x0;
    //���ٽ��
    std::vector<reg_context> traceList;
    reg_context tmpContext;
    size_t stackCodeBase;
    std::vector<unsigned char> stackBuffer;
    //�Ƿ����
    bool bContinue;
};