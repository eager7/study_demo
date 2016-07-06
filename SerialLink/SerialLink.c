#include <stdio.h>

#define SL_START_CHAR   0x01
#define SL_ESC_CHAR     0x02
#define SL_END_CHAR     0x03

#define RX_MAX_LENGTH 256

typedef unsigned char uint8;    /*!< unsigned byte (8-bits) */
typedef unsigned short uint16;  /*!< unsigned word (16-bits) */
typedef unsigned int uint32;    /*!< unsigned long (32-bits) */
typedef enum{
	TRUE = 1,
	FALSE = 0,
}bool;

typedef enum
{
    E_STATE_RX_WAIT_START,
    E_STATE_RX_WAIT_TYPEMSB,
    E_STATE_RX_WAIT_TYPELSB,
    E_STATE_RX_WAIT_LENMSB,
    E_STATE_RX_WAIT_LENLSB,
    E_STATE_RX_WAIT_CRC,
    E_STATE_RX_WAIT_DATA,
} teSL_RxState;

uint8 u8SL_CalculateCRC(uint16 u16Type, uint16 u16Length, uint8 *pu8Data)
{

    uint16 n;
    uint8 u8CRC;

    u8CRC  = (u16Type   >> 0) & 0xff;
    u8CRC ^= (u16Type   >> 8) & 0xff;
    u8CRC ^= (u16Length >> 0) & 0xff;
    u8CRC ^= (u16Length >> 8) & 0xff;

    for(n = 0; n < u16Length; n++)
    {
        u8CRC ^= pu8Data[n];
    }
	printf("CRC:%d\n", u8CRC);
    return(u8CRC);
}


static bool eSL_ReadMessage(uint16 *pu16Type, uint16 *pu16Length, uint16 u16MsgLen, uint8 *pu8Message)
{

    static teSL_RxState eRxState = E_STATE_RX_WAIT_START;
    uint8 i = 0;
    uint8 u8Data = 0;
    static uint8 u8CRC = 0;
    static uint16 u16Bytes = 0;
    static bool bInEsc = FALSE;

    while(u16MsgLen > i)
    {
        u8Data = pu8Message[i++];
		//printf("0x%02x\n", u8Data);

        switch(u8Data)
        {
        case SL_START_CHAR:
			printf("SL_START_CHAR\n");
            u16Bytes = 0;
            bInEsc = FALSE;
            eRxState = E_STATE_RX_WAIT_TYPEMSB;
            break;

        case SL_ESC_CHAR:
			printf("SL_ESC_CHAR\n");
            bInEsc = TRUE;
            break;

        case SL_END_CHAR:
            printf("SL_END_CHAR\n");
			if(*pu16Length > RX_MAX_LENGTH)
            {
                /* Sanity check length before attempting to CRC the message */
                eRxState = E_STATE_RX_WAIT_START;
                break;
            }
            if(u8CRC == u8SL_CalculateCRC(*pu16Type, *pu16Length, pu8Message))
            {
                eRxState = E_STATE_RX_WAIT_START;
                return TRUE;
            }
            break;
        default:
			//printf("default\n");
            if(bInEsc)
            {
                u8Data ^= 0x10;
				//printf("^=:%d\n", u8Data);
                bInEsc = FALSE;
            }
            switch(eRxState)
            {

                case E_STATE_RX_WAIT_START:
                    break;
                    

                case E_STATE_RX_WAIT_TYPEMSB:
                    *pu16Type = (uint16)u8Data << 8;
                    eRxState++;
                    break;

                case E_STATE_RX_WAIT_TYPELSB:
                    *pu16Type += (uint16)u8Data;
                    eRxState++;
                    break;

                case E_STATE_RX_WAIT_LENMSB:
                    *pu16Length = (uint16)u8Data << 8;
                    eRxState++;
                    break;

                case E_STATE_RX_WAIT_LENLSB:
                    *pu16Length += (uint16)u8Data;
                    if(*pu16Length > RX_MAX_LENGTH)
                    {
                        eRxState = E_STATE_RX_WAIT_START;
                    }
                    else
                    {
                        eRxState++;
                    }
                    break;

                case E_STATE_RX_WAIT_CRC:
                    u8CRC = u8Data;
					printf("u8CRC:%d", u8CRC);
                    eRxState++;
                    break;

                case E_STATE_RX_WAIT_DATA:
                    if(u16Bytes < *pu16Length)
                    {
                        pu8Message[u16Bytes++] = u8Data;
                    }
                    break;

                default:
                    eRxState = E_STATE_RX_WAIT_START;
            }
            break;
        }
    }
    return FALSE;
}

int main()
{
	printf("0x%x\n", '>');
    printf("test SerialLink\n");
	
	uint8 Msg[] = {0x01, 0x02, 0x10, 0x49, 0x02, 0x10, 0x02, 0x14, 0x2a, 0xff, 0xfc, 0x64, 0x02, 0x10, 0x03};
	
	uint16 u16Type = 0, u16Length = 0;
	bool Ret = eSL_ReadMessage(&u16Type, &u16Length, 15, Msg);
	printf("Ret:%d, u16Type:0x%02x, u16Length:0x%02x\n", Ret, u16Type, u16Length);
    return 0;

}
