/*
Navicat MySQL Data Transfer

Source Server         : 192.168.114.51
Source Server Version : 50620
Source Host           : 192.168.114.51:3306
Source Database       : paycenter

Target Server Type    : MYSQL
Target Server Version : 50620
File Encoding         : 65001

Date: 2017-03-21 16:39:25
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for pc_admin_log
-- ----------------------------
DROP TABLE IF EXISTS `pc_admin_log`;
CREATE TABLE `pc_admin_log` (
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
-- Table structure for pc_admin_member
-- ----------------------------
DROP TABLE IF EXISTS `pc_admin_member`;
CREATE TABLE `pc_admin_member` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `username` varchar(30) NOT NULL,
  `password` varchar(40) NOT NULL,
  `privilege` tinyint(4) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for pc_card_batch
-- ----------------------------
DROP TABLE IF EXISTS `pc_card_batch`;
CREATE TABLE `pc_card_batch` (
  `batch` int(11) NOT NULL AUTO_INCREMENT COMMENT '批次',
  PRIMARY KEY (`batch`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for pc_card_cardno
-- ----------------------------
DROP TABLE IF EXISTS `pc_card_cardno`;
CREATE TABLE `pc_card_cardno` (
  `cardno` int(11) NOT NULL AUTO_INCREMENT COMMENT '卡号',
  PRIMARY KEY (`cardno`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for pc_card_info
-- ----------------------------
DROP TABLE IF EXISTS `pc_card_info`;
CREATE TABLE `pc_card_info` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `cardno` varchar(40) NOT NULL COMMENT '卡号',
  `password` int(11) NOT NULL COMMENT '密码',
  `batch` varchar(11) NOT NULL COMMENT '批次号',
  `cardtype` int(11) NOT NULL DEFAULT '1' COMMENT '卡的类型 1 虚拟卡  2 实体卡',
  `price` float NOT NULL COMMENT '价格',
  `type` int(11) NOT NULL COMMENT '1游戏币,2点乐币,3 会员卡,4 道具',
  `exchangenum` int(11) NOT NULL COMMENT '兑换数量',
  `exptime` int(11) NOT NULL COMMENT 'vip天数',
  `ctime` int(11) NOT NULL,
  `gameid` int(11) NOT NULL COMMENT '游戏ID',
  `mid` int(11) NOT NULL COMMENT '使用者',
  `utime` int(11) NOT NULL COMMENT '使用时间',
  `status` tinyint(4) NOT NULL DEFAULT '1' COMMENT '状态 1 未使用 2已使用 3 已封停',
  PRIMARY KEY (`id`),
  KEY `cardno` (`cardno`),
  KEY `gameid` (`gameid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for pc_currency
-- ----------------------------
DROP TABLE IF EXISTS `pc_currency`;
CREATE TABLE `pc_currency` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `gameid` tinyint(4) NOT NULL COMMENT '游戏ID（以区分多个游戏）',
  `cid` int(11) NOT NULL COMMENT '渠道ID',
  `pmode` varchar(50) NOT NULL COMMENT '支付中心配置支付渠道ID',
  `type` int(11) NOT NULL COMMENT '1游戏币,2点乐币,3 会员卡,4 道具',
  `identifier` varchar(40) NOT NULL,
  `name` varchar(30) NOT NULL COMMENT '名称',
  `price` float NOT NULL COMMENT '价格',
  `desc` varchar(200) NOT NULL DEFAULT ' ' COMMENT '描述',
  `exchangenum` int(11) NOT NULL COMMENT '金币数',
  `order` tinyint(4) NOT NULL DEFAULT '0' COMMENT '排序',
  `label` tinyint(4) NOT NULL DEFAULT '0' COMMENT '标签 0正常 1 打折 2热门',
  `img` varchar(100) NOT NULL DEFAULT ' ' COMMENT '图片路径',
  `originmoney` varchar(30) NOT NULL COMMENT '原来的金币数',
  `exptime` int(11) NOT NULL COMMENT '天数',
  `versions` varchar(10) NOT NULL DEFAULT '1.0.0' COMMENT '版本号 如1.2.0 表示大于1.2.0才能加载此商品',
  `status` smallint(4) NOT NULL DEFAULT '2' COMMENT '状态0为下架，1为上架 2测试中',
  PRIMARY KEY (`id`),
  KEY `versions` (`versions`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for pc_iospay_redelivery
-- ----------------------------
DROP TABLE IF EXISTS `pc_iospay_redelivery`;
CREATE TABLE `pc_iospay_redelivery` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `mid` int(11) NOT NULL,
  `transaction_id` varchar(30) NOT NULL,
  `pdealno` varchar(30) NOT NULL,
  `reqparam` varchar(9000) NOT NULL,
  `ctime` int(11) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `mid` (`mid`),
  KEY `pdealno` (`pdealno`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for pc_liantong_device
-- ----------------------------
DROP TABLE IF EXISTS `pc_liantong_device`;
CREATE TABLE `pc_liantong_device` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `pdealno` varchar(30) NOT NULL,
  `macid` varchar(40) NOT NULL,
  `ip` char(12) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `pdealno` (`pdealno`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for pc_logpayment1
-- ----------------------------
DROP TABLE IF EXISTS `pc_logpayment1`;
CREATE TABLE `pc_logpayment1` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `mid` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '充值人:0为系统',
  `versions` varchar(10) NOT NULL COMMENT '客户端版本号',
  `pmoneynow` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '充值时的金币数',
  `gameid` int(11) NOT NULL DEFAULT '1' COMMENT '游戏ID',
  `sid` smallint(3) NOT NULL COMMENT '账号类型ID',
  `cid` int(11) NOT NULL COMMENT '渠道ID',
  `pid` int(11) NOT NULL,
  `ip` varchar(20) NOT NULL COMMENT '下单时的IP地址',
  `ctype` tinyint(4) NOT NULL COMMENT '客户端类型',
  `pmode` smallint(5) unsigned NOT NULL DEFAULT '0' COMMENT '充值方式',
  `ptype` smallint(5) unsigned NOT NULL DEFAULT '0' COMMENT '充值货币类型 1金币 2 元宝 3 会员卡',
  `pamount` float unsigned NOT NULL COMMENT '充值数目',
  `predelay` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '下单到发货的时间间隔',
  `pexchangenum` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '充值所获取的乐币/金币/道具数量',
  `source` tinyint(4) NOT NULL DEFAULT '0' COMMENT '0 商城  1 快速充值  2 首充  3 破产充值  4限时抢购  5 活动网页支付',
  `pbankno` varchar(50) NOT NULL DEFAULT '' COMMENT '银行交易号',
  `viptime` int(11) NOT NULL COMMENT '会员时间',
  `pdealno` varchar(50) NOT NULL,
  `pfee` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '运营商区别 1 移动 2 联通 3电信',
  `ptime` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '充值日期',
  `pstatus` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '下单状态:1 付款状态  1 支付状态  3 发货状态 ',
  `wmconfig` varchar(300) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `pstatus` (`pstatus`),
  KEY `ptime` (`ptime`),
  KEY `fmid` (`mid`),
  KEY `pdealno` (`pdealno`),
  KEY `pbankno` (`pbankno`),
  KEY `gameid` (`gameid`),
  KEY `pmode` (`pmode`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='用户充值记录表';

-- ----------------------------
-- Table structure for pc_monitoring
-- ----------------------------
DROP TABLE IF EXISTS `pc_monitoring`;
CREATE TABLE `pc_monitoring` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `type` tinyint(4) NOT NULL COMMENT '1时间段报警 2 间隔报警',
  `pmode` int(11) NOT NULL,
  `timelag` varchar(50) NOT NULL,
  `compare` tinyint(4) NOT NULL COMMENT '1 大于 2 小于 3 等于',
  `param` int(11) NOT NULL COMMENT '报警参数 1.订单数 2.成功订单数 3.成功付费人数 4.付费总额',
  `value` int(11) NOT NULL COMMENT '数值',
  `status` int(11) NOT NULL COMMENT '状态',
  `sendcount` int(11) NOT NULL COMMENT '短信发送次数',
  `ctime` int(11) NOT NULL COMMENT '时间',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for pc_payment
-- ----------------------------
DROP TABLE IF EXISTS `pc_payment`;
CREATE TABLE `pc_payment` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `mid` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '充值人:0为系统',
  `versions` varchar(10) NOT NULL COMMENT '客户端版本号',
  `pmoneynow` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '充值时的金币数',
  `productid` int(11) NOT NULL DEFAULT '1' COMMENT '产品id',
  `gameid` int(11) NOT NULL DEFAULT '1' COMMENT '游戏ID',
  `sid` smallint(3) NOT NULL COMMENT '账号类型ID',
  `cid` int(11) NOT NULL COMMENT '渠道ID',
  `pid` int(11) NOT NULL,
  `ip` varchar(64) NOT NULL COMMENT '下单时的IP地址',
  `ctype` tinyint(4) NOT NULL COMMENT '客户端类型',
  `pmode` smallint(5) unsigned NOT NULL DEFAULT '0' COMMENT '充值方式',
  `ptype` smallint(5) unsigned NOT NULL DEFAULT '0' COMMENT '充值货币类型 1金币 2 元宝 3 会员卡',
  `pamount` float unsigned NOT NULL COMMENT '充值数目',
  `predelay` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '下单到发货的时间间隔',
  `pexchangenum` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '充值所获取的乐币/金币/道具数量',
  `source` int(11) NOT NULL DEFAULT '0' COMMENT '0 商城  1 快速充值  2 首充  3 破产充值  4限时抢购  5 活动网页支付',
  `pbankno` varchar(50) NOT NULL DEFAULT '' COMMENT '银行交易号',
  `viptime` int(11) NOT NULL COMMENT '会员时间',
  `pdealno` varchar(50) NOT NULL,
  `pfee` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '运营商区别 1 移动 2 联通 3电信',
  `ptime` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '充值日期',
  `pstatus` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '下单状态:1 付款状态  1 支付状态  3 发货状态 ',
  `wmconfig` varchar(300) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `pstatus` (`pstatus`),
  KEY `ptime` (`ptime`),
  KEY `fmid` (`mid`),
  KEY `pdealno` (`pdealno`),
  KEY `pbankno` (`pbankno`),
  KEY `gameid` (`gameid`),
  KEY `pmode` (`pmode`)
) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=utf8 COMMENT='用户充值记录表';

-- ----------------------------
-- Table structure for pc_paymentid
-- ----------------------------
DROP TABLE IF EXISTS `pc_paymentid`;
CREATE TABLE `pc_paymentid` (
  `pid` int(11) NOT NULL AUTO_INCREMENT,
  PRIMARY KEY (`pid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for pc_pmode
-- ----------------------------
DROP TABLE IF EXISTS `pc_pmode`;
CREATE TABLE `pc_pmode` (
  `pmode` int(11) NOT NULL AUTO_INCREMENT,
  `payname` varchar(20) NOT NULL COMMENT '支付渠道名称',
  `status` tinyint(4) NOT NULL DEFAULT '1' COMMENT '是否显示 1显示  0 不显示',
  `order` tinyint(4) NOT NULL COMMENT '排序',
  `idc` int(11) NOT NULL DEFAULT '1',
  PRIMARY KEY (`pmode`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for pc_withdrawals
-- ----------------------------
DROP TABLE IF EXISTS `pc_withdrawals`;
CREATE TABLE `pc_withdrawals` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `mid` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '充值人:0为系统',
  `versions` varchar(10) NOT NULL COMMENT '客户端版本号',
  `pmoneynow` float(10,2) unsigned NOT NULL DEFAULT '0.00' COMMENT '充值时的金币数',
  `productid` int(11) NOT NULL DEFAULT '1' COMMENT '产品id',
  `gameid` int(11) NOT NULL DEFAULT '1' COMMENT '游戏ID',
  `sid` smallint(3) NOT NULL COMMENT '账号类型ID',
  `cid` int(11) NOT NULL COMMENT '渠道ID',
  `pid` int(11) NOT NULL,
  `ip` varchar(64) NOT NULL COMMENT '下单时的IP地址',
  `ctype` tinyint(4) NOT NULL DEFAULT '2' COMMENT '客户端类型,1为安卓，2为ios',
  `pmode` smallint(5) unsigned NOT NULL DEFAULT '0' COMMENT '提现方式方式',
  `pamount` float(10,2) unsigned NOT NULL COMMENT '提现数目',
  `predelay` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '下单到发货的时间间隔',
  `pexchangenum` float(10,2) unsigned NOT NULL DEFAULT '0.00' COMMENT '提现所需要的金币数',
  `pbankno` varchar(50) NOT NULL DEFAULT '' COMMENT '银行交易号',
  `pdealno` varchar(50) NOT NULL,
  `batchno` varchar(50) NOT NULL DEFAULT '' COMMENT '支付宝批次号',
  `batchkey` varchar(50) NOT NULL DEFAULT '' COMMENT '该批次中的序号',
  `pfee` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '运营商区别 1 移动 2 联通 3电信',
  `ptime` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '提现日期',
  `pstatus` tinyint(3) NOT NULL DEFAULT '0' COMMENT '提现状态:1 提交申请  2 审核通过  3 已到账 -1 审核未通过',
  `counterFee` int(10) NOT NULL DEFAULT '0' COMMENT '手续费',
  `remark` varchar(255) NOT NULL DEFAULT '' COMMENT '备注',
  `updated_at` int(10) NOT NULL DEFAULT '0' COMMENT '更新时间',
  PRIMARY KEY (`id`),
  KEY `pstatus` (`pstatus`),
  KEY `ptime` (`ptime`),
  KEY `fmid` (`mid`),
  KEY `pdealno` (`pdealno`),
  KEY `pbankno` (`pbankno`),
  KEY `gameid` (`gameid`),
  KEY `pmode` (`pmode`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8 COMMENT='用户提现记录表';

-- ----------------------------
-- Table structure for pc_wm_config
-- ----------------------------
DROP TABLE IF EXISTS `pc_wm_config`;
CREATE TABLE `pc_wm_config` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `gameid` int(11) NOT NULL,
  `cid` int(11) NOT NULL,
  `pmode` varchar(50) NOT NULL COMMENT '支付渠道ID',
  `price` int(11) NOT NULL,
  `identifier` varchar(40) NOT NULL,
  `exchangenum` int(11) NOT NULL COMMENT '兑换金币总数',
  `presetprice` int(11) NOT NULL COMMENT '相当于买多少钱的金币',
  `name` varchar(30) NOT NULL,
  `day1` int(11) NOT NULL COMMENT '第一天可领取的金币数',
  `day2` int(11) NOT NULL,
  `day3` int(11) NOT NULL,
  `day4` int(11) NOT NULL,
  `day5` int(11) NOT NULL,
  `day6` int(11) NOT NULL,
  `day8` int(11) NOT NULL,
  `day10` int(11) NOT NULL,
  `status` int(11) NOT NULL DEFAULT '2',
  `sort` int(11) NOT NULL COMMENT '顺序',
  `versions` varchar(10) NOT NULL DEFAULT '1.0.0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
