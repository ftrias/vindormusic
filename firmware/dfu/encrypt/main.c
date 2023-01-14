#include <stdio.h>
//#include <stdlib.h>
#include "ecrypt-sync.h"

#define CHUNKSIZE 1024

void getRandomBytes(u8 **str, u32 len);
void showData(const u8 *data, int len, const char* header);
int test();

const u8 key[ECRYPT_MAXKEYSIZE] = {10,4,2,7};
const u8 IV[ECRYPT_MAXIVSIZE] = {0};
u8 plaintext[CHUNKSIZE];
u8 ciphertext[CHUNKSIZE];

int main(int argc, char** argv)
{
    int dir=0;
    if (argc<3) {
        printf("usage: %s [-e|-d] [infile] [outfile]\n", argv[0]);
        return(1);
    }
    if (argv[1][0] == '-') {
      if (argv[1][1]=='e') {
        dir = 1;
      }
      else if (argv[1][1]=='d') {
        dir = 2;
      }
      else if (argv[1][1]=='t') {
        return(test());
      }
    }
    else {
        printf("usage: %s <-e|-d> <infile> <outfile>\n", argv[0]);
        return(1);
    }
    FILE *inf = fopen(argv[2], "rb");
    if (! inf) {
        printf("file %s not found\n", argv[2]);
        return(2);
    }
    FILE *outf = fopen(argv[3], "wb");
    if (! outf) {
        printf("file %s not found\n", argv[3]);
        return(3);
    }

    //showData(key, ECRYPT_MAXKEYSIZE, "KEY");

    ECRYPT_ctx ctx;
    ECRYPT_init();
    ECRYPT_keysetup(&ctx, key, ECRYPT_MAXKEYSIZE, ECRYPT_MAXIVSIZE);
    ECRYPT_ivsetup(&ctx, IV);

    if (dir==1) {
        while(! feof(inf)) {
            int rlen = fread(plaintext, 1, CHUNKSIZE, inf);
            ECRYPT_encrypt_bytes(&ctx, plaintext, ciphertext, rlen);
            fwrite(ciphertext, 1, rlen, outf);
            //showData(ciphertext, CHUNKSIZE, "CIPHERTEXT");
        }
    }
    else {
        while(! feof(inf)) {
            int rlen = fread(ciphertext, 1, CHUNKSIZE, inf);
            ECRYPT_decrypt_bytes(&ctx, ciphertext, plaintext, CHUNKSIZE);
            fwrite(plaintext, 1, rlen, outf);
            //showData(plaintext, CHUNKSIZE, "PLAINTEXT");
        }
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
