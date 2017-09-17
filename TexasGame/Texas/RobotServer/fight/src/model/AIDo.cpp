//
//  AIDo.cpp
//  holdem
//
//  Created by apple on 15/5/9.
//
//

#include "AIDo.h"
#include <stdio.h>
#include <stdlib.h>

#define SIMULATE_REPEAT_TIME 1000
static AIDo *AIDo_instance = NULL;
AIDo* AIDo::getInstance()
{
    if(AIDo_instance == NULL)
    {
        AIDo_instance = new AIDo();
    }
    return AIDo_instance;
}
AIDo::~AIDo()
{
    AIDo_instance = NULL;
}

int AIDo::getAIDo(BYTE myCard[2],BYTE publicCard[5],int64_t publicPool,int64_t callNeedChips,int64_t bigblind, BYTE playnum)
{
    printf("起机器静静的肯德基雕刻机卡的角度看肯德基肯德基代扣代缴雕刻机都看得见\n");
    printf("publicpool %lld callneedchip %lld,playnum %d\n",publicPool,callNeedChips,playnum);
    printf("mycard %x %x\n",myCard[0],myCard[1]);
    printf("publicard %x %x %x %x %x\n",publicCard[0],publicCard[1],publicCard[2],publicCard[3],publicCard[4]);
    double winrate = getWinRate(myCard, publicCard, playnum);
    int64_t callNeedchipback = callNeedChips;
    if (callNeedChips<=0)
    {
        callNeedChips = bigblind;
    }
    BYTE publicCardNum=0;
    for (int i = 0; i<5; i++)
    {
        if (publicCard[i]!=0)
        {
            publicCardNum++;
        }
    }

    double potrate = getPotRate(publicPool, callNeedChips);
    printf("winrate %f,potrate %f",winrate,potrate);
    double RR = winrate/potrate;
    printf("RR %f",RR);
    int action;
    int index = rand()%100;
    printf("index %d",index);
    if (RR<0.8)
    {
        if (index<95)
        {
            action = AI_FOLD;
        }
        else
        {
            action = AI_RAISE;
        }
    }
    else if (RR<1.0)
    {
        if (index<80)
        {
            action = AI_FOLD;
        }
        else if(index>=80&&index<85)
        {
            action = AI_CALL;
        }
        else
        {
            action = AI_RAISE;
        }
    }
    else if (RR<1.3)
    {
        if (index<60)
        {
            action = AI_CALL;
        }
        else
        {
            action = AI_RAISE;
        }
    }
    else
    {
        if (index>30)
        {
            action = AI_CALL;
        }
        else
        {
            action = AI_RAISE;
        }
    }

    if (callNeedchipback == 0&&action == AI_FOLD)
    {
        action = AI_CALL;
    }
    printf("action %d\n",action);
    return action;
}

double AIDo::getWinRate(BYTE myCard[2],BYTE publicCard[5],BYTE playnum)
{
    BYTE cbUseCardCount = 2;
    BYTE cbHasUseCardData[7] = {0,0,0,0,0,0,0};
    cbHasUseCardData[0]=myCard[0];
    cbHasUseCardData[1]=myCard[1];
    int publicCardNum = 0;
    for (int i = 0; i<5; i++)
    {
        if (publicCard[i]!=0)
        {
            cbHasUseCardData[cbUseCardCount] = publicCard[i];
            cbUseCardCount++;
            publicCardNum++;
        }
        else
        {
            break;
        }
    }
    BYTE cbBufferCount = FULL_COUNT-cbUseCardCount;
    BYTE cbCardBuffer[cbBufferCount];

    BYTE cbRCount = FULL_COUNT-cbUseCardCount;
    BYTE cbRCard[cbBufferCount];

    logic.LeftCardList(cbCardBuffer,cbBufferCount,cbHasUseCardData,cbUseCardCount);
    int mywintimes = 0;
    for (int i = 0; i<SIMULATE_REPEAT_TIME; i++)
    {
        int allCardIndex = 0;
        BYTE playerCard[playnum][2];
        playerCard[0][0] = myCard[0];
        playerCard[0][1] = myCard[1];
        logic.RandLeftCardList(cbRCard,cbRCount,cbCardBuffer,cbBufferCount);
        //发完公共牌
        for (int j=0; j<(5-publicCardNum); j++)
        {
            publicCard[publicCardNum+j] = cbRCard[allCardIndex++];
//            printf("publiccard[%d]:%x",publicCardNum+j,publicCard[publicCardNum+j]);
        }
        for (int k = 1; k<playnum; k++)
        {
            playerCard[k][0] = cbRCard[allCardIndex++];
            playerCard[k][1] = cbRCard[allCardIndex++];
//            printf("playercard[%d]:%x,%x",k,playerCard[k][0],playerCard[k][1]);
        }
        BYTE cbLastCardData[playnum][5];
        for (int m = 0; m<playnum; m++)
        {
           BYTE type= logic.FiveFromSeven(&(playerCard[m][0]),2,publicCard,5,&(cbLastCardData[m][0]),5);
//            printf("fivefromsever[%d]:%x %x %x %x %x   %d",m,cbLastCardData[m][0],cbLastCardData[m][1],cbLastCardData[m][2],cbLastCardData[m][3],cbLastCardData[m][4],type);
        }

        UserWinList EndResult;
        logic.SelectMaxUser(cbLastCardData,playnum,EndResult,NULL);
        for (int n = 0; n<EndResult.bSameCount; n++)
        {
            if (EndResult.wWinerList[n] == 0)
            {
                mywintimes++;
                break;
            }
        }
    }
    printf("mywintime %d",mywintimes);
    return (double)mywintimes/SIMULATE_REPEAT_TIME;
}
double AIDo::getPotRate(int64_t publicPool,int64_t callNeedChips)
{
    return (double)callNeedChips/(publicPool+callNeedChips);
}

