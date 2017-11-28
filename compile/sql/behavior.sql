/*
Navicat MySQL Data Transfer

Source Server         : 192.168.114.51
Source Server Version : 50620
Source Host           : 192.168.114.51:3306
Source Database       : behavior

Target Server Type    : MYSQL
Target Server Version : 50620
File Encoding         : 65001

Date: 2017-03-21 16:39:51
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for user_device
-- ----------------------------
DROP TABLE IF EXISTS `user_device`;
CREATE TABLE `user_device` (
  `device` varchar(255) NOT NULL DEFAULT '' COMMENT '用户设备号',
  `remark` varchar(255) NOT NULL DEFAULT '' COMMENT '备注',
  `ip` bigint(20) NOT NULL,
  `addr` varchar(255) NOT NULL DEFAULT '' COMMENT '客户端地址',
  `type` int(2) NOT NULL DEFAULT '0' COMMENT '1为ios审核组',
  `exchange_switch` int(2) NOT NULL DEFAULT '1' COMMENT '兑换开关，开启为1，关闭为0',
  `time` bigint(20) NOT NULL DEFAULT '0' COMMENT '用户最近登录时间',
  PRIMARY KEY (`device`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
