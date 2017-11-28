#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "url_code.h"

/* {{{ php_htoi 
 */  
static int php_htoi(char *s)  
{  
    int value;  
    int c;  
  
    c = ((unsigned char *)s)[0];  
    if (isupper(c))  
        c = tolower(c);  
    value = (c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10) * 16;  
  
    c = ((unsigned char *)s)[1];  
    if (isupper(c))  
        c = tolower(c);  
    value += c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10;  
  
    return (value);  
}  

#ifndef _strdup
char * _strdup(const char *szSrc)
{
	if (NULL == szSrc) return NULL;

	size_t  len = strlen(szSrc) +1;
	char *newStr = (char *)malloc(len);
	if (newStr == NULL)
		return NULL;
	return (char *)memcpy(newStr,szSrc,len);
}
#endif

string urldecode(string &str_source)  
{  
	char const *in_str = str_source.c_str();  
	int in_str_len = strlen(in_str);  
	int out_str_len = 0;  
	string out_str;  
	char *str = _strdup(in_str); 
	if (NULL == str) return "";

	char *dest = str;  
	char *data = str;  

	while (in_str_len--) {  
		if (*data == '+') {  
			*dest = ' ';  
		}  
		else if (*data == '%' && in_str_len >= 2 && isxdigit((int) *(data + 1))   
			&& isxdigit((int) *(data + 2))) {  
				*dest = (char) php_htoi(data + 1);  
				data += 2;  
				in_str_len -= 2;  
		} else {  
			*dest = *data;  
		}  
		data++;  
		dest++;  
	}  
	*dest = '\0';  
	out_str_len =  dest - str;  
	out_str = str;  
	if(str) free(str);  
	return out_str;  
}

string urlencode(string &str_source)  
{  
	char const *in_str = str_source.c_str();  
	int in_str_len = strlen(in_str);  
	int out_str_len = 0;  
	string out_str;  
	register unsigned char c;  
	unsigned char *to, *start;  
	unsigned char const *from, *end;  
	unsigned char hexchars[] = "0123456789ABCDEF";  

	from = (unsigned char *)in_str;  
	end = (unsigned char *)in_str + in_str_len;  
	start = to = (unsigned char *) malloc(3*in_str_len+1);  

	while (from < end) {  
		c = *from++;  

		if (c == ' ') {  
			*to++ = '+';  
		} else if ((c < '0' && c != '-' && c != '.') ||  
			(c < 'A' && c > '9') ||  
			(c > 'Z' && c < 'a' && c != '_') ||  
			(c > 'z')) {   
				to[0] = '%';  
				to[1] = hexchars[c >> 4];  
				to[2] = hexchars[c & 15];  
				to += 3;  
		} else {  
			*to++ = c;  
		}  
	}  
	*to = 0;  

	out_str_len = to - start;  
	out_str = (char *) start;  
	free(start);  
	return out_str;  
}

string urlencode(const char* str_source, int nLen)
{  
	char const *in_str = str_source;  
	int in_str_len = nLen;  
	int out_str_len = 0;  
	string out_str;  
	register unsigned char c;  
	unsigned char *to, *start;  
	unsigned char const *from, *end;  
	unsigned char hexchars[] = "0123456789ABCDEF";  

	from = (unsigned char *)in_str;  
	end = (unsigned char *)in_str + in_str_len;  
	start = to = (unsigned char *) malloc(3*in_str_len+1);  

	while (from < end) {  
		c = *from++;  

		if (c == ' ') {  
			*to++ = '+';  
		} else if ((c < '0' && c != '-' && c != '.') ||  
			(c < 'A' && c > '9') ||  
			(c > 'Z' && c < 'a' && c != '_') ||  
			(c > 'z')) {   
				to[0] = '%';  
				to[1] = hexchars[c >> 4];  
				to[2] = hexchars[c & 15];  
				to += 3;  
		} else {  
			*to++ = c;  
		}  
	}  
	*to = 0;  

	out_str_len = to - start;  
	out_str = (char *) start;  
	free(start);  
	return out_str;  
} 
