drop table if exists T_ZHOBTCODE;

/*==============================================================*/
/* Table: T_ZHOBTCODE                                           */
/*==============================================================*/
create table T_ZHOBTCODE
(
    obtid    varchar(10) not null comment '站点代码。',
    cityname varchar(30) not null comment '城市名称。',
    provname varchar(30) not null comment '省名称。',
    lat      int         not null comment '纬度，单位：0.01度。',
    lon      int         not null comment '经度，单位：0.01度。',
    height   int comment '海拔高度，单位：0.1米。',
    upttime  timestamp   not null comment '更新时间，数据被插入或更新的时间。',
    keyid    int         not null auto_increment comment '记录编号，自动增长列。',
    primary key (obtid),
    unique key ZHOBTCODE_KEYID (keyid)
);

alter table T_ZHOBTCODE comment '这是一个参数表，存放全国的站点参数，约800条记录。本表的数据极少变更，应用程序对本表有insert和update权限。';
