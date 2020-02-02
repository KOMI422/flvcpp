#include "FlvFileHeader.h"

const uint32_t FlvFileHeader::FLV_FILE_HEADER_SIZE = 9;

bool FlvFileHeader::checkCanDecode(uint32_t size, bool includePrevTag)
{
    uint32_t needSize = FLV_FILE_HEADER_SIZE + (includePrevTag ? 4 : 0);
    return (size >= needSize);
}

void FlvFileHeader::encode(std::string& encodedData, bool includePrevTag)
{
    char buf[9] = { 0 };

    buf[0] = 'F';
    buf[1] = 'L';
    buf[2] = 'V';
    buf[3] = 1;
    buf[4] = (m_hasAudio ? 0x4 : 0x0) | (m_hasVideo ? 0x1 : 0x0);
    buf[5] = 0;
    buf[6] = 0;
    buf[7] = 0;
    buf[8] = 9;

    encodedData.append(buf, sizeof(buf));

    if(includePrevTag)
    {
        char prevTagBuf[4] = { 0 };
        encodedData.append(prevTagBuf, sizeof(prevTagBuf));
    }
}

int FlvFileHeader::decode(const uint8_t* data, uint32_t size, bool includePrevTag)
{
    if(!FlvFileHeader::checkCanDecode(size, includePrevTag))
    {
        return 0;
    }

    if(data[0] != 'F' || data[1] != 'L' || data[2] != 'V')
    {
        return -1;
    }

    m_hasAudio = (data[4] & 0x04) > 0;
    m_hasVideo = (data[4] & 0x01) > 0;

    return FlvFileHeader::FLV_FILE_HEADER_SIZE + (includePrevTag ? 4 : 0);
}