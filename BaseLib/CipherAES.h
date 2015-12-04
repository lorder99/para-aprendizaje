#pragma once
#include "CipherBase.h"

/*
*  FIPS-197 compliant AES implementation
*
*  Copyright (C) 2001-2004  Christophe Devine
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

typedef struct {
	uint32		erk[64];	/* encryption round keys */
	uint32		drk[64];    /* decryption round keys */
	int			nr;			 /* number of rounds */
}aes_context;

class CipherAES
	:public CipherBase
{
public:
	CipherAES(void);
	~CipherAES(void);

	virtual	int SetKey(unsigned char* cipherKey, int cipherKeyLength);
	virtual int Encrypt(unsigned char *originalData, int originalDataLength, unsigned char* encData, int encDataSize);
	virtual int Decrypt(unsigned char *originalData, int originalDataLength, unsigned char* decData, int decDataSize);
	virtual void ReleaseCipher(void){ return; };

private:
	aes_context		mAESContext;

	void EncryptByBlock(unsigned char *originalData, unsigned char* outputData);
	void DecryptByBlock(unsigned char *originalData, unsigned char* outputData);
};
