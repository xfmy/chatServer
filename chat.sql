/*
 Navicat Premium Data Transfer

 Source Server         : 子系统
 Source Server Type    : MySQL
 Source Server Version : 80036
 Source Host           : 172.20.114.39:3306
 Source Schema         : chat

 Target Server Type    : MySQL
 Target Server Version : 80036
 File Encoding         : 65001

 Date: 13/06/2024 13:11:10
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for allgroup
-- ----------------------------
DROP TABLE IF EXISTS `allgroup`;
CREATE TABLE `allgroup`  (
  `id` int(0) NOT NULL AUTO_INCREMENT,
  `groupname` varchar(50) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL,
  `groupdesc` varchar(200) CHARACTER SET latin1 COLLATE latin1_swedish_ci NULL DEFAULT '',
  PRIMARY KEY (`id`) USING BTREE,
  UNIQUE INDEX `groupname`(`groupname`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb3 COLLATE = utf8mb3_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of allgroup
-- ----------------------------
INSERT INTO `allgroup` VALUES (1, 'C++ chat project', 'start develop a chat project');

-- ----------------------------
-- Table structure for friend
-- ----------------------------
DROP TABLE IF EXISTS `friend`;
CREATE TABLE `friend`  (
  `userid` int(0) NOT NULL,
  `friendid` int(0) NOT NULL,
  INDEX `userid`(`userid`, `friendid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb3 COLLATE = utf8mb3_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of friend
-- ----------------------------
INSERT INTO `friend` VALUES (13, 15);
INSERT INTO `friend` VALUES (13, 21);
INSERT INTO `friend` VALUES (21, 13);
INSERT INTO `friend` VALUES (21, 15);

-- ----------------------------
-- Table structure for groupuser
-- ----------------------------
DROP TABLE IF EXISTS `groupuser`;
CREATE TABLE `groupuser`  (
  `groupid` int(0) NOT NULL,
  `userid` int(0) NOT NULL,
  `grouprole` enum('creator','normal') CHARACTER SET latin1 COLLATE latin1_swedish_ci NULL DEFAULT NULL,
  INDEX `groupid`(`groupid`, `userid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb3 COLLATE = utf8mb3_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of groupuser
-- ----------------------------
INSERT INTO `groupuser` VALUES (1, 13, 'creator');
INSERT INTO `groupuser` VALUES (1, 21, 'normal');
INSERT INTO `groupuser` VALUES (1, 19, 'normal');

-- ----------------------------
-- Table structure for offlinemessage
-- ----------------------------
DROP TABLE IF EXISTS `offlinemessage`;
CREATE TABLE `offlinemessage`  (
  `userid` int(0) NOT NULL,
  `message` varchar(500) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL
) ENGINE = InnoDB CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of offlinemessage
-- ----------------------------
INSERT INTO `offlinemessage` VALUES (19, '{\"groupid\":1,\"id\":21,\"msg\":\"hello\",\"msgid\":10,\"name\":\"gao yang\",\"time\":\"2024-02-22 00:43:59\"}');
INSERT INTO `offlinemessage` VALUES (19, '{\"groupid\":1,\"id\":21,\"msg\":\"helo!!!\",\"msgid\":10,\"name\":\"gao yang\",\"time\":\"2024-02-22 22:43:21\"}');
INSERT INTO `offlinemessage` VALUES (19, '{\"groupid\":1,\"id\":13,\"msg\":\"hahahahaha\",\"msgid\":10,\"name\":\"zhang san\",\"time\":\"2024-02-22 22:59:56\"}');
INSERT INTO `offlinemessage` VALUES (19, '{\"groupid\":1,\"id\":13,\"msg\":\"hahahahaha\",\"msgid\":10,\"name\":\"zhang san\",\"time\":\"2024-02-23 17:59:26\"}');
INSERT INTO `offlinemessage` VALUES (19, '{\"groupid\":1,\"id\":21,\"msg\":\"wowowowowow\",\"msgid\":10,\"name\":\"gao yang\",\"time\":\"2024-02-23 17:59:34\"}');

-- ----------------------------
-- Table structure for user
-- ----------------------------
DROP TABLE IF EXISTS `user`;
CREATE TABLE `user`  (
  `id` int(0) NOT NULL AUTO_INCREMENT,
  `name` varchar(50) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL,
  `password` varchar(50) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL,
  `state` enum('online','offline') CHARACTER SET latin1 COLLATE latin1_swedish_ci NULL DEFAULT 'offline',
  PRIMARY KEY (`id`) USING BTREE,
  UNIQUE INDEX `name`(`name`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 33 CHARACTER SET = utf8mb3 COLLATE = utf8mb3_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of user
-- ----------------------------
INSERT INTO `user` VALUES (13, 'zhang san', '123456', 'offline');
INSERT INTO `user` VALUES (15, 'li si', '666666', 'offline');
INSERT INTO `user` VALUES (16, 'liu shuo', '123456', 'offline');
INSERT INTO `user` VALUES (18, 'wu yang', '123456', 'offline');
INSERT INTO `user` VALUES (19, 'pi pi', '123456', 'offline');
INSERT INTO `user` VALUES (21, 'gao yang', '123456', 'offline');
INSERT INTO `user` VALUES (22, '123', '123', 'offline');
INSERT INTO `user` VALUES (23, '1212', '1212', 'online');
INSERT INTO `user` VALUES (24, 'xf', '123123', 'offline');
INSERT INTO `user` VALUES (25, '123123', '123123', 'offline');

SET FOREIGN_KEY_CHECKS = 1;
