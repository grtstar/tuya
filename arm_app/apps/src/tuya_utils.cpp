#include <fstream>
#include <netinet/in.h>
#include <sys/stat.h>
#include <unistd.h>

#include "utils/log_.h"
#include "utils/json.hpp"
#include "lz4/lz4.h"

#include "tuya_utils.h"

using json = nlohmann::json;

float _mapResolution = 0.05;

#undef TAG
#define TAG "UTL"

// x -> -y; y -> x
Point TuyaXYToMars(int x, int y)
{
    return Point{x * _mapResolution / 10, y * _mapResolution / 10, 0};
}

int MarsXToTuya(Point p)
{
    return p.x * 10 / _mapResolution;
}
int MarsYToTuya(Point p)
{
    return p.y * 10 / _mapResolution;
}

bool TuyaLoadKey(const char *path, std::string &id, std::string &uuid, std::string &authKey)
{
    std::ifstream f(path);
    try
    {
        json data = json::parse(f);
        id = data["product_id"].get<std::string>();
        uuid = data["device_uuid"].get<std::string>();
        authKey = data["product_authkey"].get<std::string>();
        LOGD(TAG, "Got the tuya.key id={}, uuid={}", id, uuid);
        return true;
    }
    catch (const std::exception &e)
    {
        LOGE(TAG, "parse KEY catch : {0}", e.what());
        return false;
    }
}

bool SoftVersion(const char *path, std::string &soft_version, std::string &mcu_version, std::string &system_version)
{
    std::ifstream f(path);
    try
    {
        json data = json::parse(f);
        if (data.contains("soft"))
        {
            soft_version = data["soft"].get<std::string>();
            LOGD(TAG, "Got the soft version, soft: {0}", soft_version);
        }
        if (data.contains("MCU"))
        {
            mcu_version = data["MCU"].get<std::string>();
            LOGD(TAG, "Got the MCU version, soft: {0}", mcu_version);
        }
        if (data.contains("system"))
        {
            system_version = data["system"].get<std::string>();
            LOGD(TAG, "Got the system version, soft: {0}", system_version);
        }
        return true;
    }
    catch (const std::exception &e)
    {
        LOGE(TAG, "parse KEY catch : {0}", e.what());
        return false;
    }
}

bool UpdateSoftVersion(const char *path, const std::string &soft_version, const std::string &mcu_version, const std::string &system_version)
{
    std::ofstream out_file(path, std::ios::binary | std::ios::trunc);
    if (!out_file.is_open())
    {
        LOGE(TAG, "open {0} fail", path);
        return false;
    }

    json ver_info;
    ver_info["soft"] = soft_version;
    ver_info["MCU"] = mcu_version;
    ver_info["system"] = system_version;
    out_file << ver_info;
    out_file.flush();
    out_file.close();

    return true;
}

int CompareVersion(const std::string &version1, const std::string &version2)
{
    int n = version1.length(), m = version2.length();
    int i = 0, j = 0;
    while (i < n || j < m)
    {
        int x = 0;
        for (; i < n && version1[i] != '.'; ++i)
        {
            x = x * 10 + version1[i] - '0';
        }
        ++i; // 跳过点号
        int y = 0;
        for (; j < m && version2[j] != '.'; ++j)
        {
            y = y * 10 + version2[j] - '0';
        }
        ++j; // 跳过点号
        if (x != y)
        {
            return x > y ? 1 : -1;
        }
    }
    return 0;
}

bool MkdirFolder(const char *path)
{
    if (nullptr == path)
    {
        LOGE(TAG, "path is null");
        return false;
    }

    if (0 != access(path, F_OK))
    {
        int status = mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (0 != status)
        {
            LOGE(TAG, "mkdir %s fail", path);
            return false;
        }
        else
        {
            LOGD(TAG, "mkdir %s success", path);
            return true;
        }
    }
    return true;
}

