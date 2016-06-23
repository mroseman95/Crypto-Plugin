/* openssl headers */
#include "include/openssl/rsa.h"
#include "include/openssl/bn.h"
#include "include/openssl/evp.h"
#include "include/openssl/aes.h"
#include "include/openssl/rand.h"
#include "include/openssl/conf.h"
#include "include/openssl/evp.h"
#include "include/openssl/err.h"
#include "include/openssl/pem.h"

/* openssl base64 encypt/decrypt */
#include "OpenSSL_B64/Base64Encode.c"
#include "OpenSSL_B64/Base64Decode.c"

/* general headers */
#include <string.h>

RSA *generate_rsa_keys();
void print_rsa_public_key(RSA*, FILE*);
RSA *read_rsa_public_key(FILE*);
unsigned char *generate_aes_key();
unsigned char *generate_aes_iv();
unsigned int rsa_encrypt_message(unsigned char*, unsigned int, RSA*, unsigned char**);
unsigned int rsa_decrypt_message(unsigned char*, unsigned int, RSA*, unsigned char**);
unsigned int aes_encrypt_message(unsigned char *, unsigned int, unsigned char *, unsigned char *, unsigned char **);
unsigned int aes_decrypt_message(unsigned char *, unsigned int, unsigned char *, unsigned char *, unsigned char **);

//  RSA constants
const int padding = RSA_PKCS1_PADDING;
const int kBits = 2048;
const unsigned long kExp_long = 65537;
BIGNUM *kExp = NULL;

//  AES constants
const int aes_key_length = 256;


int main() {
    RSA *rsa_key = generate_rsa_keys();

    printf ("RSA keys generated\n");

    unsigned char *plaintext = (unsigned char *)"Hello World!";
    unsigned int plaintext_length = 12;

    unsigned char *encrypted_message = (unsigned char *)malloc(kBits);

    printf ("beginning rsa encryption\n");

    unsigned int encrypted_length = rsa_encrypt_message(plaintext, plaintext_length, rsa_key, &encrypted_message);

    printf ("\'Hello World!\' encrypted is:\n");
    printf ("%s\n", encrypted_message);

    unsigned char *decrypted_message = (unsigned char *)malloc(plaintext_length*sizeof(unsigned char));

    rsa_decrypt_message(encrypted_message, encrypted_length, rsa_key, &decrypted_message);

    printf ("The decrypted message is:\n");
    printf ("%s\n", decrypted_message);

    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
    OPENSSL_config(NULL);

    unsigned char *aes_key = generate_aes_key();
    unsigned char *aes_iv = generate_aes_iv();

    printf ("AES key and iv generated\n");

    unsigned char *aes_encrypted_message = (unsigned char *)malloc(plaintext_length*(unsigned char) + AES_BLOCK_SIZE);

    printf ("beginning aes encryption\n");

    unsigned int aes_encrypted_length = aes_encrypt_message(plaintext, plaintext_length, aes_key, aes_iv, &aes_encrypted_message);

    printf ("\'Hello World encrypted is:\n");
    printf ("%s\n", aes_encrypted_message);
    printf ("%u\n", aes_encrypted_length);

    unsigned char *aes_decrypted_message = (unsigned char *)malloc(plaintext_length*(unsigned char) + AES_BLOCK_SIZE);

    unsigned int aes_decrypted_length = aes_decrypt_message(aes_encrypted_message, aes_encrypted_length, aes_key, aes_iv, &aes_decrypted_message);

    printf ("The decrypted message is:\n");
    printf ("%s\n", aes_decrypted_message);

    exit(EXIT_SUCCESS);
}


/*
 * Generates RSA public and private key
 * @param public_key: where the DER encoded public key is put
 * @param private_key: where the DER encoded private key is put
 * @return: pointer to RSA object
 */
RSA *generate_rsa_keys(unsigned char **public_key, unsigned char **private_key) {
    RSA *key = RSA_new();
    kExp = BN_new();
    BN_set_word(kExp, kExp_long);

    if (RSA_generate_key_ex(key, kBits, kExp, 0) < 0) {
        printf("RSA_generate_key_ex failed\n");
        exit(EXIT_FAILURE);
    }

    return key;
}


/* 
 * Prints the public part of a RSA key to a file
 */
void print_rsa_public_key(RSA* key, FILE* file) {

    PEM_write_RSAPublicKey(file, key);
}

/*
 * Reads a PEM file and gets the public RSA key
 */
RSA *read_rsa_public_key(FILE* file) {

    RSA *key = RSA_new();

    PEM_read_RSA_PUBKEY(file, &key, NULL, NULL);

    return key;
}

/*
 * Encrypts a plaintext and sets it to out
 * @param message: the plaintext string
 * @param length: the length of the plaintext string in bytes
 * @param key: the RSA object that is the key
 * @param out: where the output encrypted message is returned
 * @return: the length of the encrypted message
 */
unsigned int rsa_encrypt_message(unsigned char *message, unsigned int length, RSA *key, unsigned char **out) {
    unsigned char *encrypted_message = (unsigned char *)malloc(RSA_size(key));
    unsigned int size;

    if ((size = RSA_public_encrypt(length, message, encrypted_message, key, padding)) < 0) {
        printf("RSA_public_encrypt() failed\n");
        exit(EXIT_FAILURE);
    }

    if (Base64Encode(encrypted_message, size, (char **)out) < 0) {
        printf("Base64Encode failed\n");
        exit(EXIT_FAILURE);
    }

    return size;
}

