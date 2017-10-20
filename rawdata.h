
/** @file
 *
 *  @brief Interfaces for interpreting raw packets from the Velodyne 3D LIDAR.
 *
 */

#ifndef _RAWDATA_H
#define _RAWDATA_H

	#include <stdint.h>

	#define PORT 9966
	#define PI 3.1415926535898 
 
	
	
	static const int    POINT_PER_CIRCLE_ =  2000;
	static const int    DATA_NUMBER_PER_SCAN = 40000 ; //Set 40000 to be large enough
	static const int    SIZE_BLOCK = 100;
	static const int    RAW_SCAN_SIZE = 3;
	static const int    SCANS_PER_BLOCK = 32;
	static const int    BLOCK_DATA_SIZE = (SCANS_PER_BLOCK * RAW_SCAN_SIZE); //96

	static const float  ROTATION_RESOLUTION = 0.01f; /**< degrees 旋转角分辨率*/
	static const uint16_t ROTATION_MAX_UNITS = 36000; /**< hundredths of degrees */

	static const float  DISTANCE_MAX = 150.0f;        /**< meters */
	static const float  DISTANCE_MIN = 0.2f;        /**< meters */
    	static const float  DISTANCE_RESOLUTION = 0.01f; /**< meters */
	static const float  DISTANCE_MAX_UNITS = (DISTANCE_MAX
		                                     / DISTANCE_RESOLUTION + 1.0);
	/** @todo make this work for both big and little-endian machines */
	static const uint16_t UPPER_BANK = 0xeeff; //
	static const uint16_t LOWER_BANK = 0xddff;


	/** Special Defines for RS16 support **/
	static const int    RS16_FIRINGS_PER_BLOCK =   2;
	static const int    RS16_SCANS_PER_FIRING  =  16;
	static const float  RS16_BLOCK_TDURATION   = 100.0f;   // [µs]
	static const float  RS16_DSR_TOFFSET       =   3.0f;   // [µs]
	static const float  RS16_FIRING_TOFFSET    =  50.0f;   // [µs]
	
	static const int PACKET_SIZE = 1248;
	static const int BLOCKS_PER_PACKET = 12;
	static const int PACKET_STATUS_SIZE = 4;
	static const int SCANS_PER_PACKET = (SCANS_PER_BLOCK * BLOCKS_PER_PACKET);
	static const int POINT_NUM = BLOCKS_PER_PACKET * RS16_FIRINGS_PER_BLOCK * RS16_SCANS_PER_FIRING;
	//unsigned char buf[PACKET_SIZE];

	// header
	typedef struct _raw_header
	{
		uint8_t header[8];
		uint8_t reserve_1[12];
		uint8_t timestamp[10];
		uint8_t reserve_2[12];
	} raw_header_t;

	// block
	typedef struct _raw_block
	{
		uint16_t header;        
		uint8_t rotation_1;
		uint8_t rotation_2;     
		uint8_t  data[BLOCK_DATA_SIZE]; //96
	} raw_block_t;
	
	// tail
	typedef struct _raw_tail
	{
		uint8_t reserve_3[4];
		uint8_t tail[2];
	}raw_tail_t;

	typedef struct _raw_packet
	{
		raw_header_t  header_id;
		raw_block_t   blocks[BLOCKS_PER_PACKET];
		raw_tail_t    tail_id;
	} raw_packet_t;
	
	union two_bytes
	{
		uint16_t uint;
		uint8_t  bytes[2];
	};
	
	typedef struct _point4
	{
		float x;
		float y;
		float z;
		uint8_t intensity;
	} point4_t;
	
	typedef struct _pack_sov
	{
		float azimuth[BLOCKS_PER_PACKET];
		float distance[BLOCKS_PER_PACKET][RS16_FIRINGS_PER_BLOCK][RS16_SCANS_PER_FIRING];
		float intensity[BLOCKS_PER_PACKET][RS16_FIRINGS_PER_BLOCK][RS16_SCANS_PER_FIRING];
	} pack_sov_t;
	
	typedef struct _unpack_point
	{
		
		float azimuth[BLOCKS_PER_PACKET];
		point4_t point[POINT_NUM];
	} unpack_point_t;
	

	extern float   VERT_ANGLE[16];
    	extern float   aIntensityCal[1600][16];
    	extern int     g_ChannelNum[16];

        /*load the cablibrated files: angle, distance, intensity*/
        void    loadConfigFile();

        /*unpack the UDP packet and opuput PointXYZI type*/
        unpack_point_t*  unpack(const raw_packet_t *raw);

        /*calibrated the disctance*/
        float   pixelToDistance(int pixelValue, int passageway);

        /*calibrated the intensity*/
        float   calibrateIntensity(float inten,int calIdx,int distance);

	//pack_point_t unpack_point;



#endif // __RAWDATA_H
