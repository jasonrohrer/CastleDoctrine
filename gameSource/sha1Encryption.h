


// decrypts data that was encrypted with key-and-counter based SHA1 keystream
// result destroyed by caller.
// makes use of sharedServerSecret.
char *sha1Decrypt( const char *inKey, const char *inEncryptedDataBase64 );