/* 
 * Decyrpts an encrypted text and sets it to out
 * @param encrypted_message: the RSA encrypted ciphertext
 * @param length: the length of the original plaintext
 * @param key: the RSA key to decrypt with
 * @param out: the string to print plaintext to
 */
unsigned int rsa_decrypt_message(unsigned char *encrypted_message, unsigned int length, RSA *key, unsigned char **out) {
    unsigned char *encrypted_ascii_message = (unsigned char *)malloc(RSA_size(key));
    unsigned char *decrypted_message = (unsigned char *)malloc(length);
    unsigned int size;
    
    if (Base64Decode((char *)encrypted_message, &encrypted_ascii_message, (size_t *)&length) < 0) {
        printf("Base64Decode failed\n");
        exit(EXIT_FAILURE);
    }

    if ((size = RSA_private_decrypt(length, encrypted_ascii_message, decrypted_message, key, padding)) < 0) {
        printf("RSA_private_decrypt() failed\n");
        exit(EXIT_FAILURE);
    }

    *(decrypted_message + size) = '\0';

    *out = decrypted_message;

    return size;
}

/* 
 * Creates the aes key which is essentially 256 random bits
 * @return: the string representation of the aes key
 */
unsigned char *generate_aes_key() {
    unsigned char *encKey = (unsigned char *)malloc(aes_key_length);

    if (!RAND_bytes(encKey, sizeof encKey)) {
        printf("RAND_bytes for encKey failed\n");
        exit(EXIT_FAILURE);
    }

    return encKey;
}

/* 
 * Creates the aes iv which is essentially 128 random bits
 * @return: the string representation of the aes iv
 */
unsigned char *generate_aes_iv() {
    unsigned char *encIv = (unsigned char *)malloc(AES_BLOCK_SIZE*sizeof(unsigned char));

    if (!RAND_bytes(encIv, sizeof encIv)) {
        printf("RAND_bytes for encIv failed\n");
        exit(EXIT_FAILURE);
    }

    return encIv;
}

/* 
 * Encrypts the plaintext and sets it to out
 * @param message: the plaintext string
 * @param length: the number of chars for the plaintext
 * @param encKey: the aes 256 key
 * @param encIv: the aes 128 iv
 * @param out: where the enc message is put
 */
unsigned int aes_encrypt_message(unsigned char *message, unsigned int length, unsigned char *encKey, unsigned char *encIv, unsigned char **out) {
    unsigned char *encMsg = (unsigned char *)malloc(length*(unsigned char) + AES_BLOCK_SIZE);
    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;

    if (!(ctx = EVP_CIPHER_CTX_new())) {
        printf ("EVP_CIPHER_CTX_new() failed\n");
        exit (EXIT_FAILURE);
    }

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, encKey, encIv)) {
        printf ("EVP_EncryptInit_ex failed\n");
        exit (EXIT_FAILURE);
    }

    if (1 != EVP_EncryptUpdate(ctx, encMsg, &len, message, length)) {
        printf ("EVP_ENcryptUpdate failed\n");
        exit (EXIT_FAILURE);
    }

    ciphertext_len = len;

    if (1 != EVP_EncryptFinal_ex(ctx, encMsg + len, &len)) {
        printf ("EVP_EncryptFinal_ex() failed\n");
        exit (EXIT_FAILURE);
    }

    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    *(encMsg + ciphertext_len) = '\0';

    if (Base64Encode(encMsg, ciphertext_len, (char **)out) < 0) {
        printf ("Base64Encode in aes_encrypt_message failed\n");
    }

    return ciphertext_len;
}

/* 
 * Decrypts the aes encrypted text and sets it to out
 */
unsigned int aes_decrypt_message(unsigned char *encrypted_message, unsigned int length, unsigned char *encKey, unsigned char *encIv, unsigned char **out) {
    unsigned char *encrypted_ascii_message = (unsigned char *)malloc(length*(unsigned char) + AES_BLOCK_SIZE);

    if (Base64Decode((char *)encrypted_message, &encrypted_ascii_message, (size_t *)&length) < 0) {
        printf("Base64Decode in aes_decrypt_message failed\n");
        exit(EXIT_FAILURE);
    }

    EVP_CIPHER_CTX *ctx;
    unsigned char *decMsg = (unsigned char *)malloc(length*(unsigned char) + AES_BLOCK_SIZE);

    int plaintext_len;
    int len;

    if (!(ctx = EVP_CIPHER_CTX_new())) {
        printf("EVP_CIPHER_CTX_new() failed\n");
        exit(EXIT_FAILURE);
    }

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, encKey, encIv)) {
        printf("EVP_DecryptInit_ex() failed\n");
        exit(EXIT_FAILURE);
    }

    if (1 != EVP_DecryptUpdate(ctx, decMsg, &len, encrypted_ascii_message, length)) {
        printf("EVP_DecryptUpdate() failed\n");
        exit(EXIT_FAILURE);
    }

    plaintext_len = len;

    if (1 != EVP_DecryptFinal_ex(ctx, decMsg + len, &len)) {
        printf("EVP_DecryptFinal_ex() failed\n");
        exit(EXIT_FAILURE);
    }

    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    *(decMsg + plaintext_len) = '\0';

    *out = decMsg;

    return plaintext_len;
}
