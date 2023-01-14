#include "ecrypt-sync.h"

#define CHUNKSIZE 1024

const u8 key[ECRYPT_MAXKEYSIZE] = {10,4,2,7};
const u8 IV[ECRYPT_MAXIVSIZE] = {0};
u8 plaintext[CHUNKSIZE];

int crypt_init() {
    ECRYPT_ctx ctx;
    ECRYPT_init();
    ECRYPT_keysetup(&ctx, key, ECRYPT_MAXKEYSIZE, ECRYPT_MAXIVSIZE);
    ECRYPT_ivsetup(&ctx, IV);
}

int crypt_decrypt(u8 *data, int len) {
    ECRYPT_decrypt_bytes(&ctx, data, plaintext, len);
    for(int i=0;i<len;i++) {
      data[i] = plaintext[i];
    }
}

int test() {
    char plaintext[CHUNKSIZE];
    for(int i=0;i<CHUNKSIZE; i++) {
        plaintext[i] = '0' + (i%10);
    }
    ECRYPT_ctx ctx;

    printf("Encrypting [\n%s\n] using random %d bit key and %d bit IV:\n", plaintext, ECRYPT_MAXKEYSIZE, ECRYPT_MAXIVSIZE);

    ECRYPT_init();
    ECRYPT_keysetup(&ctx, key, ECRYPT_MAXKEYSIZE, ECRYPT_MAXIVSIZE);
    ECRYPT_ivsetup(&ctx, IV);

    ECRYPT_encrypt_bytes(&ctx, (u8*)plaintext, ciphertext, CHUNKSIZE);

    showData(key, ECRYPT_MAXKEYSIZE, "KEY");
    showData(ciphertext, CHUNKSIZE, "CIPHERTEXT");

    ECRYPT_ivsetup(&ctx, IV);
    ECRYPT_decrypt_bytes(&ctx, ciphertext, (u8*)plaintext, CHUNKSIZE);
    printf("And now decrypting back: [\n%s\n]\n", plaintext);

    return 0;
}

void showData(const u8 *data, int len, const char* header) {
    printf("\n-----BEGIN %s-----\n", header);
    for(int i=0;i<len;i++) {
      if (i%32==0) printf("\n");
      printf("%02x", data[i]);
    }
    printf("\n-----END %s-----\n", header);
}
