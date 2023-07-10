#pragma once
/*
    二进制协议通用解析器,解析包含[头部][数据长度][校验和][变长数据]的二进制协议
*/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <vector>

typedef enum ProtoSegType
{
    SEG_HEADER,      //帧头
    SEG_VERSION,     //协议版本
    SEG_VARDATE_LEN, //数据长度
    SEG_VARDATA,     //数据区
    SEG_CHECKSUM,    //校验和
    SEG_CMD,         //命令类型
    SEG_CUSTOM,      //定制化
} ProtoSegType;

typedef enum ProtoEndian
{
    LITTLE_ENDIAN_, // 11 22 33 44 means 0x44332211
    BIG_ENDIAN_     // 11 22 33 44 means 0x11223344
} ProtoEndian;

typedef struct ProtoSeg
{
    int type;
    int pos;
    int len;
} ProtoSeg;

enum
{
    PH_CHECK_HEADER,
    PH_CHECK_SEG
};

typedef struct ProtoHead
{
    uint8_t header[8]; // 最长支持头字节为 8
    int len;
} ProtoHead;

struct ProtoHex
{
    ProtoHead header;    // 协议头
    int byteOrder;       // 字节序
    ProtoSeg *segs;      // 数据段定义
    int segCount;        // 数据段个数
    std::vector<uint8_t> recvBuf; // 接收缓冲区
    int protoLen;        // 缓冲区长度
    int varDataByteSize; // 变长数据单个数据字节数

    std::vector<uint8_t> sendBuf;

    int currIndex;     // 当前接收数据个数
    int state;         // 状态
    int varDataLenPos; // 变长数据长度数据段位置
    int varDataLenLen;
    int varDataPos;
    int varDataLen;
    int fixedDataLen; // 定长数据长度
    int totalLen;     // 当前协议总长度

    int totalSendLen;

private:
    ProtoSeg *GetSeg(ProtoSegType type)
    {
        ProtoSeg *seg = NULL;
        for (int i = 0; i < this->segCount; i++)
        {
            if (this->segs[i].type == type)
            {
                seg = &this->segs[i];
                return seg;
            }
        }
        return NULL;
    }

public:
    ProtoHex()
    {
        byteOrder = LITTLE_ENDIAN_;
        segCount = 0;
        protoLen = 0;
        varDataByteSize = 1;
        currIndex = 0;
        state = 0;
        fixedDataLen = 0;
    }

