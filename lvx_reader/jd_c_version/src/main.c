#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define PI 3.14159265359

/**
 * Decode Livox LVX (1.x) files.
 * 
 * Specification:
 * https://www.livoxtech.com/3296f540ecf5458a8829e01cf429798e/downloads/Livox%20Viewer/LVX%20Specifications%20EN_20190924.pdf
 * 
 *
 * Top level file structure:
 *   * Public header block (fixed size 24? bytes)
 *   * Private header block (fixed size 5 bytes)
 *   * Device info block
 *   * Point data block (usually very long)
 * 
 * Private header:
 *   * uint32_t fame duration in ms (fixed at 50m)
 *   * uint8_t  number of devices
 *
 * Device info block: 
 *   * Sub-block of 59 bytes for each device
 * 
 * Point cloud data block:
 *   * Sequence of frames. Each frame comprises 'packages'. Each package comprises points.
 *
 *
 * @author Joe Desbonnet
 */

typedef struct __attribute__((__packed__)) {
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
typedef struct __attribute__((__packed__)) {
	uint32_t frame_duration;
	uint8_t device_count;
} lvx2_header_private_t;

/*
typedef struct __attribute__((__packed__)) {
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
*/

typedef struct __attribute__((__packed__)) {
	char lidar_sn[16];
	char hub_sn[16];
	uint8_t lidar_index;
	uint8_t device_type;
	uint8_t extrinsic_enable;
	float roll;
	float pitch;
	float yaw;
	float X;
	float Y;
	float Z;
} lvx2_device_info_t;

/*
typedef struct __attribute__((__packed__)) {
	uint64_t offset;
	uint64_t offset_next;
	uint64_t frame_index;
	uint64_t package_count;
} lvx1_pc_frame_header_t;
*/

typedef struct __attribute__((__packed__)) {
	uint64_t offset;
	uint64_t offset_next;
	uint64_t frame_index;
} lvx2_pc_frame_header_t;


typedef struct __attribute__((__packed__)) {
	uint8_t device_index;
	uint8_t version;
	uint8_t slot_id;
	uint8_t lidar_id;
	uint8_t reserved;
	uint32_t lidar_status;
	uint8_t timestamp_type;
	uint8_t data_type;
	uint64_t timestamp;
} lvx1_pc_package_header_t;

/*
typedef struct __attribute__((__packed__)) {
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
*/

typedef struct __attribute__((__packed__)) {
	float x;
	float y;
	float z;
	uint8_t reflectivity;
} lvx1_pc_point_t;

typedef struct __attribute__((__packed__)) {
	uint32_t x_mm;
	uint32_t y_mm;
	uint32_t z_mm;
	uint8_t reflectivity;
	uint8_t tag;
} lvx2_pc_point1_t;

typedef struct __attribute__((__packed__)) {
	uint16_t x_cm;
	uint16_t y_cm;
	uint16_t z_cm;
	uint8_t reflectivity;
	uint8_t tag;
} lvx2_pc_point2_t;


typedef struct __attribute__((__packed__)) {
	uint32_t word0;
	uint32_t word1;
	uint32_t word2;
	uint32_t word3;
} lvx_test_t;


int main (int argc, char **argv) {
	int i, len, c, prevc;
	int bytes_read = 0;

	lvx1_header_t header;
	fread (&header, sizeof(lvx1_header_t), 1,stdin);
	fprintf (stdout, "public header: signature=%s magic=%x a=%x b=%x c=%x d=%x\n", header.signature, header.magic, header.version_a, header.version_b, header.version_c, header.version_d);
	bytes_read += sizeof(lvx1_header_t);

	fprintf (stdout, "bytes_read=%d (0x%x)\n", bytes_read, bytes_read);


	lvx2_header_private_t header_private;
	fread (&header_private, sizeof(lvx2_header_private_t), 1,stdin);
	fprintf (stdout, "private header: frame_duration=%d device_count=%d\n", header_private.frame_duration, header_private.device_count);
	bytes_read += sizeof(lvx2_header_private_t);


	fprintf (stdout, "bytes_read=%d (0x%x)\n", bytes_read, bytes_read);


	lvx2_device_info_t devinfo;
	for (i = 0; i < header_private.device_count; i++) {
		fread (&devinfo, sizeof(lvx2_device_info_t), 1,stdin);
		fprintf (stdout, "Device[%d] SN=%s lidar_id=%x device_type=%x extrinsic_enable=%d roll=%f pitch=%f yaw=%f x=%f y=%f z=%f\n", i, q
			devinfo.lidar_sn, devinfo.lidar_index, devinfo.device_type, devinfo.extrinsic_enable,
			devinfo.roll, devinfo.pitch, devinfo.yaw, devinfo.X, devinfo.Y, devinfo.Z
		);
		bytes_read += sizeof(lvx2_device_info_t);
	}

	fprintf (stdout, "bytes_read=%d (0x%x)\n", bytes_read, bytes_read);

	// All good up to here.

	//
	// Read point data - this is a sequence of frames. Each 'frame' comprises a sequence of 'packages'.
	//

	lvx2_pc_frame_header_t frame_header;
	lvx1_pc_package_header_t package_header;

	while ( ! feof (stdin) ) {

		fread (&frame_header, sizeof(frame_header), 1,stdin);
		bytes_read += sizeof(frame_header);
		fprintf (stdout, "bytes_read=%d (0x%x)\n", bytes_read, bytes_read);

		fprintf (stdout, "Frame header: file_offset=%lu frame_length=%ld next_frame=%ld\n", frame_header.offset, (frame_header.offset_next - frame_header.offset), frame_header.offset_next );


		for (i = 0; i < 16; i++) {
			fread (&package_header, sizeof(package_header), 1,stdin);
			fprintf (stdout, "Package header:    version=%d timestamp=%ld data_type=%d\n", package_header.version, package_header.timestamp, package_header.data_type);
			//int numPoints = package
		}


	}
}
