#include "XyCrypto.h"


#include <stdio.h>
#include <string.h>
//#include "base64.h"

typedef unsigned long ULONG;

void xy_init(unsigned char *s, unsigned char *key, unsigned long Len)
{
	int i =0, j = 0;
	char k[256] = {0};
	unsigned char tmp = 0;
	for (i=0;i<256;i++) {
		s[i] = i;
		k[i] = key[i%Len];
	}
	for (i=0; i<256; i++) {
		j=(j+s[i]+k[i])%256;
		tmp = s[i];
		s[i] = s[j];
		s[j] = tmp;
	}
}

void xy_crypt(unsigned char *s, unsigned char *Data, unsigned long Len)
{
	int i = 0, j = 0, t = 0;
	unsigned long k = 0;
	unsigned char tmp;
	for(k=0;k<Len;k++) {
		i=(i+1)%256;
		j=(j+s[i])%256;
		tmp = s[i];
		s[i] = s[j];
		s[j] = tmp;
		t=(s[i]+s[j])%256;
		Data[k] ^= s[t];
	}
} 

int XyCrypt(unsigned char *pData, unsigned long nLen)
{
	if (NULL == pData || 0 == nLen) return -1;

	unsigned char s[256] = {0};
	char key[256] = {"N!fRm1@v*^MP9AcrPBji^1lPsY#1rp!LeFew#aF1RcC$!KXFKeJt2!RU@0%Ic0Eq"};

	xy_init(s,(unsigned char *)key, strlen(key));
	xy_crypt(s,(unsigned char *)pData,nLen);
	return nLen;
}
