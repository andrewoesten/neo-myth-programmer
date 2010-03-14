#ifndef CHECKSUM_H_
#define CHECKSUM_H_
#ifdef __cplusplus
extern "C" {
#endif 
//Needs a shared buffer MULTIPLE OF 4 and its length.For buffered IO reading
int CRC_Patch(char* sharedBuffer,const int sharedBufferLength);
int CRC_PatchAuto();

#ifdef __cplusplus
}
#endif 
#endif // CHECKSUM_H_
