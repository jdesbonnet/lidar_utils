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
	uint64_t time_offset = 0;

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


	int chunk_number = 0;
	char filenamebuf[80];
	uint64_t bytes_written = 0;
	uint64_t chunk_time_offset = 0;

	sprintf (filenamebuf, "part-%08d.lvx", chunk_number);

	FILE *chunkf;
	chunkf = fopen(filenamebuf, "wb");

	// Works
	fwrite (&header, sizeof(header), 1, chunkf);
	bytes_written += sizeof(header);

	// Works
	fwrite (&header_private, sizeof(header_private), 1, chunkf);
	bytes_written += sizeof(header_private);

	// Works
	for (i = 0; i < header_private.device_count; i++) {
		fwrite (&devinfo, sizeof(lvx_device_info_t), 1, chunkf);
		bytes_written += sizeof(lvx_device_info_t);
	}




	//
	// Read point data - this is a sequence of frames. Each 'frame' comprises a sequence of 'packages'.
	//

	const uint64_t max_frame_size = 4000000;
	uint8_t frame_buffer[max_frame_size];
	lvx_frame_header_t   frame_header;
	lvx_package_header_t package_header;

	uint64_t new_offset;
	uint64_t new_offset_next;
	uint64_t new_frame_index;

	while ( ! feof (stdin) ) {

		fread (&frame_header, sizeof(frame_header), 1,stdin);

		if (feof(stdin)) {
			fprintf (stderr,"ERROR: feof after frame header read\n");
			break;
		}

		uint64_t frame_size = (frame_header.offset_next - frame_header.offset);

		fprintf (stdout, "Frame: file_offset=%lu frame_length=%ld next_frame=%ld time_offset_ms=%ld\n", frame_header.offset, (frame_header.offset_next - frame_header.offset), frame_header.offset_next, time_offset );

		if (frame_size > max_frame_size) {
			fprintf (stderr,"ERROR: max frame size (%ld bytes) exceeded\n",max_frame_size);
			return -1;
		}

		bytes_read += sizeof(frame_header);
		fprintf (stdout, "bytes_read=%ld (0x%lx)\n", bytes_read, bytes_read);


		// Writing these properties corrupts the frame_header: why?!
		//frame_header.offset = bytes_written;
		//frame_header.offset_next = bytes_written + frame_size;
		//fwrite (&frame_header, sizeof(frame_header), 1, chunkf);

		// Let's try something different
		new_offset = bytes_written;
		new_offset_next = new_offset + frame_size;
		new_frame_index = frame_header.frame_index;

		fprintf (stdout, "new_offset=%ld new_offset_next=%ld\n", new_offset, new_offset_next);

		fwrite (&new_offset, sizeof(uint64_t), 1, chunkf);
		fwrite (&new_offset_next, sizeof(uint64_t), 1, chunkf);
		fwrite (&new_frame_index, sizeof(uint64_t), 1, chunkf);
		bytes_written += sizeof(uint64_t)*3;


		// Read the remaining content of frame and write back verbatim
		fread  (&frame_buffer, frame_size - sizeof(frame_header), 1, stdin);
		fwrite (&frame_buffer, frame_size - sizeof(frame_header), 1, chunkf);

		bytes_read    += (frame_size - sizeof(frame_header));
		bytes_written += (frame_size - sizeof(frame_header));
		fprintf (stdout, "bytes_read=%ld (0x%lx)\n", bytes_read, bytes_read);

		if (bytes_read != frame_header.offset_next) {
			fprintf (stdout,"WARNING: anomaly, frame_header.offset_next not consistent with bytes read: expecting %lu from frame header but got %lu bytes read, diff %ld\n", 
				frame_header.offset_next, bytes_read, frame_header.offset_next - bytes_read);
			return -1;
		}
		if (bytes_written != frame_header.offset_next) {
			fprintf (stdout,"WARNING: anomaly, frame_header.offset_next not consistent with bytes written: expecting %lu from frame header but got %lu bytes written, diff %ld\n", 
				frame_header.offset_next, bytes_written, frame_header.offset_next - bytes_written);
			return -1;
		}


		time_offset       += header_private.frame_duration;
		chunk_time_offset += header_private.frame_duration;

		if (chunk_time_offset >= 300000) {
			fclose (chunkf);
			bytes_written = 0;
			chunk_time_offset = 0;
			chunk_number++;
			sprintf (filenamebuf, "part-%08d.lvx", chunk_number);
			chunkf = fopen(filenamebuf, "wb");
			fwrite (&header, sizeof(header), 1, chunkf);
			bytes_written += sizeof(header);

			fwrite (&header_private, sizeof(header_private), 1, chunkf);
			bytes_written += sizeof(header_private);

			for (i = 0; i < header_private.device_count; i++) {
				fwrite (&devinfo, sizeof(lvx_device_info_t), 1, chunkf);
				bytes_written += sizeof(lvx_device_info_t);
			}
		}


	}
}
