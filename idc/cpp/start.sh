####################################################################
# 启动数据中心后台服务程序的脚本。
####################################################################

# 检查服务程序是否超时，配置在/etc/rc.local中由root用户执行。
#/project/tools/bin/procctl 30 /project/tools/bin/checkproc

# 压缩数据中心后台服务程序的备份日志。
/project/tools/bin/procctl 300 /project/tools/bin/gzipfiles /logs/idc "*.log.20*" 0.01

# 生成用于测试的全国气象站点观测的分钟数据。
/project/tools/bin/procctl 60 /project/idc/bin/crtsurfdata /project/idc/ini/stcode.ini /tmp/idc/surfdata /logs/idc/crtsurfdata.log xml,json,csv

# 清理原始的全国气象站点观测的分钟数据目录/tmp/idc/surfdata中的历史数据文件。
/project/tools/bin/procctl 300 /project/tools/bin/deletefiles /tmp/idc/surfdata "*" 0.02

# 假设站点数据是会改变的，那么就需要每隔一定时间来更新
/project/tools/bin/procctl 120 /project/idc/bin/obtcodetodb /project/idc/ini/stcode.ini "192.168.0.74,qxidc,qxidcpwd,db_qxidc,3306" utf8 /logs/idc/obtcodetodb.log

# 将站点观测数据入库，只有插入没有更新操作
/project/tools/bin/procctl 120 /project/idc/bin/obtmindtodb /tmp/idc/surfdata "192.168.0.74,qxidc,qxidcpwd,db_qxidc,3306" utf8 /logs/idc/obtmindtodb.log

# 定期清理入库的观测数据
/project/tools/bin/procctl 120 /project/tools/bin/execsql /project/idc/sql/cleardata.sql "192.168.0.74,qxidc,qxidcpwd,db_qxidc,3306" utf8 /logs/tools/execsql.log
