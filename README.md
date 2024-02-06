# b3.h

Definitions for [B3](https://www.b3.com.br/en_us/) FIX/SBE messages.

To use it, just `#include` it as any other C header.

## Compatibility

This is ISO C90 (`-pedantic -ansi`), but depends on extensions that GCC kindly
does not disable (`#pragma scalar_storage_order little-endian` and
`__attribute__((packed))`). Clang does fail to recognize the pragma and throws
warnings ignoring it, but you will probably be fine in a little endian machine.

## Usage

The protocol is simple enough to just cast pointers around. For example,
assuming a packet was received and is stored in the buffer `buf` with size
`size`:

```c
size_t size;
uint8_t *buf = receive_buffer_from_somewhere(&size);

B3PacketHeader *header = (B3PacketHeader*) buf;
B3MessageHeader *first_msg = (B3MessageHeader*) (((size_t) buf) + sizeof(B3PacketHeader));
B3MessageHeader *second_msg = (B3MessageHeader*) (((size_t) first_msg) + first_msg->length);
/* And so on and so forth... */

switch (first_msg->template_id) {
case B3_HIGH_PRICE:
    B3MessageHighPrice *msg = (B3MessageHighPrice*) (((size_t) first_msg) + sizeof(B3MessageHeader));
    printf("Received high price message at %d", msg->md_entry_timestamp);
case B3_LOW_PRICE:
    B3MessageLowPrice *msg = (B3MessageHighPrice*) (((size_t) first_msg) + sizeof(B3MessageHeader));
    printf("Received high price message at %d", msg->md_entry_timestamp);
}
```
