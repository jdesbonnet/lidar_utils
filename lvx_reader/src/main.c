#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define PI 3.14159265359

/**
 * Decode Livox LVX (1.x) files.
 * 
 * 
 * @author Joe Desbonnet
 */

/**
 * LVX file header. Same accross all known versions.
 */
typedef struct {
	char signature[16];
	char version_a;
	char version_b;
	char version_c;
	char version_d;
	uint32_t magic;
} lvx_header_t;

/**
 * Unclear why this is called 'private'
 */
typedef struct {
	uint32_t frame_duration;
	uint8_t device_count;
} lvx2_header_private_t;

/*
typedef struct {
	char lidar_sn[16];
	char hub_sn[16];
	uint8_t device_index;
	uint8_t device_type;
	float extrinsic_roll;
	float extrinsic_pitch;
	float extrinsic_yaw;
	float extrinsic_X;
	float extrinsic_Y;
	float extrinsic_Z;
} lvx1_device_info_t;
*/

typedef struct {
	char lidar_sn[16];
	char hub_sn[16];
	//uint32_t lidar_id; // is this right?
	uint8_t lidar_type;
	uint8_t device_type;
	uint8_t extrinsic_enable;
	float extrinsic_roll;
	float extrinsic_pitch;
	float extrinsic_yaw;
	float extrinsic_X;
	float extrinsic_Y;
	//float extrinsic_Z;
} lvx2_device_info_t;


/*
typedef struct {
	uint64_t offset;
	uint64_t offset_next;
	uint64_t frame_index;
	uint64_t package_count;
} lvx1_pc_frame_header_t;
*/

typedef struct {
	uint64_t offset;
	uint64_t offset_next;
	uint64_t frame_index;
} lvx2_pc_frame_header_t;


/*
typedef struct {
	uint8_t device_index;
	uint8_t package_protocol_version;
	uint8_t slot_id;
	uint8_t lidar_id;
	uint8_t reserved;
	uint32_t lidar_status;
	uint8_t timestamp_type;
	uint8_t coordinate_system;
	char timestamp[8];
} lvx1_pc_package_header_t;
*/

typedef struct {
	uint8_t version;
	uint32_t lidar_id;
	uint8_t lidar_type;
	uint8_t timestamp_type;
	uint8_t timestamp[8];
	uint16_t udp_counter;
	uint8_t data_type;
	uint32_t length;
	uint8_t frame_counter;
	uint8_t reserved[4];
} lvx2_pc_package_header_t;

/*
typedef struct {
	float x;
	float y;
	float z;
	uint8_t reflectivity;
} lvx1_pc_point_t;
*/

typedef struct {
	uint32_t x_mm;
	uint32_t y_mm;
	uint32_t z_mm;
	uint8_t reflectivity;
	uint8_t tag;
} lvx2_pc_point1_t;

typedef struct {
	uint16_t x_cm;
	uint16_t y_cm;
	uint16_t z_cm;
	uint8_t reflectivity;
	uint8_t tag;
} lvx2_pc_point2_t;

typedef struct {
	uint32_t word0;
	uint32_t word1;
	uint32_t word2;
	uint32_t word3;
} lvx_test_t;


int main (int argc, char **argv) {
	int i, len, c, prevc;
	int bytes_read = 0;

	// This is ok
	lvx_header_t header;
	fread (&header, sizeof(lvx_header_t), 1,stdin);
	fprintf (stdout, "signature=%s magic=%x a=%x b=%x c=%x d=%x\n", header.signature, 
		header.magic, 
		header.version_a, 
		header.version_b, 
		header.version_c, 
		header.version_d);
	bytes_read += sizeof(lvx_header_t);

	// This seems to be ok
	lvx2_header_private_t header_private;
	fread (&header_private, sizeof(lvx2_header_private_t), 1,stdin);
	fprintf (stdout, "frame_duration=%d device_count=%d\n", header_private.frame_duration, header_private.device_count);
	bytes_read += sizeof(lvx2_header_private_t);

	// Reading too much?
	lvx2_device_info_t devinfo;
	for (i = 0; i < header_private.device_count; i++) {
		fread (&devinfo, sizeof(lvx2_device_info_t), 1,stdin);
		fprintf (stdout, "Device[%d] SN=%s device_type=%x extrinsic_enable=%d\n", 
			i, 
			devinfo.lidar_sn, 
			devinfo.device_type, 
			devinfo.extrinsic_enable
		);
		bytes_read += sizeof(lvx2_device_info_t);
	}

	fprintf (stdout, "bytes_read=%d (0x%x)\n", bytes_read, bytes_read);

	// At this point we seem to have read 4 bytes more than we should have.
	// Which points to lvx2_device_into_t being 4 bytes longer than it
	// should be (?!).

	//
	// Read point data - this is a sequence of frames. Each 'frame' comprises a sequence of 'packages'.
	//

	//lvx_test_t test;
	//fread (&test, sizeof(lvx_test_t), 1,stdin);
	//fprintf (stdout, "word0=%x word1=%x, word=%x, word3=%x\n", test.word0, test.word1, test.word2, test.word3 );

	//lvx1_pc_frame_header_t frame_header1;
	lvx2_pc_frame_header_t frame_header2;
	lvx2_pc_package_header_t package_header;

	while ( ! feof (stdin) ) {

		//fread (&frame_header1, sizeof(lvx1_pc_frame_header_t), 1,stdin);
		//fprintf (stdout, "offset=%lu length=%ld\n", frame_header1.offset, (frame_header1.offset_next - frame_header1.offset) );

		fread (&frame_header2, sizeof(lvx2_pc_frame_header_t), 1,stdin);
		fprintf (stdout, "frame_index=%ld offset=%ld length=%ld\n", 
			frame_header2.frame_index, 
			frame_header2.offset, 
			(frame_header2.offset_next - frame_header2.offset) 
		);

		for (i = 0; i < 16; i++) {
			fread (&package_header, sizeof(lvx2_pc_package_header_t), 1,stdin);
			fprintf (stdout, "    version=%d udp_counter=%d timestamp=%s data_type=%d\n", package_header.version, package_header.udp_counter, package_header.timestamp, package_header.data_type);
			//int numPoints = package
		}

	}
}
