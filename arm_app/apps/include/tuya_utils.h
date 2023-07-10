#pragma once
#include <string>

#include "mars_message/Point.hpp"
#include "mars_message/AppMap.hpp"
#include "mars_message/AppPath.hpp"
#include "mars_message/AppVirtualWall.hpp"
#include "mars_message/AppRestrictedArea.hpp"

#include "tuya_enums.h"

using namespace mars_message;

typedef struct {
  char fileName[48] = {0};
  unsigned int len = 0;
  char *buff = NULL;
}FILE_INFO;

Point TuyaXYToMars(int x, int y);
int MarsXToTuya(Point p);
int MarsYToTuya(Point p);
bool TuyaLoadKey(const char *path, std::string &id, std::string &uuid, std::string &authKey);
bool SoftVersion(const char *path, std::string &soft_version, std::string &mcu_version, std::string &system_version);
bool UpdateSoftVersion(const char *path, const std::string &soft_version, const std::string &mcu_version, const std::string &system_version);
// 版本格式:xx.xx.xx ,version1 > version2 返回1, version1 < version2 返回-1,其他返回0
int CompareVersion(const std::string &version1, const std::string &version2);
uint8_t PixelToTuyaPixel(int8_t p);
uint8_t Combine4Pixel(const int8_t *pix, int len);
int lz4compress(void *data, size_t len, std::vector<uint8_t> &out);
int16_t nbto16(uint8_t *d);
std::vector<uint8_t> tonb16(int16_t d);
int nbto32(uint8_t *d);
std::vector<uint8_t> tonb32(int32_t d);
TuyaMap ToTuyaMap(const AppMap &map);
void SaveTuyaMap(TuyaMap &tuya, const char *file_path);
TuyaPath ToTuyaPath(const AppPath &path, int pathId, int pathType);
void SaveTuyaPath(TuyaPath &tuya, const char *file_path);
bool ToTuyaVirtualWall(const AppVirtualWall &appVirtualWall, uint8_t cmd, std::vector<uint8_t> &tuyaVirtualWall);
bool ToTuyaRestrictedArea(AppRestrictedArea &appRestrictedArea, uint8_t cmd, std::vector<uint8_t> &tuyaRestrictedArea);
bool MkdirFolder(const char *path);
bool ReadFile(const char *path, FILE_INFO *pStrFileInfo);