bool ReadFile(const char *path, FILE_INFO *pStrFileInfo)
{
    FILE *fp = fopen(path, "rb");
    if (fp == NULL)
    {
        LOGE(TAG, "File Open Failed:{}", path);
        return false;
    }

    fseek(fp, 0, SEEK_END);
    int len = ftell(fp);
    if (len <= 0)
    {
        LOGE(TAG, "file len is :{}", len);
        fclose(fp);
        return false;
    }
    fseek(fp, 0, SEEK_SET);

    pStrFileInfo->buff = (char *)malloc(len);
    if (pStrFileInfo->buff == NULL)
    {
        LOGE(TAG, "malloc failed :{}", len);
        fclose(fp);
        return false;
    }

    int read_len = fread(pStrFileInfo->buff, 1, len, fp);
    if (read_len != len)
    {
        LOGE(TAG, "fread {} failed!", path);
        fclose(fp);
        free(pStrFileInfo->buff);
        pStrFileInfo->buff = NULL;
        return false;
    }

    fclose(fp);
    fp = NULL;

    pStrFileInfo->len = len;
    return true;
}

uint8_t PixelToTuyaPixel(int8_t p)
{
    if (p >= 60)
    {
        return 0x01;
    }
    else if (p < 60 && p >= 45)
    {
        return 0x03;
    }
    else if (p < 45 && p >= 0)
    {
        return 0x00;
    }
    else if (p < 0)
    {
        return 0x03;
    }
    return 0x03;
}

uint8_t PixelToTuyaPixelv1(int8_t p)
{
    if (p == -1)
    {
        return 0x03;
    }
    if (p >= 60)
    {
        return 0x01;
    }
    else if (p < 60 && p >= 45)
    {
        return 0x03;
    }
    else if (p <= -10 && p >= -40)
    {
        uint8_t r = MarRoomIdToTuya(p) << 2;
        return r;
    }
    return 0x03;
}

uint8_t PixelToTuyaPixelv2(int8_t p) {
  if (p == -1) 
  {
    return 0x00;
  }
  
  if (p >= 60) 
  {
    return 0x01;
  } 
  else if (p < 60 && p >= 45) 
  {
    return 0x00;
  } 
  else if (p <= -10 && p >= -36) 
  {
    uint8_t r = MarRoomIdToTuya(p) << 3;
    return r | 0x07;
  }
  else if(p >= -66 && p <= -40)
  {
    uint8_t r = -(p + 30);
    r = MarRoomIdToTuya(r) << 3;
    r = r | 0x02;
    return r;
  }
  return 0x00;
}

uint8_t Combine4Pixel(const int8_t *pix, int len)
{
    uint8_t r = 0x0;
    for (int i = 0; i < len; i++)
    {
        uint8_t d = PixelToTuyaPixel(*pix++);
        r <<= 2;
        r |= d;
    }
    for (int i = len; i < 4; i++)
    {
        r <<= 2;
        r |= 0x3;
    }
    return r;
}

int lz4compress(void *data, size_t len, std::vector<uint8_t> &out)
{
    uint8_t *lzBuf = new uint8_t[len];
    int r = LZ4_compress_default((const char *)data, (char *)lzBuf, len, len);
    if (r > 0)
    {
        out.assign(lzBuf, lzBuf + r);
    }
    delete lzBuf;
    return r;
}

/**
 * @brief 网络字节序转主机字节序
 *
 * @param d
 * @return int16_t
 */
int16_t nbto16(uint8_t *d)
{
    return (d[0] << 8) | d[1];
}

/**
 * @brief 主机字节序转网络字节序
 *
 * @param d
 * @return std::vector<uint8_t>
 */
std::vector<uint8_t> tonb16(int16_t d)
{
    std::vector<uint8_t> v;
    v.push_back(d >> 8);
    v.push_back(d);
    return v;
}

/**
 * @brief 网络字节序转主机字节序
 *
 * @param d
 * @return int16_t
 */
int nbto32(uint8_t *d)
{
    return d[0] << 24 | d[1] << 16 | d[2] << 8 | d[3];
}

/**
 * @brief 主机字节序转网络字节序
 *
 * @param d
 * @return std::vector<uint8_t>
 */
