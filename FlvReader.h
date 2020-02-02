#ifndef _FLV_READER_H_
#define _FLV_READER_H_

#include "FlvFileHeader.h"
#include "FlvTag.h"
#include <vector>
#include <memory>

class FlvReader
{
public:
    FlvReader();
    bool appendAndParse(const std::string& buf, std::vector<std::shared_ptr<FlvTag>>& tagVec);

private:
    std::shared_ptr<FlvTag> parseFlvTag(const uint8_t* data, uint32_t size);
private:
    bool m_hasParsedFileHeader;
    FlvFileHeader m_flvFileHeader;
    std::string m_flvBuffer;
};

#endif