    void Init(ProtoHead header, ProtoEndian byteOrder, ProtoSeg *segs, int segCount, int protoLen, int varItemSize = 1)
    {
        varDataPos = 0;
        varDataLen = 0;
        this->header = header;
        this->segs = segs;
        this->byteOrder = byteOrder;
        this->protoLen = protoLen;
        this->segCount = segCount;
        this->varDataByteSize = varItemSize;

        for (int i = 0; i < segCount; i++)
        {
            if (segs[i].type == SEG_VARDATE_LEN)
            {
                varDataLenPos = segs[i].pos;
                varDataLenLen = segs[i].len;
            }
            if (segs[i].type == SEG_VARDATA)
            {
                varDataPos = segs[i].pos;
            }
            fixedDataLen += segs[i].len;
        }
        fixedDataLen += header.len;
        recvBuf.resize(protoLen);
        sendBuf.resize(protoLen);
    }
    /*
        获取某个数据段的值

        @type       数据段类型
        @ret        根据大小端返回对应的值,只支持小于等于4字节的数据
    */
    int GetSegValue(ProtoSegType type)
    {
        ProtoSeg *seg = GetSeg(type);
        if (seg == NULL)
        {
            return 0;
        }

        int pos = seg->pos;
        if (seg->pos >= this->varDataPos && seg->type != SEG_VARDATA)
        {
            pos = this->varDataLenPos + this->varDataLen + (seg->pos - this->varDataPos);
        }

        int value = 0;
        if (this->byteOrder == LITTLE_ENDIAN_)
        {
            switch (seg->len)
            {
            case 1:
                value = recvBuf[pos];
                break;
            case 2:
                value = recvBuf[pos] | recvBuf[pos + 1] << 8;
                break;
            case 3:
                value = recvBuf[pos] | recvBuf[pos + 1] << 8 | recvBuf[pos + 2] << 16;
                break;
            case 4:
                value = recvBuf[pos] | recvBuf[pos + 1] << 8 | recvBuf[pos + 2] << 16 | recvBuf[pos + 3] << 24;
                break;
            default:
                break;
            }
        }
        if (this->byteOrder == BIG_ENDIAN_)
        {
            switch (seg->len)
            {
            case 1:
                value = recvBuf[pos];
                break;
            case 2:
                value = recvBuf[pos] << 8 | recvBuf[pos + 1];
                break;
            case 3:
                value = recvBuf[pos] << 16 | recvBuf[pos + 1] << 8 | recvBuf[pos + 2];
                break;
            case 4:
                value = recvBuf[pos] << 24 | recvBuf[pos + 1] << 16 | recvBuf[pos + 2] << 8 | recvBuf[pos + 3];
                break;
            default:
                break;
            }
        }
        return value;
    }
    /*
        获取变长数据段指针
    */
    const uint8_t *GetVarData()
    {
        return (const uint8_t *)&recvBuf[this->varDataPos];
    }
    /*
    获取变长数据段长度
    */
    int GetVarLen()
    {
        return varDataLen;
    }
    /*
        二进制协议解析器解析函数

        @d          输入数据缓冲区指针
        @len        输入数据的长度

        @ret        解析完成返回 true, 还未解析出完整数据返回 false
                    本函数不校验协议校验和
    */
    bool Parse(uint8_t d)
    {

        switch (state)
        {
        case PH_CHECK_HEADER:
            if (header.header[currIndex] == d)
            {
                currIndex++;
            }
            else
            {
                currIndex = 0;
            }

            if (currIndex == header.len)
            {
                this->state = PH_CHECK_SEG;
                this->totalLen = fixedDataLen;
            }
            break;
        case PH_CHECK_SEG:
            recvBuf[this->currIndex++] = d;
            if (this->currIndex == this->varDataLenPos + this->varDataLenLen)
            {
                // 已经获取到变长数据长度信息,计算出该条数据总长度
                // 该条数据总长度=变长数据长度+定长数据长度
                varDataLen = GetSegValue(SEG_VARDATE_LEN);
                totalLen = varDataLen * varDataByteSize + fixedDataLen;
                if (totalLen > protoLen)
                {
                    // 如果本条协议长度大于协议最大长度,说明解析出错
                    currIndex = 0;
                    state = PH_CHECK_HEADER;
                    break;
                }
            }
            if (totalLen == currIndex)
            {
                // 获取到完整数据
                currIndex = 0;
                state = PH_CHECK_HEADER;
                return true;
            }
            break;
        default:
            break;
        }

        return false;
    }
    void PackSegValue(ProtoSegType type, int value)
    {
        ProtoSeg *seg = GetSeg(type);
        if (seg == NULL)
        {
            return;
        }

        int pos = seg->pos;
        if (seg->pos >= this->varDataPos && seg->type != SEG_VARDATA)
        {
            pos += this->varDataLen + (seg->pos - this->varDataPos);
        }

        if (this->byteOrder == LITTLE_ENDIAN_)
        {
            switch (seg->len)
            {
            case 1:
                this->sendBuf[pos] = value;
                break;
            case 2:
                this->sendBuf[pos] = value;
                this->sendBuf[pos + 1] = value >> 8;
                break;
            case 3:
                this->sendBuf[pos] = value;
                this->sendBuf[pos + 1] = value >> 8;
                this->sendBuf[pos + 2] = value >> 16;
                break;
            case 4:
                this->sendBuf[pos] = value;
                this->sendBuf[pos + 1] = value >> 8;
                this->sendBuf[pos + 2] = value >> 16;
                this->sendBuf[pos + 3] = value >> 24;
                break;
            default:
                break;
            }
        }
        if (this->byteOrder == BIG_ENDIAN_)
        {
            switch (seg->len)
            {
            case 1:
                this->sendBuf[pos] = value;
                break;
            case 2:
                this->sendBuf[pos + 1] = value;
                this->sendBuf[pos + 0] = value >> 8;
                break;
            case 3:
                this->sendBuf[pos + 2] = value;
                this->sendBuf[pos + 1] = value >> 8;
                this->sendBuf[pos + 0] = value >> 16;
                break;
            case 4:
                this->sendBuf[pos + 3] = value;
                this->sendBuf[pos + 2] = value >> 8;
                this->sendBuf[pos + 1] = value >> 16;
                this->sendBuf[pos + 0] = value >> 24;
                break;
            default:
                break;
            }
        }
    }

    void PackVarData(uint8_t *d, int len)
    {
        if (len > protoLen - varDataPos)
        {
            len = protoLen - varDataPos;
        }
        memcpy(&sendBuf[varDataPos], d, len);
        totalSendLen = fixedDataLen + len;
        varDataLen = len;

        PackSegValue(SEG_VARDATE_LEN, len);
    }
};

#ifndef DISABLE_TEST

#include <gtest/gtest.h>
TEST(ProtoHexTest, ParseTest)
{
    uint8_t protoTestData[] = {
    0xAA, 0x55, 0xA5, 0x5A, 0x34, 0xFD, 0xC8, 0x05, 0x00, 0x00, 0x01, 0x00};

    ProtoSeg    seg[] = {
    {SEG_CHECKSUM, 4, 2},
    {SEG_CMD, 6, 2},
    {SEG_VARDATE_LEN, 8, 2},
    {SEG_VERSION, 10, 2},
    {SEG_VARDATA, 12, 0},       // 变长数据的长度需要设置为 0
    };

    ProtoHex proto;
    proto.Init({{0xAA, 0x55, 0xA5, 0x5A}, 4}, BIG_ENDIAN_, seg, sizeof(seg)/sizeof(seg[0]), 32);
    for(int i=0; i<sizeof(protoTestData); i++)
    {
        if(i == sizeof(protoTestData) - 1)
        {
            EXPECT_EQ(true, proto.Parse(protoTestData[i]));
        }
        else
        {
            proto.Parse(protoTestData[i]);
        }
    }
    EXPECT_EQ(0x34FD, proto.GetSegValue(SEG_CHECKSUM));
    EXPECT_EQ(0xC805, proto.GetSegValue(SEG_CMD));
}

#endif