std::vector<uint8_t> tonb32(int32_t d)
{
    std::vector<uint8_t> v;
    v.push_back(d >> 24);
    v.push_back(d >> 16);
    v.push_back(d >> 8);
    v.push_back(d);
    return v;
}

TuyaMap ToTuyaMap(const AppMap &map)
{
    _mapResolution = map.resolution;

    TuyaMap tuya;
    // 普通版本
    tuya.header.version = map.version;
    // 地图识别 id
    tuya.header.map_id = map.mapId;
    // 地图非稳定状态
    tuya.header.type = map.type;
    // 地图宽度
    tuya.header.map_width = map.width;
    //
    tuya.header.map_height = map.height;
    // ?
    tuya.header.map_ox = (-map.origin.pose.x) / map.resolution * 10;
    // ?
    tuya.header.map_oy = ((map.height - 1) - (-map.origin.pose.y) / map.resolution) * 10;
    //
    tuya.header.map_resolution = map.resolution * 100;
    //
    if (map.charger.pose.x == 0 && map.charger.pose.y == 0)
    {
        tuya.header.charge_x = 0;
        tuya.header.charge_y = 0;
    }
    else
    {
        tuya.header.charge_x = (-map.charger.pose.x) / map.resolution * 10;
        tuya.header.charge_y = ((map.height - 1) - (-map.charger.pose.y) / map.resolution) * 10;
    }

    //
    if (tuya.header.version == 0x00)
    {
        tuya.header.pix_len = (map.width * map.height + 3) / 4;
    }
    else
    {
        tuya.header.pix_len = map.width * map.height;
    }
    // ?
    tuya.header.pix_lz4len = 0;

    std::vector<uint8_t> pixels;
    pixels.reserve(tuya.header.pix_len);

    std::vector<int8_t> data;
    data.resize(map.data.size());
    // 颠倒 Y
    for (int y = 0; y < map.height; y++)
    {
        for (int x = 0; x < map.width; x++)
        {
            data[(map.height - y - 1) * map.width + x] = map.data[y * map.width + x];
        }
    }

    // 普通版本
    if (tuya.header.version == 0x00)
    {
        for (int i = 0; i < data.size(); i += 4)
        {
            int len = data.size() - i >= 4 ? 4 : data.size() - i;
            pixels.push_back(Combine4Pixel(&data[i], len));
        }

        if (lz4compress(&pixels[0], pixels.size(), tuya.pix) <= 0)
        {
            tuya.pix = pixels;
            tuya.header.pix_lz4len = 0;
            LOGD(TAG, "map lz4 compress failed, send origin data");
        }
        else
        {
            tuya.header.pix_lz4len = tuya.pix.size();
            LOGD(TAG, "map lz4 compress len = {}", tuya.header.pix_lz4len);
        }
        tuya.region_num = 0;
    }
    // 分区版本　&& 地板材质版本
    if (tuya.header.version == 0x01 || tuya.header.version == 0x02)
    {   
        
        for (int i = 0; i < data.size(); i++)
        {
          // 分区版本
            if (tuya.header.version == 0x01) 
            {   
                pixels.push_back(PixelToTuyaPixelv1(data[i]));
            }
            // 地板材质版本
            else
            {
                pixels.push_back(PixelToTuyaPixelv2(data[i]));
            }
        }

        tuya.pix = pixels;
        tuya.header.pix_lz4len = 0;

        tuya.region_num = map.roomNum;
        for (int i = 0; i < tuya.region_num; i++)
        {
            TuyaRoom room;
            room.room_propeties.room_id = map.roomPropeties[i].roomId;
            room.room_propeties.clean_order = map.roomPropeties[i].cleanOrder;
            room.room_propeties.clean_repeat = map.roomPropeties[i].cleanRepeart;
            room.room_propeties.mop_repeat = map.roomPropeties[i].mopRepeat;
            room.room_propeties.color_order = map.roomPropeties[i].colorOrder;
            room.room_propeties.donot_sweep = map.roomPropeties[i].donotSweep;
            room.room_propeties.donot_mop = map.roomPropeties[i].donotMop;
            room.room_propeties.fan_power = MarsSuctionToTuya(map.roomPropeties[i].fanPower);
            room.room_propeties.water_level = MarsCisternToTuya(map.roomPropeties[i].waterLevel);
            room.room_propeties.enable_ymop = map.roomPropeties[i].enableYMop;

            room.room_name[0] = map.roomName[i].length();
            strncpy((char *)&room.room_name[1], map.roomName[i].c_str(), sizeof(room.room_name) - 1);

            room.vertices_num = map.roomPolygon[i].count;
            for (int j = 0; j < room.vertices_num; j++)
            {
                TuyaVertex vertex;
                vertex.x = MarsXToTuya(map.roomPolygon[i].vetex[j]);
                vertex.y = MarsYToTuya(map.roomPolygon[i].vetex[j]);
                room.room_vertices.push_back(vertex);
            }
            tuya.rooms.push_back(room);
        }
    }

    return tuya;
}

