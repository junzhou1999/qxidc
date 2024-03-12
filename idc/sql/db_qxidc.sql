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



drop table if exists T_ZHOBTMIND;

/*==============================================================*/
/* Table: T_ZHOBTMIND                                           */
/*==============================================================*/
create table T_ZHOBTMIND
(
    obtid     varchar(10) not null comment '站点代码。',
    ddatetime datetime    not null comment '数据时间，精确到分钟',
    t         int comment '温度，单位：0.1摄氏度。',
    p         int comment '气压：单位：0.1千帕。',
    u         int comment '相对湿度，0-100之间的值。',
    wd        int comment '风向，0-360之间的值。',
    wf        int comment '风速，单位：0.1m/s。',
    r         int comment '降雨量，单位：0.1mm。',
    vis       int comment '能见度，单位：0.1米。',
    upttime   timestamp   not null comment '更新时间。',
    keyid     bigint      not null auto_increment comment '记录编号，自动增长列。',
    primary key (obtid, ddatetime),
    key       ZHOBTMIND_KEYID (keyid)
);

alter table T_ZHOBTMIND comment '本表存放了设备观测到的全国气象站点分钟观测数据。';

/*==============================================================*/
/* Index: IDX_ZHOBTMIND_1                                       */
/*==============================================================*/
create unique index IDX_ZHOBTMIND_1 on T_ZHOBTMIND
    (
     ddatetime,
     obtid
        );

/*==============================================================*/
/* Index: IDX_ZHOBTMIND_2                                       */
/*==============================================================*/
create index IDX_ZHOBTMIND_2 on T_ZHOBTMIND
    (
     ddatetime
        );

/*==============================================================*/
/* Index: IDX_ZHOBTMIND_3                                       */
/*==============================================================*/
create index IDX_ZHOBTMIND_3 on T_ZHOBTMIND
    (
     obtid
        );

alter table T_ZHOBTMIND
    add constraint FK_ZHOBTMIND_ZHOBTCODE foreign key (obtid)
        references T_ZHOBTCODE (obtid) on delete restrict on update restrict;

