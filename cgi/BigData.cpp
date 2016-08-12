#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "RSA.h"
#include "BigData.h"
//#include "PrimeNumList.h"

BigData::BigData(INT32 value)
	:_value(value)
{
	//�Ȼ�ȡvalue��λ��
	int sz = _GetSize(value);
	_strData.resize(sz + 1);
	_strData[0] = '+';
	if (value < 0)
	{
		_strData[0] = '-';
	}
	char *StrTmp = new char[sz + 2];
	sprintf(StrTmp+1, "%ld", value );
	StrTmp[sz + 1] = '\0';


	if (value > 0)
	{
		for (int Idx = 1; Idx <= sz; Idx++)
		{
			_strData[Idx] = StrTmp[Idx - 1];
		}
	}
	else
	{
		for (int Idx = 1; Idx <= sz; Idx++)
		{
			_strData[Idx] = StrTmp[Idx];
		}
	}
	delete []  StrTmp;
}

BigData::BigData(const char *pData)
{
	assert(pData);
	char cSymbol = pData[0];
	if (cSymbol == '0' && strlen(pData) == 1)
	{
		_value = 0;
		_strData.resize(1);
		_strData[0] = '+';
		return;
	}
	//�����һλ
	if (cSymbol >= '0' && cSymbol <= '9')
	{
		cSymbol = '+';
	}
	else if(cSymbol == '+' || cSymbol == '-' )
	{
		++pData;
	}
	else
	{
		_value = UN_INIT;
		return;
	}
	//ȥ��ǰ��� ��0��
	while (*pData == '0')
	{
		++pData;
	}
	//ת��
	_strData.resize(strlen(pData) + 1);
	_strData[0] = '+';
	int iCount = 1;
	_value = 0;
	while (*pData <= '9' && *pData >= '0')
	{
		_strData[iCount++] = *pData;
		_value = _value * 10 + *pData - '0';
		pData++;
	}
	if (*pData != '\0')
	{
		_strData.resize(iCount + 1);
	}
	if (cSymbol == '-')
	{
		_value = 0 - _value;
		_strData[0] = '-';
	}
}

bool BigData::IsINT32Overflow() const
{
	string temp("+2147483647");
	if ('-' == _strData[0])
	{
		temp = "-2147483647";
	}

	if (_strData.size() < temp.size())
	{
		return false;  //û����� ������С��
	}
	else if (_strData.size() == temp.size() && _strData <= temp)
	{
		return false;	//û�������������ȣ����ڷ�Χ�ڣ�
	}
	return true;
}

ostream & operator<<(ostream & os, BigData & bd)
{
	if (!bd.IsINT32Overflow())
	{
		os << bd._value;
	}
	else
	{
		char *pData = (char*)bd._strData.c_str();
		if ('+' == bd._strData[0])
		{
			pData++;
		}
		os << pData;
	}
	return os;
}


/*
	�ӷ�
	�������ͣ�
		��ӵĽ��û�г�����Χ
		��ӵĽ��������Χ
	�����������ͣ�

*/
BigData BigData::operator+(const BigData & bigData)
{
	if (!IsINT32Overflow() && !bigData.IsINT32Overflow())
	{
		if (_strData[0] != bigData._strData[0])		//����λ��ͬ�������ֱ�����㣬�϶����������
		{
			return BigData(_value + bigData._value);
		}
		else             //else���洦�������ͬ�����
		{ 
			// -10         -7 + -1             -10 - -7 
			if(
				(_strData[0] == '+' && (MAX_INT32 - _value) >= bigData._value)
			||( _strData[0] == '-' &&   (MIN_INT32 - _value) <= bigData._value)
				)			//��֤�ڷ�Χ�ڣ�ֱ������
			{
				int val = _value + bigData._value;
				return BigData(val);
			}
		}
	}
	//������������һ�����
	//����ӵĽ�����
	if (_strData[0] == bigData._strData[0] )
	{
		return BigData((char*)Add(_strData, bigData._strData).c_str());
	}
	//�����෴
	return BigData((char*)Sub(_strData, bigData._strData).c_str());
}

