- https://github.com/phuslu/nginx-ssl-fingerprint
- https://github.com/cnlnn/ja3-server

## JA3 指纹

JA3指纹（JA3 fingerprint）是一种用于网络流量分析的技术，旨在识别和分类不同的TLS（Transport Layer Security）客户端。TLS是一种常用于加密互联网通信的协议，它提供了安全的数据传输和身份验证机制。

JA3指纹通过分析TLS握手阶段中客户端和服务器之间交换的握手消息来创建一个唯一的指纹。在TLS握手期间，客户端和服务器交换一系列的握手消息，包括支持的TLS版本、密码套件、压缩算法和TLS扩展等信息。JA3指纹将这些消息中的特定字段进行哈希处理，生成一个字符串来表示客户端的TLS配置。

由于不同的TLS客户端在握手消息中的字段值可能会有所不同，因此它们生成的JA3指纹也会有所不同。这使得JA3指纹可以用于识别和区分不同类型的TLS客户端，例如Web浏览器、移动应用程序、恶意软件等。

通过分析网络流量中的JA3指纹，安全分析人员和网络管理员可以检测和识别异常的TLS行为、恶意软件的活动或者潜在的网络攻击。然而，值得注意的是，JA3指纹并非绝对可靠，因为攻击者可以通过更改TLS

### JA3指纹的组成和格式
JA3指纹由TLS握手消息中的特定字段值组成，并使用逗号进行分隔。

组成
JA3_HASH,JA3_SSLVersion,JA3_CipherSuites,JA3_Extensions

JA3_HASH：这是根据TLS握手消息中的客户端Hello消息计算得出的哈希值，通常使用MD5或SHA256算法生成。JA3_HASH用于唯一标识TLS客户端的配置。

JA3_SSLVersion：这是客户端支持的TLS版本号。例如，TLS 1.2的版本号是0x0303，TLS 1.3的版本号是0x0304。

JA3_CipherSuites：这是客户端支持的加密套件列表。加密套件指定了用于加密通信的加密算法和密钥交换协议。每个加密套件都有一个唯一的标识号。在JA3指纹中，多个加密套件由逗号分隔。

JA3_Extensions：这是客户端在TLS握手消息中发送的TLS扩展列表。TLS扩展提供了额外的功能和安全性选项。常见的扩展包括Server Name Indication (SNI)、Supported Elliptic Curves、Supported Point Formats等。在JA3指纹中，多个扩展由逗号分隔。

### 格式
以下是一个示例JA3指纹的格式：

eb6f49e8db7ad1809f885d12232f4855,0x0303,0xc02c,c02b,c02f,c00a,c009,c013,c014,0xff01,0x0000
在上述示例中，

JA3_HASH为eb6f49e8db7ad1809f885d12232f4855，
JA3_SSLVersion为0x0303（表示TLS 1.2），
JA3_CipherSuites包括多个加密套件
JA3_Extensions为空。
JA3握手指纹和JA3S指纹的区别
JA3指纹和JA3S指纹是两种相关但略有不同的TLS指纹技术。

JA3指纹（Just Another SSL/TLS Fingerprint）：它是根据客户端发送的TLS握手消息中的字段值生成的指纹。这些字段包括SSL/TLS版本、加密套件、TLS扩展等信息。JA3指纹用于识别和分类不同的TLS客户端。

JA3S指纹（Just Another SSL/TLS Signature）：它是基于服务器在TLS握手过程中发送的服务器Hello消息中的字段值生成的指纹。与JA3指纹不同，JA3S指纹用于识别和分类不同的TLS服务器。

JA3指纹和JA3S指纹的生成方法类似，都是通过计算握手消息中的字段值的哈希值来生成唯一的指纹。它们的区别在于指纹生成的消息来源不同：JA3指纹是基于客户端Hello消息，而JA3S指纹是基于服务器Hello消息。

这两种指纹技术在网络安全领域中被广泛应用，可以用于检测异常的TLS行为、识别恶意软件和网络攻击等。同时，它们也有一定的局限性，因为攻击者可以伪造或篡改握手消息中的字段值，以逃避指纹识别。因此，在使用JA3和JA3S指纹进行分析时，需要结合其他技术和方法进行验证和确认。

**JA3和JA3S工具现在已经开源**
https://github.com/salesforce/ja3

## 原理

### TLS握手过程的关键信息
TLS握手过程中包含了多个关键信息，这些信息对于建立安全连接和进行身份验证非常重要。以下是TLS握手过程中的一些关键信息：

