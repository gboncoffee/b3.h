#include <stdio.h>
#include <stdint.h>
#define B3_IMPLEMENTATION
#include "b3.h"

static uint8_t example_packet[160] = {
	0x37, 0x00, 0x01, 0x00, 0xb1, 0x68, 0xde, 0x3a,
	0x00, 0xc8, 0x98, 0x65, 0xf4, 0xac, 0xeb, 0x15,
	0x4c, 0x00, 0x50, 0xeb, 0x40, 0x00, 0x32, 0x00,
	0x02, 0x00, 0x05, 0x00, 0xa4, 0x92, 0x78, 0x48,
	0x17, 0x00, 0x00, 0x00, 0x80, 0x01, 0x31, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x61, 0xbc, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x78, 0xe0, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xc8, 0x98, 0x65,
	0xf4, 0xac, 0xeb, 0x15, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00,
	0x39, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x44, 0x00, 0x50, 0xeb,
	0x38, 0x00, 0x35, 0x00, 0x02, 0x00, 0x05, 0x00,
	0xa4, 0x92, 0x78, 0x48, 0x17, 0x00, 0x00, 0x00,
	0x80, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x61, 0xbc, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xd0, 0x07, 0x00, 0x00, 0x81, 0xe3, 0x01, 0x00,
	0x39, 0x30, 0x00, 0x00, 0x74, 0x4a, 0x45, 0x00,
	0x00, 0xc8, 0x98, 0x65, 0xf4, 0xac, 0xeb, 0x15,
};

int main(void)
{
	int8_t n_msgs = b3_packet_number_of_msg(example_packet, 160);
	if (n_msgs != 2) {
		fprintf(stderr, "Wrong number of messages with size 160: %d\n", n_msgs);
		return 1;
	}

	n_msgs = b3_packet_number_of_msg(example_packet, 15);
	if (n_msgs != -1) {
		fprintf(stderr, "Wrong number of messages with size 15: %d\n", n_msgs);
	}

	n_msgs = b3_packet_number_of_msg(example_packet, 130);
	if (n_msgs != -1) {
		fprintf(stderr, "Wrong number of messages with size 130: %d\n", n_msgs);
	}

	n_msgs = b3_packet_number_of_msg(example_packet, 92);
	if (n_msgs != 1) {
		fprintf(stderr, "Wrong number of messages with size 92: %d\n", n_msgs);
	}

	printf("Tests passed correctly ;)\n");

	return 0;
}