//ֻҪ����Add����һ��������ͬ
string BigData::Add(string left, string right)
{
	int LSize = left.size();
	int RSize = right.size();
	char cStep = 0;  //��λ
	if (LSize < RSize)	//��֤�������
	{
		swap(left, right);
		swap(LSize, RSize);
	}
	string strRet;
	strRet.resize(LSize + 1);
	strRet[0] = left[0];
	//�ӵ�λ����λ����λ���
	for (int index = 1; index < LSize; index++)
	{
		char cRet = left[LSize - index] + cStep - '0';
		if (RSize - index > 0)
		{
			cRet += right[RSize - index] - '0';
		}
		strRet[LSize - index + 1] = cRet % 10 + '0';  //���ǵ�����λ������±���Ҫ+1
		cStep = cRet / 10;
	}
	strRet[1] = cStep + '0';
	cout << "ADD" << endl;
	cout << strRet << endl;
	return strRet;
}

/*
	����
	����������ݶ�û�����
		ͬ�ţ�ֱ�����
		��ţ�
	�������������������һ�����ݳ�������������������
*/
BigData BigData::operator-(const BigData & bigData)
{
	if (!IsINT32Overflow() && !bigData.IsINT32Overflow() )
	{
		if (_strData[0] == bigData._strData[0])			//���ڷ�Χ�ڣ�������ͬ
		{
			return BigData(_value - bigData._value);
		}
		else                     //���ڷ�Χ�ڣ������Ų�ͬ
		{
			if (				//  ��֤�ڷ�Χ�ڵ����															
				(_strData[0] == '+' && (MAX_INT32 - _value) >= (bigData._value * -1) )
				|| (_strData[0] == '-' && (MIN_INT32 - _value) <= (bigData._value * -1) )
				)
			{
				return BigData(_value - bigData._value);
			}
			else    //�����ڷ�Χ������
			{
				return BigData((char*)Sub(_strData, bigData._strData).c_str());
			}
		}
	}
	else if (_strData[0] != bigData._strData[0])	//һ���ڷ�Χ������
	{
		return BigData((char*)Sub(_strData, bigData._strData).c_str() );
	}
	return BigData((char*)Sub(_strData, bigData._strData).c_str());

}

string BigData::Sub(string left, string right)
{
	int LSize = left.size();
	int RSize = right.size();
	char cSymbol = left[0];

	string strRet;
	strRet.resize(left.size());
	strRet[0] = cSymbol;
	
	//��������෴������Է���λ���ӷ���Ȼ�������Ϸ���λ�������ķ���λ��
	if (left[0] != right[0])
	{
		right[0] = cSymbol;
		strRet = Add(left, right);
		return strRet;
	}

	if (LSize < RSize || LSize == RSize  && left < right)
	{
		swap(left, right);				//������
		swap(LSize, RSize);
		if (left[0] == '+')
		{
			if (cSymbol = '+')
			{
				cSymbol = '-';				//����ÿ�η��Ŷ�һ���Ǹ���
			}
			else
			{
				cSymbol = '+';
			}
		}
		else
		{
			cSymbol = '+';
		}
	}

	//��λ������
	for (int iIdx = 1; iIdx < LSize; iIdx++)
	{
		char cRet = left[LSize - iIdx] - '0';
		if (RSize - iIdx > 0)
		{
			cRet -= right[RSize - iIdx] - '0';
		}
		//��λ
		if (cRet < 0)
		{
			left[LSize - iIdx - 1]--;
			cRet += 10;
		}
		strRet[LSize - iIdx] = cRet + '0';
	}
	strRet[0] = cSymbol;
	return strRet;
}


/*
		�˷�
		��������û�г������Ҳû�г�����
			ֱ�ӳˣ�ͬ����ŵ����⣩
		������һ�����������߽��������
			Mul()
			���������⡿�����������ر��ʱ����������
*/
BigData BigData::operator*(const BigData & bigData)
{
	if (!IsINT32Overflow() && !bigData.IsINT32Overflow() )
	{
		cout << " ??????????????" << endl;
		//���������������һ��Ϊ0 ֱ�ӷ���0  
		if (_value == 0 || bigData._value == 0)
		{
			return BigData("0");
		}

		if (_strData[0] == bigData._strData[0] )	//���������ͬ
		{
			if (
				_strData[0] == '+'  && MAX_INT32 / _value >= bigData._value				
				|| _strData[0] == '-' && MIN_INT32 / _value <= bigData._value
				)
			{
				cout << "***********" << endl;
				cout << _value << endl;
				cout << bigData._value << endl;
				return BigData(_value * bigData._value);
			}
		}
		else    //���
		{
			cout << "else !!!" << endl;
			if (_strData[0] == '+'  && MAX_INT32 / _value >= (bigData._value * -1)				
				|| _strData[0] == '-' && MIN_INT32 / _value >= bigData._value
				)
			{
				return BigData(_value * bigData._value);
			}
		}
	}
	if (_strData[1] == '0' || bigData._strData[1] == '0')
	{
		return BigData("0");
	}
	return BigData((char*)(Mul(_strData, bigData._strData).c_str()));
}

