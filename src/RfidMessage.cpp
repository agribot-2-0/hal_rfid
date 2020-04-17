#include <hal_rfid/RfidMessage.h>

#define FRAME_START 0x5a50
#define BYTE_MASK 0xff
#define FRAME_DATA_START 8
#define FRAME_END 0xa5


RfidMessage::RfidMessage(uint16_t u16BuffSize)
{
    _pchMessage = new char[u16BuffSize];
    bzero(_pchMessage, (sizeof(char)*u16BuffSize));
    _u16FrameIt = FRAME_DATA_START;
}

uint8_t RfidMessage::u8PrintFrame()
{
    uint8_t u8FrameSize;
    for (u8FrameSize = 0; u8FrameSize < 40; u8FrameSize++)
    {
        std::cout << std::hex << (((uint)_pchMessage[u8FrameSize]&BYTE_MASK)) << std:: endl;
        if((_pchMessage[u8FrameSize]&BYTE_MASK) == FRAME_END)
        {
            break;
        }
    }
    return u8FrameSize;
}

void RfidMessage::vSetFrameStart()
{
    _pchMessage[0] = (FRAME_START&0xff00)>>8;
    _pchMessage[1] = (FRAME_START&BYTE_MASK);
}

void RfidMessage::vSetFrameLen(uint32_t u32MsgLen)
{
    _pchMessage[2] = (u32MsgLen&0xff);
    _pchMessage[3] = (u32MsgLen&0xff00)>>8;
    _pchMessage[4] = (u32MsgLen&0xff0000)>>16;
    _pchMessage[5] = (u32MsgLen&0xff000000)>>24;
}

void RfidMessage::vSetFrameId(uint16_t u16Id)
{
    _pchMessage[6] = (u16Id&0xff);
    _pchMessage[7] = (u16Id&0xff00)>>8;
}

void RfidMessage::vSetFrameData(uint8_t* pu8Data, uint8_t u8DataLength)
{
    for(uint8_t i = 0;i < u8DataLength; i++)
    {
        _pchMessage[_u16FrameIt] = pu8Data[i];
        _u16FrameIt++;
    }
}

void RfidMessage::vSetFrameEnd()
{
    _pchMessage[_u16FrameIt] = FRAME_END;
}

char* RfidMessage::pchGetFrame()
{
    return _pchMessage;
}

uint16_t RfidMessage::u16GetMsgLen()
{
    return _u16FrameIt+1;
}

RfidMessage::~RfidMessage()
{
    delete[] _pchMessage;
}