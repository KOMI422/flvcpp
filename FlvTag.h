#ifndef _FLV_TAG_H_
#define _FLV_TAG_H_

#include <stdint.h>
#include <string>
#include <map>

class FlvTag
{
public:
    enum FlvTagType
    {
        AUDIO = 8,
        VIDEO = 9,
        SCRIPT_DATA = 18
    };
public:
    static const uint32_t FLV_TAG_HEADER_SIZE;
    static bool checkCanDecode(const uint8_t* data, uint32_t size, bool includePrevTag = true);
    static FlvTagType parseFlvTagType(const uint8_t* data);
public:
    FlvTag() : m_tagType(VIDEO), m_dataSize(0), m_timeStamp(0) {}
    virtual ~FlvTag() {}

    void setAsVideoTag() { m_tagType = VIDEO; }
    bool isVideoTag() const { return m_tagType == VIDEO; }
    
    void setAsAudioTag() { m_tagType = AUDIO; }
    bool isAudioTag() const { return m_tagType == AUDIO; }
    
    void setAsScriptDataTag() { m_tagType = SCRIPT_DATA; }
    bool isScriptDataTag() const { return m_tagType == SCRIPT_DATA; }

    void setTagTimeStamp(uint32_t ts) { m_timeStamp = ts; }
    uint32_t getTagTimeStamp() const { return m_timeStamp; }

    uint32_t getTagSize(bool includePrevTag = true) const;
    FlvTagType getTagType() const { return m_tagType; }

    /*
    * 编码成FlvTag字符串
    * @param includePrevTag 表示编码的内容是否包含FlvTag后的4字节PrevTagSize
    */
    virtual void encode(std::string& encodedData, bool includePrevTag = true) const;

    /*
    * 解码FlvTag字符串
    * @param includePrevTag 指标是否要同时解码Tag后的4字节PrevTagSize
    * @ret 返回值大于0表示解码成功，并表示解码的tag的数据长度
    */
    virtual int decode(const uint8_t* data, uint32_t size, bool includePrevTag = true);
private:
    /*
    * 具体的FlvTag类型进行编码，encodedData已编码了FlvTag Header内容
    */
    virtual void encodeTagData(std::string& encodedData) const = 0;
    /*
    * 具体的FlvTag类型进行解码，data内容从FlvTag data数据区开始，不包含FlvTag Header内容
    * @param data为数据区开始的位置
    * @param dataSize为FlvTag数据区的大小，FlvTag Header解析得到的大小
    */
    virtual void decodeTagData(const uint8_t* data, uint32_t dataSize) = 0;
private:
    FlvTagType m_tagType;
    uint32_t m_dataSize;
    uint32_t m_timeStamp;
};

class GeneralFlvTag : public FlvTag
{
/*
    透传用的FlvTag，不对FlvTag的数据区内容进行解析，仅储存
*/
private:
    void encodeTagData(std::string& encodedData) const;
    void decodeTagData(const uint8_t* data, uint32_t dataSize);
private:
    std::string m_flvData;
};

class AVCVideoFlvTag : public FlvTag
{
public:
    enum AVCPacketType
    {
        AVC_HEADER = 0,
        AVC_NALU = 1,
        AVC_END = 2,
    };
public:
    AVCVideoFlvTag();

    void setAsAvcHeader() { m_avcPktType = AVC_HEADER; }
    bool isAvcHeader() const { return m_avcPktType == AVC_HEADER; }

    void setAsAvcNalu() { m_avcPktType = AVC_NALU; }
    bool isAvcNalu() const { return m_avcPktType == AVC_NALU; }

    void setAsAvcEnd() { m_avcPktType = AVC_END; }
    bool isAvcEnd() const { return m_avcPktType == AVC_END; }

    void setIsIFrame(bool iframe) { m_isIFrame = iframe; }
    bool isIFrame() const { return m_isIFrame; }

    void setCts(uint32_t cts) { m_cts = cts; }
    uint32_t getCts() const { return m_cts; }

