#ifndef  _CDL_Decoder_H_
#define _CDL_Decoder_H_
 
 class CDL_Decoder
 {
	 public:
		 CDL_Decoder(){};
		virtual ~CDL_Decoder(){};
		virtual int ParsePacket(char * data, int len)=0;
 };

 
#endif


