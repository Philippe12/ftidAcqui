//
//  ftdiAcqui.c
//  ftidAcqui
//
//  Created by Philippe Fouquet on 09/09/13.
//  Copyright (c) 2013 Philippe Fouquet. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../D2XX/Samples/ftd2xx.h"

#define BUFF_SIZE 20000

static double Value[BUFF_SIZE][4];
static int CountValue;
static FT_HANDLE hCom = NULL;
static int Position;
static unsigned int dwRead;

static int TraitementBuff(unsigned char *fi, unsigned char sbuf[8])
{
	int id;
	int test = 1;
	const int size_to_read = (10*5)+2;
	unsigned char *buf = fi;
	
	while( (Position+size_to_read) < dwRead )
	{
		test = 1;
		for( id = Position; id < Position+size_to_read-10; id=id+10 )
		{
			if( (buf[id] != '/')
               || ( ((buf[id+1]+1)!=buf[id+11]) && ((buf[id+1]-255)!=buf[id+11])  ) )
			{
				test = 0;
				break;
            }
		}
		if( test == 1 )
		{
			for( id = 0; id < 8; id++ )
			{
                sbuf[id] = buf[Position+id+2];
			}
			Position = Position + 8;
			return 1;
		}
		else
		{
			Position++;
        }
	}
	return 0;
}


int ftdiConnect( void )
{
	FT_DEVICE_LIST_INFO_NODE * devInfo;
	FT_STATUS ftStatus;
	DWORD numDevs;
    
	hCom = 0;
    
	// create list
	ftStatus = FT_CreateDeviceInfoList(&numDevs);
	if( ftStatus != FT_OK ){
        return 0;
	}
	devInfo = (FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE)*numDevs);
	ftStatus = FT_GetDeviceInfoList(devInfo , &numDevs);
	if( ftStatus != FT_OK )	{
        return 0;
	}
    
	FTDCB dcb;
	FTTIMEOUTS ct;
    
	FT_W32_CloseHandle(hCom);
    ftStatus = FT_OTHER_ERROR;
	for( int id = 0; id < numDevs; id++ )
	{
		if( strcmp(devInfo[id].SerialNumber, "COMTSTBRUIT") == 0 )
		{
			ftStatus = FT_OpenEx( devInfo[id].SerialNumber, FT_OPEN_BY_SERIAL_NUMBER, &hCom);
			break;
		}
	}
	if( ftStatus != FT_OK )
	{
		return 0;
	}
    
	FT_W32_GetCommState(hCom, &dcb);
	dcb.BaudRate = 4000000;
	dcb.fParity = FT_PARITY_NONE;
	dcb.fOutxCtsFlow = FT_FLOW_NONE;
	dcb.fOutxDsrFlow = FT_FLOW_NONE;
//	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fDsrSensitivity = FALSE;
	dcb.fOutX = FALSE;
	dcb.fInX = FALSE;
//	dcb.fRtsControl = RTS_CONTROL_DISABLE;
	dcb.fAbortOnError = FALSE;
	dcb.ByteSize = FT_BITS_8;
	dcb.Parity = FT_PARITY_NONE;
	dcb.StopBits = FT_STOP_BITS_1;
	FT_W32_SetCommState(hCom, &dcb);
    
	FT_W32_GetCommTimeouts(hCom, &ct);
	ct.ReadIntervalTimeout = 5000;
    ct.ReadTotalTimeoutMultiplier =5000;
    ct.ReadTotalTimeoutConstant = 5000;
	FT_W32_SetCommTimeouts(hCom, &ct);
    
	FT_W32_SetCommMask(hCom, EV_RXCHAR);

    return 1;
}

int ftdiAcqui( void )
{
    unsigned char Buffer[BUFF_SIZE*10];
	unsigned int dwToRead = BUFF_SIZE*10;
	unsigned char sbuf[8];
    int id;
    
    memset(Buffer, 0, dwToRead);
    FT_Read(hCom, Buffer, dwToRead, &dwRead);
		
    Position = 0;
    CountValue = 0;
		  
    while( TraitementBuff(Buffer, sbuf ) == 1 )
    {
        for( id = 0; id < 4; id++ )
        {
            Value[CountValue][id] = (sbuf[id*2] + (sbuf[(id*2)+1]<<8))*(4096.0/65536.0);
        }
        CountValue++;
    }
 
   return 1;
}

double ftdiGetValue( int id, int voie )
{
    if( (id < 0 ) || (id > CountValue) || (voie < 0 ) || (voie > 4))
        return -1;
    
    return Value[id][voie];
}

void ftdiClose( void )
{
    FT_Close(hCom);
}
