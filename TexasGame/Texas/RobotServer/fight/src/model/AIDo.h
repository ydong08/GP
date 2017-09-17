//
//  AIDo.h
//  holdem
//
//  Created by apple on 15/5/9.
//
//
#include <string>
#include <assert.h>
#include <stdint.h>
#ifndef __holdem__AIDo__
#define __holdem__AIDo__
#include "Typedef.h"
#include "GameLogic.h"

#define AI_FOLD 1
#define AI_CALL 2
#define AI_RAISE 3

class AIDo
{
public:
    //publicCard[5] 没有填0
    //callNeedChips 没有填0
     static AIDo* getInstance();
    int getAIDo(BYTE myCard[2],BYTE publicCard[5],int64_t publicPool,int64_t callNeedChips,int64_t bigblind, BYTE playnum);
private:
private:
    AIDo(){};
    virtual ~AIDo();
    double getWinRate(BYTE myCard[2],BYTE publicCard[5],BYTE playnum);
    double getPotRate(int64_t publicPool,int64_t callNeedChips);
    GameLogic logic;
};
#endif /* defined(__holdem__AIDo__) */
