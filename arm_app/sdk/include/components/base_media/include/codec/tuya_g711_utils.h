/** @file tuya_g711_utils.h
 * @brief g711 encode and decode utility
 */
#ifndef __TUYA_G711_UTILS_H__
#define __TUYA_G711_UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif


#define		TUYA_G711_A_LAW     (0)
#define		TUYA_G711_MU_LAW    (1)

#if 1
/** @brief encode 8K pcm to g711
 * @param[in]  type    TUYA_G711_A_LAW or TUYA_G711_MU_LAW
 * @param[in]  src     pcm data
 * @param[in]  src_len  size of pcm data
 * @param[in]  drc     g711 out data
 * @param[in]  p_out   size of g711 out data
 * @return error code
 * - 0  success
 * - -1 fail
 */
int tuya_g711_encode(unsigned char type, unsigned short *src, unsigned int src_len, unsigned char *drc, unsigned int *p_out);

/** @brief decode 8K g711 to pcm
 * @param[in]  type    TUYA_G711_A_LAW or TUYA_G711_MU_LAW
 * @param[in]  src     g711 data
 * @param[in]  src_len  size of g711 data
 * @param[in]  drc     pcm out data
 * @param[in]  p_out   size of pcm out data
 * @return error code
 * - 0 success
 * - -1 fail
 */
int tuya_g711_decode(unsigned char type, unsigned short *src, unsigned int src_len, unsigned char *drc, unsigned int *p_out);

/** @brief encode 16K pcm to g711
 * @param[in]  type    TUYA_G711_A_LAW or TUYA_G711_MU_LAW
 * @param[in]  src     pcm data
 * @param[in]  src_len  size of pcm data
 * @param[in]  drc     g711 out data
 * @param[in]  p_out   size of g711 out data
 * @return error code
 * - 0  success
 * - -1 fail
 */
int tuya_g711_encode_16K(unsigned char type, unsigned short *src, unsigned int src_len, unsigned char *drc, unsigned int *p_out);


/** @brief resample data from 8K to 16K
 * @param[in]  p_in_audio_data   8k audio data
 * @param[in]  n_in_audio_len    8k audio data len
 * @param[out]  p_out_audio_data  16k audio data
 * @param[out]  n_out_audio_len   16k audio data len
 */
void tuya_8k_resample_16k(short* p_in_audio_data, int n_in_audio_len, short* p_out_audio_data, int* n_out_audio_len);

#else

void tuya_g711_encode(unsigned int type, unsigned int sample_num, unsigned char *samples, unsigned char *bitstream);

void tuya_g711_encode_16k(unsigned int type, unsigned int sample_num, unsigned char *samples, unsigned char *bitstream);

void tuya_g711_decode(unsigned int type, unsigned int sample_num, unsigned char *samples, unsigned char *bitstream);

#endif
#ifdef __cplusplus
}
#endif

#endif // TUYA_G711_UTILS_H
