/* 确保此头文件只被包含一次 */
#ifndef THINKGEAR_STREAM_PARSER_H_
#define THINKGEAR_STREAM_PARSER_H_

/* 包含所有所需外部库*/
#include <stdlib.h>

/*当编译CPP时定义外部C类型*/
#ifdef __cplusplus
extern "C" {
#endif

/* 解析类型*/
#define PARSER_TYPE_NULL       0x00
#define PARSER_TYPE_PACKETS    0x01    /* TGAM流数据*/
#define PARSER_TYPE_2BYTERAW   0x02    /* 2位的源数据*/

/* 定义数据代码*/
#define PARSER_CODE_BATTERY            0x01
#define PARSER_CODE_POOR_QUALITY       0x02 //信号质量
#define PARSER_CODE_ATTENTION          0x04 //注意度
#define PARSER_CODE_MEDITATION         0x05 //冥想值
#define PARSER_CODE_8BITRAW_SIGNAL     0x06 //8位源信号
#define PARSER_CODE_RAW_MARKER         0x07 //源标志

#define PARSER_CODE_RAW_SIGNAL         0x80 //源信号
#define PARSER_CODE_EEG_POWERS         0x81 
#define PARSER_CODE_ASIC_EEG_POWER_INT 0x83

/**
 * 解析器是一个管理解析状态的状态机
 */
typedef struct _ThinkGearStreamParser {

    unsigned char   type;
    unsigned char   state;

    unsigned char   lastByte;

    unsigned char   payloadLength;
    unsigned char   payloadBytesReceived;
    unsigned char   payload[256];
    unsigned char   payloadSum;
    unsigned char   chksum;//检验值

    void (*handleDataValue)( unsigned char extendedCodeLevel,
                             unsigned char code, unsigned char numBytes,
                             const unsigned char *value, void *customData );
    void  *customData;

} ThinkGearStreamParser;

/**
 * @param parser              Pointer to a ThinkGearStreamParser object.
 * @param parserType          One of the PARSER_TYPE_* constants defined above:
 *                            PARSER_TYPE_PACKETS or PARSER_TYPE_2BYTERAW.
 * @param handleDataValueFunc A user-defined callback function that will
 *                            be called whenever a data value is parsed
 *                            from a Packet.
 * @param customData          A pointer to any arbitrary data that will
 *                            also be passed to the handleDataValueFunc
 *                            whenever a data value is parsed from a
 *                            Packet.
 *
 * @return -1 if @c parser is NULL.
 * @return -2 if @c parserType is invalid.
 * @return 0 成功.
 */
int
THINKGEAR_initParser( ThinkGearStreamParser *parser, unsigned char parserType,
                      void (*handleDataValueFunc)(
                          unsigned char extendedCodeLevel,
                          unsigned char code, unsigned char numBytes,
                          const unsigned char *value, void *customData),
                      void *customData );

/**
 * This is merely an example function prototype for a handleDataValueFunc()
 * callback function to be passed to THINKGEAR_initParser().  The user is
 * expected to actually define a function with this function signature, and pass
 * its name into the THINKGEAR_initParser() to create a ThinkGearStreamParser.
 * Then, each time a Data Value is successfully parsed by THINKGEAR_parseByte(),
 * this function gets automatically called, so that the user's application can
 * do whatever it likes with the @c value.
 *
 * The @c extendedCodeLevel and @c code together tells the application
 * what type of Data Value was received. The @c numBytes indicates the
 * number of bytes in the @c value[] array.
 *
 * The @c customData is a pointer to whatever user-defined data was
 * originally passed to THINKGEAR_initParser().
 *
 */
void
exampleHandleDataValueFunc( unsigned char extendedCodeLevel,
                            unsigned char code,
                            unsigned char numBytes,
                            const unsigned char *value,
                            void *customData );


/**
 * Feeds the @c byte into the @c parser.  If the @c byte completes a
 * complete, valid parser, then the @c parser's handleDataValue()
 * function is automatically called on each DataRow in the Packet.
 * The return value provides an indication of the state of the
 * @c parser after parsing the byte.
 *
 * @param parser Pointer to an initialized ThinkGearDataParser object.
 * @param byte   The next byte of the data stream.
 *
 * @return -1 if @c parser is NULL.
 * @return -2 if a complete Packet was received, but the checksum failed.
 * @return -3 if an invalid Packet with PLENGTH > 170 was detected.
 * @return -4 if an invalid Packet with PLENGTH == 170 was detected.
 * @return -5 if the @c parser is somehow in an unrecognized state.
 * @return 0 if the @c byte did not yet complete a Packet.
 * @return 1 if a Packet was received and parsed successfully.
 *
 */
int
THINKGEAR_parseByte( ThinkGearStreamParser *parser, unsigned char byte );


#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* THINKGEAR_STREAM_DECODER_H_ */
