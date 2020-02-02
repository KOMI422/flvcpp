#ifndef _FLV_FILE_HEADER_H
#define _FLV_FILE_HEADER_H

#include <stdint.h>
#include <string>

class FlvFileHeader
{
public:
    static const uint32_t FLV_FILE_HEADER_SIZE;
    static bool checkCanDecode(uint32_t size, bool includePrevTag = true);
public:
    FlvFileHeader() : m_hasVideo(true), m_hasAudio(true) {}
    void encode(std::string& encodedData, bool includePrevTag = true);
    int decode(const uint8_t* data, uint32_t size, bool includePrevTag = true);
private:
    bool m_hasVideo;
    bool m_hasAudio;
};

#endif