void SaveTuyaMap(TuyaMap &tuya, const char *file_path)
{
    FILE *fp = fopen(file_path, "wb");
    if (fp)
    {   
        tuya.header.map_id = htons(tuya.header.map_id);
        tuya.header.map_width = htons(tuya.header.map_width);
        tuya.header.map_height = htons(tuya.header.map_height);
        tuya.header.map_ox = htons(tuya.header.map_ox);
        tuya.header.map_oy = htons(tuya.header.map_oy);
        tuya.header.map_resolution = htons(tuya.header.map_resolution);
        tuya.header.charge_x = htons(tuya.header.charge_x);
        tuya.header.charge_y = htons(tuya.header.charge_y);
        tuya.header.pix_len = htonl(tuya.header.pix_len);
        tuya.header.pix_lz4len = htons(tuya.header.pix_lz4len);

        fwrite(&tuya.header.version, sizeof(tuya.header.version), 1, fp);
        fwrite(&tuya.header.map_id, sizeof(tuya.header.map_id), 1, fp);
        fwrite(&tuya.header.type, sizeof(tuya.header.type), 1, fp);
        fwrite(&tuya.header.map_width, sizeof(tuya.header.map_width), 1, fp);
        fwrite(&tuya.header.map_height, sizeof(tuya.header.map_height), 1, fp);
        fwrite(&tuya.header.map_ox, sizeof(tuya.header.map_ox), 1, fp);
        fwrite(&tuya.header.map_oy, sizeof(tuya.header.map_oy), 1, fp);
        fwrite(&tuya.header.map_resolution, sizeof(tuya.header.map_resolution), 1, fp);
        fwrite(&tuya.header.charge_x, sizeof(tuya.header.charge_x), 1, fp);
        fwrite(&tuya.header.charge_y, sizeof(tuya.header.charge_y), 1, fp);
        fwrite(&tuya.header.pix_len, sizeof(tuya.header.pix_len), 1, fp);

        if (tuya.header.version != 0x00)
        {
            static unsigned int malloc_len = 1 * 1024 * 1024;
            static unsigned char *buf = (unsigned char *)malloc(malloc_len);
            unsigned int ori_len = tuya.pix.size() + sizeof(tuya.region_num) + tuya.rooms.size() * sizeof(TuyaRoom);
            unsigned int total_size = 0;
            if (ori_len > malloc_len)
            {
                LOGD(TAG, "Realloc size {} success", ori_len);
                buf = (unsigned char *)realloc(buf, ori_len);
                malloc_len = ori_len;
            }
            if (NULL == buf)
            {
                LOGE(TAG, "Malloc len {} fail", malloc_len);
                return;
            }

            memcpy(buf, &tuya.pix[0], tuya.pix.size());
            total_size += tuya.pix.size();
            if (tuya.region_num > 0)
            {
                uint16_t region_num = (1 << 8) | (tuya.region_num & 0xFF);
                region_num = htons(region_num);
                memcpy(buf + total_size, &region_num, sizeof(region_num));
                total_size += sizeof(region_num);
                for (int i = 0; i < tuya.region_num; i++)
                {
                    auto roomProp = tuya.rooms[i].room_propeties;
                    roomProp.room_id = htons(roomProp.room_id);
                    roomProp.clean_order = htons(roomProp.clean_order);
                    roomProp.clean_repeat = htons(roomProp.clean_repeat);
                    roomProp.mop_repeat = htons(roomProp.mop_repeat);
                    memcpy(buf + total_size, &roomProp, sizeof(roomProp));
                    total_size += sizeof(roomProp);

                    memcpy(buf + total_size, tuya.rooms[i].room_name, sizeof(tuya.rooms[i].room_name));
                    total_size += sizeof(tuya.rooms[i].room_name);

                    memcpy(buf + total_size, &tuya.rooms[i].vertices_num, sizeof(tuya.rooms[i].vertices_num));
                    total_size += sizeof(tuya.rooms[i].vertices_num);
                    for (int j = 0; j < tuya.rooms[i].vertices_num; j++)
                    {
                        auto vertex = tuya.rooms[i].room_vertices[j];
                        vertex.x = htons(vertex.x);
                        vertex.y = htons(vertex.y);
                        memcpy(buf + total_size, &vertex.x, sizeof(vertex.x));
                        total_size += sizeof(vertex.x);
                        memcpy(buf + total_size, &vertex.y, sizeof(vertex.y));
                        total_size += sizeof(vertex.y);
                    }
                }
            }
            std::vector<uint8_t> lz4_data;
            if (lz4compress(buf, ori_len, lz4_data) <= 0)
            {
                LOGD(TAG, "map lz4 compress failed, send origin data");
                tuya.header.pix_lz4len = 0;
                fwrite(&tuya.header.pix_lz4len, sizeof(tuya.header.pix_lz4len), 1, fp);
                fwrite(buf, ori_len, 1, fp);
            }
            else
            {
                tuya.header.pix_lz4len = htons(lz4_data.size());
                fwrite(&tuya.header.pix_lz4len, sizeof(tuya.header.pix_lz4len), 1, fp);
                fwrite(&lz4_data[0], lz4_data.size(), 1, fp);
            }
        }
        else
        {
            fwrite(&tuya.header.pix_lz4len, sizeof(tuya.header.pix_lz4len), 1, fp);
            fwrite(&tuya.pix[0], tuya.pix.size(), 1, fp);
        }

        fclose(fp);
    }
}

