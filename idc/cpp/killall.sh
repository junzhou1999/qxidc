####################################################################
# 停止数据中心后台服务程序的脚本。
####################################################################

killall -9 procctl
killall gzipfiles crtsurfdata deletefiles obtcodetodb

sleep 3

killall -9 gzipfiles crtsurfdata deletefiles obtcodetodb
