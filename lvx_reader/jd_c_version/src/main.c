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
} lvx_header_t;

/**
 * Unclear why this is called 'private'
 */
typedef struct __attribute__((__packed__)) {
	uint32_t frame_duration;
	uint8_t device_count;
} lvx_header_private_t;


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
} lvx_device_info_t;

typedef struct __attribute__((__packed__)) {
	uint64_t offset;
	uint64_t offset_next;
	uint64_t frame_index;
} lvx_frame_header_t;


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
} lvx_package_header_t;


typedef struct __attribute__((__packed__)) {
	uint32_t x_mm;
	uint32_t y_mm;
	uint32_t z_mm;
	uint8_t reflectivity;
	uint8_t tag;
} lvx_point2_t;

typedef struct __attribute__((__packed__)) {
	float gyro_x;
	float gyro_y;
	float gyro_z;
	float acc_x;
	float acc_y;
	float acc_z;
} lvx_point6_t;



int main (int argc, char **argv) {
	int i, len, c, prevc;
	int bytes_read = 0;

	lvx_header_t header;
	fread (&header, sizeof(header), 1,stdin);
	fprintf (stdout, "public header: signature=%s magic=%x a=%x b=%x c=%x d=%x\n", header.signature, header.magic, header.version_a, header.version_b, header.version_c, header.version_d);
	bytes_read += sizeof(header);

	fprintf (stdout, "bytes_read=%d (0x%x)\n", bytes_read, bytes_read);


	lvx_header_private_t header_private;
	fread (&header_private, sizeof(header_private), 1,stdin);
	fprintf (stdout, "private header: frame_duration=%d device_count=%d\n", header_private.frame_duration, header_private.device_count);
	bytes_read += sizeof(header_private);


	fprintf (stdout, "bytes_read=%d (0x%x)\n", bytes_read, bytes_read);


	lvx_device_info_t devinfo;
	for (i = 0; i < header_private.device_count; i++) {
		fread (&devinfo, sizeof(devinfo), 1,stdin);
		fprintf (stdout, "Device[%d] SN=%s lidar_id=%x device_type=%x extrinsic_enable=%d roll=%f pitch=%f yaw=%f x=%f y=%f z=%f\n", i, 
			devinfo.lidar_sn, devinfo.lidar_index, devinfo.device_type, devinfo.extrinsic_enable,
			devinfo.roll, devinfo.pitch, devinfo.yaw, devinfo.X, devinfo.Y, devinfo.Z
		);
		bytes_read += sizeof(devinfo);
	}

	fprintf (stdout, "bytes_read=%d (0x%x)\n", bytes_read, bytes_read);

	// All good up to here.

	//
	// Read point data - this is a sequence of frames. Each 'frame' comprises a sequence of 'packages'.
	//

	lvx_frame_header_t   frame_header;
	lvx_package_header_t package_header;

	while ( ! feof (stdin) ) {

		fread (&frame_header, sizeof(frame_header), 1,stdin);
		bytes_read += sizeof(frame_header);
		fprintf (stdout, "bytes_read=%d (0x%x)\n", bytes_read, bytes_read);

		fprintf (stdout, "Frame header: file_offset=%lu frame_length=%ld next_frame=%ld\n", frame_header.offset, (frame_header.offset_next - frame_header.offset), frame_header.offset_next );


		do {
			fread (&package_header, sizeof(package_header), 1,stdin);
			bytes_read += sizeof(package_header);
			fprintf (stdout, "Package header:    version=%d timestamp=%ld data_type=%d\n", package_header.version, package_header.timestamp, package_header.data_type);
			int points_per_package;
			switch (package_header.data_type) {
				case 0:
				case 1:
					points_per_package = 100;
					break;
				case 2:
				case 3: {
					points_per_package = 96;
					lvx_point2_t point;
					for (int i = 0; i < points_per_package; i++) {
						fread (&point, sizeof(point), 1,stdin);
						bytes_read += sizeof(point);
						//fprintf (stdout,"%d %d %d %d %d\n", point.x_mm, point.y_mm, point.z_mm, point.reflectivity, point.tag);
					}
					break;
				}
				case 4:
				case 5:
					points_per_package = 48;
					break;
				case 6: {
					points_per_package = 1;
					lvx_point6_t point;
					for (int i = 0; i < points_per_package; i++) {
						fread (&point, sizeof(point), 1,stdin);
						bytes_read += sizeof(point);
						//fprintf (stdout,"%d %d %d %d %d\n", point.x_mm, point.y_mm, point.z_mm, point.reflectivity, point.tag);
					}
					break;
				}
			}

			fprintf (stdout, "bytes_read=%d (0x%x)\n", bytes_read, bytes_read);
		} while (bytes_read < frame_header.offset_next);


	}
}
