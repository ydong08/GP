#!/bin/bash

#!/bin/bash

now_date=`date +%Y%m%d`
port=$1
config_dir=`redis-cli -p $port config get dir`
config_dir=`echo $config_dir | awk '{print $2}'`
redis-cli -p $port save
mv $config_dir/dump.rdb $config_dir/dump_${port}_${now_date}.rdb


cd $config_dir
#复制到另外一台服务器上
echo `pwd`
ftpserver=$2
ftpuser=$3
ftppassword=$4
remotedir="~/"
filename=dump_${port}_${now_date}.rdb

ftp -in << EOM  
    open $ftpserver 21
    user $ftpuser $ftppassword
    bin  
    cd $remotedir
    put $filename  
    bye  
EOM

