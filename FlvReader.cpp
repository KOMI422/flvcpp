#include "FlvReader.h"
#include "FlvParseException.h"
#include <iostream>

FlvReader::FlvReader()
{
    m_hasParsedFileHeader = false;
}

bool FlvReader::appendAndParse(const std::string& buf, std::vector<std::shared_ptr<FlvTag>>& tagVec)
{
    m_flvBuffer.append(buf);

    if(!m_hasParsedFileHeader)
    {
        int ret = m_flvFileHeader.decode((const uint8_t*)m_flvBuffer.data(), m_flvBuffer.size());
        if(ret == -1)
        {
            throw FlvParseException("FlvFileHeader parse exception");
        }
        else if(ret == 0)
        {
            return false;
        }
        else
        {
            m_hasParsedFileHeader = true;
            m_flvBuffer = m_flvBuffer.substr(ret);
        }
    }

    while(true)
    {
        std::shared_ptr<FlvTag> pTag;
        if(!FlvTag::checkCanDecode((const uint8_t*)m_flvBuffer.data(), m_flvBuffer.size()))
        {
            break;
        }

        pTag = parseFlvTag((const uint8_t*)m_flvBuffer.data(), m_flvBuffer.size());
        tagVec.push_back(pTag);
        // std::cout << "tagTs=" << pTag->getTagTimeStamp() << std::endl;
        m_flvBuffer = m_flvBuffer.substr(pTag->getTagSize());
    }

    return tagVec.size() > 0;
}

std::shared_ptr<FlvTag> FlvReader::parseFlvTag(const uint8_t* data, uint32_t size)
{
    std::shared_ptr<FlvTag> pRetTag;

    FlvTag::FlvTagType tagType = FlvTag::parseFlvTagType(data);
    switch(tagType)
    {
        case FlvTag::VIDEO:
        pRetTag = std::shared_ptr<AVCVideoFlvTag>(new AVCVideoFlvTag());
        break;
        
        case FlvTag::AUDIO:
        pRetTag = std::shared_ptr<AudioFlvTag>(new AudioFlvTag());
        break;

        default:
        pRetTag = std::shared_ptr<GeneralFlvTag>(new GeneralFlvTag());
        break;
    }

    pRetTag->decode(data, size);
    return pRetTag;
}