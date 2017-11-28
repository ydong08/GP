
#pragma once


namespace base64
{
	// must free out manual!!!
	int base64Decode(const unsigned char *in, unsigned int inLength, unsigned char **out);
	// must free out manual!!!
	int base64Encode(const unsigned char *in, unsigned int inLength, char **out);
}