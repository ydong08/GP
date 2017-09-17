/*
Navicat MySQL Data Transfer

Source Server         : 192.168.114.51
Source Server Version : 50620
Source Host           : 192.168.114.51:3306
Source Database       : zq_admin

Target Server Type    : MYSQL
Target Server Version : 50620
File Encoding         : 65001

Date: 2017-03-21 16:39:06
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for aliaccount
-- ----------------------------
DROP TABLE IF EXISTS `aliaccount`;
CREATE TABLE `aliaccount` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '支付宝提现账号表',
  `partner` varchar(64) NOT NULL DEFAULT '' COMMENT '商户号',
  `key` varchar(64) NOT NULL DEFAULT '' COMMENT '密钥',
  `email` varchar(64) NOT NULL DEFAULT '' COMMENT '邮件',
  `account_name` varchar(255) NOT NULL DEFAULT '' COMMENT '公司名称',
  `status` int(2) NOT NULL DEFAULT '0' COMMENT '1为正在使用',
  `created_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' ON UPDATE CURRENT_TIMESTAMP,
  `updated_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for aliuser_record
-- ----------------------------
DROP TABLE IF EXISTS `aliuser_record`;
CREATE TABLE `aliuser_record` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `mid` int(20) NOT NULL DEFAULT '0' COMMENT '用户id',
  `alipay` varchar(255) NOT NULL DEFAULT '' COMMENT '支付宝账号',
  `aliname` varchar(255) NOT NULL DEFAULT '' COMMENT '支付宝真实姓名',
  `u_alipay` varchar(255) NOT NULL DEFAULT '' COMMENT '新支付宝账号',
  `u_aliname` varchar(255) NOT NULL DEFAULT '' COMMENT '新支付宝真实姓名',
  `ip` varchar(20) NOT NULL DEFAULT '' COMMENT '操作iP',
  `user` varchar(20) NOT NULL DEFAULT '' COMMENT '操作人',
  `updated_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '更新时间',
  `created_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '创建时间',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=4 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for channel_manager
-- ----------------------------
DROP TABLE IF EXISTS `channel_manager`;
CREATE TABLE `channel_manager` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `name` varchar(60) NOT NULL,
  `key` varchar(16) NOT NULL,
  `url` varchar(250) NOT NULL,
  `atime` int(10) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=8 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for cheat
-- ----------------------------
DROP TABLE IF EXISTS `cheat`;
CREATE TABLE `cheat` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `table_id` int(11) NOT NULL COMMENT '桌子id',
  `game_board` int(11) NOT NULL COMMENT '局数',
  `productid` int(11) NOT NULL COMMENT '产品ID',
  `gameid` int(11) NOT NULL COMMENT '游戏ID',
  `informantid` int(11) NOT NULL COMMENT '举报人id',
  `mids` varchar(32) NOT NULL COMMENT '用户id',
  `reply_type` int(11) NOT NULL DEFAULT '0' COMMENT '审核状态 1审核 0未审核',
  `created_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' ON UPDATE CURRENT_TIMESTAMP COMMENT '创建时间',
  `updated_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '跟新时间',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=18165 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for feedtpl
-- ----------------------------
DROP TABLE IF EXISTS `feedtpl`;
CREATE TABLE `feedtpl` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `uid` int(11) NOT NULL DEFAULT '1',
  `tpl` tinyint(3) NOT NULL DEFAULT '1' COMMENT '1欢迎语、2游戏、3充值、4提款、5结束语',
  `content` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=30 DEFAULT CHARSET=utf8 COMMENT='反馈回复模板';

-- ----------------------------
-- Table structure for forbid
-- ----------------------------
DROP TABLE IF EXISTS `forbid`;
CREATE TABLE `forbid` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '用户禁止登陆表',
  `forbid` varchar(50) NOT NULL DEFAULT '' COMMENT '禁止字段',
  `type` int(11) NOT NULL DEFAULT '1' COMMENT '1  设备号禁止   2   ip禁止',
  `forbid_type` int(11) NOT NULL DEFAULT '1' COMMENT '1  登陆禁止  2 注册禁止  3 登陆注册禁止',
  `create_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '创建时间',
  `updated_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for kucun_set
-- ----------------------------
DROP TABLE IF EXISTS `kucun_set`;
CREATE TABLE `kucun_set` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `uid` int(11) DEFAULT NULL,
  `bankerwin_before` int(11) DEFAULT NULL,
  `userbanker_before` int(11) DEFAULT NULL,
  `bankerwin_after` int(11) DEFAULT NULL,
  `userbanker_after` int(11) DEFAULT NULL,
  `type` tinyint(2) DEFAULT NULL COMMENT '1:百人牛牛 2：百人金华',
  `ctime` int(10) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=13 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for log_pushcard
-- ----------------------------
DROP TABLE IF EXISTS `log_pushcard`;
CREATE TABLE `log_pushcard` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `pai` varchar(200) NOT NULL DEFAULT '',
  `uid` int(10) NOT NULL DEFAULT '0',
  `otime` int(10) NOT NULL DEFAULT '0',
  `ip` varchar(20) NOT NULL DEFAULT '',
  `gameid` int(10) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=19 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for migrations
-- ----------------------------
DROP TABLE IF EXISTS `migrations`;
CREATE TABLE `migrations` (
  `migration` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `batch` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- ----------------------------
-- Table structure for password_record
-- ----------------------------
DROP TABLE IF EXISTS `password_record`;
CREATE TABLE `password_record` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `ip` varchar(20) NOT NULL COMMENT '操作IP',
  `mid` int(11) NOT NULL COMMENT '用户ID',
  `upassword` varchar(50) NOT NULL COMMENT '修改登录密码',
  `name` varchar(50) NOT NULL COMMENT '操作管理员',
  `type` int(11) NOT NULL COMMENT '1:密码修改 2:保险箱修改',
  `baoxiangui` varchar(200) NOT NULL COMMENT '保险箱密码',
  `created_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `updated_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=11 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for password_resets
-- ----------------------------
DROP TABLE IF EXISTS `password_resets`;
CREATE TABLE `password_resets` (
  `email` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `token` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `created_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  KEY `password_resets_email_index` (`email`),
  KEY `password_resets_token_index` (`token`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- ----------------------------
-- Table structure for payali
-- ----------------------------
DROP TABLE IF EXISTS `payali`;
CREATE TABLE `payali` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '支付宝提现账号表',
  `partner` varchar(64) NOT NULL DEFAULT '' COMMENT '商户号',
  `key` varchar(64) NOT NULL DEFAULT '' COMMENT '密钥',
  `email` varchar(64) NOT NULL DEFAULT '' COMMENT '邮件',
  `account_name` varchar(255) NOT NULL DEFAULT '' COMMENT '公司名称',
  `status` int(2) NOT NULL DEFAULT '0',
  `remark` varchar(255) NOT NULL DEFAULT '' COMMENT '备注',
  `created_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' ON UPDATE CURRENT_TIMESTAMP,
  `updated_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for paycard77
-- ----------------------------
DROP TABLE IF EXISTS `paycard77`;
CREATE TABLE `paycard77` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '支付宝提现账号表',
  `agentid` varchar(64) NOT NULL DEFAULT '' COMMENT '商户id',
  `key` varchar(64) NOT NULL DEFAULT '' COMMENT '密钥',
  `account_name` varchar(255) NOT NULL DEFAULT '' COMMENT '商户名称',
  `status` int(2) NOT NULL DEFAULT '0',
  `remark` varchar(255) NOT NULL DEFAULT '' COMMENT '备注',
  `created_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' ON UPDATE CURRENT_TIMESTAMP,
  `updated_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for payhee
-- ----------------------------
DROP TABLE IF EXISTS `payhee`;
CREATE TABLE `payhee` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '支付宝提现账号表',
  `agentid` varchar(64) NOT NULL DEFAULT '' COMMENT '商户id',
  `key` varchar(64) NOT NULL DEFAULT '' COMMENT '密钥',
  `account_name` varchar(255) NOT NULL DEFAULT '' COMMENT '商户名称',
  `status` int(2) NOT NULL DEFAULT '0',
  `remark` varchar(255) NOT NULL DEFAULT '' COMMENT '备注',
  `created_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' ON UPDATE CURRENT_TIMESTAMP,
  `updated_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for paywx
-- ----------------------------
DROP TABLE IF EXISTS `paywx`;
CREATE TABLE `paywx` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '支付宝提现账号表',
  `mchid` varchar(64) NOT NULL DEFAULT '' COMMENT '商户号',
  `key` varchar(64) NOT NULL DEFAULT '' COMMENT '密钥',
  `appid` varchar(64) NOT NULL DEFAULT '' COMMENT '应用id',
  `account_name` varchar(255) NOT NULL DEFAULT '' COMMENT '微信名称',
  `status` int(2) NOT NULL DEFAULT '0',
  `remark` varchar(255) NOT NULL DEFAULT '' COMMENT '备注',
  `created_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' ON UPDATE CURRENT_TIMESTAMP,
  `updated_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for permissions
-- ----------------------------
DROP TABLE IF EXISTS `permissions`;
CREATE TABLE `permissions` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `label` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `description` varchar(255) COLLATE utf8_unicode_ci DEFAULT NULL,
  `status` int(4) NOT NULL DEFAULT '1' COMMENT '系统日志是否需要记录该操作',
  `created_at` timestamp NULL DEFAULT NULL,
  `updated_at` timestamp NULL DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=134 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- ----------------------------
-- Table structure for permission_role
-- ----------------------------
DROP TABLE IF EXISTS `permission_role`;
CREATE TABLE `permission_role` (
  `permission_id` int(10) unsigned NOT NULL,
  `role_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`permission_id`,`role_id`),
  KEY `permission_role_role_id_foreign` (`role_id`),
  CONSTRAINT `permission_role_ibfk_1` FOREIGN KEY (`permission_id`) REFERENCES `permissions` (`id`) ON DELETE CASCADE,
  CONSTRAINT `permission_role_ibfk_2` FOREIGN KEY (`role_id`) REFERENCES `roles` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- ----------------------------
-- Table structure for product
-- ----------------------------
DROP TABLE IF EXISTS `product`;
CREATE TABLE `product` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '产品表',
  `name` varchar(255) NOT NULL DEFAULT '' COMMENT '产品名称',
  `desc` varchar(255) NOT NULL DEFAULT '' COMMENT '产品描述',
  `alipayid` int(11) NOT NULL,
  `wxpayid` int(11) NOT NULL,
  `heepayid` int(11) NOT NULL,
  `card77id` int(11) NOT NULL DEFAULT '0' COMMENT 'card77账号',
  `url` varchar(255) NOT NULL DEFAULT '' COMMENT '默认更新url',
  `leader` varchar(255) NOT NULL DEFAULT '' COMMENT '项目负责人',
  `type` int(4) NOT NULL DEFAULT '2' COMMENT '1：安卓 2：ios',
  `appkey` varchar(32) NOT NULL DEFAULT '' COMMENT '产品唯一码',
  `created_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' ON UPDATE CURRENT_TIMESTAMP COMMENT '创建时间',
  `updated_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '更新时间',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=123123143 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for product_route
-- ----------------------------
DROP TABLE IF EXISTS `product_route`;
CREATE TABLE `product_route` (
  `appkey` varchar(32) NOT NULL DEFAULT '' COMMENT '应用key',
  `version` int(11) NOT NULL DEFAULT '1' COMMENT '版本号',
  `url` varchar(255) NOT NULL DEFAULT '' COMMENT '连接'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for product_version
-- ----------------------------
DROP TABLE IF EXISTS `product_version`;
CREATE TABLE `product_version` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '产品版本管理',
  `productid` int(11) NOT NULL DEFAULT '0' COMMENT '产品id',
  `version` int(25) NOT NULL DEFAULT '1' COMMENT '版本号',
  `desc` varchar(255) NOT NULL DEFAULT '' COMMENT '版本描述',
  `type` int(4) NOT NULL DEFAULT '1' COMMENT '1:测试 2:审核中 3:线上',
  `url` varchar(255) NOT NULL DEFAULT '' COMMENT '该版本更新url',
  `update` int(4) NOT NULL DEFAULT '0' COMMENT '是否强制更新',
  `created_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' ON UPDATE CURRENT_TIMESTAMP COMMENT '创建时间',
  `updated_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '更新时间',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1267 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for roles
-- ----------------------------
DROP TABLE IF EXISTS `roles`;
CREATE TABLE `roles` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `label` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `description` varchar(255) COLLATE utf8_unicode_ci DEFAULT NULL,
  `created_at` timestamp NULL DEFAULT NULL,
  `updated_at` timestamp NULL DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- ----------------------------
-- Table structure for role_user
-- ----------------------------
DROP TABLE IF EXISTS `role_user`;
CREATE TABLE `role_user` (
  `user_id` int(10) unsigned NOT NULL,
  `role_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`role_id`,`user_id`),
  KEY `role_user_user_id_foreign` (`user_id`),
  CONSTRAINT `role_user_ibfk_1` FOREIGN KEY (`role_id`) REFERENCES `roles` (`id`) ON DELETE CASCADE,
  CONSTRAINT `role_user_ibfk_2` FOREIGN KEY (`user_id`) REFERENCES `users` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- ----------------------------
-- Table structure for roll_message
-- ----------------------------
DROP TABLE IF EXISTS `roll_message`;
CREATE TABLE `roll_message` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `productid` int(10) NOT NULL DEFAULT '1',
  `content` text NOT NULL,
  `logs` int(10) NOT NULL DEFAULT '1' COMMENT '发送次数',
  `sort` int(10) NOT NULL COMMENT '排序',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=13 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for tips
-- ----------------------------
DROP TABLE IF EXISTS `tips`;
CREATE TABLE `tips` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `productid` int(10) NOT NULL DEFAULT '0' COMMENT '产品id',
  `cateid` int(4) NOT NULL DEFAULT '0' COMMENT 'tips类别',
  `classid` int(4) NOT NULL DEFAULT '0' COMMENT 'tips分类',
  `sort` int(10) NOT NULL DEFAULT '0' COMMENT '排序值',
  `status` int(4) NOT NULL DEFAULT '1' COMMENT '1为播放 0为不播放',
  `content` varchar(255) NOT NULL DEFAULT '' COMMENT 'tips内容',
  `created_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '创建时间',
  `updated_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '更新时间',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for tips_cate
-- ----------------------------
DROP TABLE IF EXISTS `tips_cate`;
CREATE TABLE `tips_cate` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'tip类型表',
  `name` varchar(255) NOT NULL DEFAULT '' COMMENT '分类名字',
  `type` int(4) NOT NULL DEFAULT '1' COMMENT '1为tips类别  2为tips分类',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for users
-- ----------------------------
DROP TABLE IF EXISTS `users`;
CREATE TABLE `users` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `email` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `password` varchar(60) COLLATE utf8_unicode_ci NOT NULL,
  `remember_token` varchar(100) COLLATE utf8_unicode_ci DEFAULT NULL,
  `productids` varchar(255) COLLATE utf8_unicode_ci NOT NULL DEFAULT '' COMMENT '能查看的产品',
  `created_at` timestamp NULL DEFAULT NULL,
  `updated_at` timestamp NULL DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `users_email_unique` (`email`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- ----------------------------
-- Table structure for user_log
-- ----------------------------
DROP TABLE IF EXISTS `user_log`;
CREATE TABLE `user_log` (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT '用户操作记录表',
  `userid` int(11) NOT NULL DEFAULT '0' COMMENT '用户id',
  `name` varchar(64) NOT NULL DEFAULT '' COMMENT '用户名',
  `desc` varchar(255) NOT NULL DEFAULT '' COMMENT '操作描述',
  `ip` bigint(20) NOT NULL DEFAULT '0',
  `created_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '创建时间',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1185 DEFAULT CHARSET=utf8;
