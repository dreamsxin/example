```php
<?php

// Generate a public and private key
function generate()
{
    // Set the key parameters
    $config = array(
        "digest_alg" => "sha512",
        "private_key_bits" => 4096,
        "private_key_type" => OPENSSL_KEYTYPE_RSA,
    );

    // Create the private and public key
    $res = openssl_pkey_new($config);

    // Extract the private key from $res to $privKey
    openssl_pkey_export($res, $privKey);

    // Extract the public key from $res to $pubKey
    $pubKey = openssl_pkey_get_details($res);

    return array(
        'private' => $privKey,
        'public' => $pubKey["key"],
        'type' => $config,
    );
}

// Encrypt data using the public key
function encrypt($data, $publicKey)
{
    // Encrypt the data using the public key
    openssl_public_encrypt($data, $encryptedData, $publicKey);

    // Return encrypted data
    return $encryptedData;
}

// Decrypt data using the private key
function decrypt($data, $privateKey)
{
    // Decrypt the data using the private key
    openssl_private_decrypt($data, $decryptedData, $privateKey);

    // Return decrypted data
    return $decryptedData;
}


// generate private key
$privKey = openssl_pkey_new(array(
    'private_key_bits' => 1024,
    'private_key_type' => OPENSSL_KEYTYPE_RSA
));


function sshEncodePublicKey($privKey)
{
    $pubKey = openssl_pkey_get_details($privKey);

    $buffer  = pack("N", 7) . "ssh-rsa" . 
               sshEncodeBuffer($pubKey['rsa']['e']) . 
               sshEncodeBuffer($pubKey['rsa']['n']);

    return "ssh-rsa " . base64_encode($buffer); 
}

function sshEncodeBuffer($buffer)
{
    $len = strlen($buffer);
    if (ord($buffer[0]) & 0x80) {
        $len++;
        $buffer = "\x00" . $buffer;
    }

    return pack("Na*", $len, $buffer);
}

$fp = fopen('/home/myleft/.ssh/id_rsa', 'r');
if ($fp == null) { 
    $arr = array('response' => 'failure', 'message' => 'private key not readable');
    echo json_encode($arr);
    die();
}

$private = fread($fp, 8192);
fclose($fp);

$privKey = openssl_pkey_get_private($private);
$pubKey = sshEncodePublicKey($privKey);

echo "PHP generated RSA public key:\n$pubKey\n\n";

$fp = fopen('/home/myleft/.ssh/id_rsa.pub', 'r');
if ($fp == null) { 
    $arr = array('response' => 'failure', 'message' => 'private key not readable');
    echo json_encode($arr);
    die();
}

$public = fread($fp, 8192);
fclose($fp);

var_dump($pubKey, $public);

$public = openssl_pkey_get_details($privKey);

// Encrypt and then decrypt a string
//$arrKeys = generate();
$strEncrypted = encrypt('Hello World!', $public["key"]);
$strDecrypted = decrypt($strEncrypted, $private);
echo $strDecrypted;
```