enum PATH_TYPE
{
    CLEAN_PATH,
    NAVI_PATH,
    CHARGE_PATH,
    MOP_PATH
};

TuyaPath ToTuyaPath(const AppPath &path, int pathId, int pathType)
{
    TuyaPath tuya;
    tuya.header.version = 0x00;
    tuya.header.path_id = pathId & 0xFF;
    tuya.header.init_flag = 0x01;
    tuya.header.type = 0x2;
    tuya.header.direction = path.degree * 10;

#if 1
    tuya.header.count = path.points.size();
    std::vector<int16_t> points;
    points.resize(tuya.header.count * 2);
    for (int i = 0; i < tuya.header.count; i++)
    {
        points[2 * i] = MarsXToTuya(path.points[i]);
        points[2 * i + 1] = MarsYToTuya(path.points[i]);
        points[2 * i] &= ~1;
        points[2 * i + 1] &= ~1;
        // if(path.pointType[i] == CLEAN_PATH)
        // {
        //     points[2 * i] |= 0;
        //     points[2 * i + 1] |= 0;
        // }
        // if(path.pointType[i] == NAVI_PATH)
        // {
        //     points[2 * i] |= 0;
        //     points[2 * i + 1] |= 1;
        // }
        // if(path.pointType[i] == CHARGE_PATH)
        // {
        //     points[2 * i] |= 1;
        //     points[2 * i + 1] |= 0;
        // }
        // if(path.pointType[i] == MOP_PATH)
        // {
        //     points[2 * i] |= 1;
        //     points[2 * i + 1] |= 1;
        // }
        points[2 * i] = htons(points[2 * i]);
        points[2 * i + 1] = htons(points[2 * i + 1]);
    }
    if (1) // lz4compress(&points[0], points.size() * 2, tuya.points) <= 0)
    {
        tuya.points.assign((uint8_t *)&points[0], ((uint8_t *)&points[0]) + points.size() * 2);
        tuya.header.lz4len = 0;
        LOGD(TAG, "path lz4 compress failed, send origin data");
    }
    else
    {
        tuya.header.lz4len = tuya.points.size();
        LOGD(TAG, "path lz4 compress len = {}", tuya.header.lz4len);
    }
#else
    tuya.header.count = 2;
    std::vector<int16_t> points;
    points.resize(tuya.header.count * 2);
    points[0] = htons(0);
    points[1] = htons(0);
    points[2] = htons(200);
    points[3] = htons(200);
    tuya.points.assign((uint8_t *)&points[0], ((uint8_t *)&points[0]) + points.size() * 2);
    tuya.header.lz4len = 0;
#endif
    return tuya;
}

