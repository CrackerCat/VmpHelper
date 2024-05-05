#pragma once
#include <string>
#include <vector>

struct SegmentInfomation
{
	size_t segStart;					  //������ʼ��ַ
	size_t segSize;						  //���δ�С
	std::string segName;                  //��������
	std::vector<unsigned char> segData;   //��������
};

class SectionManager
{
public:
	SectionManager();
	static SectionManager& Main();
	bool InitSectionManager();
	//���Ե�ַת��Ϊ�����ַ
	unsigned char* LinearAddrToVirtualAddr(size_t LinerAddr);
	//�жϵ�ǰ��ַ���ĸ�����
	int SectionIndex(size_t addr);
public:
	std::vector<SegmentInfomation> segList;
	//�洢��һ�����е�����,���ڼ��ٷ���
	int saveIndex = 0x0;
};