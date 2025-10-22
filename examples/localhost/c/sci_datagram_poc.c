#include <stdio.h>

#include <rmemory.h>
#include <sci.h>
#include <sci_telegram_factory.h>

static void print_hex_bytes(const unsigned char *data, unsigned int length) {
    for (unsigned int i = 0; i < length; ++i) {
        printf("0x%02X%s", data[i], (i + 1 == length) ? "" : " ");
    }
    printf("\n");
}

int main(void) {
    sci_telegram *telegram = sci_create_base_telegram(
        SCI_PROTOCOL_LS,
        "init_sender",
        "remote_device",
        0x0021
    );

    if (telegram == NULL) {
        fprintf(stderr, "failed to allocate SCI telegram\n");
        return 1;
    }

    const unsigned char payload_bytes[] = {0xDE, 0xAD, 0xBE, 0xEF};
    rmemcpy(telegram->payload.data, payload_bytes, sizeof(payload_bytes));
    telegram->payload.used_bytes = sizeof(payload_bytes);

    struct RastaByteArray encoded = sci_encode_telegram(telegram);

    printf("Encoded SCI telegram (%u bytes):\n", encoded.length);
    print_hex_bytes(encoded.bytes, encoded.length);

    printf("\nHeader breakdown:\n");
    printf("  Protocol type: 0x%02X\n", encoded.bytes[0]);
    printf("  Message type : 0x%02X 0x%02X (host order 0x%04X)\n",
           encoded.bytes[1], encoded.bytes[2], sci_get_message_type(telegram));

    char *sender_name = sci_get_name_string(telegram->sender);
    char *receiver_name = sci_get_name_string(telegram->receiver);
    printf("  Sender       : %s\n", sender_name);
    printf("  Receiver     : %s\n", receiver_name);

    printf("\nPayload (%u bytes):\n", telegram->payload.used_bytes);
    if (telegram->payload.used_bytes > 0) {
        print_hex_bytes(
            encoded.bytes + SCI_TELEGRAM_LENGTH_WITHOUT_PAYLOAD,
            telegram->payload.used_bytes
        );
    } else {
        printf("  <no payload>\n");
    }

    rfree(sender_name);
    rfree(receiver_name);
    freeRastaByteArray(&encoded);
    rfree(telegram);
    return 0;
}
