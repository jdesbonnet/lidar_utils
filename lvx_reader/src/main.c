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

typedef struct {
	char signature[16];
	char version_a;
	char version_b;
	char version_c;
	char version_d;
	uint32_t magic;
} lvx1_header_t;

/**
 * Unclear why this is called 'private'
 */
typedef struct {
	uint32_t frame_duration;
	uint8_t device_count;
} lvx1_header_private_t;

typedef struct {
	char lidar_sn[16];
	char hub_sn[16];
	char device_index;
	char device_type;
	float extrinsic_roll;
	float extrinsic_pitch;
	float extrinsic_yaw;
	float extrinsic_X;
	float extrinsic_Y;
	float extrinsic_Z;
} lvx1_device_info_t;

typedef struct {
	char lidar_sn[16];
	char hub_sn[16];
	uint32_t lidar_id;
	uint8_t lidar_type;
	uint8_t device_type;
	uint8_t extrinsic_enable;
	float extrinsic_roll;
	float extrinsic_pitch;
	float extrinsic_yaw;
	float extrinsic_X;
	float extrinsic_Y;
	float extrinsic_Z;
} lvx2_device_info_t;


typedef struct {
	uint64_t offset;
	uint64_t offset_next;
	uint64_t frame_index;
	uint64_t package_count;
} lvx1_pc_frame_header_t;

typedef struct {
	uint64_t offset;
	uint64_t offset_next;
	uint64_t frame_index;
} lvx2_pc_frame_header_t;


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

typedef struct {
	float x;
	float y;
	float z;
	uint8_t reflectivity;
} lvx1_pc_point_t;

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


int main (int argc, char **argv) {
	int i, len, c, prevc;

	lvx1_header_t header;
	fread (&header, sizeof(lvx1_header_t), 1,stdin);
	fprintf (stdout, "signature=%s magic=%x a=%x b=%x c=%x d=%x\n", header.signature, header.magic, header.version_a, header.version_b, header.version_c, header.version_d);

	lvx1_header_private_t header_private;
	fread (&header_private, sizeof(lvx1_header_private_t), 1,stdin);
	fprintf (stdout, "frame_duration=%d device_count=%d\n", header_private.frame_duration, header_private.device_count);


	lvx2_device_info_t devinfo;
	for (i = 0; i < header_private.device_count; i++) {
		fread (&devinfo, sizeof(lvx2_device_info_t), 1,stdin);
		fprintf (stdout, "Device[%d] SN=%s lidar_id=%x device_type=%x extrinsic_enable=%d\n", i, devinfo.lidar_sn, devinfo.lidar_id, devinfo.device_type, devinfo.extrinsic_enable);
	}

	// Read point data
	lvx2_pc_frame_header_t frame_header;

	while ( ! feof (stdin) ) {
		fread (&frame_header, sizeof(lvx2_pc_frame_header_t), 1,stdin);
		fprintf (stdout, "frame_index=%ld offset=%ld\n", frame_header.frame_index, frame_header.offset);

	}
}
