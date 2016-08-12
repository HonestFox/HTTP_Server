/*************************************************************************
	> File Name: test.cpp
	> Author: HonestFox
	> Mail: zhweizhi@foxmail.com 
	> Created Time: Fri 12 Aug 2016 11:25:45 PM CST
 ************************************************************************/

#include<iostream>
#include"BigData.h"
using namespace std;
int main()
{
	BigData data1("1");
	BigData data2("2");
	BigData data3 = data1 + data2;
	cout << data1 << " + " << data2 << " = " << data3 << endl;
	return 0;
}
