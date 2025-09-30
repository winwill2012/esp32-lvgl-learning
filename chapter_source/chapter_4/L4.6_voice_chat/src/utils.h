#ifndef UTILS_H
#define UTILS_H
#include <vector>

std::string generateTaskId();

std::string getChipId(const char* prefix);

int32_t readInt32(const uint8_t* bytes);

std::string readString(const uint8_t* bytes, uint32_t length);

std::vector<uint8_t> uint32ToUint8Array(uint32_t size);

std::pair<int, size_t> findMinIndexOfDelimiter(const String& input);

#endif
