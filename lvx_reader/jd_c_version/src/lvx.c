#include <stdio.h>
#include <stdint.h>

#include "lvx.h"

int lvx_check_file_header (lvx_header_t *header) {


	if (  ! (header->signature[0]=='l' && header->signature[1]=='i' && header->signature[2]=='v') ) {
		fprintf (stderr,"ERROR: unrecognized file signature version %s, expecting 'livox_tech' \n",header->signature);
		return -1;
	}

	if (header->format_version != 0x0101) {
		fprintf (stderr,"ERROR: unrecognized file format version %08x\n",header->format_version);
		return -1;
	}

	if (header->magic != 0xAC0EA767) {
		fprintf (stderr,"ERROR: unrecognized file magic number %08x, expecting 0xAC0EA767\n",header->magic);
		return -1;
	}

	return 0;

}

