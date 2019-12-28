#include "FlvTag.h"

const uint32_t FlvTag::FLV_TAG_HEADER_SIZE = 11;

bool FlvTag::checkCanDecode(const uint8_t* data, uint32_t size, bool includePrevTag)
{
    if(size < FLV_TAG_HEADER_SIZE)
    {
        return false;
    }
    
    uint32_t tagSize = 0;
    tagSize |= (data[1] << 16) & 0x00FF0000;
    tagSize |= (data[2] << 8) & 0x0000FF00;
    tagSize |= (data[3]) & 0x000000FF;

    tagSize += FLV_TAG_HEADER_SIZE;

    if(includePrevTag)
        tagSize += 4;

    return (size >= tagSize);
}

FlvTag::FlvTagType FlvTag::parseFlvTagType(const uint8_t* data)
{
    return (FlvTag::FlvTagType)(data[0]);
}

void FlvTag::encode(std::string& encodedData, bool includePrevTag) const
{
    uint8_t buf[11] = { 0 };
    
    buf[0] = (uint8_t)m_tagType;

    buf[1] = (m_dataSize & 0xFF0000) >> 16;
    buf[2] = (m_dataSize & 0x00FF00) >> 8;
    buf[3] = (m_dataSize & 0x0000FF);

    buf[4] = (m_timeStamp & 0x00FF0000) >> 16;
    buf[5] = (m_timeStamp & 0x0000FF00) >> 8;
    buf[6] = (m_timeStamp & 0x000000FF);
    buf[7] = (m_timeStamp & 0xFF000000) >> 24;

    //StreamID，always 0
    buf[8] = 0;
    buf[9] = 0;
    buf[10] = 0;

    encodedData.append((char*)buf, sizeof(buf));

    encodeTagData(encodedData);

    if(includePrevTag)
    {
        uint8_t prevTagBuf[4] = { 0 };
        uint32_t prevTagSize = FLV_TAG_HEADER_SIZE + m_dataSize;

        prevTagBuf[0] = (prevTagSize & 0xFF000000) >> 24;
        prevTagBuf[1] = (prevTagSize & 0x00FF0000) >> 16;
        prevTagBuf[2] = (prevTagSize & 0x0000FF00) >> 8;
        prevTagBuf[3] = (prevTagSize & 0x000000FF);

        encodedData.append((char*)prevTagBuf, sizeof(prevTagBuf));
    }
}

int FlvTag::decode(const uint8_t* data, uint32_t size, bool includePrevTag = true)
{
    if(!checkCanDecode(data, size, includePrevTag))
        return -1;

    m_tagType = (FlvTagType) data[0];

    m_dataSize = 0;
    m_dataSize |= (data[1] << 16) & 0x00FF0000;
    m_dataSize |= (data[2] << 8) & 0x0000FF00;
    m_dataSize |= (data[3]) & 0x000000FF;

    m_timeStamp = 0;
    m_timeStamp |= (data[4] << 16) & 0x00FF0000;
    m_timeStamp |= (data[5] << 8) & 0x0000FF00;
    m_timeStamp |= (data[6]) & 0x000000FF;
    m_timeStamp |= (data[7] << 24) & 0xFF000000;

    decodeTagData(data + FLV_TAG_HEADER_SIZE, size - FLV_TAG_HEADER_SIZE);

    uint32_t parsedSize = FLV_TAG_HEADER_SIZE + m_dataSize;
    if(includePrevTag)
        parsedSize += 4;

    return parsedSize;
}

AVCVideoFlvTag::AVCVideoFlvTag() : m_avcPktType(AVC_NALU), m_cts(0)
{
}

void AVCVideoFlvTag::encodeTagData(std::string& encodedData)
{
    uint8_t buf[5] = { 0 };

    buf[0] = m_isIFrame ? 0x17 : 0x27;
    buf[1] = (uint8_t)m_avcPktType;

    buf[2] = (m_cts & 0xFF0000) >> 16;
    buf[3] = (m_cts & 0x00FF00) >> 8;
    buf[4] = (m_cts & 0x0000FF);

    encodedData.append((char*)buf, sizeof(buf));
    encodedData.append(m_rawData);
}

void AVCVideoFlvTag::decodeTagData(const uint8_t* data, uint32_t dataSize)
{
    m_isIFrame = ((data[0] & 0xF0) == 0x10);
    
    m_avcPktType = (AVCPacketType) data[1];

    m_cts = 0;
    m_cts |= (data[2] & 0x00FF0000) << 16;
    m_cts |= (data[3] & 0x0000FF00) << 8;
    m_cts |= (data[4] & 0x000000FF);

    m_rawData.clear();
    m_rawData.assign((char*)(data + 5), dataSize - 5);
}

AudioFlvTag::AudioFlvTag()
    : m_audioFmt(AAC), m_sampleRate(_440000HZ), m_sampleBit(SND_16BIT), m_channel(STEREO)
{
}

void AudioFlvTag::encodeTagData(std::string& encodedData)
{
    uint8_t audioHeader = 0;

    audioHeader |= (((uint8_t)m_audioFmt) << 4) & 0xF0;
    audioHeader |= ((uint8_t)m_sampleRate) & 0x0C;
    audioHeader |= ((uint8_t)m_sampleBit) & 0x02;
    audioHeader |= ((uint8_t)m_channel) & 0x01;

    encodedData.append((char*)&audioHeader, sizeof(uint8_t));
    encodedData.append(m_rawData);
}

void AudioFlvTag::decodeTagData(const uint8_t* data, uint32_t dataSize)
{
    m_audioFmt = (AudioFormat)((data[0] & 0xF0) >> 4);
    m_sampleRate = (SampleRate)(data[0] & 0x0C);
    m_sampleBit = (SampleBit)(data[0] & 0x02);
    m_channel = (SoundChannel)(data[0] & 0x01);

    m_rawData.clear();
    m_rawData.assign((char*)(data + 1), dataSize - 1);
}
