#ifndef __XY_RSA__HPP
#define __XY_RSA__HPP

int public_encrypt(unsigned char * data, int data_len, unsigned char * key, unsigned char *encrypted);
int private_decrypt(unsigned char * enc_data, int data_len, unsigned char * key, unsigned char *decrypted);

int private_encrypt(unsigned char * data, int data_len, unsigned char * key, unsigned char *encrypted);
int public_decrypt(unsigned char * enc_data, int data_len, unsigned char * key, unsigned char *decrypted);	
#endif // !__XY_RSA__HPP

