ulimit -n 65535
sleep 5
cd /home/mo/HttpSvr/bin/
pidHttpSvr=`ps -ef | grep -v "run" | grep "HttpSvr$" | grep -v "grep" | grep -v "ps -ef" | awk '{print $2}'`
if [ -z $pidHttpSvr ]; then
    sh ./restart.sh
    echo "started HttpSvr"
fi
echo "HttpSvr pid:" ${pidHttpSvr}
HttpSvrfdnum=`ls -l /proc/${pidHttpSvr}/fd |wc -l`
if [ $HttpSvrfdnum -gt 10000 ]; then
    killall -9 HttpSvr;
    sleep 2
    sh ./restart.sh
fi
