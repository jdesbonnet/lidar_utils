
/**
 * For gcc compiler it's important to add __attribute__((__packed__)) to ensure there
 * is no padding. 
 */


/**
 * File header
 */
typedef struct __attribute__((__packed__)) {
	char signature[16];
	uint32_t format_version;
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


