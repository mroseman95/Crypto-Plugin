/**
 * generates a RSA key pair
 * @param: pass a passphrase that protects the private key
 * @return: a cryptico key object
 */
var generate_asym_keys = function(pass) {
    return cryptico.generateRSAKey(pass, 2048);
};
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA FUCK YOU YOU YOU YUO
console.log("Hitler did nothing wrong");
/**
 * encrypts a plaintext message
 * @param: plaintext the unencrypted text
 * @param: pubkey the public key to encrypt with
 * @return: the string of the encrypted armored ciphertext
 */
var encrypt_asym_message = function(plaintext, pubkey) {
    return cryptico.encrypt(plaintext, pubkey).cipher;
};

/**
 * decrypts an encrypted armored ciphertext
 * @param: ciphertext the armored encrypted ciphertext
 * @param: privkey the cryptico key to decrypt
 * @return: the plaintext original message
 */
var decrypt_asym_message = function(ciphertext, privkey) {
    return cryptico.decrypt(ciphertext, privkey).plaintext;
};

/**
 * generates a AES key
 *  key - 128 bits or 32 characters
 * @return: a 128 bit key
 */
var generate_sym_key = function() {
    var key = generate_random_bits(16);
    return key;
};

/**
 * Encrypts a message using a given key
 * @param: plaintext the plaintext string to encrypt
 * @param: key the 16 byte key to encrypt with
 * @return: the encrypted string
 */
var encrypt_sym_message = function(plaintext, key) {
    /*var ciphertext = "";
    var encryptedBytes = null;
    var aesCbc = new aesjs.ModeOfOperation.cbc(key, iv);

    while (plaintext.length !== 0) {
        // first 16 characters of plaintext
        var Byte16SubString = plaintext.slice(0,16);
        // if there are x spaces to buffer then add x x's to the end of the string
        //Byte16SubString += (" " * (16 - Byte16SubString.length));
        var x;
        if ((x = 16 - Byte16SubString.length) > 0) {
            for (var i=0; i<x; i++) {
                Byte16SubString += x.toString(16);
            }
        }
        var textBytes = aesjs.util.convertStringToBytes(Byte16SubString);
        if (encryptedBytes !== null) {
            encryptedBytes += aesCbc.encrypt(textBytes);
        } else {
            encryptedBytes = aesCbc.encrypt(textBytes);
        }
        ciphertext +=  aesjs.util.convertBytesToString(encryptedBytes);

        plaintext = plaintext.slice(16, plaintext.length);
    }

    return ciphertext;*/
   var textBytes = aesjs.util.convertStringToBytes(plaintext);
   var aesCtr = new aesjs.ModeOfOperation.ctr(key, new aesjs.Counter(5));
   var encryptedBytes = aesCtr.encrypt(textBytes);

   return encryptedBytes;
};

/**
 * Decrypts a message using a given key
 * @param: ciphertext the encrypted byte array to decrypt
 * @param: key the key to decrypt with
 * @return: the decrypted string
 */
var decrypt_sym_message = function(ciphertext, key) {
    /*var aesCbc = new aesjs.ModeOfOperation.cbc(key, iv);
    var decryptedBytes = aesCbc.decrypt(ciphertext);
    return aesjs.util.convertBytesToString(decryptedBytes);*/
   var aesCtr = new aesjs.ModeOfOperation.ctr(key, new aesjs.Counter(5));
   var decryptedBytes = aesCtr.decrypt(ciphertext);
   var decryptedText = aesjs.util.convertBytesToString(decryptedBytes);
   return decryptedText;
};

/**
 * randomly generates a string of bits
 * @param: nBytes the number of bytes to generate
 * @return: a Uint8Array of nBytes length with randomly generated bytes
 */
var generate_random_bits = function(nBytes) {
    var i;
    var random_arr = new Uint8Array(nBytes);
    for (i = 0; i < nBytes; i++) {
       random_arr[i] = Math.floor(Math.random() * 256);
    }

    return random_arr;
};
