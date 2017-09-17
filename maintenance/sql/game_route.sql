/*
Navicat MySQL Data Transfer

Source Server         : 192.168.114.51
Source Server Version : 50620
Source Host           : 192.168.114.51:3306
Source Database       : game_route

Target Server Type    : MYSQL
Target Server Version : 50620
File Encoding         : 65001

Date: 2017-03-21 16:39:42
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for gameresource
-- ----------------------------
DROP TABLE IF EXISTS `gameresource`;
CREATE TABLE `gameresource` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `name` varchar(50) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for games
-- ----------------------------
DROP TABLE IF EXISTS `games`;
CREATE TABLE `games` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `name` varchar(50) NOT NULL,
  `levels` varchar(200) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=15 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for levels
-- ----------------------------
DROP TABLE IF EXISTS `levels`;
CREATE TABLE `levels` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `gameid` int(10) NOT NULL DEFAULT '0' COMMENT '产品id',
  `levelid` int(10) NOT NULL DEFAULT '0' COMMENT 'levelid',
  `name` varchar(50) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `product` (`gameid`,`levelid`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=48 DEFAULT CHARSET=utf8;

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
) ENGINE=InnoDB AUTO_INCREMENT=172 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

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
  `leader` varchar(255) NOT NULL,
  `proleader` varchar(255) NOT NULL,
  `artleader` varchar(255) NOT NULL,
  `jishuleader` varchar(255) NOT NULL,
  `gameids` varchar(2550) NOT NULL,
  `desc` varchar(255) NOT NULL DEFAULT '' COMMENT '产品描述',
  `type` int(4) NOT NULL DEFAULT '2' COMMENT '1：安卓 2：ios',
  `appkey` varchar(32) NOT NULL DEFAULT '' COMMENT '产品唯一码',
  `created_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' ON UPDATE CURRENT_TIMESTAMP COMMENT '创建时间',
  `updated_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '更新时间',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1524 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for product_gamelobby
-- ----------------------------
DROP TABLE IF EXISTS `product_gamelobby`;
CREATE TABLE `product_gamelobby` (
  `productid` int(11) NOT NULL,
  `version` int(11) NOT NULL,
  `name` varchar(255) NOT NULL,
  `sort` int(6) NOT NULL DEFAULT '0' COMMENT '排序值',
  PRIMARY KEY (`productid`,`version`,`name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='游戏大厅表';

-- ----------------------------
-- Table structure for product_gamesetting
-- ----------------------------
DROP TABLE IF EXISTS `product_gamesetting`;
CREATE TABLE `product_gamesetting` (
  `productid` int(11) NOT NULL,
  `version` int(11) NOT NULL,
  `gameid` int(11) NOT NULL DEFAULT '0' COMMENT '游戏id',
  `labby` varchar(255) NOT NULL DEFAULT '' COMMENT '大厅',
  `level` int(4) NOT NULL DEFAULT '0' COMMENT '房间lervel',
  `sort` int(6) NOT NULL DEFAULT '0' COMMENT '排序值',
  PRIMARY KEY (`productid`,`version`,`gameid`,`level`,`labby`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

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
) ENGINE=InnoDB AUTO_INCREMENT=8 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

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
-- Table structure for route_area
-- ----------------------------
DROP TABLE IF EXISTS `route_area`;
CREATE TABLE `route_area` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `area_desc` varchar(64) NOT NULL,
  `url` varchar(2048) NOT NULL,
  `backurl` varchar(2048) NOT NULL,
  `paycenter` varchar(1024) NOT NULL,
  `area` char(1) NOT NULL DEFAULT 'A',
  PRIMARY KEY (`id`),
  UNIQUE KEY `route_area_id_uindex` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8 COMMENT='路由分区表';

-- ----------------------------
-- Table structure for route_product
-- ----------------------------
DROP TABLE IF EXISTS `route_product`;
CREATE TABLE `route_product` (
  `appkey` varchar(32) NOT NULL DEFAULT '' COMMENT '应用key',
  `version` int(11) NOT NULL DEFAULT '1' COMMENT '版本号',
  `desc` varchar(355) NOT NULL COMMENT '版本描述',
  `area_id` int(11) NOT NULL,
  KEY `route_product_route_area_id_fk` (`area_id`),
  CONSTRAINT `route_product_route_area_id_fk` FOREIGN KEY (`area_id`) REFERENCES `route_area` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for route_reloadurl
-- ----------------------------
DROP TABLE IF EXISTS `route_reloadurl`;
CREATE TABLE `route_reloadurl` (
  `url` varchar(255) NOT NULL DEFAULT '' COMMENT '路由刷新路径',
  PRIMARY KEY (`url`)
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
  `created_at` timestamp NULL DEFAULT NULL,
  `updated_at` timestamp NULL DEFAULT NULL,
  `productids` varchar(300) COLLATE utf8_unicode_ci DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `users_email_unique` (`email`)
) ENGINE=InnoDB AUTO_INCREMENT=1024 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
