/*
Navicat MySQL Data Transfer

Source Server         : 192.168.114.51
Source Server Version : 50620
Source Host           : 192.168.114.51:3306
Source Database       : logchip

Target Server Type    : MYSQL
Target Server Version : 50620
File Encoding         : 65001

Date: 2017-03-21 16:39:34
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for channel_stat
-- ----------------------------
DROP TABLE IF EXISTS `channel_stat`;
CREATE TABLE `channel_stat` (
  `id` int(20) unsigned NOT NULL AUTO_INCREMENT COMMENT '渠道商统计表',
  `key` varchar(64) NOT NULL DEFAULT '' COMMENT '渠道商key',
  `ip` bigint(20) NOT NULL DEFAULT '0' COMMENT 'ip',
  `device_no` varchar(128) NOT NULL DEFAULT '' COMMENT '设备号',
  `ctime` bigint(20) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_active1
-- ----------------------------
DROP TABLE IF EXISTS `log_active1`;
CREATE TABLE `log_active1` (
  `mid` int(11) NOT NULL,
  PRIMARY KEY (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_active2
-- ----------------------------
DROP TABLE IF EXISTS `log_active2`;
CREATE TABLE `log_active2` (
  `mid` int(11) NOT NULL,
  PRIMARY KEY (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_active3
-- ----------------------------
DROP TABLE IF EXISTS `log_active3`;
CREATE TABLE `log_active3` (
  `mid` int(11) NOT NULL,
  PRIMARY KEY (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_active4
-- ----------------------------
DROP TABLE IF EXISTS `log_active4`;
CREATE TABLE `log_active4` (
  `mid` int(11) NOT NULL,
  PRIMARY KEY (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_active5
-- ----------------------------
DROP TABLE IF EXISTS `log_active5`;
CREATE TABLE `log_active5` (
  `mid` int(11) NOT NULL,
  PRIMARY KEY (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_active6
-- ----------------------------
DROP TABLE IF EXISTS `log_active6`;
CREATE TABLE `log_active6` (
  `mid` int(11) NOT NULL,
  PRIMARY KEY (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_active7
-- ----------------------------
DROP TABLE IF EXISTS `log_active7`;
CREATE TABLE `log_active7` (
  `mid` int(11) NOT NULL,
  PRIMARY KEY (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_bank0
-- ----------------------------
DROP TABLE IF EXISTS `log_bank0`;
CREATE TABLE `log_bank0` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `mid` int(11) NOT NULL COMMENT '用户游戏ID',
  `type` int(11) NOT NULL COMMENT '操作类型 1存 2取  3转账',
  `amount` int(11) NOT NULL COMMENT '操作金额数',
  `money` bigint(20) NOT NULL COMMENT '操作后最新金币数',
  `freezemoney` bigint(20) NOT NULL COMMENT '操作后保险柜最新金币数',
  `tomid` int(11) NOT NULL DEFAULT '0' COMMENT '转账时，对方的mid',
  `ctime` int(11) NOT NULL COMMENT '操作时间',
  `gameid` int(11) NOT NULL DEFAULT '2',
  `productid` int(11) NOT NULL DEFAULT '1' COMMENT '产品id',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_bank1
-- ----------------------------
DROP TABLE IF EXISTS `log_bank1`;
CREATE TABLE `log_bank1` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `mid` int(11) NOT NULL COMMENT '用户游戏ID',
  `type` int(11) NOT NULL COMMENT '操作类型 1存 2取  3转账',
  `amount` int(11) NOT NULL COMMENT '操作金额数',
  `money` bigint(20) NOT NULL COMMENT '操作后最新金币数',
  `freezemoney` bigint(20) NOT NULL COMMENT '操作后保险柜最新金币数',
  `tomid` int(11) NOT NULL DEFAULT '0' COMMENT '转账时，对方的mid',
  `ctime` int(11) NOT NULL COMMENT '操作时间',
  `gameid` int(11) NOT NULL DEFAULT '1',
  `productid` int(11) NOT NULL DEFAULT '1' COMMENT '产品id',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_bank2
-- ----------------------------
DROP TABLE IF EXISTS `log_bank2`;
CREATE TABLE `log_bank2` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `mid` int(11) NOT NULL COMMENT '用户游戏ID',
  `type` int(11) NOT NULL COMMENT '操作类型 1存 2取  3转账',
  `amount` int(11) NOT NULL COMMENT '操作金额数',
  `money` bigint(20) NOT NULL COMMENT '操作后最新金币数',
  `freezemoney` bigint(20) NOT NULL COMMENT '操作后保险柜最新金币数',
  `tomid` int(11) NOT NULL DEFAULT '0' COMMENT '转账时，对方的mid',
  `ctime` int(11) NOT NULL COMMENT '操作时间',
  `gameid` int(11) NOT NULL DEFAULT '2',
  `productid` int(11) NOT NULL DEFAULT '1' COMMENT '产品id',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_bank3
-- ----------------------------
DROP TABLE IF EXISTS `log_bank3`;
CREATE TABLE `log_bank3` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `mid` int(11) NOT NULL COMMENT '用户游戏ID',
  `type` int(11) NOT NULL COMMENT '操作类型 1存 2取  3转账',
  `amount` int(11) NOT NULL COMMENT '操作金额数',
  `money` bigint(20) NOT NULL COMMENT '操作后最新金币数',
  `freezemoney` bigint(20) NOT NULL COMMENT '操作后保险柜最新金币数',
  `tomid` int(11) NOT NULL DEFAULT '0' COMMENT '转账时，对方的mid',
  `ctime` int(11) NOT NULL COMMENT '操作时间',
  `gameid` int(11) NOT NULL DEFAULT '3',
  `productid` int(11) NOT NULL DEFAULT '1' COMMENT '产品id',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_bank4
-- ----------------------------
DROP TABLE IF EXISTS `log_bank4`;
CREATE TABLE `log_bank4` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `mid` int(11) NOT NULL COMMENT '用户游戏ID',
  `type` int(11) NOT NULL COMMENT '操作类型 1存 2取  3转账',
  `amount` int(11) NOT NULL COMMENT '操作金额数',
  `money` bigint(20) NOT NULL COMMENT '操作后最新金币数',
  `freezemoney` bigint(20) NOT NULL COMMENT '操作后保险柜最新金币数',
  `tomid` int(11) NOT NULL DEFAULT '0' COMMENT '转账时，对方的mid',
  `ctime` int(11) NOT NULL COMMENT '操作时间',
  `gameid` int(11) NOT NULL DEFAULT '4',
  `productid` int(11) NOT NULL DEFAULT '1' COMMENT '产品id',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_bank5
-- ----------------------------
DROP TABLE IF EXISTS `log_bank5`;
CREATE TABLE `log_bank5` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `mid` int(11) NOT NULL COMMENT '用户游戏ID',
  `type` int(11) NOT NULL COMMENT '操作类型 1存 2取  3转账',
  `amount` int(11) NOT NULL COMMENT '操作金额数',
  `money` bigint(20) NOT NULL COMMENT '操作后最新金币数',
  `freezemoney` bigint(20) NOT NULL COMMENT '操作后保险柜最新金币数',
  `tomid` int(11) NOT NULL DEFAULT '0' COMMENT '转账时，对方的mid',
  `ctime` int(11) NOT NULL COMMENT '操作时间',
  `gameid` int(11) NOT NULL DEFAULT '4',
  `productid` int(11) NOT NULL DEFAULT '1' COMMENT '产品id',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=11 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_bank6
-- ----------------------------
DROP TABLE IF EXISTS `log_bank6`;
CREATE TABLE `log_bank6` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `mid` int(11) NOT NULL COMMENT '用户游戏ID',
  `type` int(11) NOT NULL COMMENT '操作类型 1存 2取  3转账',
  `amount` int(11) NOT NULL COMMENT '操作金额数',
  `money` bigint(20) NOT NULL COMMENT '操作后最新金币数',
  `freezemoney` bigint(20) NOT NULL COMMENT '操作后保险柜最新金币数',
  `tomid` int(11) NOT NULL DEFAULT '0' COMMENT '转账时，对方的mid',
  `ctime` int(11) NOT NULL COMMENT '操作时间',
  `gameid` int(11) NOT NULL DEFAULT '6',
  `productid` int(11) NOT NULL DEFAULT '1' COMMENT '产品id',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=196 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_bank7
-- ----------------------------
DROP TABLE IF EXISTS `log_bank7`;
CREATE TABLE `log_bank7` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `mid` int(11) NOT NULL COMMENT '用户游戏ID',
  `type` int(11) NOT NULL COMMENT '操作类型 1存 2取  3转账',
  `amount` int(11) NOT NULL COMMENT '操作金额数',
  `money` bigint(20) NOT NULL COMMENT '操作后最新金币数',
  `freezemoney` bigint(20) NOT NULL COMMENT '操作后保险柜最新金币数',
  `tomid` int(11) NOT NULL DEFAULT '0' COMMENT '转账时，对方的mid',
  `ctime` int(11) NOT NULL COMMENT '操作时间',
  `gameid` int(11) NOT NULL DEFAULT '7',
  `productid` int(11) NOT NULL DEFAULT '1' COMMENT '产品id',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_bank8
-- ----------------------------
DROP TABLE IF EXISTS `log_bank8`;
CREATE TABLE `log_bank8` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `mid` int(11) NOT NULL COMMENT '用户游戏ID',
  `type` int(11) NOT NULL COMMENT '操作类型 1存 2取  3转账',
  `amount` int(11) NOT NULL COMMENT '操作金额数',
  `money` bigint(20) NOT NULL COMMENT '操作后最新金币数',
  `freezemoney` bigint(20) NOT NULL COMMENT '操作后保险柜最新金币数',
  `tomid` int(11) NOT NULL DEFAULT '0' COMMENT '转账时，对方的mid',
  `ctime` int(11) NOT NULL COMMENT '操作时间',
  `gameid` int(11) NOT NULL DEFAULT '7',
  `productid` int(11) NOT NULL DEFAULT '1' COMMENT '产品id',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_bank9
-- ----------------------------
DROP TABLE IF EXISTS `log_bank9`;
CREATE TABLE `log_bank9` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `mid` int(11) NOT NULL COMMENT '用户游戏ID',
  `type` int(11) NOT NULL COMMENT '操作类型 1存 2取  3转账',
  `amount` int(11) NOT NULL COMMENT '操作金额数',
  `money` bigint(20) NOT NULL COMMENT '操作后最新金币数',
  `freezemoney` bigint(20) NOT NULL COMMENT '操作后保险柜最新金币数',
  `tomid` int(11) NOT NULL DEFAULT '0' COMMENT '转账时，对方的mid',
  `ctime` int(11) NOT NULL COMMENT '操作时间',
  `gameid` int(11) NOT NULL DEFAULT '2',
  `productid` int(11) NOT NULL DEFAULT '1' COMMENT '产品id',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_bank_tmp
-- ----------------------------
DROP TABLE IF EXISTS `log_bank_tmp`;
CREATE TABLE `log_bank_tmp` (
  `mid` int(11) NOT NULL COMMENT '用户游戏ID',
  `type` int(11) NOT NULL COMMENT '操作类型 1存 2取  3转账',
  `amount` int(11) NOT NULL COMMENT '操作金额数',
  `money` bigint(20) NOT NULL COMMENT '操作后最新金币数',
  `freezemoney` bigint(20) NOT NULL COMMENT '操作后保险柜最新金币数',
  `tomid` int(11) NOT NULL DEFAULT '0' COMMENT '转账时，对方的mid',
  `ctime` int(11) NOT NULL COMMENT '操作时间',
  `gameid` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_calregister
-- ----------------------------
DROP TABLE IF EXISTS `log_calregister`;
CREATE TABLE `log_calregister` (
  `mid` int(11) NOT NULL,
  `ip` varchar(20) NOT NULL,
  PRIMARY KEY (`mid`),
  KEY `ip` (`ip`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_login
-- ----------------------------
DROP TABLE IF EXISTS `log_login`;
CREATE TABLE `log_login` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '用户登录登出表',
  `pid` int(11) NOT NULL DEFAULT '0' COMMENT '产品id',
  `versions` varchar(32) NOT NULL DEFAULT '' COMMENT '产品版本',
  `mid` int(11) NOT NULL DEFAULT '0' COMMENT '用户mid',
  `login_time` int(11) NOT NULL DEFAULT '0' COMMENT '登录时间',
  `logout_time` int(11) NOT NULL DEFAULT '0' COMMENT '登出时间',
  `deviceno` varchar(50) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_login0
-- ----------------------------
DROP TABLE IF EXISTS `log_login0`;
CREATE TABLE `log_login0` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `ip` varchar(20) NOT NULL COMMENT '操作IP',
  `ip_arr` varchar(100) NOT NULL COMMENT 'IP地址',
  `ctime` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '登录时间',
  `mid` int(11) NOT NULL COMMENT '会员ID',
  `productid` int(11) NOT NULL COMMENT '产品id',
  `deviceno` varchar(50) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=90 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_login1
-- ----------------------------
DROP TABLE IF EXISTS `log_login1`;
CREATE TABLE `log_login1` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `ip` varchar(20) NOT NULL COMMENT '操作IP',
  `ip_arr` varchar(100) NOT NULL COMMENT 'IP地址',
  `ctime` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '登录时间',
  `mid` int(11) NOT NULL COMMENT '会员ID',
  `productid` int(11) NOT NULL COMMENT '产品id',
  `deviceno` varchar(50) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_login2
-- ----------------------------
DROP TABLE IF EXISTS `log_login2`;
CREATE TABLE `log_login2` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `ip` varchar(20) NOT NULL COMMENT '操作IP',
  `ip_arr` varchar(100) NOT NULL COMMENT 'IP地址',
  `ctime` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '登录时间',
  `mid` int(11) NOT NULL COMMENT '会员ID',
  `productid` int(11) NOT NULL COMMENT '产品id',
  `deviceno` varchar(50) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=411 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_login201612
-- ----------------------------
DROP TABLE IF EXISTS `log_login201612`;
CREATE TABLE `log_login201612` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '用户登录登出表',
  `pid` int(11) NOT NULL DEFAULT '0' COMMENT '产品id',
  `versions` varchar(32) NOT NULL DEFAULT '' COMMENT '产品版本',
  `mid` int(11) NOT NULL DEFAULT '0' COMMENT '用户mid',
  `deviceno` varchar(64) NOT NULL DEFAULT '' COMMENT '设备号',
  `login_time` bigint(20) NOT NULL DEFAULT '0' COMMENT '登录时间',
  `logout_time` bigint(20) NOT NULL DEFAULT '0' COMMENT '登出时间',
  `login_delta` int(11) NOT NULL DEFAULT '-10' COMMENT '上次到本次的登录间隔(s)',
  PRIMARY KEY (`id`),
  KEY `i_pid` (`pid`),
  KEY `i_logintime` (`login_time`),
  KEY `i_pl` (`pid`,`login_time`)
) ENGINE=InnoDB AUTO_INCREMENT=506010 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_login201701
-- ----------------------------
DROP TABLE IF EXISTS `log_login201701`;
CREATE TABLE `log_login201701` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '用户登录登出表',
  `pid` int(11) NOT NULL DEFAULT '0' COMMENT '产品id',
  `versions` varchar(32) NOT NULL DEFAULT '' COMMENT '产品版本',
  `mid` int(11) NOT NULL DEFAULT '0' COMMENT '用户mid',
  `deviceno` varchar(64) NOT NULL DEFAULT '' COMMENT '设备号',
  `login_time` bigint(20) NOT NULL DEFAULT '0' COMMENT '登录时间',
  `logout_time` bigint(20) NOT NULL DEFAULT '0' COMMENT '登出时间',
  `login_delta` int(11) NOT NULL DEFAULT '-10' COMMENT '上次到本次的登录间隔(s)',
  PRIMARY KEY (`id`),
  KEY `i_pid` (`pid`),
  KEY `i_logintime` (`login_time`),
  KEY `i_pl` (`pid`,`login_time`)
) ENGINE=InnoDB AUTO_INCREMENT=241 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_login201702
-- ----------------------------
DROP TABLE IF EXISTS `log_login201702`;
CREATE TABLE `log_login201702` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '用户登录登出表',
  `pid` int(11) NOT NULL DEFAULT '0' COMMENT '产品id',
  `versions` varchar(32) NOT NULL DEFAULT '' COMMENT '产品版本',
  `mid` int(11) NOT NULL DEFAULT '0' COMMENT '用户mid',
  `deviceno` varchar(64) NOT NULL DEFAULT '' COMMENT '设备号',
  `login_time` bigint(20) NOT NULL DEFAULT '0' COMMENT '登录时间',
  `logout_time` bigint(20) NOT NULL DEFAULT '0' COMMENT '登出时间',
  `login_delta` int(11) NOT NULL DEFAULT '-10' COMMENT '上次到本次的登录间隔(s)',
  PRIMARY KEY (`id`),
  KEY `i_pid` (`pid`),
  KEY `i_logintime` (`login_time`),
  KEY `i_pl` (`pid`,`login_time`)
) ENGINE=InnoDB AUTO_INCREMENT=1353 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_login201703
-- ----------------------------
DROP TABLE IF EXISTS `log_login201703`;
CREATE TABLE `log_login201703` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '用户登录登出表',
  `pid` int(11) NOT NULL DEFAULT '0' COMMENT '产品id',
  `versions` varchar(32) NOT NULL DEFAULT '' COMMENT '产品版本',
  `mid` int(11) NOT NULL DEFAULT '0' COMMENT '用户mid',
  `deviceno` varchar(64) NOT NULL DEFAULT '' COMMENT '设备号',
  `login_time` bigint(20) NOT NULL DEFAULT '0' COMMENT '登录时间',
  `logout_time` bigint(20) NOT NULL DEFAULT '0' COMMENT '登出时间',
  `login_delta` int(11) NOT NULL DEFAULT '-10' COMMENT '上次到本次的登录间隔(s)',
  PRIMARY KEY (`id`),
  KEY `i_pid` (`pid`),
  KEY `i_logintime` (`login_time`),
  KEY `i_pl` (`pid`,`login_time`)
) ENGINE=InnoDB AUTO_INCREMENT=1030 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_login3
-- ----------------------------
DROP TABLE IF EXISTS `log_login3`;
CREATE TABLE `log_login3` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `ip` varchar(20) NOT NULL COMMENT '操作IP',
  `ip_arr` varchar(100) NOT NULL COMMENT 'IP地址',
  `ctime` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '登录时间',
  `mid` int(11) NOT NULL COMMENT '会员ID',
  `productid` int(11) NOT NULL COMMENT '产品id',
  `deviceno` varchar(50) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_login4
-- ----------------------------
DROP TABLE IF EXISTS `log_login4`;
CREATE TABLE `log_login4` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `ip` varchar(20) NOT NULL COMMENT '操作IP',
  `ip_arr` varchar(100) NOT NULL COMMENT 'IP地址',
  `ctime` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '登录时间',
  `mid` int(11) NOT NULL COMMENT '会员ID',
  `productid` int(11) NOT NULL COMMENT '产品id',
  `deviceno` varchar(50) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=697 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_login5
-- ----------------------------
DROP TABLE IF EXISTS `log_login5`;
CREATE TABLE `log_login5` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `ip` varchar(20) NOT NULL COMMENT '操作IP',
  `ip_arr` varchar(100) NOT NULL COMMENT 'IP地址',
  `ctime` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '登录时间',
  `mid` int(11) NOT NULL COMMENT '会员ID',
  `productid` int(11) NOT NULL COMMENT '产品id',
  `deviceno` varchar(50) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_login6
-- ----------------------------
DROP TABLE IF EXISTS `log_login6`;
CREATE TABLE `log_login6` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `ip` varchar(20) NOT NULL COMMENT '操作IP',
  `ip_arr` varchar(100) NOT NULL COMMENT 'IP地址',
  `ctime` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '登录时间',
  `mid` int(11) NOT NULL COMMENT '会员ID',
  `productid` int(11) NOT NULL COMMENT '产品id',
  `deviceno` varchar(50) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=7308 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_login7
-- ----------------------------
DROP TABLE IF EXISTS `log_login7`;
CREATE TABLE `log_login7` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `ip` varchar(20) NOT NULL COMMENT '操作IP',
  `ip_arr` varchar(100) NOT NULL COMMENT 'IP地址',
  `ctime` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '登录时间',
  `mid` int(11) NOT NULL COMMENT '会员ID',
  `productid` int(11) NOT NULL COMMENT '产品id',
  `deviceno` varchar(50) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_login8
-- ----------------------------
DROP TABLE IF EXISTS `log_login8`;
CREATE TABLE `log_login8` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `ip` varchar(20) NOT NULL COMMENT '操作IP',
  `ip_arr` varchar(100) NOT NULL COMMENT 'IP地址',
  `ctime` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '登录时间',
  `mid` int(11) NOT NULL COMMENT '会员ID',
  `productid` int(11) NOT NULL COMMENT '产品id',
  `deviceno` varchar(50) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=605 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_login9
-- ----------------------------
DROP TABLE IF EXISTS `log_login9`;
CREATE TABLE `log_login9` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `ip` varchar(20) NOT NULL COMMENT '操作IP',
  `ip_arr` varchar(100) NOT NULL COMMENT 'IP地址',
  `ctime` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '登录时间',
  `mid` int(11) NOT NULL COMMENT '会员ID',
  `productid` int(11) NOT NULL COMMENT '产品id',
  `deviceno` varchar(50) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_member0
-- ----------------------------
DROP TABLE IF EXISTS `log_member0`;
CREATE TABLE `log_member0` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `gameid` int(11) NOT NULL COMMENT '游戏ID  1梭哈 2百家乐 3 斗地主 ',
  `mid` int(11) NOT NULL COMMENT '游戏ID',
  `ctype` tinyint(4) NOT NULL COMMENT '客户端类型 1 android 2 iphoe 3 ipad 4 android pad',
  `cid` int(11) NOT NULL COMMENT '渠道商ID 参考后台定义',
  `sid` int(11) NOT NULL COMMENT '账号类型 参考后台定义',
  `pid` int(11) NOT NULL COMMENT '包ID 参考后台定义',
  `tax` int(11) NOT NULL COMMENT '台费',
  `ante` int(11) NOT NULL COMMENT '底注',
  `svid` int(11) NOT NULL COMMENT '服务器ID',
  `tid` int(11) NOT NULL COMMENT '房间ID',
  `level` int(11) NOT NULL COMMENT '房间等级',
  `boardid` varchar(64) NOT NULL COMMENT '牌局ID',
  `endtype` int(11) NOT NULL COMMENT '结束类型  0 正常结束  1 弃牌  2 逃跑',
  `money` bigint(11) NOT NULL COMMENT '输赢金币数 ',
  `moneynow` bigint(20) NOT NULL COMMENT '当前金币数',
  `stime` int(11) NOT NULL COMMENT '开始时间',
  `etime` int(11) NOT NULL COMMENT '结束时间',
  `brand` int(11) NOT NULL COMMENT '结束牌型',
  PRIMARY KEY (`id`),
  KEY `stime` (`stime`),
  KEY `etime` (`etime`),
  KEY `mid` (`mid`),
  KEY `boardid` (`boardid`),
  KEY `ctype` (`ctype`),
  KEY `pid` (`pid`),
  KEY `level` (`level`)
) ENGINE=InnoDB AUTO_INCREMENT=931881 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_member1
-- ----------------------------
DROP TABLE IF EXISTS `log_member1`;
CREATE TABLE `log_member1` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `gameid` int(11) NOT NULL COMMENT '游戏ID  1梭哈 2百家乐 3 斗地主 ',
  `mid` int(11) NOT NULL COMMENT '游戏ID',
  `ctype` tinyint(4) NOT NULL COMMENT '客户端类型 1 android 2 iphoe 3 ipad 4 android pad',
  `cid` int(11) NOT NULL COMMENT '渠道商ID 参考后台定义',
  `sid` int(11) NOT NULL COMMENT '账号类型 参考后台定义',
  `pid` int(11) NOT NULL COMMENT '包ID 参考后台定义',
  `tax` int(11) NOT NULL COMMENT '台费',
  `ante` int(11) NOT NULL COMMENT '底注',
  `svid` int(11) NOT NULL COMMENT '服务器ID',
  `tid` int(11) NOT NULL COMMENT '房间ID',
  `level` int(11) NOT NULL COMMENT '房间等级',
  `boardid` varchar(64) NOT NULL COMMENT '牌局ID',
  `endtype` int(11) NOT NULL COMMENT '结束类型  0 正常结束  1 弃牌  2 逃跑',
  `money` bigint(11) NOT NULL COMMENT '输赢金币数 ',
  `moneynow` bigint(20) NOT NULL COMMENT '当前金币数',
  `stime` int(11) NOT NULL COMMENT '开始时间',
  `etime` int(11) NOT NULL COMMENT '结束时间',
  `brand` int(11) NOT NULL COMMENT '结束牌型',
  PRIMARY KEY (`id`),
  KEY `stime` (`stime`),
  KEY `etime` (`etime`),
  KEY `mid` (`mid`),
  KEY `boardid` (`boardid`),
  KEY `ctype` (`ctype`),
  KEY `pid` (`pid`),
  KEY `level` (`level`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_member10
-- ----------------------------
DROP TABLE IF EXISTS `log_member10`;
CREATE TABLE `log_member10` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `gameid` int(11) NOT NULL COMMENT '游戏ID  1梭哈 2百家乐 3 斗地主 ',
  `mid` int(11) NOT NULL COMMENT '游戏ID',
  `ctype` tinyint(4) NOT NULL COMMENT '客户端类型 1 android 2 iphoe 3 ipad 4 android pad',
  `cid` int(11) NOT NULL COMMENT '渠道商ID 参考后台定义',
  `sid` int(11) NOT NULL COMMENT '账号类型 参考后台定义',
  `pid` int(11) NOT NULL COMMENT '包ID 参考后台定义',
  `tax` int(11) NOT NULL COMMENT '台费',
  `ante` int(11) NOT NULL COMMENT '底注',
  `svid` int(11) NOT NULL COMMENT '服务器ID',
  `tid` int(11) NOT NULL COMMENT '房间ID',
  `level` int(11) NOT NULL COMMENT '房间等级',
  `boardid` varchar(64) NOT NULL COMMENT '牌局ID',
  `endtype` int(11) NOT NULL COMMENT '结束类型  0 正常结束  1 弃牌  2 逃跑',
  `money` bigint(11) NOT NULL COMMENT '输赢金币数 ',
  `moneynow` bigint(20) NOT NULL COMMENT '当前金币数',
  `stime` int(11) NOT NULL COMMENT '开始时间',
  `etime` int(11) NOT NULL COMMENT '结束时间',
  `brand` int(11) NOT NULL COMMENT '结束牌型',
  PRIMARY KEY (`id`),
  KEY `stime` (`stime`),
  KEY `etime` (`etime`),
  KEY `mid` (`mid`),
  KEY `boardid` (`boardid`),
  KEY `ctype` (`ctype`),
  KEY `pid` (`pid`),
  KEY `level` (`level`)
) ENGINE=InnoDB AUTO_INCREMENT=2053507 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_member2
-- ----------------------------
DROP TABLE IF EXISTS `log_member2`;
CREATE TABLE `log_member2` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `gameid` int(11) NOT NULL COMMENT '游戏ID  1梭哈 2百家乐 3 斗地主 ',
  `mid` int(11) NOT NULL COMMENT '游戏ID',
  `ctype` tinyint(4) NOT NULL COMMENT '客户端类型 1 android 2 iphoe 3 ipad 4 android pad',
  `cid` int(11) NOT NULL COMMENT '渠道商ID 参考后台定义',
  `sid` int(11) NOT NULL COMMENT '账号类型 参考后台定义',
  `pid` int(11) NOT NULL COMMENT '包ID 参考后台定义',
  `tax` int(11) NOT NULL COMMENT '台费',
  `ante` int(11) NOT NULL COMMENT '底注',
  `svid` int(11) NOT NULL COMMENT '服务器ID',
  `tid` int(11) NOT NULL COMMENT '房间ID',
  `level` int(11) NOT NULL COMMENT '房间等级',
  `boardid` varchar(64) NOT NULL COMMENT '牌局ID',
  `endtype` int(11) NOT NULL COMMENT '结束类型  0 正常结束  1 弃牌  2 逃跑',
  `money` bigint(11) NOT NULL COMMENT '输赢金币数 ',
  `moneynow` bigint(20) NOT NULL COMMENT '当前金币数',
  `stime` int(11) NOT NULL COMMENT '开始时间',
  `etime` int(11) NOT NULL COMMENT '结束时间',
  `brand` int(11) NOT NULL COMMENT '结束牌型',
  PRIMARY KEY (`id`),
  KEY `stime` (`stime`),
  KEY `etime` (`etime`),
  KEY `mid` (`mid`),
  KEY `boardid` (`boardid`),
  KEY `ctype` (`ctype`),
  KEY `pid` (`pid`),
  KEY `level` (`level`)
) ENGINE=InnoDB AUTO_INCREMENT=190836 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_member3
-- ----------------------------
DROP TABLE IF EXISTS `log_member3`;
CREATE TABLE `log_member3` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `gameid` int(11) NOT NULL COMMENT '游戏ID  1梭哈 2百家乐 3 斗地主 ',
  `mid` int(11) NOT NULL COMMENT '游戏ID',
  `ctype` tinyint(4) NOT NULL COMMENT '客户端类型 1 android 2 iphoe 3 ipad 4 android pad',
  `cid` int(11) NOT NULL COMMENT '渠道商ID 参考后台定义',
  `sid` int(11) NOT NULL COMMENT '账号类型 参考后台定义',
  `pid` int(11) NOT NULL COMMENT '包ID 参考后台定义',
  `tax` int(11) NOT NULL COMMENT '台费',
  `ante` int(11) NOT NULL COMMENT '底注',
  `svid` int(11) NOT NULL COMMENT '服务器ID',
  `tid` int(11) NOT NULL COMMENT '房间ID',
  `level` int(11) NOT NULL COMMENT '房间等级',
  `boardid` varchar(64) NOT NULL COMMENT '牌局ID',
  `endtype` int(11) NOT NULL COMMENT '结束类型  0 正常结束  1 弃牌  2 逃跑',
  `money` bigint(11) NOT NULL COMMENT '输赢金币数 ',
  `moneynow` bigint(20) NOT NULL COMMENT '当前金币数',
  `stime` int(11) NOT NULL COMMENT '开始时间',
  `etime` int(11) NOT NULL COMMENT '结束时间',
  `brand` int(11) NOT NULL COMMENT '结束牌型',
  PRIMARY KEY (`id`),
  KEY `stime` (`stime`),
  KEY `etime` (`etime`),
  KEY `mid` (`mid`),
  KEY `boardid` (`boardid`),
  KEY `ctype` (`ctype`),
  KEY `pid` (`pid`),
  KEY `level` (`level`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_member4
-- ----------------------------
DROP TABLE IF EXISTS `log_member4`;
CREATE TABLE `log_member4` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `gameid` int(11) NOT NULL COMMENT '游戏ID  1梭哈 2百家乐 3 斗地主 ',
  `mid` int(11) NOT NULL COMMENT '游戏ID',
  `ctype` tinyint(4) NOT NULL COMMENT '客户端类型 1 android 2 iphoe 3 ipad 4 android pad',
  `cid` int(11) NOT NULL COMMENT '渠道商ID 参考后台定义',
  `sid` int(11) NOT NULL COMMENT '账号类型 参考后台定义',
  `pid` int(11) NOT NULL COMMENT '包ID 参考后台定义',
  `tax` int(11) NOT NULL COMMENT '台费',
  `ante` int(11) NOT NULL COMMENT '底注',
  `svid` int(11) NOT NULL COMMENT '服务器ID',
  `tid` int(11) NOT NULL COMMENT '房间ID',
  `level` int(11) NOT NULL COMMENT '房间等级',
  `boardid` varchar(64) NOT NULL COMMENT '牌局ID',
  `endtype` int(11) NOT NULL COMMENT '结束类型  0 正常结束  1 弃牌  2 逃跑',
  `money` bigint(11) NOT NULL COMMENT '输赢金币数 ',
  `moneynow` bigint(20) NOT NULL COMMENT '当前金币数',
  `stime` int(11) NOT NULL COMMENT '开始时间',
  `etime` int(11) NOT NULL COMMENT '结束时间',
  `brand` int(11) NOT NULL COMMENT '结束牌型',
  PRIMARY KEY (`id`),
  KEY `stime` (`stime`),
  KEY `etime` (`etime`),
  KEY `mid` (`mid`),
  KEY `boardid` (`boardid`),
  KEY `ctype` (`ctype`),
  KEY `pid` (`pid`),
  KEY `level` (`level`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_member5
-- ----------------------------
DROP TABLE IF EXISTS `log_member5`;
CREATE TABLE `log_member5` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `gameid` int(11) NOT NULL COMMENT '游戏ID  1梭哈 2百家乐 3 斗地主 ',
  `mid` int(11) NOT NULL COMMENT '游戏ID',
  `ctype` tinyint(4) NOT NULL COMMENT '客户端类型 1 android 2 iphoe 3 ipad 4 android pad',
  `cid` int(11) NOT NULL COMMENT '渠道商ID 参考后台定义',
  `sid` int(11) NOT NULL COMMENT '账号类型 参考后台定义',
  `pid` int(11) NOT NULL COMMENT '包ID 参考后台定义',
  `tax` int(11) NOT NULL COMMENT '台费',
  `ante` int(11) NOT NULL COMMENT '底注',
  `svid` int(11) NOT NULL COMMENT '服务器ID',
  `tid` int(11) NOT NULL COMMENT '房间ID',
  `level` int(11) NOT NULL COMMENT '房间等级',
  `boardid` varchar(64) NOT NULL COMMENT '牌局ID',
  `endtype` int(11) NOT NULL COMMENT '结束类型  0 正常结束  1 弃牌  2 逃跑',
  `money` bigint(11) NOT NULL COMMENT '输赢金币数 ',
  `moneynow` bigint(20) NOT NULL COMMENT '当前金币数',
  `stime` int(11) NOT NULL COMMENT '开始时间',
  `etime` int(11) NOT NULL COMMENT '结束时间',
  `brand` int(11) NOT NULL COMMENT '结束牌型',
  PRIMARY KEY (`id`),
  KEY `stime` (`stime`),
  KEY `etime` (`etime`),
  KEY `mid` (`mid`),
  KEY `boardid` (`boardid`),
  KEY `ctype` (`ctype`),
  KEY `pid` (`pid`),
  KEY `level` (`level`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_member6
-- ----------------------------
DROP TABLE IF EXISTS `log_member6`;
CREATE TABLE `log_member6` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `gameid` int(11) NOT NULL COMMENT '游戏ID  1梭哈 2百家乐 3 斗地主 ',
  `mid` int(11) NOT NULL COMMENT '游戏ID',
  `ctype` tinyint(4) NOT NULL COMMENT '客户端类型 1 android 2 iphoe 3 ipad 4 android pad',
  `cid` int(11) NOT NULL COMMENT '渠道商ID 参考后台定义',
  `sid` int(11) NOT NULL COMMENT '账号类型 参考后台定义',
  `pid` int(11) NOT NULL COMMENT '包ID 参考后台定义',
  `tax` int(11) NOT NULL COMMENT '台费',
  `ante` int(11) NOT NULL COMMENT '底注',
  `svid` int(11) NOT NULL COMMENT '服务器ID',
  `tid` int(11) NOT NULL COMMENT '房间ID',
  `level` int(11) NOT NULL COMMENT '房间等级',
  `boardid` varchar(64) NOT NULL COMMENT '牌局ID',
  `endtype` int(11) NOT NULL COMMENT '结束类型  0 正常结束  1 弃牌  2 逃跑',
  `money` bigint(11) NOT NULL COMMENT '输赢金币数 ',
  `moneynow` bigint(20) NOT NULL COMMENT '当前金币数',
  `stime` int(11) NOT NULL COMMENT '开始时间',
  `etime` int(11) NOT NULL COMMENT '结束时间',
  `brand` int(11) NOT NULL COMMENT '结束牌型',
  PRIMARY KEY (`id`),
  KEY `stime` (`stime`),
  KEY `etime` (`etime`),
  KEY `mid` (`mid`),
  KEY `boardid` (`boardid`),
  KEY `ctype` (`ctype`),
  KEY `pid` (`pid`),
  KEY `level` (`level`)
) ENGINE=InnoDB AUTO_INCREMENT=1140 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_member7
-- ----------------------------
DROP TABLE IF EXISTS `log_member7`;
CREATE TABLE `log_member7` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `gameid` int(11) NOT NULL COMMENT '游戏ID  1梭哈 2百家乐 3 斗地主 ',
  `mid` int(11) NOT NULL COMMENT '游戏ID',
  `ctype` tinyint(4) NOT NULL COMMENT '客户端类型 1 android 2 iphoe 3 ipad 4 android pad',
  `cid` int(11) NOT NULL COMMENT '渠道商ID 参考后台定义',
  `sid` int(11) NOT NULL COMMENT '账号类型 参考后台定义',
  `pid` int(11) NOT NULL COMMENT '包ID 参考后台定义',
  `tax` int(11) NOT NULL COMMENT '台费',
  `ante` int(11) NOT NULL COMMENT '底注',
  `svid` int(11) NOT NULL COMMENT '服务器ID',
  `tid` int(11) NOT NULL COMMENT '房间ID',
  `level` int(11) NOT NULL COMMENT '房间等级',
  `boardid` varchar(64) NOT NULL COMMENT '牌局ID',
  `endtype` int(11) NOT NULL COMMENT '结束类型  0 正常结束  1 弃牌  2 逃跑',
  `money` bigint(11) NOT NULL COMMENT '输赢金币数 ',
  `moneynow` bigint(20) NOT NULL COMMENT '当前金币数',
  `stime` int(11) NOT NULL COMMENT '开始时间',
  `etime` int(11) NOT NULL COMMENT '结束时间',
  `brand` int(11) NOT NULL COMMENT '结束牌型',
  PRIMARY KEY (`id`),
  KEY `stime` (`stime`),
  KEY `etime` (`etime`),
  KEY `mid` (`mid`),
  KEY `boardid` (`boardid`),
  KEY `ctype` (`ctype`),
  KEY `pid` (`pid`),
  KEY `level` (`level`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_member8
-- ----------------------------
DROP TABLE IF EXISTS `log_member8`;
CREATE TABLE `log_member8` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `gameid` int(11) NOT NULL COMMENT '游戏ID  1梭哈 2百家乐 3 斗地主 ',
  `mid` int(11) NOT NULL COMMENT '游戏ID',
  `ctype` tinyint(4) NOT NULL COMMENT '客户端类型 1 android 2 iphoe 3 ipad 4 android pad',
  `cid` int(11) NOT NULL COMMENT '渠道商ID 参考后台定义',
  `sid` int(11) NOT NULL COMMENT '账号类型 参考后台定义',
  `pid` int(11) NOT NULL COMMENT '包ID 参考后台定义',
  `tax` int(11) NOT NULL COMMENT '台费',
  `ante` int(11) NOT NULL COMMENT '底注',
  `svid` int(11) NOT NULL COMMENT '服务器ID',
  `tid` int(11) NOT NULL COMMENT '房间ID',
  `level` int(11) NOT NULL COMMENT '房间等级',
  `boardid` varchar(64) NOT NULL COMMENT '牌局ID',
  `endtype` int(11) NOT NULL COMMENT '结束类型  0 正常结束  1 弃牌  2 逃跑',
  `money` bigint(11) NOT NULL COMMENT '输赢金币数 ',
  `moneynow` bigint(20) NOT NULL COMMENT '当前金币数',
  `stime` int(11) NOT NULL COMMENT '开始时间',
  `etime` int(11) NOT NULL COMMENT '结束时间',
  `brand` int(11) NOT NULL COMMENT '结束牌型',
  PRIMARY KEY (`id`),
  KEY `stime` (`stime`),
  KEY `etime` (`etime`),
  KEY `mid` (`mid`),
  KEY `boardid` (`boardid`),
  KEY `ctype` (`ctype`),
  KEY `pid` (`pid`),
  KEY `level` (`level`)
) ENGINE=InnoDB AUTO_INCREMENT=152 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_member9
-- ----------------------------
DROP TABLE IF EXISTS `log_member9`;
CREATE TABLE `log_member9` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `gameid` int(11) NOT NULL COMMENT '游戏ID  1梭哈 2百家乐 3 斗地主 ',
  `mid` int(11) NOT NULL COMMENT '游戏ID',
  `ctype` tinyint(4) NOT NULL COMMENT '客户端类型 1 android 2 iphoe 3 ipad 4 android pad',
  `cid` int(11) NOT NULL COMMENT '渠道商ID 参考后台定义',
  `sid` int(11) NOT NULL COMMENT '账号类型 参考后台定义',
  `pid` int(11) NOT NULL COMMENT '包ID 参考后台定义',
  `tax` int(11) NOT NULL COMMENT '台费',
  `ante` int(11) NOT NULL COMMENT '底注',
  `svid` int(11) NOT NULL COMMENT '服务器ID',
  `tid` int(11) NOT NULL COMMENT '房间ID',
  `level` int(11) NOT NULL COMMENT '房间等级',
  `boardid` varchar(64) NOT NULL COMMENT '牌局ID',
  `endtype` int(11) NOT NULL COMMENT '结束类型  0 正常结束  1 弃牌  2 逃跑',
  `money` bigint(11) NOT NULL COMMENT '输赢金币数 ',
  `moneynow` bigint(20) NOT NULL COMMENT '当前金币数',
  `stime` int(11) NOT NULL COMMENT '开始时间',
  `etime` int(11) NOT NULL COMMENT '结束时间',
  `brand` int(11) NOT NULL COMMENT '结束牌型',
  PRIMARY KEY (`id`),
  KEY `stime` (`stime`),
  KEY `etime` (`etime`),
  KEY `mid` (`mid`),
  KEY `boardid` (`boardid`),
  KEY `ctype` (`ctype`),
  KEY `pid` (`pid`),
  KEY `level` (`level`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_member_fish
-- ----------------------------
DROP TABLE IF EXISTS `log_member_fish`;
CREATE TABLE `log_member_fish` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `mid` int(11) NOT NULL,
  `ctype` int(11) NOT NULL,
  `cid` int(11) NOT NULL,
  `pid` int(11) NOT NULL,
  `gid` int(11) NOT NULL DEFAULT '5',
  `roomid` int(11) NOT NULL COMMENT '房间ID',
  `shotnum` int(11) NOT NULL COMMENT '打炮总数',
  `killedtype` varchar(1000) NOT NULL COMMENT '打死的鱼的种类，种类之间用,分隔',
  `skills` varchar(100) NOT NULL COMMENT '技能使用情况[{1,2},{2,3}]',
  `skillmoney` int(11) NOT NULL COMMENT '使用技能金币数',
  `unlock` int(11) NOT NULL COMMENT '解锁情况，位运算',
  `unlockmoney` int(11) NOT NULL COMMENT '解锁使用的金币数',
  `losemoney` int(11) NOT NULL COMMENT '消耗了多少金币',
  `getmoney` int(11) NOT NULL COMMENT '获得了多少金币',
  `stmoney` int(11) NOT NULL COMMENT '开始时的金币数',
  `endmoney` int(11) NOT NULL COMMENT '结束后的金币数',
  `stime` int(11) NOT NULL COMMENT '进场时间',
  `etime` int(11) NOT NULL COMMENT '离场时间',
  PRIMARY KEY (`id`),
  KEY `mid` (`mid`,`ctype`,`cid`),
  KEY `roomid` (`roomid`),
  KEY `ctype` (`ctype`),
  KEY `cid` (`cid`),
  KEY `stime` (`stime`),
  KEY `etime` (`etime`),
  KEY `gid` (`gid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_register1
-- ----------------------------
DROP TABLE IF EXISTS `log_register1`;
CREATE TABLE `log_register1` (
  `mid` int(11) NOT NULL,
  PRIMARY KEY (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_register2
-- ----------------------------
DROP TABLE IF EXISTS `log_register2`;
CREATE TABLE `log_register2` (
  `mid` int(11) NOT NULL,
  PRIMARY KEY (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_register3
-- ----------------------------
DROP TABLE IF EXISTS `log_register3`;
CREATE TABLE `log_register3` (
  `mid` int(11) NOT NULL,
  PRIMARY KEY (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_register4
-- ----------------------------
DROP TABLE IF EXISTS `log_register4`;
CREATE TABLE `log_register4` (
  `mid` int(11) NOT NULL,
  PRIMARY KEY (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_register5
-- ----------------------------
DROP TABLE IF EXISTS `log_register5`;
CREATE TABLE `log_register5` (
  `mid` int(11) NOT NULL,
  PRIMARY KEY (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_register6
-- ----------------------------
DROP TABLE IF EXISTS `log_register6`;
CREATE TABLE `log_register6` (
  `mid` int(11) NOT NULL,
  PRIMARY KEY (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_register7
-- ----------------------------
DROP TABLE IF EXISTS `log_register7`;
CREATE TABLE `log_register7` (
  `mid` int(11) NOT NULL,
  PRIMARY KEY (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_roll1
-- ----------------------------
DROP TABLE IF EXISTS `log_roll1`;
CREATE TABLE `log_roll1` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `ctype` int(11) NOT NULL COMMENT '客户端类型ID',
  `cid` int(11) NOT NULL COMMENT '渠道ID',
  `pid` int(11) NOT NULL COMMENT '包ID',
  `sid` int(11) NOT NULL COMMENT '账号类型ID',
  `mid` int(11) NOT NULL COMMENT '玩家ID',
  `amount` int(11) NOT NULL COMMENT '数量',
  `rollnow` int(11) NOT NULL COMMENT '当前的乐卷数',
  `level` int(11) NOT NULL COMMENT '场次ID',
  `taskid` bigint(20) NOT NULL,
  `rmode` int(11) NOT NULL COMMENT '扣减乐卷的ID',
  `ctime` int(11) NOT NULL COMMENT '得到的时间',
  `gameid` int(11) NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`),
  KEY `ctype` (`ctype`,`cid`,`pid`,`taskid`),
  KEY `mid` (`mid`),
  KEY `cid` (`cid`),
  KEY `pid` (`pid`),
  KEY `rmode` (`rmode`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_roll2
-- ----------------------------
DROP TABLE IF EXISTS `log_roll2`;
CREATE TABLE `log_roll2` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `ctype` int(11) NOT NULL COMMENT '客户端类型ID',
  `cid` int(11) NOT NULL COMMENT '渠道ID',
  `pid` int(11) NOT NULL COMMENT '包ID',
  `sid` int(11) NOT NULL COMMENT '账号类型ID',
  `mid` int(11) NOT NULL COMMENT '玩家ID',
  `amount` int(11) NOT NULL COMMENT '数量',
  `rollnow` int(11) NOT NULL COMMENT '当前的乐卷数',
  `level` int(11) NOT NULL COMMENT '场次ID',
  `taskid` bigint(20) NOT NULL,
  `rmode` int(11) NOT NULL COMMENT '扣减乐卷的ID',
  `ctime` int(11) NOT NULL COMMENT '得到的时间',
  `gameid` int(11) NOT NULL DEFAULT '2',
  PRIMARY KEY (`id`),
  KEY `ctype` (`ctype`,`cid`,`pid`,`taskid`),
  KEY `mid` (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_roll3
-- ----------------------------
DROP TABLE IF EXISTS `log_roll3`;
CREATE TABLE `log_roll3` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `ctype` int(11) NOT NULL COMMENT '客户端类型ID',
  `cid` int(11) NOT NULL COMMENT '渠道ID',
  `pid` int(11) NOT NULL COMMENT '包ID',
  `sid` int(11) NOT NULL COMMENT '账号类型ID',
  `mid` int(11) NOT NULL COMMENT '玩家ID',
  `amount` int(11) NOT NULL COMMENT '数量',
  `rollnow` int(11) NOT NULL COMMENT '当前的乐卷数',
  `level` int(11) NOT NULL COMMENT '场次ID',
  `taskid` bigint(20) NOT NULL,
  `rmode` int(11) NOT NULL COMMENT '扣减乐卷的ID',
  `ctime` int(11) NOT NULL COMMENT '得到的时间',
  `gameid` int(11) NOT NULL DEFAULT '3',
  PRIMARY KEY (`id`),
  KEY `ctype` (`ctype`,`cid`,`pid`,`taskid`),
  KEY `mid` (`mid`),
  KEY `cid` (`cid`),
  KEY `pid` (`pid`),
  KEY `rmode` (`rmode`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_roll4
-- ----------------------------
DROP TABLE IF EXISTS `log_roll4`;
CREATE TABLE `log_roll4` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `ctype` int(11) NOT NULL COMMENT '客户端类型ID',
  `cid` int(11) NOT NULL COMMENT '渠道ID',
  `pid` int(11) NOT NULL COMMENT '包ID',
  `sid` int(11) NOT NULL COMMENT '账号类型ID',
  `mid` int(11) NOT NULL COMMENT '玩家ID',
  `amount` int(11) NOT NULL COMMENT '数量',
  `rollnow` int(11) NOT NULL COMMENT '当前的乐卷数',
  `level` int(11) NOT NULL COMMENT '场次ID',
  `taskid` bigint(20) NOT NULL,
  `rmode` int(11) NOT NULL COMMENT '扣减乐卷的ID',
  `ctime` int(11) NOT NULL COMMENT '得到的时间',
  `gameid` int(11) NOT NULL DEFAULT '4',
  PRIMARY KEY (`id`),
  KEY `ctype` (`ctype`,`cid`,`pid`,`taskid`),
  KEY `mid` (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_roll5
-- ----------------------------
DROP TABLE IF EXISTS `log_roll5`;
CREATE TABLE `log_roll5` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `ctype` int(11) NOT NULL COMMENT '客户端类型ID',
  `cid` int(11) NOT NULL COMMENT '渠道ID',
  `pid` int(11) NOT NULL COMMENT '包ID',
  `sid` int(11) NOT NULL COMMENT '账号类型ID',
  `mid` int(11) NOT NULL COMMENT '玩家ID',
  `amount` int(11) NOT NULL COMMENT '数量',
  `rollnow` int(11) NOT NULL COMMENT '当前的乐卷数',
  `level` int(11) NOT NULL COMMENT '场次ID',
  `taskid` bigint(20) NOT NULL,
  `rmode` int(11) NOT NULL COMMENT '扣减乐卷的ID',
  `ctime` int(11) NOT NULL COMMENT '得到的时间',
  `gameid` int(11) NOT NULL DEFAULT '4',
  PRIMARY KEY (`id`),
  KEY `ctype` (`ctype`,`cid`,`pid`,`taskid`),
  KEY `mid` (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_roll6
-- ----------------------------
DROP TABLE IF EXISTS `log_roll6`;
CREATE TABLE `log_roll6` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `ctype` int(11) NOT NULL COMMENT '客户端类型ID',
  `cid` int(11) NOT NULL COMMENT '渠道ID',
  `pid` int(11) NOT NULL COMMENT '包ID',
  `sid` int(11) NOT NULL COMMENT '账号类型ID',
  `mid` int(11) NOT NULL COMMENT '玩家ID',
  `amount` int(11) NOT NULL COMMENT '数量',
  `rollnow` int(11) NOT NULL COMMENT '当前的乐卷数',
  `level` int(11) NOT NULL COMMENT '场次ID',
  `taskid` bigint(20) NOT NULL,
  `rmode` int(11) NOT NULL COMMENT '扣减乐卷的ID',
  `ctime` int(11) NOT NULL COMMENT '得到的时间',
  `gameid` int(11) NOT NULL DEFAULT '6',
  PRIMARY KEY (`id`),
  KEY `ctype` (`ctype`,`cid`,`pid`,`taskid`),
  KEY `mid` (`mid`),
  KEY `cid` (`cid`),
  KEY `pid` (`pid`),
  KEY `rmode` (`rmode`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_roll7
-- ----------------------------
DROP TABLE IF EXISTS `log_roll7`;
CREATE TABLE `log_roll7` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `ctype` int(11) NOT NULL COMMENT '客户端类型ID',
  `cid` int(11) NOT NULL COMMENT '渠道ID',
  `pid` int(11) NOT NULL COMMENT '包ID',
  `sid` int(11) NOT NULL COMMENT '账号类型ID',
  `mid` int(11) NOT NULL COMMENT '玩家ID',
  `amount` int(11) NOT NULL COMMENT '数量',
  `rollnow` int(11) NOT NULL COMMENT '当前的乐卷数',
  `level` int(11) NOT NULL COMMENT '场次ID',
  `taskid` bigint(20) NOT NULL,
  `rmode` int(11) NOT NULL COMMENT '扣减乐卷的ID',
  `ctime` int(11) NOT NULL COMMENT '得到的时间',
  `gameid` int(11) NOT NULL DEFAULT '6',
  PRIMARY KEY (`id`),
  KEY `ctype` (`ctype`,`cid`,`pid`,`taskid`),
  KEY `mid` (`mid`),
  KEY `cid` (`cid`),
  KEY `pid` (`pid`),
  KEY `rmode` (`rmode`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_roll_tmp
-- ----------------------------
DROP TABLE IF EXISTS `log_roll_tmp`;
CREATE TABLE `log_roll_tmp` (
  `ctype` int(11) NOT NULL COMMENT '客户端类型ID',
  `cid` int(11) NOT NULL COMMENT '渠道ID',
  `pid` int(11) NOT NULL COMMENT '包ID',
  `sid` int(11) NOT NULL COMMENT '账号类型ID',
  `mid` int(11) NOT NULL COMMENT '玩家ID',
  `amount` int(11) NOT NULL COMMENT '数量',
  `rollnow` int(11) NOT NULL COMMENT '当前的乐卷数',
  `level` int(11) NOT NULL COMMENT '场次ID',
  `taskid` bigint(20) NOT NULL,
  `rmode` int(11) NOT NULL COMMENT '扣减乐卷的ID',
  `ctime` int(11) NOT NULL COMMENT '得到的时间',
  `gameid` int(11) NOT NULL,
  KEY `ctype` (`ctype`,`cid`,`pid`,`taskid`),
  KEY `mid` (`mid`),
  KEY `cid` (`cid`),
  KEY `pid` (`pid`),
  KEY `rmode` (`rmode`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_win1
-- ----------------------------
DROP TABLE IF EXISTS `log_win1`;
CREATE TABLE `log_win1` (
  `wid` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `gameid` int(11) NOT NULL COMMENT '游戏ID  1梭哈 2百家乐 3 斗地主 ',
  `mid` int(10) unsigned NOT NULL COMMENT '用户ID',
  `sid` smallint(3) unsigned NOT NULL DEFAULT '10' COMMENT '站点ID',
  `cid` int(11) NOT NULL COMMENT '渠道ID',
  `pid` int(11) NOT NULL COMMENT '包ID',
  `level` int(11) NOT NULL DEFAULT '0' COMMENT 'level id',
  `tid` int(11) NOT NULL DEFAULT '0' COMMENT '桌子id',
  `ctype` tinyint(4) NOT NULL COMMENT '客户端类型ID',
  `wmode` smallint(3) unsigned NOT NULL COMMENT '标识',
  `wflag` tinyint(3) unsigned NOT NULL COMMENT '0收获1付出',
  `before_money` int(10) NOT NULL DEFAULT '0' COMMENT '操作金币前金币数',
  `money` int(11) NOT NULL COMMENT '加减金币数',
  `after_money` int(11) NOT NULL DEFAULT '0' COMMENT '操作金币后数据',
  `wtime` int(10) unsigned NOT NULL COMMENT '输赢时间',
  `wdesc` varchar(255) NOT NULL COMMENT '附加信息:比如邀请好友的好友的sitemid,便于页面显示',
  PRIMARY KEY (`wid`),
  KEY `mid` (`mid`),
  KEY `wtime` (`wtime`),
  KEY `pid` (`pid`),
  KEY `ctype` (`ctype`)
) ENGINE=InnoDB AUTO_INCREMENT=7 DEFAULT CHARSET=utf8 COMMENT='用户输赢记录表';

-- ----------------------------
-- Table structure for log_win10
-- ----------------------------
DROP TABLE IF EXISTS `log_win10`;
CREATE TABLE `log_win10` (
  `wid` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `gameid` int(11) NOT NULL COMMENT '游戏ID  1梭哈 2百家乐 3 斗地主 ',
  `mid` int(10) unsigned NOT NULL COMMENT '用户ID',
  `sid` smallint(3) unsigned NOT NULL DEFAULT '10' COMMENT '站点ID',
  `cid` int(11) NOT NULL COMMENT '渠道ID',
  `pid` int(11) NOT NULL COMMENT '包ID',
  `level` int(11) NOT NULL DEFAULT '0' COMMENT 'level id',
  `tid` int(11) NOT NULL DEFAULT '0' COMMENT '桌子id',
  `ctype` tinyint(4) NOT NULL COMMENT '客户端类型ID',
  `wmode` smallint(3) unsigned NOT NULL COMMENT '标识',
  `wflag` tinyint(3) unsigned NOT NULL COMMENT '0收获1付出',
  `before_money` int(10) NOT NULL DEFAULT '0' COMMENT '操作金币前金币数',
  `money` int(11) NOT NULL COMMENT '加减金币数',
  `after_money` int(11) NOT NULL DEFAULT '0' COMMENT '操作金币后数据',
  `wtime` int(10) unsigned NOT NULL COMMENT '输赢时间',
  `wdesc` varchar(255) NOT NULL COMMENT '附加信息:比如邀请好友的好友的sitemid,便于页面显示',
  PRIMARY KEY (`wid`),
  KEY `mid` (`mid`),
  KEY `wtime` (`wtime`),
  KEY `pid` (`pid`),
  KEY `ctype` (`ctype`)
) ENGINE=InnoDB AUTO_INCREMENT=1171029 DEFAULT CHARSET=utf8 COMMENT='用户输赢记录表';

-- ----------------------------
-- Table structure for log_win2
-- ----------------------------
DROP TABLE IF EXISTS `log_win2`;
CREATE TABLE `log_win2` (
  `wid` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `gameid` int(11) NOT NULL COMMENT '游戏ID  1梭哈 2百家乐 3 斗地主 ',
  `mid` int(10) unsigned NOT NULL COMMENT '用户ID',
  `sid` smallint(3) unsigned NOT NULL DEFAULT '10' COMMENT '站点ID',
  `cid` tinyint(4) NOT NULL COMMENT '渠道ID',
  `pid` int(11) NOT NULL COMMENT '包ID',
  `level` int(11) NOT NULL DEFAULT '0' COMMENT 'level id',
  `tid` int(11) NOT NULL DEFAULT '0' COMMENT '桌子id',
  `ctype` tinyint(4) NOT NULL COMMENT '客户端类型ID',
  `wmode` smallint(3) unsigned NOT NULL COMMENT '标识',
  `wflag` tinyint(3) unsigned NOT NULL COMMENT '0收获1付出',
  `before_money` int(10) NOT NULL DEFAULT '0' COMMENT '操作金币前金币数',
  `money` int(11) NOT NULL COMMENT '加减金币数',
  `after_money` int(11) NOT NULL DEFAULT '0' COMMENT '操作金币后数据',
  `wtime` int(10) unsigned NOT NULL COMMENT '输赢时间',
  `wdesc` varchar(255) NOT NULL COMMENT '附加信息:比如邀请好友的好友的sitemid,便于页面显示',
  PRIMARY KEY (`wid`),
  KEY `mid` (`mid`),
  KEY `wtime` (`wtime`),
  KEY `pid` (`pid`),
  KEY `ctype` (`ctype`)
) ENGINE=InnoDB AUTO_INCREMENT=10 DEFAULT CHARSET=utf8 COMMENT='用户输赢记录表';

-- ----------------------------
-- Table structure for log_win3
-- ----------------------------
DROP TABLE IF EXISTS `log_win3`;
CREATE TABLE `log_win3` (
  `wid` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `gameid` int(11) NOT NULL COMMENT '游戏ID  1梭哈 2百家乐 3 斗地主 ',
  `mid` int(10) unsigned NOT NULL COMMENT '用户ID',
  `sid` smallint(3) unsigned NOT NULL DEFAULT '10' COMMENT '站点ID',
  `cid` int(11) NOT NULL COMMENT '渠道ID',
  `pid` int(11) NOT NULL COMMENT '包ID',
  `level` int(11) NOT NULL DEFAULT '0' COMMENT 'level id',
  `tid` int(11) NOT NULL DEFAULT '0' COMMENT '桌子id',
  `ctype` tinyint(4) NOT NULL COMMENT '客户端类型ID',
  `wmode` smallint(3) unsigned NOT NULL COMMENT '标识',
  `wflag` tinyint(3) unsigned NOT NULL COMMENT '0收获1付出',
  `before_money` int(10) NOT NULL DEFAULT '0' COMMENT '操作金币前金币数',
  `money` int(11) NOT NULL COMMENT '加减金币数',
  `after_money` int(11) NOT NULL DEFAULT '0' COMMENT '操作金币后数据',
  `wtime` int(10) unsigned NOT NULL COMMENT '输赢时间',
  `wdesc` varchar(255) NOT NULL COMMENT '附加信息:比如邀请好友的好友的sitemid,便于页面显示',
  PRIMARY KEY (`wid`),
  KEY `mid` (`mid`),
  KEY `wtime` (`wtime`),
  KEY `pid` (`pid`),
  KEY `ctype` (`ctype`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='用户输赢记录表';

-- ----------------------------
-- Table structure for log_win4
-- ----------------------------
DROP TABLE IF EXISTS `log_win4`;
CREATE TABLE `log_win4` (
  `wid` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `gameid` int(11) NOT NULL COMMENT '游戏ID  1梭哈 2百家乐 3 斗地主 ',
  `mid` int(10) unsigned NOT NULL COMMENT '用户ID',
  `sid` smallint(3) unsigned NOT NULL DEFAULT '10' COMMENT '站点ID',
  `cid` int(11) NOT NULL COMMENT '渠道ID',
  `pid` int(11) NOT NULL COMMENT '包ID',
  `level` int(11) NOT NULL DEFAULT '0' COMMENT 'level id',
  `tid` int(11) NOT NULL DEFAULT '0' COMMENT '桌子id',
  `ctype` tinyint(4) NOT NULL COMMENT '客户端类型ID',
  `wmode` smallint(3) unsigned NOT NULL COMMENT '标识',
  `wflag` tinyint(3) unsigned NOT NULL COMMENT '0收获1付出',
  `before_money` int(10) NOT NULL DEFAULT '0' COMMENT '操作金币前金币数',
  `money` int(11) NOT NULL COMMENT '加减金币数',
  `after_money` int(11) NOT NULL DEFAULT '0' COMMENT '操作金币后数据',
  `wtime` int(10) unsigned NOT NULL COMMENT '输赢时间',
  `wdesc` varchar(255) NOT NULL COMMENT '附加信息:比如邀请好友的好友的sitemid,便于页面显示',
  PRIMARY KEY (`wid`),
  KEY `mid` (`mid`),
  KEY `wtime` (`wtime`),
  KEY `pid` (`pid`),
  KEY `ctype` (`ctype`),
  KEY `cid` (`cid`)
) ENGINE=InnoDB AUTO_INCREMENT=53 DEFAULT CHARSET=utf8 COMMENT='用户输赢记录表';

-- ----------------------------
-- Table structure for log_win5
-- ----------------------------
DROP TABLE IF EXISTS `log_win5`;
CREATE TABLE `log_win5` (
  `wid` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `gameid` int(11) NOT NULL COMMENT '游戏ID  1梭哈 2百家乐 3 斗地主 ',
  `mid` int(10) unsigned NOT NULL COMMENT '用户ID',
  `sid` smallint(3) unsigned NOT NULL DEFAULT '10' COMMENT '站点ID',
  `cid` int(11) NOT NULL COMMENT '渠道ID',
  `pid` int(11) NOT NULL COMMENT '包ID',
  `level` int(11) NOT NULL DEFAULT '0' COMMENT 'level id',
  `tid` int(11) NOT NULL DEFAULT '0' COMMENT '桌子id',
  `ctype` tinyint(4) NOT NULL COMMENT '客户端类型ID',
  `wmode` smallint(3) unsigned NOT NULL COMMENT '标识',
  `wflag` tinyint(3) unsigned NOT NULL COMMENT '0收获1付出',
  `before_money` int(10) NOT NULL DEFAULT '0' COMMENT '操作金币前金币数',
  `money` int(11) NOT NULL COMMENT '加减金币数',
  `after_money` int(11) NOT NULL DEFAULT '0' COMMENT '操作金币后数据',
  `wtime` int(10) unsigned NOT NULL COMMENT '输赢时间',
  `wdesc` varchar(255) NOT NULL COMMENT '附加信息:比如邀请好友的好友的sitemid,便于页面显示',
  PRIMARY KEY (`wid`),
  KEY `mid` (`mid`),
  KEY `wtime` (`wtime`),
  KEY `pid` (`pid`),
  KEY `ctype` (`ctype`),
  KEY `cid` (`cid`)
) ENGINE=InnoDB AUTO_INCREMENT=1475781 DEFAULT CHARSET=utf8 COMMENT='用户输赢记录表';

-- ----------------------------
-- Table structure for log_win6
-- ----------------------------
DROP TABLE IF EXISTS `log_win6`;
CREATE TABLE `log_win6` (
  `wid` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `gameid` int(11) NOT NULL COMMENT '游戏ID  1梭哈 2百家乐 3 斗地主 ',
  `mid` int(10) unsigned NOT NULL COMMENT '用户ID',
  `sid` smallint(3) unsigned NOT NULL DEFAULT '10' COMMENT '站点ID',
  `cid` int(11) NOT NULL COMMENT '渠道ID',
  `pid` int(11) NOT NULL COMMENT '包ID',
  `level` int(11) NOT NULL DEFAULT '0' COMMENT 'level id',
  `tid` int(11) NOT NULL DEFAULT '0' COMMENT '桌子id',
  `ctype` tinyint(4) NOT NULL COMMENT '客户端类型ID',
  `wmode` smallint(3) unsigned NOT NULL COMMENT '标识',
  `wflag` tinyint(3) unsigned NOT NULL COMMENT '0收获1付出',
  `before_money` int(10) NOT NULL DEFAULT '0' COMMENT '操作金币前金币数',
  `money` int(11) NOT NULL COMMENT '加减金币数',
  `after_money` int(11) NOT NULL DEFAULT '0' COMMENT '操作金币后数据',
  `wtime` int(10) unsigned NOT NULL COMMENT '输赢时间',
  `wdesc` varchar(255) NOT NULL COMMENT '附加信息:比如邀请好友的好友的sitemid,便于页面显示',
  PRIMARY KEY (`wid`),
  KEY `mid` (`mid`),
  KEY `wtime` (`wtime`),
  KEY `pid` (`pid`),
  KEY `ctype` (`ctype`)
) ENGINE=InnoDB AUTO_INCREMENT=1304 DEFAULT CHARSET=utf8 COMMENT='用户输赢记录表';

-- ----------------------------
-- Table structure for log_win7
-- ----------------------------
DROP TABLE IF EXISTS `log_win7`;
CREATE TABLE `log_win7` (
  `wid` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `gameid` int(11) NOT NULL COMMENT '游戏ID  1梭哈 2百家乐 3 斗地主 ',
  `mid` int(10) unsigned NOT NULL COMMENT '用户ID',
  `sid` smallint(3) unsigned NOT NULL DEFAULT '10' COMMENT '站点ID',
  `cid` int(11) NOT NULL COMMENT '渠道ID',
  `pid` int(11) NOT NULL COMMENT '包ID',
  `level` int(11) NOT NULL DEFAULT '0' COMMENT 'level id',
  `tid` int(11) NOT NULL DEFAULT '0' COMMENT '桌子id',
  `ctype` tinyint(4) NOT NULL COMMENT '客户端类型ID',
  `wmode` smallint(3) unsigned NOT NULL COMMENT '标识',
  `wflag` tinyint(3) unsigned NOT NULL COMMENT '0收获1付出',
  `before_money` int(10) NOT NULL DEFAULT '0' COMMENT '操作金币前金币数',
  `money` int(11) NOT NULL COMMENT '加减金币数',
  `after_money` int(11) NOT NULL DEFAULT '0' COMMENT '操作金币后数据',
  `wtime` int(10) unsigned NOT NULL COMMENT '输赢时间',
  `wdesc` varchar(255) NOT NULL COMMENT '附加信息:比如邀请好友的好友的sitemid,便于页面显示',
  PRIMARY KEY (`wid`),
  KEY `mid` (`mid`),
  KEY `wtime` (`wtime`),
  KEY `pid` (`pid`),
  KEY `ctype` (`ctype`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='用户输赢记录表';

-- ----------------------------
-- Table structure for log_win8
-- ----------------------------
DROP TABLE IF EXISTS `log_win8`;
CREATE TABLE `log_win8` (
  `wid` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `gameid` int(11) NOT NULL COMMENT '游戏ID  1梭哈 2百家乐 3 斗地主 ',
  `mid` int(10) unsigned NOT NULL COMMENT '用户ID',
  `sid` smallint(3) unsigned NOT NULL DEFAULT '10' COMMENT '站点ID',
  `cid` int(11) NOT NULL COMMENT '渠道ID',
  `pid` int(11) NOT NULL COMMENT '包ID',
  `level` int(11) NOT NULL DEFAULT '0' COMMENT 'level id',
  `tid` int(11) NOT NULL DEFAULT '0' COMMENT '桌子id',
  `ctype` tinyint(4) NOT NULL COMMENT '客户端类型ID',
  `wmode` smallint(3) unsigned NOT NULL COMMENT '标识',
  `wflag` tinyint(3) unsigned NOT NULL COMMENT '0收获1付出',
  `before_money` int(10) NOT NULL DEFAULT '0' COMMENT '操作金币前金币数',
  `money` int(11) NOT NULL COMMENT '加减金币数',
  `after_money` int(11) NOT NULL DEFAULT '0' COMMENT '操作金币后数据',
  `wtime` int(10) unsigned NOT NULL COMMENT '输赢时间',
  `wdesc` varchar(255) NOT NULL COMMENT '附加信息:比如邀请好友的好友的sitemid,便于页面显示',
  PRIMARY KEY (`wid`),
  KEY `mid` (`mid`),
  KEY `wtime` (`wtime`),
  KEY `pid` (`pid`),
  KEY `ctype` (`ctype`)
) ENGINE=InnoDB AUTO_INCREMENT=173 DEFAULT CHARSET=utf8 COMMENT='用户输赢记录表';

-- ----------------------------
-- Table structure for log_win9
-- ----------------------------
DROP TABLE IF EXISTS `log_win9`;
CREATE TABLE `log_win9` (
  `wid` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `gameid` int(11) NOT NULL COMMENT '游戏ID  1梭哈 2百家乐 3 斗地主 ',
  `mid` int(10) unsigned NOT NULL COMMENT '用户ID',
  `sid` smallint(3) unsigned NOT NULL DEFAULT '10' COMMENT '站点ID',
  `cid` int(11) NOT NULL COMMENT '渠道ID',
  `pid` int(11) NOT NULL COMMENT '包ID',
  `level` int(11) NOT NULL DEFAULT '0' COMMENT 'level id',
  `tid` int(11) NOT NULL DEFAULT '0' COMMENT '桌子id',
  `ctype` tinyint(4) NOT NULL COMMENT '客户端类型ID',
  `wmode` smallint(3) unsigned NOT NULL COMMENT '标识',
  `wflag` tinyint(3) unsigned NOT NULL COMMENT '0收获1付出',
  `before_money` int(10) NOT NULL DEFAULT '0' COMMENT '操作金币前金币数',
  `money` int(11) NOT NULL COMMENT '加减金币数',
  `after_money` int(11) NOT NULL DEFAULT '0' COMMENT '操作金币后数据',
  `wtime` int(10) unsigned NOT NULL COMMENT '输赢时间',
  `wdesc` varchar(255) NOT NULL COMMENT '附加信息:比如邀请好友的好友的sitemid,便于页面显示',
  PRIMARY KEY (`wid`),
  KEY `mid` (`mid`),
  KEY `wtime` (`wtime`),
  KEY `pid` (`pid`),
  KEY `ctype` (`ctype`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8 COMMENT='用户输赢记录表';

-- ----------------------------
-- Table structure for log_win_tmp
-- ----------------------------
DROP TABLE IF EXISTS `log_win_tmp`;
CREATE TABLE `log_win_tmp` (
  `gameid` int(11) NOT NULL COMMENT '游戏ID  1梭哈 2百家乐 3 斗地主 ',
  `mid` int(10) unsigned NOT NULL COMMENT '用户ID',
  `sid` smallint(3) unsigned NOT NULL DEFAULT '10' COMMENT '站点ID',
  `cid` int(11) NOT NULL COMMENT '渠道ID',
  `pid` int(11) NOT NULL COMMENT '包ID',
  `ctype` tinyint(4) NOT NULL COMMENT '客户端类型ID',
  `wmode` smallint(3) unsigned NOT NULL COMMENT '标识',
  `wflag` tinyint(3) unsigned NOT NULL COMMENT '0收获1付出',
  `money` int(10) NOT NULL COMMENT '加减金币数',
  `wtime` int(10) unsigned NOT NULL COMMENT '输赢时间',
  `wdesc` varchar(255) NOT NULL COMMENT '附加信息:比如邀请好友的好友的sitemid,便于页面显示',
  KEY `mid` (`mid`),
  KEY `wtime` (`wtime`),
  KEY `pid` (`pid`),
  KEY `ctype` (`ctype`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='用户输赢记录表';