BigData BigData::operator/(const BigData & bigData)
{
	if (bigData._strData[1] == '0')
	{
		assert(false);
	}
	if (!IsINT32Overflow() && !bigData.IsINT32Overflow())			//��Χ�� ֱ������
	{
		return BigData(_value / bigData._value);
	}

	//��Χ��
	if (_strData.size() < bigData._strData.size() ||				//����Ϊ0  ֱ�ӷ���0
		_strData.size() == bigData._strData.size() && strcmp(_strData.c_str() + 1, bigData._strData.c_str() + 1) < 0 )
	{
		return BigData("0");
	}
	if (bigData._strData == "+1" || bigData._strData == "-1")	    //����Ϊ +- 1
	{
		string ret = _strData;
		if (_strData[0] != bigData._strData[0] )//���
		{
			ret[0] = '-';
		}
		return BigData((char*)ret.c_str() );
	}																											//������������ֵ��ȵ����
	if (_strData.size() == bigData._strData.size() &&
		strcmp(_strData.c_str() + 1, bigData._strData.c_str() + 1) == 0)
	{
		string ret = "+1";
		if (_strData[0] != bigData._strData[0])//���
		{
			ret[0] = '-';
		}
		return BigData((char*)ret.c_str());
	}
	
	return BigData( (char*) (Div(_strData, bigData._strData).c_str()) );
}

BigData BigData::operator%(const BigData & bigData)
{	
	return BigData(*this - ((*this / bigData) * bigData));
}

BigData BigData::operator^(const BigData & bigData)
{
	BigData ret(1);
	BigData count("0");
//	cout << "Ŀ�꣺ " << bigData._strData << endl;
	while (count++ != bigData)
	{
//		cout << "��ǰ�� " << count << " " << flush;
		ret = ret * (*this);
	}
//	cout << endl;
	return ret;
}



BigData BigData::operator++()
{
	*this = *this + 1;
	return *this;
}

BigData BigData::operator++(int)
{
	BigData tmp = *this;
	*this = *this + 1;
	return tmp;
}

BigData BigData::operator--()
{
	*this = *this - 1;
	return *this;
}

BigData BigData::operator--(int)
{
	BigData tmp = *this;
	*this = *this - 1;
	return tmp;
}

bool BigData::operator==(const BigData & bigData)
{
	int idx1 = 0;
	int idx2 = 0;
	string s1 = this->_strData;
	string s2 = bigData._strData;
	if (s1[0] == '+' && s2[0] == '-' || s1[0] == '-' && s2[0] == '+')
	{
		return false;
	}
	else if (s1[0] == '-' && (s2[0] >= '0' && s2[0] <= '9'))
	{
		return false;
	}
	if (s1[0] == '+' || s1[0] == '-')
	{
		++idx1;
	}
	if (s1[0] == '+' || s2[0] == '-')
	{
		++idx2;
	}
	while (s1[idx1] == s2[idx2] )
	{
		if (s1[idx1] == '\0')
		{
			return true;
		}
		++idx1;
		++idx2;
	}
	return false;
}

bool BigData::operator!=(const BigData & bigData)
{
	return!( (*this) == bigData ) ;
}

