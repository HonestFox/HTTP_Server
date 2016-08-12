#pragma once
#include <iostream>
#include <sys/types.h>
using namespace std;

typedef int INT32;
#define UN_INIT	0xCCCCCCCC             //һ��ʼ�����ֵ
#define MAX_INT32 0x7FFFFFFF
#define MIN_INT32 0x80000000

class BigData
{
	friend class RSA;    //RSA���Է���BigData�����г�Ա
public:
	BigData(INT32 value = UN_INIT);
	BigData(const char *pData );

	BigData operator+ (const BigData &bigData);
	BigData operator- (const BigData &bigData);
	BigData operator* (const BigData &bigData);
	BigData operator/ (const BigData &bigData);
	BigData operator% (const BigData &bigData);
	BigData operator^ (const BigData &bigData);

	//
	BigData operator++ ();		//ǰ��
	BigData operator++ (int);	//����
	BigData operator-- ();			//ǰ��
	BigData operator-- (int);		//����


	bool operator== (const BigData &bigData); 
	bool operator!= (const BigData &bigData); 





	string Add(string left, string right);
	string Sub(string left, string right);
	string Mul(string left, string right);
	string Div(string left, string right);
	bool IsINT32Overflow() const;
	friend ostream &operator<<(ostream &os, BigData &bd);

	
protected:
	char SubLoop(char* pLeft, int LSize, char *pRight, int RSize);
	bool IsLeftBig(char* pLeft, int LSize, char *pRight, int RSize);
	size_t _GetSize(INT32 value);



//protected:
public:
	INT32 _value;
	string _strData;
};
