#########################################################################
# File Name: start.sh
# Author: HonestFox
# mail: zhweizhi@foxmail.com
# Created Time: Wed 10 Aug 2016 10:35:22 AM CST
#########################################################################
#!/bin/bash
ROOT_PATH=$(pwd)
export LD_LIBRARY_PATH=$ROOT_PATH/lib; .httpd 127.0.0.1 8080