string BigData::Mul(string left, string right)
{
	int LSize = left.size();
	int RSize = right.size();
	if (LSize > RSize)
	{
		swap(left, right);
		swap(LSize, RSize);
	}
	char cSymbol = '+';
	string strRet;
	strRet.resize(LSize + RSize - 1);
	if (left[0] != right[0])
	{
		cSymbol = '-';
	}
	strRet[0] = cSymbol;

	char cStep = 0;		//��λ
	int  Datalen = strRet.size();
	int offset = 0;		//��λ
	for (int i = 1; i < Datalen; ++i)
	{
		strRet[i] += '0';
	}
	for (int iLidx = 1; iLidx < LSize; ++iLidx)
	{
		char cLeft = left[LSize - iLidx] - '0';
		cStep = 0;
		if (cLeft == 0)
		{
			offset++;
			continue;
		}
		for (int iRidx = 1; iRidx < RSize; ++iRidx)
		{
			char cRet = cLeft * (right[RSize - iRidx] - '0');
			//			cRet += (strRet[RSize - iRidx - offset] - '0');
			cRet += cStep;
			cStep = cRet / 10;
			strRet[Datalen - iRidx - offset] += (cRet % 10);
			while (strRet[Datalen - iRidx - offset] > '9')
			{
				++cStep;
				strRet[Datalen - iRidx - offset] -= 10;
			}
		}
		strRet[Datalen - RSize - offset] += cStep;
		++offset;
	}
	//ȥ��ǰ���0
	if (strRet[1] == '0')
	{
		strRet = BigData((char*)strRet.c_str())._strData;
	}
		return strRet;
}

/*
	������
		1�����rightΪ0��

		2�������û�г�����
			left = 0 ���� 0
			left < right   ���� 0
			right == +-1   ���� +-left
			left == +-right  ����	+-1
		
		3������г�����
		Div 

		ѭ�������
		��֤pLeft >= ����  

*/


string BigData::Div(string left, string right)   //�Ż�������
{
	string sRet;
	sRet.append(1, '+');   //��һλ׷������
	if (left[0] != right[0])   //���
	{
		sRet[0] = '-';
	}
	char *pLeft = (char*)(left.c_str() ) + 1;
	char *pRight = (char*)(right.c_str() + 1);
	int Datalen = right.size() - 1;
	int LSize = left.size() - 1;
	for (int iIdx = 0; iIdx < LSize; )
	{
		if (*pLeft == '0')				//������ǰλΪ0�������̺���׷��0 �������λ
		{
			sRet.append(1, '0');
			pLeft++;
			iIdx++;
			continue;
		}
		if (!IsLeftBig(pLeft, Datalen, pRight, right.size() - 1))				//����С�Ļ������̵ĵ�ǰλ�� 0�� �������λ
		{
			sRet.append(1, '0');
			Datalen++;
			if (Datalen + iIdx > LSize)
			{
				break;
			}
		}
		else      //������
		{
			sRet.append(1, SubLoop(pLeft, Datalen, pRight, right.size() - 1) );			//���Կ��������ô���
			while (*pLeft == '0' && Datalen > 0)
			{
				pLeft++;
				iIdx++;
				Datalen--;
			}
			Datalen++;
			if (Datalen + iIdx > LSize)
			{
				break;
			}
		}

	}
	return sRet;
}


char BigData::SubLoop(char* pLeft, int LSize, char *pRight, int RSize)  //����
{
	assert(pLeft != NULL && pRight != NULL);
	char cRet = '0';
	while (true)
	{
		if (!IsLeftBig(pLeft, LSize, pRight, RSize))
		{
			break;
		}
		//������
		int LDataLen = LSize -1;
		int RDataLen = RSize - 1;
		while (LDataLen >= 0 && RDataLen >= 0)
		{
			char ret = pLeft[LDataLen] - '0';
			ret -= pRight[RDataLen] - '0';
			if (ret < 0)
			{
				pLeft[LDataLen - 1] -= 1;
				ret += 10;
			}
			pLeft[LDataLen] = ret + '0';
			LDataLen--;
			RDataLen--;
		}

		//ȥ��ǰ��0
		//990000990000000000000000000 / 33
		while (*pLeft == '0' && LSize > 0)
		{
			pLeft++;
			LSize--;
		}
		cRet++;
	}
	return cRet;
}

bool BigData::IsLeftBig(char * pLeft, int LSize, char * pRight, int RSize)
{
	if (LSize > RSize || LSize == RSize && strcmp(pLeft, pRight) >= 0)
	{
		return true;
	}
	return false;
}

size_t BigData::_GetSize(INT32 value)
{
	int ret = 1;
	while (value >= 10 || value <= -10)
	{
		value /= 10;
		++ret;
	}
	return ret;
}


//int���͵�����
//RSA �����㷨