void SaveTuyaPath(TuyaPath &tuya, const char *file_path)
{
    FILE *fp = fopen(file_path, "wb");
    if (fp)
    {
        tuya.header.path_id = htons(tuya.header.path_id);
        tuya.header.count = htonl(tuya.header.count);
        tuya.header.direction = htons(tuya.header.direction);
        tuya.header.lz4len = htons(tuya.header.lz4len);
        fwrite(&tuya.header.version, sizeof(tuya.header.version), 1, fp);
        fwrite(&tuya.header.path_id, sizeof(tuya.header.path_id), 1, fp);
        fwrite(&tuya.header.init_flag, sizeof(tuya.header.init_flag), 1, fp);
        fwrite(&tuya.header.type, sizeof(tuya.header.type), 1, fp);
        fwrite(&tuya.header.count, sizeof(tuya.header.count), 1, fp);
        fwrite(&tuya.header.direction, sizeof(tuya.header.direction), 1, fp);
        fwrite(&tuya.header.lz4len, sizeof(tuya.header.lz4len), 1, fp);
        fwrite(&tuya.points[0], tuya.points.size(), 1, fp);
        fclose(fp);
    }
}

bool ToTuyaVirtualWall(const AppVirtualWall &appVirtualWall, uint8_t cmd, std::vector<uint8_t> &tuyaVirtualWall)
{
    tuyaVirtualWall.push_back(0xAA);
    tuyaVirtualWall.push_back(0x00);
    tuyaVirtualWall.push_back(0x00);
    tuyaVirtualWall.push_back(cmd);
    if (appVirtualWall.version == 0)
    {
        LOGD(TAG, "上报虚拟墙设置 v1.0.0");
        tuyaVirtualWall.push_back(appVirtualWall.count);
        for (int i = 0; i < appVirtualWall.count; i++)
        {
            int x0 = MarsXToTuya(appVirtualWall.wall[i].begin);
            int y0 = MarsYToTuya(appVirtualWall.wall[i].begin);
            int x1 = MarsXToTuya(appVirtualWall.wall[i].end);
            int y1 = MarsYToTuya(appVirtualWall.wall[i].end);
            auto xx = tonb16(x0);
            tuyaVirtualWall.insert(tuyaVirtualWall.end(), xx.begin(), xx.end());
            xx = tonb16(y0);
            tuyaVirtualWall.insert(tuyaVirtualWall.end(), xx.begin(), xx.end());
            xx = tonb16(x1);
            tuyaVirtualWall.insert(tuyaVirtualWall.end(), xx.begin(), xx.end());
            xx = tonb16(y1);
            tuyaVirtualWall.insert(tuyaVirtualWall.end(), xx.begin(), xx.end());
        }
        tuyaVirtualWall[2] = tuyaVirtualWall.size() - 3;
        uint8_t sum = 0;
        for (int i = 3; i < tuyaVirtualWall.size(); i++)
        {
            sum += tuyaVirtualWall[i];
        }
        tuyaVirtualWall.push_back(sum);
        return true;
    }

    return false;
}

