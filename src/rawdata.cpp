
#include "rawdata.h"
#include <math.h>


float   VERT_ANGLE[16];
float   a_intensity_cal[1600][16];
int     g_ChannelNum[16];
unpack_point_t unpack_point;

// 读参数文件 
void load_config_file() 
{

	FILE *f_inten = fopen("../conf/curves.conf", "r");
	int loopi = 0;
	int loopj = 0;

	if (!f_inten) 
	{
		printf("../conf/curves.conf does not exist\n");
	} 
	else 
	{
		while (~feof(f_inten)) 
		{
			float a[16];
			loopi++;
			if (loopi > 1600)
				break;
			fscanf(f_inten,
				       "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n",
				       &a[0], &a[1], &a[2], &a[3], &a[4],
				       &a[5], &a[6], &a[7], &a[8], &a[9],
				       &a[10], &a[11], &a[12], &a[13],
				       &a[14], &a[15]);

			for (loopj = 0; loopj < 16; loopj++) 
			{
				 a_intensity_cal[loopi - 1][loopj] = a[loopj];
			}
		}
		fclose(f_inten);
	}

//=============================================================
	FILE *f_angle = fopen("../conf/angle.conf", "r");
	if (!f_angle) 
	{
		printf( "../conf/angle.conf does not exist\n");

	} 
	else 
	{
		float b[16];
		int loopk = 0;
		int loopn = 0;
		while (~feof(f_angle)) 
		{
			fscanf(f_angle, "%f", &b[loopk]);
			loopk++;
			if (loopk > 15)
				break;
		}
			
		for (loopn = 0; loopn < 16; loopn++) 
		{
			VERT_ANGLE[loopn] = b[loopn] / 180 * PI;
		}
		fclose(f_angle);
	}

//=============================================================
	FILE *f_channel = fopen("../conf/channel.conf", "r");
	if (!f_channel) 
	{
		printf("../conf/channel.conf does not exist");
	} 
	else 
	{
		int loopl = 0;

		while (~feof(f_channel)) 
		{
			fscanf(f_channel, "%d", &g_ChannelNum[loopl]);
			loopl++;
			if (loopl > 15)
				break;
		}
		fclose(f_channel);
	}
}

//校准距离  
float pixel_to_distance(int pixelValue, int passageway) 
{
	float DistanceValue;

	if (pixelValue <= g_ChannelNum[passageway]) 
	{
		DistanceValue = 0.0;
	} 
	else 
	{
		DistanceValue = (float) (pixelValue - g_ChannelNum[passageway]);
	}

	return DistanceValue;
}

//校准反射强度值
float calibrate_intensity(float intensity, int calIdx, int distance) 
{
	int algDist;
	int sDist;
	int uplimitDist;
	float realPwr;
	float refPwr;
	float tempInten;

	uplimitDist = g_ChannelNum[calIdx] + 1400;
	realPwr = intensity;

	if ((int) realPwr < 126)
		realPwr = realPwr * 4.0;
	else if ((int) realPwr >= 126 && (int) realPwr < 226)
		realPwr = (realPwr - 125.0) * 16.0 + 500.0;
	else
		realPwr = (realPwr - 225.0) * 16.0 + 2100.0;

//-------------------------------------------------------------------------------------------------
//limit sDist belongs to [200,1600] in unit cm
	sDist =(distance > g_ChannelNum[calIdx]) ? distance : g_ChannelNum[calIdx];

	sDist = (sDist < uplimitDist) ? sDist : uplimitDist;
	//minus the static offset (this data is For the intensity cal useage only)
	algDist = sDist - g_ChannelNum[calIdx];
	//algDist = algDist < 1400? algDist : 1399;
	refPwr = a_intensity_cal[algDist][calIdx];
	tempInten = refPwr / realPwr;
	tempInten = tempInten * 200.0;
	tempInten = (int) tempInten > 255 ? 255.0 : tempInten;
	return tempInten;
//------------------------------------------------------------
}

 
unpack_point_t* unpack(const raw_packet_t *raw) 
{
	float azimuth;	//0.01 dgree
	float intensity;
	float azimuth_diff;
	float last_azimuth_diff;
	float azimuth_corrected_f;
	int azimuth_corrected;
		

	for (int block = 0,point_pos = 0; block < BLOCKS_PER_PACKET; block++)	//1 packet:12 data blocks
	{

		/*if (UPPER_BANK != raw->blocks[block].header) 
		{
			std::cout <<  "skipping invalid RSLIDAR packet: block "
					  << block << " header value is "
					  << raw-> blocks[block].header
					  << std::endl;

			break;
		}*/

		azimuth =(float) (256 * raw->blocks[block].rotation_1 +
				     raw->blocks[block].rotation_2);

		//pack_udp.azimuth[block] = azimuth * ROTATION_RESOLUTION;
		unpack_point.azimuth[block] = azimuth * ROTATION_RESOLUTION *PI / 180;

		if (block < (BLOCKS_PER_PACKET - 1))	//12
		{
			int azi1, azi2;
			azi1 = 256 * raw->blocks[block + 1].rotation_1 + raw->blocks[block + 1].rotation_2;
			azi2 = 256 * raw->blocks[block].rotation_1 + raw->blocks[block].rotation_2;
			azimuth_diff = (float) ((36000 + azi1 - azi2) % 36000);
				
			/*if (azimuth_diff <= 0.0 || azimuth_diff > 70.0) 
			{
				printf("Error: %d  %d\n", azi2, azi1);
				azimuth_diff = 40.0;
			}*/
				
			last_azimuth_diff = azimuth_diff;
		} 
		else 
		{
			azimuth_diff = last_azimuth_diff;
		}

		for (int firing = 0, k = 0; firing < RS16_FIRINGS_PER_BLOCK; firing++)	//2
		{
			for (int dsr = 0; dsr < RS16_SCANS_PER_FIRING; dsr++, k += RAW_SCAN_SIZE)	//16   3
			{
				azimuth_corrected_f = azimuth + (azimuth_diff * ((dsr * RS16_DSR_TOFFSET) +
			 						(firing * RS16_FIRING_TOFFSET)) / RS16_BLOCK_TDURATION);
				azimuth_corrected = ((int) round(azimuth_corrected_f)) % 36000;	//convert to integral value...
					

				union two_bytes tmp;
				tmp.bytes[1] = raw->blocks[block].data[k];
				tmp.bytes[0] = raw->blocks[block].data[k + 1];
				int distance = tmp.uint;

				// read intensity
				intensity = raw->blocks[block].data[k + 2];
				intensity = calibrate_intensity(intensity, dsr, distance);
				
				float dis = pixel_to_distance(distance, dsr);
				dis = dis * DISTANCE_RESOLUTION;
				

				float arg_horiz = azimuth_corrected * ROTATION_RESOLUTION *PI / 180;
				float arg_vert = VERT_ANGLE[dsr];
				if (dis > DISTANCE_MAX/3 || dis < DISTANCE_MIN)	//invalid data
				{
					unpack_point.point[point_pos].x = 0;
					unpack_point.point[point_pos].y = 0;
					unpack_point.point[point_pos].z = 0;
					unpack_point.point[point_pos].intensity = 0;
					point_pos++;

				} 
				else 
				{
					unpack_point.point[point_pos].x = dis * cos(arg_vert) * sin(arg_horiz);
					unpack_point.point[point_pos].y = dis * cos(arg_vert) * cos(arg_horiz);
					unpack_point.point[point_pos].z = dis * sin(arg_vert);
					unpack_point.point[point_pos].intensity = intensity;
					point_pos++;
							
							
				}

			}
		}
	}
	return &unpack_point;
}

