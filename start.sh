#########################################################################
# File Name: start.sh
# Author: HonestFox
# mail: zhweizhi@foxmail.com
# Created Time: Wed 10 Aug 2016 10:35:22 AM CST
#########################################################################
#!/bin/bash
make clean;
make
./httpd 127.0.0.1 8080
