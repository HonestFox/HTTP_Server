/*************************************************************************
	> File Name: sql_api.h
	> Author: HonestFox
	> Mail: zhweizhi@foxmail.com 
	> Created Time: Sat 13 Aug 2016 09:55:00 AM CST
 ************************************************************************/
#pragma once
#include <iostream>
#include <string>
#include <stdlib.h>
#include "mysql.h"

class sql_api
{
public:
//	sql_api( const std::string &host , const std::string &user , const std::string &passwd , const std::string &db );
	sql_api();
	~sql_api();
public:
	int my_connect_mysql();
	int my_insert( const std::string &cols, const std::string &data );
	int my_select();
	int my_delete( const std::string &name );
	int my_update( const std::string &name, const std::string &tag, const std::string &val );
	int my_update( const std::string &name, const std::string &tag, const int &val );
	void test();
private:
	MYSQL *_conn;
	MYSQL_RES *_res;
	std::string _host;
	std::string _user;
	std::string _passwd;
	std::string _db;
	int _port;
};
