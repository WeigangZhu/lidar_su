#include"lidar.h"
#include"grid.h"

point_grid_t *grid_cell[GRID_MAX][GRID_MAX];

FILE *fp1 = fopen("../../matlab/lidar_source001.txt","w");
FILE *fp2 = fopen("../../matlab/lidar_intensity001.txt","w");
//FILE *fp3 = fopen("../matlab/lidar_filter001.txt","w");

int main()
{
        
	
	struct sockaddr_in my_addr;
	socklen_t addrlen;
    	addrlen = sizeof(my_addr);
	int sockfd, num;
    	unsigned char buf[PACKET_SIZE];
	
	sockfd = open_net();
    	
	// load configure file
    	loadConfigFile();
	
	
	//grid_cell_memory(grid_cell);
	//grid_cell_init( grid_cell);
	
	int i=0, test_num = 100;

	const raw_packet_t *raw = (const raw_packet_t*) &buf;
	unpack_point_t*  data_unpack;
    	while (1) 
	{
		printf("Received packet from %s:%d\n", inet_ntoa(my_addr.sin_addr), ntohs(my_addr.sin_port)); 
		printf("sockfd = %d\n",sockfd);

		num =recvfrom(sockfd, buf, PACKET_SIZE, 0,
		     (struct sockaddr *) &my_addr, &addrlen);
		buf[num] = '\0';
		printf("%s\n\n",buf);
		//printf("num = %d\n",num);
		//err_recvfrom(num);
		
		//data_unpack = unpack(raw);
		//test_output( data_unpack,fp1,fp2 );

		
		//grid_on( data_unpack, grid_cell );
		
		/*if( i > test_num)
		{	
			//grid_filter_by_percent( grid_cell);
			grid_filter_by_distance( grid_cell );
			grid_cell_output(grid_cell,fp3);
			i = 0;
		 	//break;
		}
		
		i++;*/
    	}

	
	//grid_filter_by_distance( grid_cell );
	//grid_filter_by_percent( grid_cell);
	//grid_cell_output(grid_cell);
	grid_cell_memory_free(grid_cell);

	fclose(fp1);
	fclose(fp2);
	close(sockfd);
}