    void setRawData(const std::string& rawData) { m_rawData = rawData; }
    const std::string& getRawData() const { return m_rawData; }
private:
    void encodeTagData(std::string& encodedData) const;
    void decodeTagData(const uint8_t* data, uint32_t dataSize);
private:
    bool m_isIFrame;
    AVCPacketType m_avcPktType;
    uint32_t m_cts;
    std::string m_rawData;
};

class AACAudioFlvTag : public FlvTag
{
public:
    enum AudioFormat
    {
        LPCM_PLATFORM_ENDIAN = 0,
        MP3 = 2,
        LPCM_LITTLE_ENDIAN = 3,
        AAC = 10,
        SPEEX = 11
    };

    enum AACPacketType
    {
        AAC_Sequence_Header = 0,
        AAC_Raw = 1
    };

    enum SampleRate
    {
        _5500HZ = 0,
        _11000HZ = 1,
        _22000HZ = 2,
        _440000HZ = 3
    };

    enum SampleBit
    {
        SND_8BIT = 0,
        SND_16BIT = 1
    };

    enum SoundChannel
    {
        MONO = 0,
        STEREO = 1
    };

public:
    AACAudioFlvTag();

    void setAudioFormat(AudioFormat fmt) { m_audioFmt = fmt; }
    AudioFormat getAudioFormat() const { return m_audioFmt; }

    void setSampleRate(SampleRate rate) { m_sampleRate = rate; }
    SampleRate getSampleRate() const { return m_sampleRate; }

    void setSampleBit(SampleBit bit) { m_sampleBit = bit; }
    SampleBit getSampleBit() const { return m_sampleBit; }

    void setSoundChannel(SoundChannel channel) { m_channel = channel; }
    SoundChannel getSoundChannel() const { return m_channel; }

    void setRawData(const std::string& rawData) { m_rawData = rawData; }
    const std::string& getRawData() const { return m_rawData; }

    void setPacketType(AACPacketType type) { m_packetType = type; }
    AACPacketType getPacketType() const { return m_packetType; }
    bool isAACHeader() const { return (m_packetType == AAC_Sequence_Header); }
private:
    void encodeTagData(std::string& encodedData) const;
    void decodeTagData(const uint8_t* data, uint32_t dataSize);
private:
    AudioFormat m_audioFmt;
    SampleRate m_sampleRate;
    SampleBit m_sampleBit;
    SoundChannel m_channel;

    AACPacketType m_packetType;
    std::string m_rawData;
};

class ScriptData
{
public:
    enum ScriptDataType
    {
        SCRIPT_NUMBER = 0,
        SCRIPT_BOOLEAN = 1,
        SCRIPT_STRING = 2,
        SCRIPT_ARRAY = 8,
    };
public:
    ScriptData();

private:
    ScriptDataType m_dataType;
    double m_numberVal;
    bool m_boolVal;
    std::string m_stringVal;
};

class ScriptDataTag : public FlvTag
{
public:
    enum ScriptDataType
    {
        SCRIPT_NUMBER = 0,
        SCRIPT_BOOLEAN = 1,
        SCRIPT_STRING = 2,
        SCRIPT_ARRAY = 8,
    };
public:
    ScriptDataTag() {}
    virtual ~ScriptDataTag() {}

    const std::string& getScriptDataName() const { return m_scriptDataName; }
    void setScriptDataName(const std::string& name) { m_scriptDataName = name; }
    std::string getPropertyValue(const std::string& propertyName, std::string defaultValue = "");
    void addPropertyValue(const std::string& name, const std::string& value);
private:
    void encodeTagData(std::string& encodedData) const;
    void decodeTagData(const uint8_t* data, uint32_t dataSize);

    int32_t decodeScriptString(const uint8_t* data, uint32_t size, std::string& retString);
    int32_t decodeScriptNumber(const uint8_t* data, uint32_t size, std::string& retString);
    int32_t decodeScriptBoolean(const uint8_t* data, uint32_t size, std::string& retString);
    int32_t decodeScriptProperty(const uint8_t* data, uint32_t size, std::map<std::string, std::string>& properties);
private:
    std::string m_scriptDataName;
    std::map<std::string, std::string> m_propertyValueMap;
};

#endif