bool ToTuyaRestrictedArea(AppRestrictedArea &appRestrictedArea, uint8_t cmd, std::vector<uint8_t> &tuyaRestrictedArea)
{
    tuyaRestrictedArea.push_back(0xAA);
    tuyaRestrictedArea.push_back(0x00);
    tuyaRestrictedArea.push_back(0x00);
    tuyaRestrictedArea.push_back(cmd);

    if (appRestrictedArea.version == 0)
    {
        LOGD(TAG, "上报禁区设置 v1.1.0");
        return false;
    }
    if (appRestrictedArea.version == 1)
    {
        LOGD(TAG, "上报禁区设置 v1.2.0");
        tuyaRestrictedArea.push_back(appRestrictedArea.version);
        tuyaRestrictedArea.push_back(appRestrictedArea.count);
        for (int i = 0; i < appRestrictedArea.count; i++)
        {
            tuyaRestrictedArea.push_back(appRestrictedArea.mode[i]);
            tuyaRestrictedArea.push_back(appRestrictedArea.polygon[i].count);
            for (int j = 0; j < appRestrictedArea.polygon[i].count; j++)
            {
                int x0 = MarsXToTuya(appRestrictedArea.polygon[i].vetex[j]);
                int y0 = MarsYToTuya(appRestrictedArea.polygon[i].vetex[j]);
                auto d = tonb16(x0);
                tuyaRestrictedArea.insert(tuyaRestrictedArea.end(), d.begin(), d.end());
                d = tonb16(y0);
                tuyaRestrictedArea.insert(tuyaRestrictedArea.end(), d.begin(), d.end());
            }

            tuyaRestrictedArea.push_back(appRestrictedArea.name[i].length());
            appRestrictedArea.name[i].resize(19, 0);
            for (int j = 0; j < 19; j++)
            {
                tuyaRestrictedArea.push_back(appRestrictedArea.name[i][j]);
            }
        }
        tuyaRestrictedArea[2] = tuyaRestrictedArea.size() - 3;
        uint8_t sum = 0;
        for (int i = 3; i < tuyaRestrictedArea.size(); i++)
        {
            sum += tuyaRestrictedArea[i];
        }
        tuyaRestrictedArea.push_back(sum);
    }
    if (appRestrictedArea.version == 2)
    {
        LOGD(TAG, "上报禁区设置 v1.2.1");
        tuyaRestrictedArea.push_back(appRestrictedArea.version);
        tuyaRestrictedArea.push_back(appRestrictedArea.count);
        for (int i = 0; i < appRestrictedArea.count; i++)
        {
            tuyaRestrictedArea.push_back(appRestrictedArea.mode[i]);
            tuyaRestrictedArea.push_back(appRestrictedArea.type[i]);
            tuyaRestrictedArea.push_back(appRestrictedArea.polygon[i].count);
            for (int j = 0; j < appRestrictedArea.polygon[i].count; j++)
            {
                int x0 = MarsXToTuya(appRestrictedArea.polygon[i].vetex[j]);
                int y0 = MarsYToTuya(appRestrictedArea.polygon[i].vetex[j]);
                auto d = tonb16(x0);
                tuyaRestrictedArea.insert(tuyaRestrictedArea.end(), d.begin(), d.end());
                d = tonb16(y0);
                tuyaRestrictedArea.insert(tuyaRestrictedArea.end(), d.begin(), d.end());
            }

            tuyaRestrictedArea.push_back(appRestrictedArea.name[i].length());
            appRestrictedArea.name[i].resize(19, 0);
            for (int j = 0; j < 19; j++)
            {
                tuyaRestrictedArea.push_back(appRestrictedArea.name[i][j]);
            }
        }
        tuyaRestrictedArea[2] = tuyaRestrictedArea.size() - 3;
        uint8_t sum = 0;
        for (int i = 3; i < tuyaRestrictedArea.size(); i++)
        {
            sum += tuyaRestrictedArea[i];
        }
        tuyaRestrictedArea.push_back(sum);
    }
    return true;
}