客户端支持的TLS版本：客户端在ClientHello消息中指定其支持的TLS版本，例如TLS 1.2或TLS 1.3。服务器会根据客户端支持的版本选择适当的协议版本进行通信。

加密套件（Cipher Suite）：加密套件定义了在通信过程中使用的加密算法和密钥交换算法。它包括对称加密算法（如AES、DES）、密钥交换算法（如RSA、Diffie-Hellman）和消息认证算法（如HMAC）等。

随机数（Random）：客户端和服务器都会生成一个随机数，用于生成对称加密算法的密钥、初始化向量（IV）和计算消息认证码（MAC）。随机数的目的是增加通信的随机性和安全性。

数字证书（Digital Certificate）：服务器在ServerHello消息中会发送数字证书，证书用于验证服务器的身份。证书包含服务器的公钥和证书颁发机构（CA）的签名，客户端可以使用该公钥验证服务器的身份和建立安全通信。

客户端密钥交换（Client Key Exchange）：在握手过程中，客户端可能需要发送密钥交换相关的信息，用于与服务器协商会话密钥。这可以是一个PreMaster Secret（预主密钥）或Diffie-Hellman交换的公钥。

会话标识符（Session Identifier）：服务器可以为成功建立的会话分配一个唯一的会话标识符，以便在后续的握手过程中快速恢复会话状态，从而提高性能。

TLS扩展（TLS Extensions）：TLS扩展提供了额外的功能和安全性选项。常见的扩展包括Server Name Indication（SNI，用于指定服务器的域名）、支持的加密算法、应用层协议协商（ALPN）等。

这些关键信息在TLS握手期间的交换和协商，确保了通信的机密性、完整性和身份验证，从而建立了安全的TLS连接。

### 通过TLS握手信息生成JA3指纹
生成JA3指纹需要从TLS握手信息中提取关键字段，并对这些字段进行哈希处理。以下是生成JA3指纹的一般步骤：

提取TLS握手消息中的关键字段：从客户端Hello消息中提取以下字段值：

   - 支持的TLS版本（SSLVersion）
   - 加密套件（CipherSuites）
   - TLS扩展（Extensions）
将提取的字段值组合成一个字符串：将上述字段值按照特定的顺序连接起来，使用逗号进行分隔。例如：SSLVersion,CipherSuites,Extensions。

对组合的字符串进行哈希处理：使用特定的哈希算法，例如MD5或SHA256，对上述组合的字符串进行哈希处理，生成一个哈希值。

将哈希值作为JA3指纹：将生成的哈希值作为最终的JA3指纹。

需要注意的是，不同的实现和工具可能对字段的顺序和格式有所不同。为了确保一致性，建议使用广泛接受的规范来提取字段和生成JA3指纹。

### 示例
假设提取的字段值为：

SSLVersion: 0x0303
CipherSuites: c02c,c02b,c02f,c00a,c009,c013,c014
Extensions: 0xff01,0x0000
组合的字符串为：0x0303,c02c,c02b,c02f,c00a,c009,c013,c014,0xff01,0x0000
通过MD5哈希处理上述字符串得到哈希值：eb6f49e8db7ad1809f885d12232f4855
最终的JA3指纹为：eb6f49e8db7ad1809f885d12232f4855

实际实现中可能会有一些细微的差异。

JA3指纹的可变性和唯一性
JA3指纹具有一定的可变性和唯一性，这取决于TLS客户端的配置和握手消息中的字段值。

### 可变性：
不同的TLS客户端可能支持不同的TLS版本、加密套件和TLS扩展，因此它们的JA3指纹会有所不同。

特定的TLS客户端可能会在不同的环境或配置下生成不同的JA3指纹。例如，同一款Web浏览器在不同的操作系统、版本或插件配置下生成的JA3指纹可能会有差异。

### 唯一性：
在大多数情况下，每个TLS客户端的配置会生成一个唯一的JA3指纹。由于握手消息中的字段值是在握手期间由客户端动态生成的，因此不同的配置通常会产生不同的JA3指纹。

JA3指纹的哈希算法（如MD5或SHA256）通常能够保证较低的碰撞概率，即不同的配置生成相同的指纹的可能性较低。

然而，需要注意的是，JA3指纹并不是绝对唯一和确定性的标识符，也存在一些限制和局限性：

攻击者可以通过修改握手消息中的字段值来更改JA3指纹，从而规

