#include "FlvFileHeader.h"

const uint32_t FlvFileHeader::FLV_FILE_HEADER_SIZE = 9;

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