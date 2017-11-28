/*
Navicat MySQL Data Transfer

Source Server         : 192.168.114.34
Source Server Version : 50173
Source Host           : 192.168.114.34:3306
Source Database       : ucenter

Target Server Type    : MYSQL
Target Server Version : 50173
File Encoding         : 65001

Date: 2017-04-27 13:46:04
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for uc_account_binding
-- ----------------------------
DROP TABLE IF EXISTS `uc_account_binding`;
CREATE TABLE `uc_account_binding` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `sitemid` int(11) NOT NULL,
  `phoneno` char(11) NOT NULL COMMENT '手机号码',
  `status` tinyint(4) NOT NULL DEFAULT '0' COMMENT '0 没验证成功 1 验证成功',
  `productid` int(4) NOT NULL DEFAULT '1' COMMENT '产品id',
  `ctime` int(11) NOT NULL COMMENT '绑定时间',
  PRIMARY KEY (`id`),
  UNIQUE KEY `sitemid` (`sitemid`),
  KEY `phone_status` (`phoneno`,`status`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_admin_log
-- ----------------------------
DROP TABLE IF EXISTS `uc_admin_log`;
CREATE TABLE `uc_admin_log` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `username` varchar(30) NOT NULL,
  `model` varchar(50) NOT NULL,
  `page` varchar(50) NOT NULL,
  `act` varchar(50) NOT NULL,
  `req` varchar(1000) NOT NULL,
  `ctime` int(11) NOT NULL,
  `ip` varchar(20) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_admin_logmoney
-- ----------------------------
DROP TABLE IF EXISTS `uc_admin_logmoney`;
CREATE TABLE `uc_admin_logmoney` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `username` varchar(30) NOT NULL,
  `money` int(11) NOT NULL,
  `mid` int(11) NOT NULL COMMENT '对方ID',
  `time` int(11) NOT NULL,
  `ip` char(15) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_admin_member
-- ----------------------------
DROP TABLE IF EXISTS `uc_admin_member`;
CREATE TABLE `uc_admin_member` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `username` varchar(30) NOT NULL,
  `password` varchar(40) NOT NULL,
  `privilege` varchar(200) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_aliuser
-- ----------------------------
DROP TABLE IF EXISTS `uc_aliuser`;
CREATE TABLE `uc_aliuser` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `mid` int(20) NOT NULL DEFAULT '0' COMMENT '用户id',
  `gameid` int(4) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `alipay` varchar(255) NOT NULL DEFAULT '' COMMENT '支付宝账号',
  `aliname` varchar(255) NOT NULL DEFAULT '' COMMENT '支付宝真实姓名',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_antiaddiction
-- ----------------------------
DROP TABLE IF EXISTS `uc_antiaddiction`;
CREATE TABLE `uc_antiaddiction` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `mid` int(11) NOT NULL,
  `id_no` varchar(18) NOT NULL,
  `ctime` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_cardroom_history
-- ----------------------------
DROP TABLE IF EXISTS `uc_cardroom_history`;
CREATE TABLE `uc_cardroom_history` (
  `id` int(12) unsigned NOT NULL AUTO_INCREMENT,
  `gameid` int(11) NOT NULL DEFAULT '0' COMMENT '游戏ID',
  `tid` int(11) NOT NULL DEFAULT '0' COMMENT '桌子id',
  `owner` int(11) NOT NULL DEFAULT '0' COMMENT '桌子创建者',
  `createtime` int(12) NOT NULL DEFAULT '0' COMMENT '创建时间戳',
  `finishround` int(11) NOT NULL DEFAULT '0' COMMENT '完成局数 ',
  `totalround` int(11) NOT NULL COMMENT '最大局数',
  `cost` int(11) NOT NULL DEFAULT '0' COMMENT '消耗房卡',
  `status` int(11) NOT NULL DEFAULT '0' COMMENT '房间状态',
  `uid1` int(11) DEFAULT NULL,
  `info1` varchar(255) DEFAULT NULL,
  `uid2` int(11) DEFAULT NULL,
  `info2` varchar(255) DEFAULT NULL,
  `uid3` int(11) DEFAULT NULL,
  `info3` varchar(255) DEFAULT NULL,
  `uid4` int(11) DEFAULT NULL,
  `info4` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`,`gameid`,`tid`,`owner`,`createtime`),
  KEY `uid1` (`uid1`),
  KEY `uid2` (`uid2`),
  KEY `uid3` (`uid3`),
  KEY `uid4` (`uid4`)
) ENGINE=MyISAM AUTO_INCREMENT=8020 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_cardroom_loground
-- ----------------------------
DROP TABLE IF EXISTS `uc_cardroom_loground`;
CREATE TABLE `uc_cardroom_loground` (
  `id` int(12) unsigned NOT NULL AUTO_INCREMENT,
  `gameid` int(11) NOT NULL DEFAULT '0' COMMENT '游戏ID',
  `tid` int(11) NOT NULL DEFAULT '0' COMMENT '桌子id',
  `owner` int(11) NOT NULL DEFAULT '0' COMMENT '桌子创建者',
  `createtime` int(12) NOT NULL,
  `starttime` int(12) NOT NULL DEFAULT '0' COMMENT '创建时间戳',
  `endtime` int(12) NOT NULL,
  `currentround` int(11) NOT NULL DEFAULT '0' COMMENT '完成局数 ',
  `totalround` int(11) NOT NULL COMMENT '最大局数',
  `uid1` int(11) DEFAULT NULL,
  `info1` varchar(255) DEFAULT NULL,
  `uid2` int(11) DEFAULT NULL,
  `info2` varchar(255) DEFAULT NULL,
  `uid3` int(11) DEFAULT NULL,
  `info3` varchar(255) DEFAULT NULL,
  `uid4` int(11) DEFAULT NULL,
  `info4` varchar(255) DEFAULT NULL,
  `playinfo` varchar(10240) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`,`gameid`,`tid`,`owner`,`createtime`),
  KEY `uid1` (`uid1`),
  KEY `uid2` (`uid2`),
  KEY `uid3` (`uid3`),
  KEY `uid4` (`uid4`)
) ENGINE=MyISAM AUTO_INCREMENT=1407 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_feedback
-- ----------------------------
DROP TABLE IF EXISTS `uc_feedback`;
CREATE TABLE `uc_feedback` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `gameid` int(11) NOT NULL DEFAULT '0',
  `productid` int(11) NOT NULL DEFAULT '0' COMMENT '产品id',
  `cid` int(11) NOT NULL,
  `ctype` int(11) NOT NULL,
  `pid` int(11) NOT NULL,
  `sid` int(11) NOT NULL,
  `mid` int(11) NOT NULL,
  `mnick` varchar(100) NOT NULL,
  `content` varchar(1000) NOT NULL,
  `pic` varchar(200) NOT NULL COMMENT '图片地址',
  `phoneno` char(11) NOT NULL COMMENT '手机号码',
  `ip` int(11) NOT NULL,
  `ctime` int(11) NOT NULL,
  `status` tinyint(4) NOT NULL DEFAULT '0' COMMENT '状态 0 待回复 1已回复 2 忽略',
  PRIMARY KEY (`id`),
  KEY `mid` (`mid`),
  KEY `cid` (`cid`),
  KEY `ctype` (`ctype`),
  KEY `pid` (`pid`),
  KEY `sid` (`sid`),
  KEY `gameid` (`gameid`)
) ENGINE=InnoDB AUTO_INCREMENT=15 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_feedback_reply
-- ----------------------------
DROP TABLE IF EXISTS `uc_feedback_reply`;
CREATE TABLE `uc_feedback_reply` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `fid` int(11) NOT NULL COMMENT '反馈ID',
  `content` varchar(1000) NOT NULL COMMENT '回复内容',
  `reuser` varchar(30) NOT NULL COMMENT '回复者',
  `rtime` int(11) NOT NULL COMMENT '回复时间',
  `mid` int(11) NOT NULL,
  `gameid` int(11) NOT NULL COMMENT '主动推消息给用户的ID',
  `productid` int(11) NOT NULL DEFAULT '0' COMMENT '产品id',
  PRIMARY KEY (`id`),
  UNIQUE KEY `fid` (`fid`),
  KEY `mid` (`gameid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_gameinfo0
-- ----------------------------
DROP TABLE IF EXISTS `uc_gameinfo0`;
CREATE TABLE `uc_gameinfo0` (
  `mid` int(11) NOT NULL,
  `money` bigint(20) NOT NULL COMMENT '金币',
  `coins` int(11) NOT NULL COMMENT '乐币',
  `freezemoney` bigint(20) NOT NULL COMMENT '冻结资金',
  `roll` int(11) NOT NULL COMMENT '乐卷',
  `roll1` int(11) NOT NULL,
  `exp` int(11) NOT NULL COMMENT '经验值',
  `gameid` set('1','2','3','4','5','6','7','8','9') NOT NULL COMMENT '游戏ID',
  `sid` int(11) DEFAULT NULL,
  `wintimes` int(11) DEFAULT '0',
  `losetimes` int(11) DEFAULT '0',
  `runawaytimes` int(11) DEFAULT '0',
  PRIMARY KEY (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_gameinfo1
-- ----------------------------
DROP TABLE IF EXISTS `uc_gameinfo1`;
CREATE TABLE `uc_gameinfo1` (
  `mid` int(11) NOT NULL,
  `money` bigint(20) NOT NULL COMMENT '金币',
  `coins` int(11) NOT NULL COMMENT '乐币',
  `freezemoney` bigint(20) NOT NULL COMMENT '冻结资金',
  `roll` int(11) NOT NULL COMMENT '乐卷',
  `roll1` int(11) NOT NULL,
  `exp` int(11) NOT NULL COMMENT '经验值',
  `gameid` set('1','2','3','4','5','6','7','8','9') NOT NULL COMMENT '游戏ID',
  `sid` int(11) NOT NULL,
  `wintimes` int(11) DEFAULT '0',
  `losetimes` int(11) DEFAULT '0',
  `runawaytimes` int(11) DEFAULT '0',
  PRIMARY KEY (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_gameinfo2
-- ----------------------------
DROP TABLE IF EXISTS `uc_gameinfo2`;
CREATE TABLE `uc_gameinfo2` (
  `mid` int(11) NOT NULL,
  `money` bigint(20) NOT NULL COMMENT '金币',
  `coins` int(11) NOT NULL COMMENT '乐币',
  `freezemoney` bigint(20) NOT NULL COMMENT '冻结资金',
  `roll` int(11) NOT NULL COMMENT '乐卷',
  `roll1` int(11) NOT NULL,
  `exp` int(11) NOT NULL COMMENT '经验值',
  `gameid` set('1','2','3','4','5','6','7','8','9') NOT NULL COMMENT '游戏ID',
  `sid` int(11) DEFAULT NULL,
  `wintimes` int(11) DEFAULT '0',
  `losetimes` int(11) DEFAULT '0',
  `runawaytimes` int(11) DEFAULT '0',
  PRIMARY KEY (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_gameinfo3
-- ----------------------------
DROP TABLE IF EXISTS `uc_gameinfo3`;
CREATE TABLE `uc_gameinfo3` (
  `mid` int(11) NOT NULL,
  `money` bigint(20) NOT NULL COMMENT '金币',
  `coins` int(11) NOT NULL COMMENT '乐币',
  `freezemoney` bigint(20) NOT NULL COMMENT '冻结资金',
  `roll` int(11) NOT NULL COMMENT '乐卷',
  `roll1` int(11) NOT NULL,
  `exp` int(11) NOT NULL COMMENT '经验值',
  `gameid` set('1','2','3','4','5','6','7','8','9') NOT NULL COMMENT '游戏ID',
  `sid` int(11) DEFAULT NULL,
  `wintimes` int(11) DEFAULT '0',
  `losetimes` int(11) DEFAULT '0',
  `runawaytimes` int(11) DEFAULT '0',
  PRIMARY KEY (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_gameinfo4
-- ----------------------------
DROP TABLE IF EXISTS `uc_gameinfo4`;
CREATE TABLE `uc_gameinfo4` (
  `mid` int(11) NOT NULL,
  `money` bigint(20) NOT NULL COMMENT '金币',
  `coins` int(11) NOT NULL COMMENT '乐币',
  `freezemoney` bigint(20) NOT NULL COMMENT '冻结资金',
  `roll` int(11) NOT NULL COMMENT '乐卷',
  `roll1` int(11) NOT NULL,
  `exp` int(11) NOT NULL COMMENT '经验值',
  `gameid` set('1','2','3','4','5','6','7','8','9') NOT NULL COMMENT '游戏ID',
  `sid` int(11) DEFAULT NULL,
  `wintimes` int(11) DEFAULT '0',
  `losetimes` int(11) DEFAULT '0',
  `runawaytimes` int(11) DEFAULT '0',
  PRIMARY KEY (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_gameinfo5
-- ----------------------------
DROP TABLE IF EXISTS `uc_gameinfo5`;
CREATE TABLE `uc_gameinfo5` (
  `mid` int(11) NOT NULL,
  `money` bigint(20) NOT NULL COMMENT '金币',
  `coins` int(11) NOT NULL COMMENT '乐币',
  `freezemoney` bigint(20) NOT NULL COMMENT '冻结资金',
  `roll` int(11) NOT NULL COMMENT '乐卷',
  `roll1` int(11) NOT NULL,
  `exp` int(11) NOT NULL COMMENT '经验值',
  `gameid` set('1','2','3','4','5','6','7','8','9') NOT NULL COMMENT '游戏ID',
  `sid` int(11) DEFAULT NULL,
  `wintimes` int(11) DEFAULT '0',
  `losetimes` int(11) DEFAULT '0',
  `runawaytimes` int(11) DEFAULT '0',
  PRIMARY KEY (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_gameinfo6
-- ----------------------------
DROP TABLE IF EXISTS `uc_gameinfo6`;
CREATE TABLE `uc_gameinfo6` (
  `mid` int(11) NOT NULL,
  `money` bigint(20) NOT NULL COMMENT '金币',
  `coins` int(11) NOT NULL COMMENT '乐币',
  `freezemoney` bigint(20) NOT NULL COMMENT '冻结资金',
  `roll` int(11) NOT NULL COMMENT '乐卷',
  `roll1` int(11) NOT NULL,
  `exp` int(11) NOT NULL COMMENT '经验值',
  `gameid` set('1','2','3','4','5','6','7','8','9') NOT NULL COMMENT '游戏ID',
  `sid` int(11) DEFAULT NULL,
  `wintimes` int(11) DEFAULT '0',
  `losetimes` int(11) DEFAULT '0',
  `runawaytimes` int(11) DEFAULT '0',
  PRIMARY KEY (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_gameinfo7
-- ----------------------------
DROP TABLE IF EXISTS `uc_gameinfo7`;
CREATE TABLE `uc_gameinfo7` (
  `mid` int(11) NOT NULL,
  `money` bigint(20) NOT NULL COMMENT '金币',
  `coins` int(11) NOT NULL COMMENT '乐币',
  `freezemoney` bigint(20) NOT NULL COMMENT '冻结资金',
  `roll` int(11) NOT NULL COMMENT '乐卷',
  `roll1` int(11) NOT NULL,
  `exp` int(11) NOT NULL COMMENT '经验值',
  `gameid` set('1','2','3','4','5','6','7','8','9') NOT NULL COMMENT '游戏ID',
  `sid` int(11) DEFAULT NULL,
  `wintimes` int(11) DEFAULT '0',
  `losetimes` int(11) DEFAULT '0',
  `runawaytimes` int(11) DEFAULT '0',
  PRIMARY KEY (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_gameinfo8
-- ----------------------------
DROP TABLE IF EXISTS `uc_gameinfo8`;
CREATE TABLE `uc_gameinfo8` (
  `mid` int(11) NOT NULL,
  `money` bigint(20) NOT NULL COMMENT '金币',
  `coins` int(11) NOT NULL COMMENT '乐币',
  `freezemoney` bigint(20) NOT NULL COMMENT '冻结资金',
  `roll` int(11) NOT NULL COMMENT '乐卷',
  `roll1` int(11) NOT NULL,
  `exp` int(11) NOT NULL COMMENT '经验值',
  `gameid` set('1','2','3','4','5','6','7','8','9') NOT NULL COMMENT '游戏ID',
  `sid` int(11) DEFAULT NULL,
  `wintimes` int(11) DEFAULT '0',
  `losetimes` int(11) DEFAULT '0',
  `runawaytimes` int(11) DEFAULT '0',
  PRIMARY KEY (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_gameinfo9
-- ----------------------------
DROP TABLE IF EXISTS `uc_gameinfo9`;
CREATE TABLE `uc_gameinfo9` (
  `mid` int(11) NOT NULL,
  `money` bigint(20) NOT NULL COMMENT '金币',
  `coins` int(11) NOT NULL COMMENT '乐币',
  `freezemoney` bigint(20) NOT NULL COMMENT '冻结资金',
  `roll` int(11) NOT NULL COMMENT '乐卷',
  `roll1` int(11) NOT NULL,
  `exp` int(11) NOT NULL COMMENT '经验值',
  `gameid` set('1','2','3','4','5','6','7','8','9') NOT NULL COMMENT '游戏ID',
  `sid` int(11) DEFAULT NULL,
  `wintimes` int(11) DEFAULT '0',
  `losetimes` int(11) DEFAULT '0',
  `runawaytimes` int(11) DEFAULT '0',
  PRIMARY KEY (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_logbataccount
-- ----------------------------
DROP TABLE IF EXISTS `uc_logbataccount`;
CREATE TABLE `uc_logbataccount` (
  `mid` int(11) NOT NULL,
  `btime` int(11) NOT NULL,
  `btype` int(11) NOT NULL DEFAULT '1' COMMENT '封号类型  -1解封;  1 永久 ;;2 封7天 ;3 封一个月;4 封半年 ',
  `badmin` varchar(20) NOT NULL DEFAULT 'system',
  KEY `btime` (`btime`),
  KEY `mid` (`mid`),
  KEY `btype` (`btype`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_logmessage
-- ----------------------------
DROP TABLE IF EXISTS `uc_logmessage`;
CREATE TABLE `uc_logmessage` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `mid` int(11) NOT NULL COMMENT '对方游戏ID',
  `phoneno` char(11) NOT NULL,
  `content` varchar(100) NOT NULL,
  `status` varchar(40) NOT NULL COMMENT '发送状态',
  `type` tinyint(4) NOT NULL COMMENT '1 验证码 2 取回账号 3 其它',
  `msgid` varchar(30) NOT NULL,
  `ctime` datetime NOT NULL,
  PRIMARY KEY (`id`),
  KEY `mid` (`mid`),
  KEY `ctime` (`ctime`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_match_prize
-- ----------------------------
DROP TABLE IF EXISTS `uc_match_prize`;
CREATE TABLE `uc_match_prize` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `mid` int(11) NOT NULL,
  `phone` varchar(20) NOT NULL,
  `prize` varchar(50) NOT NULL,
  `rank` int(11) NOT NULL COMMENT '名次',
  `ctime` int(11) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `mid` (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_notice
-- ----------------------------
DROP TABLE IF EXISTS `uc_notice`;
CREATE TABLE `uc_notice` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `productid` int(10) NOT NULL DEFAULT '0' COMMENT '产品id',
  `ctype` tinyint(4) NOT NULL DEFAULT '0' COMMENT '客户端类型ID',
  `title` varchar(70) NOT NULL DEFAULT '',
  `content` varchar(500) NOT NULL DEFAULT '',
  `url` varchar(50) NOT NULL DEFAULT '',
  `stime` int(11) NOT NULL COMMENT '开始时间',
  `etime` int(11) NOT NULL COMMENT '结束时间',
  `ctime` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_payment
-- ----------------------------
DROP TABLE IF EXISTS `uc_payment`;
CREATE TABLE `uc_payment` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `mid` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '充值人:0为系统',
  `gameid` tinyint(4) NOT NULL,
  `versions` varchar(10) NOT NULL COMMENT '客户端版本号',
  `pmoneynow` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '充值时的金币数',
  `sid` smallint(3) NOT NULL COMMENT '账号类型ID',
  `cid` int(11) NOT NULL COMMENT '渠道ID',
  `pid` int(11) NOT NULL,
  `ip` varchar(20) NOT NULL COMMENT '下单时的IP地址',
  `ctype` tinyint(4) NOT NULL COMMENT '客户端类型',
  `pmode` smallint(5) unsigned NOT NULL DEFAULT '0' COMMENT '充值方式',
  `ptype` smallint(5) unsigned NOT NULL DEFAULT '0' COMMENT '充值货币类型 1金币 2 元宝 3 会员卡',
  `pamount` float unsigned NOT NULL COMMENT '充值数目',
  `pnum` smallint(3) unsigned NOT NULL DEFAULT '0' COMMENT '购买的数量',
  `pexchangenum` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '充值所获取的乐币/金币/道具数量',
  `source` int(11) NOT NULL DEFAULT '0' COMMENT '0 商城  1 快速充值  2 首充  3 破产充值',
  `pbankno` varchar(50) NOT NULL DEFAULT '' COMMENT '银行交易号',
  `viptime` int(11) NOT NULL COMMENT '会员时间',
  `pdealno` varchar(50) NOT NULL,
  `pfee` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '手续费',
  `ptime` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '充值日期',
  `pstatus` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '充值状态:0为提交了表单,1为取消表单,2为完成表单 3 退单  4暂时冻结  5人工审核',
  PRIMARY KEY (`id`),
  KEY `pstatus` (`pstatus`),
  KEY `ptime` (`ptime`),
  KEY `fmid` (`mid`),
  KEY `pdealno` (`pdealno`),
  KEY `pid` (`ip`),
  KEY `sid` (`sid`),
  KEY `source` (`source`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='用户充值记录表';

-- ----------------------------
-- Table structure for uc_rankaward
-- ----------------------------
DROP TABLE IF EXISTS `uc_rankaward`;
CREATE TABLE `uc_rankaward` (
  `id` int(10) unsigned zerofill NOT NULL AUTO_INCREMENT,
  `start` int(10) NOT NULL DEFAULT '0' COMMENT '开始排名',
  `end` int(10) NOT NULL DEFAULT '0' COMMENT '结束名次',
  `money` int(10) NOT NULL DEFAULT '0' COMMENT '分计算',
  `sort` int(10) NOT NULL DEFAULT '0' COMMENT '排序值高的前面',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_register_android
-- ----------------------------
DROP TABLE IF EXISTS `uc_register_android`;
CREATE TABLE `uc_register_android` (
  `sitemid` int(11) NOT NULL,
  `device_no` varchar(50) NOT NULL COMMENT '机器码',
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  PRIMARY KEY (`sitemid`),
  UNIQUE KEY `device_no` (`device_no`,`productid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_register_ios
-- ----------------------------
DROP TABLE IF EXISTS `uc_register_ios`;
CREATE TABLE `uc_register_ios` (
  `sitemid` int(11) NOT NULL,
  `device_no` varchar(50) NOT NULL COMMENT '机器码',
  `openudid` varchar(40) NOT NULL,
  `productid` int(4) NOT NULL DEFAULT '1' COMMENT '产品id',
  PRIMARY KEY (`sitemid`),
  UNIQUE KEY `device_no` (`device_no`,`productid`) USING BTREE,
  KEY `openudid` (`openudid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_register_phonenumber
-- ----------------------------
DROP TABLE IF EXISTS `uc_register_phonenumber`;
CREATE TABLE `uc_register_phonenumber` (
  `sitemid` int(11) NOT NULL,
  `phoneno` char(11) NOT NULL,
  `password` varchar(40) NOT NULL,
  PRIMARY KEY (`sitemid`),
  UNIQUE KEY `phoneno` (`phoneno`),
  KEY `password` (`password`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_register_sitemid
-- ----------------------------
DROP TABLE IF EXISTS `uc_register_sitemid`;
CREATE TABLE `uc_register_sitemid` (
  `sitemid` int(11) NOT NULL AUTO_INCREMENT,
  PRIMARY KEY (`sitemid`)
) ENGINE=InnoDB AUTO_INCREMENT=7666746 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_register_username
-- ----------------------------
DROP TABLE IF EXISTS `uc_register_username`;
CREATE TABLE `uc_register_username` (
  `sitemid` int(11) NOT NULL,
  `username` varchar(30) NOT NULL,
  `password` varchar(40) NOT NULL,
  PRIMARY KEY (`sitemid`),
  UNIQUE KEY `username` (`username`),
  KEY `password` (`password`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_register_wx
-- ----------------------------
DROP TABLE IF EXISTS `uc_register_wx`;
CREATE TABLE `uc_register_wx` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `mid` int(11) DEFAULT NULL,
  `sitemid` int(11) NOT NULL,
  `nic_name` varchar(255) DEFAULT NULL,
  `openid` varchar(255) DEFAULT NULL,
  `realname` varchar(255) DEFAULT NULL,
  `idcode` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`,`sitemid`)
) ENGINE=InnoDB AUTO_INCREMENT=13 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_room_card
-- ----------------------------
DROP TABLE IF EXISTS `uc_room_card`;
CREATE TABLE `uc_room_card` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '系统充值表',
  `mid` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '充值人:0为系统',
  `userid` int(10) NOT NULL DEFAULT '0' COMMENT '系统充值用户id',
  `coinsnow` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '充值时的金币数',
  `coinsafter` int(10) NOT NULL DEFAULT '0' COMMENT '加减金币后金币数量',
  `ip` varchar(20) NOT NULL COMMENT '下单时的IP地址',
  `pdealno` varchar(50) NOT NULL DEFAULT '' COMMENT '订单号',
  `pexchangenum` int(10) NOT NULL DEFAULT '0' COMMENT '充值所获取的乐币',
  `ptime` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '充值日期',
  `remark` varchar(255) NOT NULL DEFAULT '' COMMENT '备注',
  `pstatus` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '充值状态:0为提交了表单,1为完成表单',
  PRIMARY KEY (`id`),
  KEY `pstatus` (`pstatus`),
  KEY `ptime` (`ptime`),
  KEY `fmid` (`mid`),
  KEY `pid` (`ip`)
) ENGINE=InnoDB AUTO_INCREMENT=96 DEFAULT CHARSET=utf8 COMMENT='用户调整房卡记录表';

-- ----------------------------
-- Table structure for uc_setting_behavior
-- ----------------------------
DROP TABLE IF EXISTS `uc_setting_behavior`;
CREATE TABLE `uc_setting_behavior` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `gameid` int(11) NOT NULL,
  `beid` varchar(40) NOT NULL,
  `betitle` varchar(60) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_setting_cid
-- ----------------------------
DROP TABLE IF EXISTS `uc_setting_cid`;
CREATE TABLE `uc_setting_cid` (
  `cid` int(11) NOT NULL AUTO_INCREMENT,
  `cname` varchar(30) NOT NULL COMMENT 'æ¸ é“å•†åç§°',
  `ctype` int(11) NOT NULL COMMENT '客户端类型ID',
  `gameid` varchar(50) NOT NULL,
  `idc` tinyint(4) NOT NULL DEFAULT '1' COMMENT '机房ID',
  `vertype` int(11) NOT NULL DEFAULT '1' COMMENT '1 主线版本  2 联运渠道版本 3 大渠道',
  PRIMARY KEY (`cid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_setting_horn
-- ----------------------------
DROP TABLE IF EXISTS `uc_setting_horn`;
CREATE TABLE `uc_setting_horn` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `content` varchar(200) NOT NULL,
  `ctime` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_setting_pid
-- ----------------------------
DROP TABLE IF EXISTS `uc_setting_pid`;
CREATE TABLE `uc_setting_pid` (
  `pid` int(11) NOT NULL AUTO_INCREMENT,
  `pname` varchar(30) NOT NULL,
  `cid` int(11) NOT NULL COMMENT '渠道ID',
  `gameid` int(11) NOT NULL,
  `idc` tinyint(4) NOT NULL DEFAULT '1' COMMENT '机房id',
  `childid` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`pid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_setting_pmode
-- ----------------------------
DROP TABLE IF EXISTS `uc_setting_pmode`;
CREATE TABLE `uc_setting_pmode` (
  `pmode` int(11) NOT NULL AUTO_INCREMENT,
  `payname` varchar(20) NOT NULL COMMENT '支付渠道名称',
  `status` tinyint(4) NOT NULL DEFAULT '1' COMMENT '是否显示 1显示  0 不显示',
  `order` tinyint(4) NOT NULL COMMENT '排序',
  `idc` int(11) NOT NULL DEFAULT '1',
  `itemid` int(11) NOT NULL,
  PRIMARY KEY (`pmode`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_setting_push
-- ----------------------------
DROP TABLE IF EXISTS `uc_setting_push`;
CREATE TABLE `uc_setting_push` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `gameid` int(11) NOT NULL,
  `ctype` int(11) NOT NULL,
  `cid` int(11) NOT NULL,
  `ptype` int(11) NOT NULL COMMENT '推送类型 1 N天前没登陆  2 某段时间登陆 3 全局',
  `pcon` varchar(100) NOT NULL,
  `ptime` varchar(11) NOT NULL COMMENT '推送时间',
  `msg` varchar(200) NOT NULL,
  `status` int(11) NOT NULL COMMENT '状态 0 失效 2 生效',
  `ctime` int(11) NOT NULL COMMENT '添加/修改的时间',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_setting_sid
-- ----------------------------
DROP TABLE IF EXISTS `uc_setting_sid`;
CREATE TABLE `uc_setting_sid` (
  `sid` int(11) NOT NULL AUTO_INCREMENT,
  `sname` varchar(30) NOT NULL COMMENT '账号类型ID',
  `idc` int(11) NOT NULL DEFAULT '1' COMMENT '机房',
  PRIMARY KEY (`sid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_setting_wmode
-- ----------------------------
DROP TABLE IF EXISTS `uc_setting_wmode`;
CREATE TABLE `uc_setting_wmode` (
  `wmodeID` int(11) NOT NULL AUTO_INCREMENT,
  `gamedesc` varchar(100) NOT NULL,
  `admindesc` varchar(100) NOT NULL,
  `idc` int(11) NOT NULL DEFAULT '1',
  PRIMARY KEY (`wmodeID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_sitemid2mid
-- ----------------------------
DROP TABLE IF EXISTS `uc_sitemid2mid`;
CREATE TABLE `uc_sitemid2mid` (
  `mid` int(11) NOT NULL AUTO_INCREMENT,
  `sitemid` varchar(40) NOT NULL,
  `sid` smallint(6) NOT NULL,
  `deviceno` varchar(50) NOT NULL,
  `ip` varchar(20) NOT NULL,
  `time` int(11) NOT NULL,
  PRIMARY KEY (`mid`),
  KEY `sitemid_sid` (`sitemid`,`sid`),
  KEY `time` (`time`),
  KEY `deviceno` (`deviceno`),
  KEY `ip` (`ip`)
) ENGINE=InnoDB AUTO_INCREMENT=6893272 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_stat_pay
-- ----------------------------
DROP TABLE IF EXISTS `uc_stat_pay`;
CREATE TABLE `uc_stat_pay` (
  `mid` int(11) NOT NULL,
  `charge_amount` decimal(12,2) NOT NULL DEFAULT '0.00' COMMENT '用户充值总金额',
  `withdraw_amount` decimal(12,2) NOT NULL DEFAULT '0.00' COMMENT '用户提现总金额',
  `money` bigint(20) NOT NULL DEFAULT '0' COMMENT '身上金币',
  `freezemoney` bigint(20) NOT NULL DEFAULT '0' COMMENT '保险箱金币',
  `tax` bigint(20) NOT NULL DEFAULT '0' COMMENT '台费',
  `username` varchar(64) NOT NULL DEFAULT '' COMMENT '用户名',
  `alipay` varchar(255) NOT NULL DEFAULT '' COMMENT '支付宝账号',
  `mnick` varchar(255) NOT NULL DEFAULT '' COMMENT '昵称',
  `ip` bigint(20) NOT NULL DEFAULT '0' COMMENT '登录ip',
  `deviceno` varchar(64) NOT NULL DEFAULT '' COMMENT '设备号',
  `ctime` bigint(20) NOT NULL DEFAULT '0' COMMENT '注册时间',
  `mstatus` int(4) NOT NULL DEFAULT '10' COMMENT '用户状态',
  `productid` int(10) NOT NULL DEFAULT '0' COMMENT '产品id',
  `versions` varchar(32) NOT NULL DEFAULT '' COMMENT '产品版本',
  `sex` int(4) NOT NULL DEFAULT '1' COMMENT '性别',
  `country` varchar(64) NOT NULL DEFAULT '' COMMENT '国家',
  `province` varchar(64) NOT NULL DEFAULT '' COMMENT '省',
  `city` varchar(64) NOT NULL DEFAULT '' COMMENT '市',
  `equipment_model` varchar(128) NOT NULL DEFAULT '' COMMENT '手机设备型号',
  `networking` varchar(10) NOT NULL DEFAULT '' COMMENT '联网方式',
  PRIMARY KEY (`mid`),
  KEY `money` (`money`,`freezemoney`),
  KEY `ctime` (`ctime`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_system_recharge
-- ----------------------------
DROP TABLE IF EXISTS `uc_system_recharge`;
CREATE TABLE `uc_system_recharge` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '系统充值表',
  `mid` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '充值人:0为系统',
  `userid` int(10) NOT NULL DEFAULT '0' COMMENT '系统充值用户id',
  `pmoneynow` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '充值时的金币数',
  `pmoneyafter` int(10) NOT NULL DEFAULT '0' COMMENT '加减金币后金币数量',
  `ip` varchar(20) NOT NULL COMMENT '下单时的IP地址',
  `type` int(4) NOT NULL DEFAULT '1' COMMENT '1 金币  2保险箱',
  `ctype` tinyint(4) NOT NULL COMMENT '客户端类型',
  `ptype` smallint(5) unsigned NOT NULL DEFAULT '1' COMMENT '1 加金币  2 减金币',
  `pexchangenum` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '充值所获取的乐币',
  `pbankno` varchar(50) NOT NULL DEFAULT '' COMMENT '银行交易号',
  `pdealno` varchar(50) NOT NULL,
  `ptime` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '充值日期',
  `remark` varchar(255) NOT NULL DEFAULT '' COMMENT '备注',
  `pstatus` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '充值状态:0为提交了表单,1为完成表单',
  PRIMARY KEY (`id`),
  KEY `pstatus` (`pstatus`),
  KEY `ptime` (`ptime`),
  KEY `fmid` (`mid`),
  KEY `pdealno` (`pdealno`),
  KEY `pid` (`ip`)
) ENGINE=InnoDB AUTO_INCREMENT=12 DEFAULT CHARSET=utf8 COMMENT='用户充值记录表';

-- ----------------------------
-- Table structure for uc_system_recharge_other
-- ----------------------------
DROP TABLE IF EXISTS `uc_system_recharge_other`;
CREATE TABLE `uc_system_recharge_other` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'ç³»ç»Ÿå……å€¼è¡¨',
  `mid` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '用户mid',
  `userid` int(10) NOT NULL DEFAULT '0' COMMENT 'ç³»ç»Ÿå……å€¼ç”¨æˆ·id',
  `pmoneynow` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '调整前',
  `pmoneyafter` int(10) NOT NULL DEFAULT '0' COMMENT '调整后',
  `ip` varchar(20) NOT NULL COMMENT 'ip',
  `type` int(4) NOT NULL DEFAULT '1' COMMENT '1 é‡‘å¸  2ä¿é™©ç®±',
  `ctype` tinyint(4) NOT NULL COMMENT 'å®¢æˆ·ç«¯ç±»åž‹',
  `ptype` smallint(5) unsigned NOT NULL DEFAULT '1' COMMENT '1 åŠ é‡‘å¸  2 å‡é‡‘å¸',
  `pexchangenum` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'å……å€¼æ‰€èŽ·å–çš„ä¹å¸',
  `pbankno` varchar(50) NOT NULL DEFAULT '' COMMENT 'é“¶è¡Œäº¤æ˜“å·',
  `pdealno` varchar(50) NOT NULL,
  `ptime` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'å……å€¼æ—¥æœŸ',
  `remark` varchar(255) NOT NULL DEFAULT '' COMMENT 'å¤‡æ³¨',
  `pstatus` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT 'å……å€¼çŠ¶æ€:0ä¸ºæäº¤äº†è¡¨å•,1ä¸ºå®Œæˆè¡¨å•',
  PRIMARY KEY (`id`),
  KEY `pstatus` (`pstatus`),
  KEY `ptime` (`ptime`),
  KEY `fmid` (`mid`),
  KEY `pdealno` (`pdealno`),
  KEY `pid` (`ip`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='金币调整';

-- ----------------------------
-- Table structure for uc_userinfo0
-- ----------------------------
DROP TABLE IF EXISTS `uc_userinfo0`;
CREATE TABLE `uc_userinfo0` (
  `mid` int(11) NOT NULL,
  `sid` smallint(6) NOT NULL COMMENT '账号类型ID',
  `cid` int(11) NOT NULL COMMENT '渠道ID',
  `pid` int(11) NOT NULL COMMENT '客户端包ID ',
  `ctype` tinyint(4) NOT NULL COMMENT '客户端类型 1 android 2 iphoe 3 ipad 4 android pad ',
  `mnick` varchar(50) NOT NULL COMMENT '昵称',
  `sitemid` varchar(40) NOT NULL COMMENT '平台ID',
  `sex` tinyint(4) NOT NULL DEFAULT '0' COMMENT '性别  0 未知 1男 2 女',
  `hometown` varchar(50) NOT NULL COMMENT '故乡',
  `mstatus` tinyint(4) NOT NULL DEFAULT '10' COMMENT '用户状态 1:禁止進入 8:用戶卸載應用后重裝 9:用戶需要更新資料 10:用戶正常狀態',
  `ip` int(11) unsigned NOT NULL COMMENT 'ip',
  `devicename` varchar(30) NOT NULL COMMENT '设备名称',
  `osversion` varchar(20) NOT NULL COMMENT '系统版本号',
  `nettype` tinyint(4) NOT NULL COMMENT '0无 1 3G 2 wifi 3 2G',
  `versions` varchar(20) NOT NULL COMMENT '版本号',
  `mactivetime` varchar(200) NOT NULL COMMENT '最后登陆时间 json格式 {1:"123456789"} gameid=>最后登陆时间',
  `mentercount` varchar(100) NOT NULL COMMENT '登陆的总数（每天统计一次）json格式 {1:"12"} gameid=>登陆次数',
  `mtime` varchar(200) NOT NULL COMMENT '注册时间 json格式 {1:"123456789"} gameid=>注册时间',
  `openid` varchar(255) NOT NULL,
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `icon` varchar(255) NOT NULL DEFAULT '' COMMENT '用户头像',
  `bankPWD` varchar(255) NOT NULL DEFAULT '' COMMENT '保险箱密码',
  `ctime` bigint(20) NOT NULL DEFAULT '0' COMMENT '注册时间',
  PRIMARY KEY (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_userinfo1
-- ----------------------------
DROP TABLE IF EXISTS `uc_userinfo1`;
CREATE TABLE `uc_userinfo1` (
  `mid` int(11) NOT NULL,
  `sid` smallint(6) NOT NULL COMMENT '账号类型ID',
  `cid` int(11) NOT NULL COMMENT '渠道ID',
  `pid` int(11) NOT NULL COMMENT '客户端包ID ',
  `ctype` tinyint(4) NOT NULL COMMENT '客户端类型 1 android 2 iphoe 3 ipad 4 android pad ',
  `mnick` varchar(50) NOT NULL COMMENT '昵称',
  `sitemid` varchar(40) NOT NULL COMMENT '平台ID',
  `sex` tinyint(4) NOT NULL DEFAULT '0' COMMENT '性别  0 未知 1男 2 女',
  `hometown` varchar(50) NOT NULL COMMENT '故乡',
  `mstatus` tinyint(4) NOT NULL DEFAULT '10' COMMENT '用户状态 1:禁止進入 8:用戶卸載應用后重裝 9:用戶需要更新資料 10:用戶正常狀態',
  `ip` int(11) unsigned NOT NULL COMMENT 'ip',
  `devicename` varchar(30) NOT NULL COMMENT '设备名称',
  `osversion` varchar(20) NOT NULL COMMENT '系统版本号',
  `nettype` tinyint(4) NOT NULL COMMENT '0无 1 3G 2 wifi 3 2G',
  `versions` varchar(20) NOT NULL COMMENT '版本号',
  `mactivetime` varchar(200) NOT NULL COMMENT '最后登陆时间 json格式 {1:"123456789"} gameid=>最后登陆时间',
  `mentercount` varchar(100) NOT NULL COMMENT '登陆的总数（每天统计一次）json格式 {1:"12"} gameid=>登陆次数',
  `mtime` varchar(200) NOT NULL COMMENT '注册时间 json格式 {1:"123456789"} gameid=>注册时间',
  `openid` varchar(255) NOT NULL,
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `icon` varchar(255) NOT NULL DEFAULT '' COMMENT '用户头像',
  `bankPWD` varchar(255) NOT NULL DEFAULT '' COMMENT '保险箱密码',
  `ctime` bigint(20) NOT NULL DEFAULT '0' COMMENT '注册时间',
  PRIMARY KEY (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_userinfo2
-- ----------------------------
DROP TABLE IF EXISTS `uc_userinfo2`;
CREATE TABLE `uc_userinfo2` (
  `mid` int(11) NOT NULL,
  `sid` smallint(6) NOT NULL COMMENT '账号类型ID',
  `cid` int(11) NOT NULL COMMENT '渠道ID',
  `pid` int(11) NOT NULL COMMENT '客户端包ID ',
  `ctype` tinyint(4) NOT NULL COMMENT '客户端类型 1 android 2 iphoe 3 ipad 4 android pad ',
  `mnick` varchar(50) NOT NULL COMMENT '昵称',
  `sitemid` varchar(40) NOT NULL COMMENT '平台ID',
  `sex` tinyint(4) NOT NULL DEFAULT '0' COMMENT '性别  0 未知 1男 2 女',
  `hometown` varchar(50) NOT NULL COMMENT '故乡',
  `mstatus` tinyint(4) NOT NULL DEFAULT '10' COMMENT '用户状态 1:禁止進入 8:用戶卸載應用后重裝 9:用戶需要更新資料 10:用戶正常狀態',
  `ip` int(11) unsigned NOT NULL COMMENT 'ip',
  `devicename` varchar(30) NOT NULL COMMENT '设备名称',
  `osversion` varchar(20) NOT NULL COMMENT '系统版本号',
  `nettype` tinyint(4) NOT NULL COMMENT '0无 1 3G 2 wifi 3 2G',
  `versions` varchar(20) NOT NULL COMMENT '版本号',
  `mactivetime` varchar(200) NOT NULL COMMENT '最后登陆时间 json格式 {1:"123456789"} gameid=>最后登陆时间',
  `mentercount` varchar(100) NOT NULL COMMENT '登陆的总数（每天统计一次）json格式 {1:"12"} gameid=>登陆次数',
  `mtime` varchar(200) NOT NULL COMMENT '注册时间 json格式 {1:"123456789"} gameid=>注册时间',
  `openid` varchar(255) NOT NULL,
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `icon` varchar(255) NOT NULL DEFAULT '' COMMENT '用户头像',
  `bankPWD` varchar(255) NOT NULL DEFAULT '' COMMENT '保险箱密码',
  `ctime` bigint(20) NOT NULL DEFAULT '0' COMMENT '注册时间',
  PRIMARY KEY (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_userinfo3
-- ----------------------------
DROP TABLE IF EXISTS `uc_userinfo3`;
CREATE TABLE `uc_userinfo3` (
  `mid` int(11) NOT NULL,
  `sid` smallint(6) NOT NULL COMMENT '账号类型ID',
  `cid` int(11) NOT NULL COMMENT '渠道ID',
  `pid` int(11) NOT NULL COMMENT '客户端包ID ',
  `ctype` tinyint(4) NOT NULL COMMENT '客户端类型 1 android 2 iphoe 3 ipad 4 android pad ',
  `mnick` varchar(50) NOT NULL COMMENT '昵称',
  `sitemid` varchar(40) NOT NULL COMMENT '平台ID',
  `sex` tinyint(4) NOT NULL DEFAULT '0' COMMENT '性别  0 未知 1男 2 女',
  `hometown` varchar(50) NOT NULL COMMENT '故乡',
  `mstatus` tinyint(4) NOT NULL DEFAULT '10' COMMENT '用户状态 1:禁止進入 8:用戶卸載應用后重裝 9:用戶需要更新資料 10:用戶正常狀態',
  `ip` int(11) unsigned NOT NULL COMMENT 'ip',
  `devicename` varchar(30) NOT NULL COMMENT '设备名称',
  `osversion` varchar(20) NOT NULL COMMENT '系统版本号',
  `nettype` tinyint(4) NOT NULL COMMENT '0无 1 3G 2 wifi 3 2G',
  `versions` varchar(20) NOT NULL COMMENT '版本号',
  `mactivetime` varchar(200) NOT NULL COMMENT '最后登陆时间 json格式 {1:"123456789"} gameid=>最后登陆时间',
  `mentercount` varchar(100) NOT NULL COMMENT '登陆的总数（每天统计一次）json格式 {1:"12"} gameid=>登陆次数',
  `mtime` varchar(200) NOT NULL COMMENT '注册时间 json格式 {1:"123456789"} gameid=>注册时间',
  `openid` varchar(255) NOT NULL,
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `icon` varchar(255) NOT NULL DEFAULT '' COMMENT '用户头像',
  `bankPWD` varchar(255) NOT NULL DEFAULT '' COMMENT '保险箱密码',
  `ctime` bigint(20) NOT NULL DEFAULT '0' COMMENT '注册时间',
  PRIMARY KEY (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_userinfo4
-- ----------------------------
DROP TABLE IF EXISTS `uc_userinfo4`;
CREATE TABLE `uc_userinfo4` (
  `mid` int(11) NOT NULL,
  `sid` smallint(6) NOT NULL COMMENT '账号类型ID',
  `cid` int(11) NOT NULL COMMENT '渠道ID',
  `pid` int(11) NOT NULL COMMENT '客户端包ID ',
  `ctype` tinyint(4) NOT NULL COMMENT '客户端类型 1 android 2 iphoe 3 ipad 4 android pad ',
  `mnick` varchar(50) NOT NULL COMMENT '昵称',
  `sitemid` varchar(40) NOT NULL COMMENT '平台ID',
  `sex` tinyint(4) NOT NULL DEFAULT '0' COMMENT '性别  0 未知 1男 2 女',
  `hometown` varchar(50) NOT NULL COMMENT '故乡',
  `mstatus` tinyint(4) NOT NULL DEFAULT '10' COMMENT '用户状态 1:禁止進入 8:用戶卸載應用后重裝 9:用戶需要更新資料 10:用戶正常狀態',
  `ip` int(11) unsigned NOT NULL COMMENT 'ip',
  `devicename` varchar(30) NOT NULL COMMENT '设备名称',
  `osversion` varchar(20) NOT NULL COMMENT '系统版本号',
  `nettype` tinyint(4) NOT NULL COMMENT '0无 1 3G 2 wifi 3 2G',
  `versions` varchar(20) NOT NULL COMMENT '版本号',
  `mactivetime` varchar(200) NOT NULL COMMENT '最后登陆时间 json格式 {1:"123456789"} gameid=>最后登陆时间',
  `mentercount` varchar(100) NOT NULL COMMENT '登陆的总数（每天统计一次）json格式 {1:"12"} gameid=>登陆次数',
  `mtime` varchar(200) NOT NULL COMMENT '注册时间 json格式 {1:"123456789"} gameid=>注册时间',
  `openid` varchar(255) NOT NULL,
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `icon` varchar(255) NOT NULL DEFAULT '' COMMENT '用户头像',
  `bankPWD` varchar(255) NOT NULL DEFAULT '' COMMENT '保险箱密码',
  `ctime` bigint(20) NOT NULL DEFAULT '0' COMMENT '注册时间',
  PRIMARY KEY (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_userinfo5
-- ----------------------------
DROP TABLE IF EXISTS `uc_userinfo5`;
CREATE TABLE `uc_userinfo5` (
  `mid` int(11) NOT NULL,
  `sid` smallint(6) NOT NULL COMMENT '账号类型ID',
  `cid` int(11) NOT NULL COMMENT '渠道ID',
  `pid` int(11) NOT NULL COMMENT '客户端包ID ',
  `ctype` tinyint(4) NOT NULL COMMENT '客户端类型 1 android 2 iphoe 3 ipad 4 android pad ',
  `mnick` varchar(50) NOT NULL COMMENT '昵称',
  `sitemid` varchar(40) NOT NULL COMMENT '平台ID',
  `sex` tinyint(4) NOT NULL DEFAULT '0' COMMENT '性别  0 未知 1男 2 女',
  `hometown` varchar(50) NOT NULL COMMENT '故乡',
  `mstatus` tinyint(4) NOT NULL DEFAULT '10' COMMENT '用户状态 1:禁止進入 8:用戶卸載應用后重裝 9:用戶需要更新資料 10:用戶正常狀態',
  `ip` int(11) unsigned NOT NULL COMMENT 'ip',
  `devicename` varchar(30) NOT NULL COMMENT '设备名称',
  `osversion` varchar(20) NOT NULL COMMENT '系统版本号',
  `nettype` tinyint(4) NOT NULL COMMENT '0无 1 3G 2 wifi 3 2G',
  `versions` varchar(20) NOT NULL COMMENT '版本号',
  `mactivetime` varchar(200) NOT NULL COMMENT '最后登陆时间 json格式 {1:"123456789"} gameid=>最后登陆时间',
  `mentercount` varchar(100) NOT NULL COMMENT '登陆的总数（每天统计一次）json格式 {1:"12"} gameid=>登陆次数',
  `mtime` varchar(200) NOT NULL COMMENT '注册时间 json格式 {1:"123456789"} gameid=>注册时间',
  `openid` varchar(255) NOT NULL,
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `icon` varchar(255) NOT NULL DEFAULT '' COMMENT '用户头像',
  `bankPWD` varchar(255) NOT NULL DEFAULT '' COMMENT '保险箱密码',
  `ctime` bigint(20) NOT NULL DEFAULT '0' COMMENT '注册时间',
  PRIMARY KEY (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_userinfo6
-- ----------------------------
DROP TABLE IF EXISTS `uc_userinfo6`;
CREATE TABLE `uc_userinfo6` (
  `mid` int(11) NOT NULL,
  `sid` smallint(6) NOT NULL COMMENT '账号类型ID',
  `cid` int(11) NOT NULL COMMENT '渠道ID',
  `pid` int(11) NOT NULL COMMENT '客户端包ID ',
  `ctype` tinyint(4) NOT NULL COMMENT '客户端类型 1 android 2 iphoe 3 ipad 4 android pad ',
  `mnick` varchar(50) NOT NULL COMMENT '昵称',
  `sitemid` varchar(40) NOT NULL COMMENT '平台ID',
  `sex` tinyint(4) NOT NULL DEFAULT '0' COMMENT '性别  0 未知 1男 2 女',
  `hometown` varchar(50) NOT NULL COMMENT '故乡',
  `mstatus` tinyint(4) NOT NULL DEFAULT '10' COMMENT '用户状态 1:禁止進入 8:用戶卸載應用后重裝 9:用戶需要更新資料 10:用戶正常狀態',
  `ip` int(11) unsigned NOT NULL COMMENT 'ip',
  `devicename` varchar(30) NOT NULL COMMENT '设备名称',
  `osversion` varchar(20) NOT NULL COMMENT '系统版本号',
  `nettype` tinyint(4) NOT NULL COMMENT '0无 1 3G 2 wifi 3 2G',
  `versions` varchar(20) NOT NULL COMMENT '版本号',
  `mactivetime` varchar(200) NOT NULL COMMENT '最后登陆时间 json格式 {1:"123456789"} gameid=>最后登陆时间',
  `mentercount` varchar(100) NOT NULL COMMENT '登陆的总数（每天统计一次）json格式 {1:"12"} gameid=>登陆次数',
  `mtime` varchar(200) NOT NULL COMMENT '注册时间 json格式 {1:"123456789"} gameid=>注册时间',
  `openid` varchar(255) NOT NULL,
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `icon` varchar(255) NOT NULL DEFAULT '' COMMENT '用户头像',
  `bankPWD` varchar(255) NOT NULL DEFAULT '' COMMENT '保险箱密码',
  `ctime` bigint(20) NOT NULL DEFAULT '0' COMMENT '注册时间',
  PRIMARY KEY (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_userinfo7
-- ----------------------------
DROP TABLE IF EXISTS `uc_userinfo7`;
CREATE TABLE `uc_userinfo7` (
  `mid` int(11) NOT NULL,
  `sid` smallint(6) NOT NULL COMMENT '账号类型ID',
  `cid` int(11) NOT NULL COMMENT '渠道ID',
  `pid` int(11) NOT NULL COMMENT '客户端包ID ',
  `ctype` tinyint(4) NOT NULL COMMENT '客户端类型 1 android 2 iphoe 3 ipad 4 android pad ',
  `mnick` varchar(50) NOT NULL COMMENT '昵称',
  `sitemid` varchar(40) NOT NULL COMMENT '平台ID',
  `sex` tinyint(4) NOT NULL DEFAULT '0' COMMENT '性别  0 未知 1男 2 女',
  `hometown` varchar(50) NOT NULL COMMENT '故乡',
  `mstatus` tinyint(4) NOT NULL DEFAULT '10' COMMENT '用户状态 1:禁止進入 8:用戶卸載應用后重裝 9:用戶需要更新資料 10:用戶正常狀態',
  `ip` int(11) unsigned NOT NULL COMMENT 'ip',
  `devicename` varchar(30) NOT NULL COMMENT '设备名称',
  `osversion` varchar(20) NOT NULL COMMENT '系统版本号',
  `nettype` tinyint(4) NOT NULL COMMENT '0无 1 3G 2 wifi 3 2G',
  `versions` varchar(20) NOT NULL COMMENT '版本号',
  `mactivetime` varchar(200) NOT NULL COMMENT '最后登陆时间 json格式 {1:"123456789"} gameid=>最后登陆时间',
  `mentercount` varchar(100) NOT NULL COMMENT '登陆的总数（每天统计一次）json格式 {1:"12"} gameid=>登陆次数',
  `mtime` varchar(200) NOT NULL COMMENT '注册时间 json格式 {1:"123456789"} gameid=>注册时间',
  `openid` varchar(255) NOT NULL,
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `icon` varchar(255) NOT NULL DEFAULT '' COMMENT '用户头像',
  `bankPWD` varchar(255) NOT NULL DEFAULT '' COMMENT '保险箱密码',
  `ctime` bigint(20) NOT NULL DEFAULT '0' COMMENT '注册时间',
  PRIMARY KEY (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_userinfo8
-- ----------------------------
DROP TABLE IF EXISTS `uc_userinfo8`;
CREATE TABLE `uc_userinfo8` (
  `mid` int(11) NOT NULL,
  `sid` smallint(6) NOT NULL COMMENT '账号类型ID',
  `cid` int(11) NOT NULL COMMENT '渠道ID',
  `pid` int(11) NOT NULL COMMENT '客户端包ID ',
  `ctype` tinyint(4) NOT NULL COMMENT '客户端类型 1 android 2 iphoe 3 ipad 4 android pad ',
  `mnick` varchar(50) NOT NULL COMMENT '昵称',
  `sitemid` varchar(40) NOT NULL COMMENT '平台ID',
  `sex` tinyint(4) NOT NULL DEFAULT '0' COMMENT '性别  0 未知 1男 2 女',
  `hometown` varchar(50) NOT NULL COMMENT '故乡',
  `mstatus` tinyint(4) NOT NULL DEFAULT '10' COMMENT '用户状态 1:禁止進入 8:用戶卸載應用后重裝 9:用戶需要更新資料 10:用戶正常狀態',
  `ip` int(11) unsigned NOT NULL COMMENT 'ip',
  `devicename` varchar(30) NOT NULL COMMENT '设备名称',
  `osversion` varchar(20) NOT NULL COMMENT '系统版本号',
  `nettype` tinyint(4) NOT NULL COMMENT '0无 1 3G 2 wifi 3 2G',
  `versions` varchar(20) NOT NULL COMMENT '版本号',
  `mactivetime` varchar(200) NOT NULL COMMENT '最后登陆时间 json格式 {1:"123456789"} gameid=>最后登陆时间',
  `mentercount` varchar(100) NOT NULL COMMENT '登陆的总数（每天统计一次）json格式 {1:"12"} gameid=>登陆次数',
  `mtime` varchar(200) NOT NULL COMMENT '注册时间 json格式 {1:"123456789"} gameid=>注册时间',
  `openid` varchar(255) NOT NULL,
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `icon` varchar(255) NOT NULL DEFAULT '' COMMENT '用户头像',
  `bankPWD` varchar(255) NOT NULL DEFAULT '' COMMENT '保险箱密码',
  `ctime` bigint(20) NOT NULL DEFAULT '0' COMMENT '注册时间',
  PRIMARY KEY (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_userinfo9
-- ----------------------------
DROP TABLE IF EXISTS `uc_userinfo9`;
CREATE TABLE `uc_userinfo9` (
  `mid` int(11) NOT NULL,
  `sid` smallint(6) NOT NULL COMMENT '账号类型ID',
  `cid` int(11) NOT NULL COMMENT '渠道ID',
  `pid` int(11) NOT NULL COMMENT '客户端包ID ',
  `ctype` tinyint(4) NOT NULL COMMENT '客户端类型 1 android 2 iphoe 3 ipad 4 android pad ',
  `mnick` varchar(50) NOT NULL COMMENT '昵称',
  `sitemid` varchar(40) NOT NULL COMMENT '平台ID',
  `sex` tinyint(4) NOT NULL DEFAULT '0' COMMENT '性别  0 未知 1男 2 女',
  `hometown` varchar(50) NOT NULL COMMENT '故乡',
  `mstatus` tinyint(4) NOT NULL DEFAULT '10' COMMENT '用户状态 1:禁止進入 8:用戶卸載應用后重裝 9:用戶需要更新資料 10:用戶正常狀態',
  `ip` int(11) unsigned NOT NULL COMMENT 'ip',
  `devicename` varchar(30) NOT NULL COMMENT '设备名称',
  `osversion` varchar(20) NOT NULL COMMENT '系统版本号',
  `nettype` tinyint(4) NOT NULL COMMENT '0无 1 3G 2 wifi 3 2G',
  `versions` varchar(20) NOT NULL COMMENT '版本号',
  `mactivetime` varchar(200) NOT NULL COMMENT '最后登陆时间 json格式 {1:"123456789"} gameid=>最后登陆时间',
  `mentercount` varchar(100) NOT NULL COMMENT '登陆的总数（每天统计一次）json格式 {1:"12"} gameid=>登陆次数',
  `mtime` varchar(200) NOT NULL COMMENT '注册时间 json格式 {1:"123456789"} gameid=>注册时间',
  `openid` varchar(255) NOT NULL,
  `productid` int(10) NOT NULL DEFAULT '1' COMMENT '产品id',
  `icon` varchar(255) NOT NULL DEFAULT '' COMMENT '用户头像',
  `bankPWD` varchar(255) NOT NULL DEFAULT '' COMMENT '保险箱密码',
  `ctime` bigint(20) NOT NULL DEFAULT '0' COMMENT '注册时间',
  PRIMARY KEY (`mid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for uc_versions
-- ----------------------------
DROP TABLE IF EXISTS `uc_versions`;
CREATE TABLE `uc_versions` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `gameid` tinyint(4) NOT NULL,
  `ctype` tinyint(4) NOT NULL,
  `cid` varchar(100) NOT NULL DEFAULT '0',
  `con` varchar(5) NOT NULL COMMENT '条件',
  `versions` varchar(40) NOT NULL COMMENT '版本号',
  `updatetype` tinyint(4) NOT NULL COMMENT '更新类型',
  `url` varchar(100) NOT NULL,
  `description` varchar(500) NOT NULL COMMENT '更新描述',
  `time` int(11) NOT NULL,
  `status` tinyint(4) NOT NULL COMMENT '状态 1 有效  2 失效',
  PRIMARY KEY (`id`),
  KEY `time` (`time`),
  KEY `ctype_status_gameid_cid` (`ctype`,`status`,`gameid`,`cid`,`time`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
