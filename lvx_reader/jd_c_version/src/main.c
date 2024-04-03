#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define PI 3.14159265359

/**
 * Decode Livox LVX (1.1) files.
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

#include "lvx.h"

int main (int argc, char **argv) {
	int i, len, c, prevc;
	uint64_t bytes_read = 0;
	long time_offset = 0;

	lvx_header_t header;
	fread (&header, sizeof(header), 1,stdin);


	// 
	// Check if it looks like a valid LVX 1.1 file
	//

	if (  ! (header.signature[0]=='l' && header.signature[1]=='i' && header.signature[2]=='v') ) {
		fprintf (stderr,"ERROR: unrecognized file signature version %s, expecting 'livox_tech' \n",header.signature);
		return -1;
	}

	if (header.format_version != 0x0101) {
		fprintf (stderr,"ERROR: unrecognized file format version %08x\n",header.format_version);
		return -1;
	}

	if (header.magic != 0xAC0EA767) {
		fprintf (stderr,"ERROR: unrecognized file magic number %08x, expecting 0xAC0EA767\n",header.magic);
		return -1;
	}
 


	fprintf (stdout, "public header: signature=%s magic=%x format_version=%08x\n", header.signature, header.magic, header.format_version);
	bytes_read += sizeof(header);

	fprintf (stdout, "bytes_read=%ld (0x%lx)\n", bytes_read, bytes_read);


	lvx_header_private_t header_private;
	fread (&header_private, sizeof(header_private), 1,stdin);
	fprintf (stdout, "private header: frame_duration=%d device_count=%d\n", header_private.frame_duration, header_private.device_count);
	bytes_read += sizeof(header_private);


	fprintf (stdout, "bytes_read=%ld (0x%lx)\n", bytes_read, bytes_read);


	lvx_device_info_t devinfo;
	for (i = 0; i < header_private.device_count; i++) {
		fread (&devinfo, sizeof(devinfo), 1,stdin);
		fprintf (stdout, "Device[%d] SN=%s lidar_id=%x device_type=%x extrinsic_enable=%d roll=%f pitch=%f yaw=%f x=%f y=%f z=%f\n", i, 
			devinfo.lidar_sn, devinfo.lidar_index, devinfo.device_type, devinfo.extrinsic_enable,
			devinfo.roll, devinfo.pitch, devinfo.yaw, devinfo.X, devinfo.Y, devinfo.Z
		);
		bytes_read += sizeof(devinfo);
	}

	fprintf (stdout, "bytes_read=%ld (0x%lx)\n", bytes_read, bytes_read);


	//
	// Read point data - this is a sequence of frames. Each 'frame' comprises a sequence of 'packages'.
	//

	lvx_frame_header_t   frame_header;
	lvx_package_header_t package_header;

	while ( ! feof (stdin) ) {

		fread (&frame_header, sizeof(frame_header), 1,stdin);
		fprintf (stdout, "Frame: file_offset=%lu frame_length=%ld next_frame=%ld time_offset_ms=%ld\n", frame_header.offset, (frame_header.offset_next - frame_header.offset), frame_header.offset_next, time_offset );

		bytes_read += sizeof(frame_header);
		fprintf (stdout, "bytes_read=%ld (0x%lx)\n", bytes_read, bytes_read);

		do {
			fread (&package_header, sizeof(package_header), 1,stdin);
			bytes_read += sizeof(package_header);
			//fprintf (stdout, "Package:    version=%d timestamp=%ld data_type=%d\n", package_header.version, package_header.timestamp, package_header.data_type);

			if (package_header.version != 5) {
				fprintf (stderr, "ERROR: unrecognized package version %d, expected 5\n", package_header.version);
				return -1;
			}

			int points_per_package;
			switch (package_header.data_type) {
				case 0:
					fprintf (stderr,"ERROR: can't handle data type 0\n");
					return -1;
				case 1:
					points_per_package = 100;
					fprintf (stderr,"ERROR: can't handle data type 1\n");
					return -1;
					//break;

				case 2: {
					points_per_package = 96;
					lvx_point2_t point;
					for (int i = 0; i < points_per_package; i++) {
						fread (&point, sizeof(point), 1,stdin);
						bytes_read += sizeof(point);
						//fprintf (stdout,"%d %d %d %d %d\n", point.x_mm, point.y_mm, point.z_mm, point.reflectivity, point.tag);
					}
					break;
				}
				case 3:
					fprintf (stderr,"ERROR: can't handle data type 2\n");
					return -1;
				case 4:
					fprintf (stderr,"ERROR: can't handle data type 0\n");
					return -1;
				case 5:
					points_per_package = 48;
					fprintf (stderr,"ERROR: can't handle data type 5\n");
					return -1;
					//break;
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
				default: {
					fprintf (stderr,"ERROR: can't handle data type 0\n");
					return -1;
				}
			}

			//fprintf (stdout, "bytes_read=%d (0x%x)\n", bytes_read, bytes_read);

			
		} while (bytes_read < frame_header.offset_next);

		if (bytes_read != frame_header.offset_next) {
			fprintf (stdout,"WARNING: anomaly, frame_header.offset_next not consistent with bytes read: expecting %lu from frame header but %lu bytes read, diff %ld\n", 
				frame_header.offset_next, bytes_read, frame_header.offset_next - bytes_read);
		}

		time_offset += header_private.frame_duration;
	}
}
