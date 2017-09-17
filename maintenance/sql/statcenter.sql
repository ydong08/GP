/*
Navicat MySQL Data Transfer

Source Server         : 192.168.114.51
Source Server Version : 50620
Source Host           : 192.168.114.51:3306
Source Database       : statcenter

Target Server Type    : MYSQL
Target Server Version : 50620
File Encoding         : 65001

Date: 2017-03-21 16:39:14
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for active_day_0
-- ----------------------------
DROP TABLE IF EXISTS `active_day_0`;
CREATE TABLE `active_day_0` (
  `stat_datestr` varchar(29) NOT NULL,
  `actives` bigint(20) NOT NULL DEFAULT '0' COMMENT '活跃数',
  `tax` bigint(20) NOT NULL DEFAULT '0' COMMENT '台费',
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `gameid` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  PRIMARY KEY (`stat_datestr`,`productid`,`level`,`gameid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for active_day_1
-- ----------------------------
DROP TABLE IF EXISTS `active_day_1`;
CREATE TABLE `active_day_1` (
  `stat_datestr` varchar(29) NOT NULL,
  `actives` bigint(20) NOT NULL DEFAULT '0' COMMENT '活跃数',
  `tax` bigint(20) NOT NULL DEFAULT '0' COMMENT '台费',
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `gameid` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  PRIMARY KEY (`stat_datestr`,`productid`,`level`,`gameid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for active_day_10
-- ----------------------------
DROP TABLE IF EXISTS `active_day_10`;
CREATE TABLE `active_day_10` (
  `stat_datestr` varchar(29) NOT NULL,
  `actives` bigint(20) NOT NULL DEFAULT '0' COMMENT '活跃数',
  `tax` bigint(20) NOT NULL DEFAULT '0' COMMENT '台费',
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `gameid` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  PRIMARY KEY (`stat_datestr`,`productid`,`level`,`gameid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for active_day_2
-- ----------------------------
DROP TABLE IF EXISTS `active_day_2`;
CREATE TABLE `active_day_2` (
  `stat_datestr` varchar(29) NOT NULL,
  `actives` bigint(20) NOT NULL DEFAULT '0' COMMENT '活跃数',
  `tax` bigint(20) NOT NULL DEFAULT '0' COMMENT '台费',
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `gameid` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  PRIMARY KEY (`stat_datestr`,`productid`,`level`,`gameid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for active_day_3
-- ----------------------------
DROP TABLE IF EXISTS `active_day_3`;
CREATE TABLE `active_day_3` (
  `stat_datestr` varchar(29) NOT NULL,
  `actives` bigint(20) NOT NULL DEFAULT '0' COMMENT '活跃数',
  `tax` bigint(20) NOT NULL DEFAULT '0' COMMENT '台费',
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `gameid` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  PRIMARY KEY (`stat_datestr`,`productid`,`level`,`gameid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for active_day_4
-- ----------------------------
DROP TABLE IF EXISTS `active_day_4`;
CREATE TABLE `active_day_4` (
  `stat_datestr` varchar(29) NOT NULL,
  `actives` bigint(20) NOT NULL DEFAULT '0' COMMENT '活跃数',
  `tax` bigint(20) NOT NULL DEFAULT '0' COMMENT '台费',
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `gameid` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  PRIMARY KEY (`stat_datestr`,`productid`,`level`,`gameid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for active_day_5
-- ----------------------------
DROP TABLE IF EXISTS `active_day_5`;
CREATE TABLE `active_day_5` (
  `stat_datestr` varchar(29) NOT NULL,
  `actives` bigint(20) NOT NULL DEFAULT '0' COMMENT '活跃数',
  `tax` bigint(20) NOT NULL DEFAULT '0' COMMENT '台费',
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `gameid` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  PRIMARY KEY (`stat_datestr`,`productid`,`level`,`gameid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for active_day_6
-- ----------------------------
DROP TABLE IF EXISTS `active_day_6`;
CREATE TABLE `active_day_6` (
  `stat_datestr` varchar(29) NOT NULL,
  `actives` bigint(20) NOT NULL DEFAULT '0' COMMENT '活跃数',
  `tax` bigint(20) NOT NULL DEFAULT '0' COMMENT '台费',
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `gameid` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  PRIMARY KEY (`stat_datestr`,`productid`,`level`,`gameid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for active_day_7
-- ----------------------------
DROP TABLE IF EXISTS `active_day_7`;
CREATE TABLE `active_day_7` (
  `stat_datestr` varchar(29) NOT NULL,
  `actives` bigint(20) NOT NULL DEFAULT '0' COMMENT '活跃数',
  `tax` bigint(20) NOT NULL DEFAULT '0' COMMENT '台费',
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `gameid` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  PRIMARY KEY (`stat_datestr`,`productid`,`level`,`gameid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for active_day_8
-- ----------------------------
DROP TABLE IF EXISTS `active_day_8`;
CREATE TABLE `active_day_8` (
  `stat_datestr` varchar(29) NOT NULL,
  `actives` bigint(20) NOT NULL DEFAULT '0' COMMENT '活跃数',
  `tax` bigint(20) NOT NULL DEFAULT '0' COMMENT '台费',
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `gameid` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  PRIMARY KEY (`stat_datestr`,`productid`,`level`,`gameid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for active_day_9
-- ----------------------------
DROP TABLE IF EXISTS `active_day_9`;
CREATE TABLE `active_day_9` (
  `stat_datestr` varchar(29) NOT NULL,
  `actives` bigint(20) NOT NULL DEFAULT '0' COMMENT '活跃数',
  `tax` bigint(20) NOT NULL DEFAULT '0' COMMENT '台费',
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `gameid` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  PRIMARY KEY (`stat_datestr`,`productid`,`level`,`gameid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for active_hour_0
-- ----------------------------
DROP TABLE IF EXISTS `active_hour_0`;
CREATE TABLE `active_hour_0` (
  `stat_datestr` varchar(29) NOT NULL,
  `actives` bigint(20) NOT NULL DEFAULT '0' COMMENT '活跃数',
  `tax` bigint(20) NOT NULL DEFAULT '0' COMMENT '台费',
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `gameid` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  PRIMARY KEY (`stat_datestr`,`productid`,`level`,`gameid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for active_hour_1
-- ----------------------------
DROP TABLE IF EXISTS `active_hour_1`;
CREATE TABLE `active_hour_1` (
  `stat_datestr` varchar(29) NOT NULL,
  `actives` bigint(20) NOT NULL DEFAULT '0' COMMENT '活跃数',
  `tax` bigint(20) NOT NULL DEFAULT '0' COMMENT '台费',
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `gameid` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  PRIMARY KEY (`stat_datestr`,`productid`,`level`,`gameid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for active_hour_10
-- ----------------------------
DROP TABLE IF EXISTS `active_hour_10`;
CREATE TABLE `active_hour_10` (
  `stat_datestr` varchar(29) NOT NULL,
  `actives` bigint(20) NOT NULL DEFAULT '0' COMMENT '活跃数',
  `tax` bigint(20) NOT NULL DEFAULT '0' COMMENT '台费',
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `gameid` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  PRIMARY KEY (`stat_datestr`,`productid`,`level`,`gameid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for active_hour_2
-- ----------------------------
DROP TABLE IF EXISTS `active_hour_2`;
CREATE TABLE `active_hour_2` (
  `stat_datestr` varchar(29) NOT NULL,
  `actives` bigint(20) NOT NULL DEFAULT '0' COMMENT '活跃数',
  `tax` bigint(20) NOT NULL DEFAULT '0' COMMENT '台费',
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `gameid` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  PRIMARY KEY (`stat_datestr`,`productid`,`level`,`gameid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for active_hour_3
-- ----------------------------
DROP TABLE IF EXISTS `active_hour_3`;
CREATE TABLE `active_hour_3` (
  `stat_datestr` varchar(29) NOT NULL,
  `actives` bigint(20) NOT NULL DEFAULT '0' COMMENT '活跃数',
  `tax` bigint(20) NOT NULL DEFAULT '0' COMMENT '台费',
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `gameid` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  PRIMARY KEY (`stat_datestr`,`productid`,`level`,`gameid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for active_hour_4
-- ----------------------------
DROP TABLE IF EXISTS `active_hour_4`;
CREATE TABLE `active_hour_4` (
  `stat_datestr` varchar(29) NOT NULL,
  `actives` bigint(20) NOT NULL DEFAULT '0' COMMENT '活跃数',
  `tax` bigint(20) NOT NULL DEFAULT '0' COMMENT '台费',
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `gameid` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  PRIMARY KEY (`stat_datestr`,`productid`,`level`,`gameid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for active_hour_5
-- ----------------------------
DROP TABLE IF EXISTS `active_hour_5`;
CREATE TABLE `active_hour_5` (
  `stat_datestr` varchar(29) NOT NULL,
  `actives` bigint(20) NOT NULL DEFAULT '0' COMMENT '活跃数',
  `tax` bigint(20) NOT NULL DEFAULT '0' COMMENT '台费',
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `gameid` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  PRIMARY KEY (`stat_datestr`,`productid`,`level`,`gameid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for active_hour_6
-- ----------------------------
DROP TABLE IF EXISTS `active_hour_6`;
CREATE TABLE `active_hour_6` (
  `stat_datestr` varchar(29) NOT NULL,
  `actives` bigint(20) NOT NULL DEFAULT '0' COMMENT '活跃数',
  `tax` bigint(20) NOT NULL DEFAULT '0' COMMENT '台费',
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `gameid` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  PRIMARY KEY (`stat_datestr`,`productid`,`level`,`gameid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for active_hour_7
-- ----------------------------
DROP TABLE IF EXISTS `active_hour_7`;
CREATE TABLE `active_hour_7` (
  `stat_datestr` varchar(29) NOT NULL,
  `actives` bigint(20) NOT NULL DEFAULT '0' COMMENT '活跃数',
  `tax` bigint(20) NOT NULL DEFAULT '0' COMMENT '台费',
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `gameid` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  PRIMARY KEY (`stat_datestr`,`productid`,`level`,`gameid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for active_hour_8
-- ----------------------------
DROP TABLE IF EXISTS `active_hour_8`;
CREATE TABLE `active_hour_8` (
  `stat_datestr` varchar(29) NOT NULL,
  `actives` bigint(20) NOT NULL DEFAULT '0' COMMENT '活跃数',
  `tax` bigint(20) NOT NULL DEFAULT '0' COMMENT '台费',
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `gameid` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  PRIMARY KEY (`stat_datestr`,`productid`,`level`,`gameid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for active_hour_9
-- ----------------------------
DROP TABLE IF EXISTS `active_hour_9`;
CREATE TABLE `active_hour_9` (
  `stat_datestr` varchar(29) NOT NULL,
  `actives` bigint(20) NOT NULL DEFAULT '0' COMMENT '活跃数',
  `tax` bigint(20) NOT NULL DEFAULT '0' COMMENT '台费',
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `gameid` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  PRIMARY KEY (`stat_datestr`,`productid`,`level`,`gameid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for active_month_0
-- ----------------------------
DROP TABLE IF EXISTS `active_month_0`;
CREATE TABLE `active_month_0` (
  `stat_datestr` varchar(29) NOT NULL,
  `actives` bigint(20) NOT NULL DEFAULT '0' COMMENT '活跃数',
  `tax` bigint(20) NOT NULL DEFAULT '0' COMMENT '台费',
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `gameid` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  PRIMARY KEY (`stat_datestr`,`productid`,`level`,`gameid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for active_month_1
-- ----------------------------
DROP TABLE IF EXISTS `active_month_1`;
CREATE TABLE `active_month_1` (
  `stat_datestr` varchar(29) NOT NULL,
  `actives` bigint(20) NOT NULL DEFAULT '0' COMMENT '活跃数',
  `tax` bigint(20) NOT NULL DEFAULT '0' COMMENT '台费',
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `gameid` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  PRIMARY KEY (`stat_datestr`,`productid`,`level`,`gameid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for active_month_10
-- ----------------------------
DROP TABLE IF EXISTS `active_month_10`;
CREATE TABLE `active_month_10` (
  `stat_datestr` varchar(29) NOT NULL,
  `actives` bigint(20) NOT NULL DEFAULT '0' COMMENT '活跃数',
  `tax` bigint(20) NOT NULL DEFAULT '0' COMMENT '台费',
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `gameid` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  PRIMARY KEY (`stat_datestr`,`productid`,`level`,`gameid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for active_month_2
-- ----------------------------
DROP TABLE IF EXISTS `active_month_2`;
CREATE TABLE `active_month_2` (
  `stat_datestr` varchar(29) NOT NULL,
  `actives` bigint(20) NOT NULL DEFAULT '0' COMMENT '活跃数',
  `tax` bigint(20) NOT NULL DEFAULT '0' COMMENT '台费',
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `gameid` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  PRIMARY KEY (`stat_datestr`,`productid`,`level`,`gameid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for active_month_3
-- ----------------------------
DROP TABLE IF EXISTS `active_month_3`;
CREATE TABLE `active_month_3` (
  `stat_datestr` varchar(29) NOT NULL,
  `actives` bigint(20) NOT NULL DEFAULT '0' COMMENT '活跃数',
  `tax` bigint(20) NOT NULL DEFAULT '0' COMMENT '台费',
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `gameid` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  PRIMARY KEY (`stat_datestr`,`productid`,`level`,`gameid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for active_month_4
-- ----------------------------
DROP TABLE IF EXISTS `active_month_4`;
CREATE TABLE `active_month_4` (
  `stat_datestr` varchar(29) NOT NULL,
  `actives` bigint(20) NOT NULL DEFAULT '0' COMMENT '活跃数',
  `tax` bigint(20) NOT NULL DEFAULT '0' COMMENT '台费',
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `gameid` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  PRIMARY KEY (`stat_datestr`,`productid`,`level`,`gameid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for active_month_5
-- ----------------------------
DROP TABLE IF EXISTS `active_month_5`;
CREATE TABLE `active_month_5` (
  `stat_datestr` varchar(29) NOT NULL,
  `actives` bigint(20) NOT NULL DEFAULT '0' COMMENT '活跃数',
  `tax` bigint(20) NOT NULL DEFAULT '0' COMMENT '台费',
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `gameid` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  PRIMARY KEY (`stat_datestr`,`productid`,`level`,`gameid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for active_month_6
-- ----------------------------
DROP TABLE IF EXISTS `active_month_6`;
CREATE TABLE `active_month_6` (
  `stat_datestr` varchar(29) NOT NULL,
  `actives` bigint(20) NOT NULL DEFAULT '0' COMMENT '活跃数',
  `tax` bigint(20) NOT NULL DEFAULT '0' COMMENT '台费',
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `gameid` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  PRIMARY KEY (`stat_datestr`,`productid`,`level`,`gameid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for active_month_7
-- ----------------------------
DROP TABLE IF EXISTS `active_month_7`;
CREATE TABLE `active_month_7` (
  `stat_datestr` varchar(29) NOT NULL,
  `actives` bigint(20) NOT NULL DEFAULT '0' COMMENT '活跃数',
  `tax` bigint(20) NOT NULL DEFAULT '0' COMMENT '台费',
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `gameid` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  PRIMARY KEY (`stat_datestr`,`productid`,`level`,`gameid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for active_month_8
-- ----------------------------
DROP TABLE IF EXISTS `active_month_8`;
CREATE TABLE `active_month_8` (
  `stat_datestr` varchar(29) NOT NULL,
  `actives` bigint(20) NOT NULL DEFAULT '0' COMMENT '活跃数',
  `tax` bigint(20) NOT NULL DEFAULT '0' COMMENT '台费',
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `gameid` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  PRIMARY KEY (`stat_datestr`,`productid`,`level`,`gameid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for active_month_9
-- ----------------------------
DROP TABLE IF EXISTS `active_month_9`;
CREATE TABLE `active_month_9` (
  `stat_datestr` varchar(29) NOT NULL,
  `actives` bigint(20) NOT NULL DEFAULT '0' COMMENT '活跃数',
  `tax` bigint(20) NOT NULL DEFAULT '0' COMMENT '台费',
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `gameid` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  PRIMARY KEY (`stat_datestr`,`productid`,`level`,`gameid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for online_number
-- ----------------------------
DROP TABLE IF EXISTS `online_number`;
CREATE TABLE `online_number` (
  `time` int(15) DEFAULT NULL,
  `num` int(10) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for rank
-- ----------------------------
DROP TABLE IF EXISTS `rank`;
CREATE TABLE `rank` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `productid` int(11) NOT NULL DEFAULT '0' COMMENT '产品id',
  `mid` bigint(20) NOT NULL DEFAULT '0' COMMENT '用户id',
  `score` float(20,2) NOT NULL DEFAULT '0.00' COMMENT '分数',
  `rank` int(10) NOT NULL DEFAULT '0' COMMENT '名次',
  `date` date NOT NULL DEFAULT '0000-00-00' COMMENT '日期',
  `status` int(2) NOT NULL DEFAULT '0' COMMENT '0未发送，1已发送成功',
  `money` int(10) NOT NULL DEFAULT '0' COMMENT '需发送用户的奖励，分计算',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for rank_product
-- ----------------------------
DROP TABLE IF EXISTS `rank_product`;
CREATE TABLE `rank_product` (
  `id` bigint(20) NOT NULL,
  `productid` int(11) NOT NULL DEFAULT '0' COMMENT '产品id',
  `mid` bigint(20) NOT NULL DEFAULT '0' COMMENT '用户id',
  `score` float(20,2) NOT NULL DEFAULT '0.00' COMMENT '分数',
  `rank` int(10) NOT NULL DEFAULT '0' COMMENT '名次',
  `date` date NOT NULL DEFAULT '0000-00-00' COMMENT '日期',
  `status` int(2) NOT NULL DEFAULT '0' COMMENT '0未发送，1已发送成功',
  `money` int(10) NOT NULL DEFAULT '0' COMMENT '需发送用户的奖励，分计算',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for stat_register
-- ----------------------------
DROP TABLE IF EXISTS `stat_register`;
CREATE TABLE `stat_register` (
  `mid` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '用户id',
  `productid` int(10) NOT NULL DEFAULT '0' COMMENT '产品id',
  `ip` bigint(20) NOT NULL DEFAULT '0' COMMENT '注册ip',
  `deviceno` varchar(64) NOT NULL DEFAULT '' COMMENT '设备号',
  `ctime` bigint(20) NOT NULL DEFAULT '0' COMMENT '创建时间',
  PRIMARY KEY (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for taxandactive_all
-- ----------------------------
DROP TABLE IF EXISTS `taxandactive_all`;
CREATE TABLE `taxandactive_all` (
  `productid` int(10) NOT NULL,
  `gamecount` int(10) NOT NULL DEFAULT '0' COMMENT '累计游戏人数',
  `gamecount_charge` int(10) NOT NULL DEFAULT '0' COMMENT '累计充值游戏人数',
  `cur_gamecount` int(10) NOT NULL DEFAULT '0' COMMENT '当日游戏人数',
  `cur_gamecount_charge` int(10) NOT NULL DEFAULT '0' COMMENT '当日充值游戏人数',
  `cur_newplayers` int(10) NOT NULL DEFAULT '0' COMMENT '当日新游戏人数',
  `cur_oldplayers` int(10) NOT NULL DEFAULT '0' COMMENT '当日老充值游戏人数',
  `all_taxs` bigint(20) NOT NULL DEFAULT '0' COMMENT '累计服务器收入',
  `cur_taxs` bigint(20) NOT NULL DEFAULT '0' COMMENT '累计当日服务器收入',
  `date` varchar(50) NOT NULL COMMENT ' 时间',
  PRIMARY KEY (`productid`,`date`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for taxandactive_day_1
-- ----------------------------
DROP TABLE IF EXISTS `taxandactive_day_1`;
CREATE TABLE `taxandactive_day_1` (
  `gameid` bigint(20) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  `cur_gamecount` int(10) NOT NULL DEFAULT '0' COMMENT '当日游戏数',
  `cur_gamecount_charge` int(10) NOT NULL DEFAULT '1' COMMENT '当日充值游戏数',
  `cur_newplayers` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `cur_oldplayers` int(10) NOT NULL,
  `all_sumtax` bigint(20) NOT NULL,
  `cur_sumtax` bigint(20) NOT NULL,
  `productid` int(10) NOT NULL,
  `date` varchar(40) NOT NULL,
  PRIMARY KEY (`gameid`,`level`,`productid`,`date`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for taxandactive_day_10
-- ----------------------------
DROP TABLE IF EXISTS `taxandactive_day_10`;
CREATE TABLE `taxandactive_day_10` (
  `gameid` bigint(20) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  `cur_gamecount` int(10) NOT NULL DEFAULT '0' COMMENT '当日游戏数',
  `cur_gamecount_charge` int(10) NOT NULL DEFAULT '1' COMMENT '当日充值游戏数',
  `cur_newplayers` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `cur_oldplayers` int(10) NOT NULL,
  `all_sumtax` bigint(20) NOT NULL,
  `cur_sumtax` bigint(20) NOT NULL,
  `productid` int(10) NOT NULL,
  `date` varchar(40) NOT NULL,
  PRIMARY KEY (`productid`,`gameid`,`level`,`date`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for taxandactive_day_2
-- ----------------------------
DROP TABLE IF EXISTS `taxandactive_day_2`;
CREATE TABLE `taxandactive_day_2` (
  `gameid` bigint(20) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  `cur_gamecount` int(10) NOT NULL DEFAULT '0' COMMENT '当日游戏数',
  `cur_gamecount_charge` int(10) NOT NULL DEFAULT '1' COMMENT '当日充值游戏数',
  `cur_newplayers` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `cur_oldplayers` int(10) NOT NULL,
  `all_sumtax` bigint(20) NOT NULL,
  `cur_sumtax` bigint(20) NOT NULL,
  `productid` int(10) NOT NULL,
  `date` varchar(40) NOT NULL,
  PRIMARY KEY (`gameid`,`level`,`productid`,`date`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for taxandactive_day_3
-- ----------------------------
DROP TABLE IF EXISTS `taxandactive_day_3`;
CREATE TABLE `taxandactive_day_3` (
  `gameid` bigint(20) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  `cur_gamecount` int(10) NOT NULL DEFAULT '0' COMMENT '当日游戏数',
  `cur_gamecount_charge` int(10) NOT NULL DEFAULT '1' COMMENT '当日充值游戏数',
  `cur_newplayers` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `cur_oldplayers` int(10) NOT NULL,
  `all_sumtax` bigint(20) NOT NULL,
  `cur_sumtax` bigint(20) NOT NULL,
  `productid` int(10) NOT NULL,
  `date` varchar(40) NOT NULL,
  PRIMARY KEY (`gameid`,`level`,`productid`,`date`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for taxandactive_day_4
-- ----------------------------
DROP TABLE IF EXISTS `taxandactive_day_4`;
CREATE TABLE `taxandactive_day_4` (
  `gameid` bigint(20) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  `cur_gamecount` int(10) NOT NULL DEFAULT '0' COMMENT '当日游戏数',
  `cur_gamecount_charge` int(10) NOT NULL DEFAULT '1' COMMENT '当日充值游戏数',
  `cur_newplayers` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `cur_oldplayers` int(10) NOT NULL,
  `all_sumtax` bigint(20) NOT NULL,
  `cur_sumtax` bigint(20) NOT NULL,
  `productid` int(10) NOT NULL,
  `date` varchar(40) NOT NULL,
  PRIMARY KEY (`gameid`,`level`,`productid`,`date`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for taxandactive_day_5
-- ----------------------------
DROP TABLE IF EXISTS `taxandactive_day_5`;
CREATE TABLE `taxandactive_day_5` (
  `gameid` bigint(20) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  `cur_gamecount` int(10) NOT NULL DEFAULT '0' COMMENT '当日游戏数',
  `cur_gamecount_charge` int(10) NOT NULL DEFAULT '1' COMMENT '当日充值游戏数',
  `cur_newplayers` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `cur_oldplayers` int(10) NOT NULL,
  `all_sumtax` bigint(20) NOT NULL,
  `cur_sumtax` bigint(20) NOT NULL,
  `productid` int(10) NOT NULL,
  `date` varchar(40) NOT NULL,
  PRIMARY KEY (`gameid`,`level`,`productid`,`date`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for taxandactive_day_6
-- ----------------------------
DROP TABLE IF EXISTS `taxandactive_day_6`;
CREATE TABLE `taxandactive_day_6` (
  `gameid` bigint(20) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  `cur_gamecount` int(10) NOT NULL DEFAULT '0' COMMENT '当日游戏数',
  `cur_gamecount_charge` int(10) NOT NULL DEFAULT '1' COMMENT '当日充值游戏数',
  `cur_newplayers` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `cur_oldplayers` int(10) NOT NULL,
  `all_sumtax` bigint(20) NOT NULL,
  `cur_sumtax` bigint(20) NOT NULL,
  `productid` int(10) NOT NULL,
  `date` varchar(40) NOT NULL,
  PRIMARY KEY (`gameid`,`level`,`productid`,`date`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for taxandactive_day_7
-- ----------------------------
DROP TABLE IF EXISTS `taxandactive_day_7`;
CREATE TABLE `taxandactive_day_7` (
  `gameid` bigint(20) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  `cur_gamecount` int(10) NOT NULL DEFAULT '0' COMMENT '当日游戏数',
  `cur_gamecount_charge` int(10) NOT NULL DEFAULT '1' COMMENT '当日充值游戏数',
  `cur_newplayers` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `cur_oldplayers` int(10) NOT NULL,
  `all_sumtax` bigint(20) NOT NULL,
  `cur_sumtax` bigint(20) NOT NULL,
  `productid` int(10) NOT NULL,
  `date` varchar(40) NOT NULL,
  PRIMARY KEY (`gameid`,`level`,`productid`,`date`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for taxandactive_day_8
-- ----------------------------
DROP TABLE IF EXISTS `taxandactive_day_8`;
CREATE TABLE `taxandactive_day_8` (
  `gameid` bigint(20) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  `cur_gamecount` int(10) NOT NULL DEFAULT '0' COMMENT '当日游戏数',
  `cur_gamecount_charge` int(10) NOT NULL DEFAULT '1' COMMENT '当日充值游戏数',
  `cur_newplayers` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `cur_oldplayers` int(10) NOT NULL,
  `all_sumtax` bigint(20) NOT NULL,
  `cur_sumtax` bigint(20) NOT NULL,
  `productid` int(10) NOT NULL,
  `date` varchar(40) NOT NULL,
  PRIMARY KEY (`gameid`,`level`,`productid`,`date`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for taxandactive_day_9
-- ----------------------------
DROP TABLE IF EXISTS `taxandactive_day_9`;
CREATE TABLE `taxandactive_day_9` (
  `gameid` bigint(20) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `level` int(10) NOT NULL,
  `cur_gamecount` int(10) NOT NULL DEFAULT '0' COMMENT '当日游戏数',
  `cur_gamecount_charge` int(10) NOT NULL DEFAULT '1' COMMENT '当日充值游戏数',
  `cur_newplayers` int(10) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `cur_oldplayers` int(10) NOT NULL,
  `all_sumtax` bigint(20) NOT NULL,
  `cur_sumtax` bigint(20) NOT NULL,
  `productid` int(10) NOT NULL,
  `date` varchar(40) NOT NULL,
  PRIMARY KEY (`gameid`,`level`,`productid`,`date`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for tax_day_1
-- ----------------------------
DROP TABLE IF EXISTS `tax_day_1`;
CREATE TABLE `tax_day_1` (
  `stat_datestr` varchar(29) NOT NULL,
  `tax` decimal(12,2) NOT NULL DEFAULT '0.00' COMMENT '统计时间总台费',
  PRIMARY KEY (`stat_datestr`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for tax_monty_1
-- ----------------------------
DROP TABLE IF EXISTS `tax_monty_1`;
CREATE TABLE `tax_monty_1` (
  `stat_datestr` varchar(29) NOT NULL,
  `tax` decimal(12,2) NOT NULL DEFAULT '0.00' COMMENT '统计时间总台费',
  PRIMARY KEY (`stat_datestr`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