```python
import socket
import ssl
import logging
import argparse
from hashlib import md5
from datetime import datetime

CERT_FILE = 'server.crt'
KEY_FILE = 'server.key'

TLS_VERSION = {
    '0300': 'SSL 3.0',
    '0301': 'TLS 1.0',
    '0302': 'TLS 1.1',
    '0303': 'TLS 1.2',
    '0304': 'TLS 1.3',
}

EXTENSION_TYPE = {
    0: 'server_name',
    1: 'max_fragment_length',
    2: 'client_certificate_url',
    3: 'trusted_ca_keys',
    4: 'truncated_hmac',
    5: 'status_request',
    6: 'user_mapping',
    7: 'client_authz',
    8: 'server_authz',
    9: 'cert_type',
    10: 'supported_groups',
    11: 'ec_point_formats',
    12: 'srp',
    13: 'signature_algorithms',
    14: 'use_srtp',
    15: 'heartbeat',
    16: 'application_layer_protocol_negotiation',
    17: 'status_request_v2',
    18: 'signed_certificate_timestamp',
    19: 'client_certificate_type',
    20: 'server_certificate_type',
    21: 'padding',
    22: 'encrypt_then_mac',
    23: 'extended_master_secret',
    24: 'token_binding',
    25: 'cached_info',
    26: 'tls_lts',
    27: 'compress_certificate',
    28: 'record_size_limit',
    29: 'pwd_protect',
    30: 'pwd_clear',
    31: 'password_salt',
    32: 'ticket_pinning',
    33: 'tls_cert_with_extern_psk',
    34: 'delegated_credential',
    35: 'session_ticket',
    36: 'TLMSP',
    37: 'TLMSP_proxying',
    38: 'TLMSP_delegate',
    39: 'supported_ekt_ciphers',
    40: 'Reserved',
    41: 'pre_shared_key',
    42: 'early_data',
    43: 'supported_versions',
    44: 'cookie',
    45: 'psk_key_exchange_modes',
    46: 'Reserved',
    47: 'certificate_authorities',
    48: 'oid_filters',
    49: 'post_handshake_auth',
    50: 'signature_algorithms_cert',
    51: 'key_share',
    52: 'transparency_info',
    54: 'connection_id',
    55: 'external_id_hash',
    56: 'external_session_id',
    57: 'quic_transport_parameters',
    58: 'ticket_request',
    59: 'dnssec_chain',
    60: 'sequence_number_encryption_algorithms'
}


def get_arguments():
    parser = argparse.ArgumentParser()
    parser.add_argument('-l', '--listen', help='specify listening address (example: 127.0.0.1:443)', default='127.0.0.1:443')
    parser.add_argument('-v', '--verbose', help='show log', action='store_true')
    args = parser.parse_args()
    try:
        listen_host, listen_port = args.listen.split(':')
        socket.inet_aton(listen_host)
        listen_port = int(listen_port)
        if not 1 <= listen_port <= 65535:
            raise Exception
        if args.verbose:
            logging.basicConfig(format='%(message)s', level=logging.DEBUG)
    except Exception as args_error:
        logging.error(args_error)
        print("Invalid listening address or port")
        exit()
    return listen_host, listen_port


def socket_listen(listen_host, listen_port):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server_socket.bind((listen_host, listen_port))
        server_socket.listen(0)
        connect, address = server_socket.accept()
        return connect, address


def ssl_response(socket_conn, response_data):
    try:
        ssl_context = ssl.create_default_context(purpose=ssl.Purpose.CLIENT_AUTH)
        ssl_context.load_cert_chain(certfile=CERT_FILE, keyfile=KEY_FILE)
        with ssl_context.wrap_socket(socket_conn, server_side=True, suppress_ragged_eofs=True) as ssl_socket:
            ssl_socket.do_handshake()
            response = f"HTTP/1.1 200 OK\r\nContent-Length: {len(str(response_data))}\r\n\r\n{response_data}"
            ssl_socket.sendall(response.encode())
            ssl_socket.shutdown(socket.SHUT_RDWR)
    except ssl.SSLError as ssl_error:
        logging.warning(ssl_error)
        pass


def ja3(pack):
    cipher_suites_list, extension_list, supported_groups_list, ec_point_formats_list = ([] for i in range(4))
    tls_version_num = pack[9:11].hex()
    session_id_length = pack[43]
    cipher_suites_length = int.from_bytes(pack[44 + session_id_length:46 + session_id_length], 'big')
    cipher_suites_begin = 44 + session_id_length + 2
    for cipher_suite in range(cipher_suites_begin, cipher_suites_begin + cipher_suites_length, 2):
        cipher_suites_list.append(str(int.from_bytes(pack[cipher_suite:cipher_suite + 2], 'big')))
    compression_method_length = pack[cipher_suites_begin + cipher_suites_length]
    compression_method_begin = cipher_suites_begin + cipher_suites_length + 1
    extensions_length = int.from_bytes(pack[compression_method_begin +
                                            compression_method_length:compression_method_begin +
                                            compression_method_length + 2], 'big')
    logging.debug("Handshake Protocol:")
    logging.debug(" ├─ Handshake Type: %s ", pack[5])
    logging.debug(" ├─ Length: %s ", int.from_bytes(pack[6:9], 'big'))
    logging.debug(" ├─ Version: %s (0x%s)", TLS_VERSION[tls_version_num], tls_version_num)
    logging.debug(" ├─ Random: %s ", pack[11:43].hex())
    logging.debug(" ├─ Session ID Length: %s ", session_id_length)
    logging.debug(" ├─ Session ID: %s ", pack[44:44 + session_id_length].hex())
    logging.debug(" ├─ Cipher Suites Length: %s ", cipher_suites_length)
    logging.debug(" ├─ Cipher Suites: %s ", pack[cipher_suites_begin:cipher_suites_begin + cipher_suites_length].hex())
    logging.debug(" ├─ Compression Method Length: %s ", compression_method_length)
    logging.debug(" ├─ Compression Method: %s ", pack[compression_method_begin:compression_method_begin +
                                                      compression_method_length].hex())
    logging.debug(" ├─ Extentions Length: %s ", extensions_length)
    extensions_begin = extension_begin = compression_method_begin + compression_method_length + 2
    extension_length = 0
    while extension_begin + extension_length < extensions_begin + extensions_length:
        extension_type_num = int.from_bytes(pack[extension_begin:extension_begin + 2], 'big')
        extension_length = int.from_bytes(pack[extension_begin + 2:extension_begin + 4], 'big')
        if extension_type_num in EXTENSION_TYPE.keys():
            extension = EXTENSION_TYPE[extension_type_num]
        else:
            extension = 'unknown'
        logging.debug(" ├─ Extention: %s", extension)
        extension_list.append(str(extension_type_num))
        if extension_type_num == 10:
            supported_groups_list_length = int.from_bytes(pack[extension_begin + 4:extension_begin + 6], 'big')
            logging.debug(" │   ├─ Supported Groups List Length: %s", supported_groups_list_length)
            logging.debug(" │   ├─ Supported Groups: %s",
                          pack[extension_begin + 6:extension_begin + 6 + supported_groups_list_length].hex())
            for supported_group in range(extension_begin + 6, extension_begin + 6 + supported_groups_list_length, 2):
                supported_groups_list.append(str(int.from_bytes(pack[supported_group:supported_group + 2], 'big')))
        if extension_type_num == 11:
            ec_point_formats_length = pack[extension_begin + 4]
            logging.debug(" │   ├─ EC point formats Length: %s", ec_point_formats_length)
            logging.debug(" │   ├─ Elliptic curves point formats")
            for ec_point_format in range(extension_begin + 5, extension_begin + 5 + ec_point_formats_length):
                ec_point_formats_list.append(str(pack[ec_point_format]))
        logging.debug(" │   └─ Length: %s", extension_length)
        extension_begin += extension_length + 4
    ja3_full_string = ','.join([str(int(tls_version_num, 16)), '-'.join(cipher_suites_list), '-'.join(extension_list),
                               '-'.join(supported_groups_list), '-'.join(ec_point_formats_list)])
    logging.debug(" └─ JA3 Full string: %s", ja3_full_string)
    ja3_fingerprint = md5(ja3_full_string.encode()).hexdigest()
    return ja3_fingerprint


if __name__ == '__main__':
    host, port = get_arguments()
    while True:
        try:
            conn, addr = socket_listen(host, port)
            with conn:
                data = conn.recv(1024, socket.MSG_PEEK)
                if data and data[0] == 22:
                    time_now = datetime.now()
                    content = {"time": time_now.strftime("%Y/%m/%d-%H:%M:%S"), "ip": addr[0], "port": addr[1]}
                    content.update({"ja3": ja3(data)})
                    print(content)
                    ssl_response(conn, content)
                else:
                    continue
        except KeyboardInterrupt as e:
            print("\nUser exit.")
            break
        except Exception as e:
            logging.error(e)
            if "Address already in use" in str(e) or "Permission denied" in str(e):
                break
            else:
                continue
```
