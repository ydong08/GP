#ifndef _MAHJONG_DEF_H_
#define _MAHJONG_DEF_H_

#include <stdint.h>
#include <assert.h>
#include <string.h>
#include "vector"
#include "math.h"
#include "stdlib.h"

#include "Typedef.h"
#define ZeroMemory(Destination,Length) memset((Destination),0,(Length))
#define ASSERT(f)  assert(f)
#define CopyMemory(Destination,Source,Length) memcpy((Destination),(Source),(Length))
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))

//////////////////////////////////////////////////////////////////////////
//所有麻将牌公共的常量
#define FULL_COUNT				144				// 一副麻将牌所有牌的数量
#define MAX_INDEX				42				// 最大可能的牌的序号
#define MAX_COLOR				6				// 牌的种类
#define INVALID_CHAIR			0xFF			// 无效的位置信息

//////////////////////////////////////////////////////////////////////////
//麻将牌值相关的掩码
#define	MASK_COLOR				0xF0			// 花色掩码
#define	MASK_VALUE				0x0F			// 数值掩码

//本玩法使用的牌的定义
#define USE_CARD_CHARACTER		1				// 万字牌(一万~九万)
#define USE_CARD_BANBOO			1				// 条子牌(一条~九条)
#define USE_CARD_DOT			1				// 筒子牌(一筒~九筒)
#define USE_CARD_WIND			0				// 风牌(东、南、西、北)
#define USE_CARD_DRAGON			0				// 箭牌(中、发、白)
#define USE_CARD_FLOWER			0				// 花牌（春夏秋冬，梅兰竹菊）

#define GAME_PLAYER				4				// 游戏人数
#define MAX_COUNT				14				// 手牌的最大数目
#define MAX_WEAVE				(MAX_COUNT/3)	// 最大可能的组合数
#define MAX_RIGHT_COUNT			1				// 最大权位个数(目前最大支持到16，一共表达的胡牌类型为16*28中)

//////////////////////////////////////////////////////////////////////////
//公用的麻将动作定义
#define WIK_NULL					0x00					// 没有类型
#define WIK_LEFT					0x01					// 左吃类型
#define WIK_CENTER					0x02					// 中吃类型
#define WIK_RIGHT					0x04					// 右吃类型
#define WIK_PENG					0x08					// 碰牌类型
#define WIK_GANG					0x10					// 杠牌类型
#define WIK_LISTEN					0x20					// 听牌类型
#define WIK_CHI_HU					0x40					// 吃胡类型

extern const uint8_t g_repertoryCard[];		// 启用的所有的牌 
extern const uint8_t g_maxRepertory;		// 启用的所有牌的数量
extern const uint8_t g_nMaxWave;			// 最大组合数
extern const uint8_t g_nMaxIndex;			// 牌索引的最大值
extern const uint8_t g_nHeapFullCount;

#endif