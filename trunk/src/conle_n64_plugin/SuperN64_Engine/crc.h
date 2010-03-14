#ifndef CRC_H
#define CRC_H
#ifdef __cplusplus
extern "C" {
#endif 
unsigned int CRC_Calculate(unsigned int crc, const unsigned char* buf, unsigned int len);
#ifdef __cplusplus
}
#endif 
#endif // CRC_H
