#!/bin/bash

now_date=`date +%Y%m%d`
mysqldump -uroot -p$1 --all-databases | gzip > /tmp/database_${now_date}.sql.gz

cd /tmp
#复制到另外一台服务器上
ftpserver=$2
ftpuser=$3
ftppassword=$4
remotedir="~/"  
filename=database_${now_date}.sql.gz  
  
ftp -in << EOM  
	open $ftpserver 21
    user $ftpuser $ftppassword
    bin  
	cd $remotedir    
	put $filename  
	bye  
EOM
