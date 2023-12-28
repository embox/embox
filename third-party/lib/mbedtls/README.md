When including the library in the project, use the phrase

	include third_party.lib.mbedtls(tls_config="config-name")

Where config-name is the name of the file in the config folder (without extension). If the tls_config option is absent, the default configuration file config.h will be used. To create your own mbedtls configuration, create a custom parameters file in the config folder with the necessary set of options. See the config.h file for a description of all available options. The current version supports the following configurations:

ccm-psk-tls1_2 - Minimal configuration for TLS 1.2 with PSK and AES-CCM ciphersuites
				* no bignum, no PK, no X509
				* fully modern and secure (provided the pre-shared keys have high entropy)
				* very low record overhead with CCM-8
				* optimized for low RAM usage

iec61850       - Special configuration for use with libiec61850

mini-tls1_1    - Minimal configuration for TLS 1.1 (RFC 4346), implementing only the
 			     required ciphersuite: MBEDTLS_TLS_RSA_WITH_3DES_EDE_CBC_SHA

no-entropy     - Minimal configuration of features that do not require an entropy source
				* no entropy module
				* no TLS protocol implementation available due to absence of an entropy source

suite-b        - Minimal configuration for TLS NSA Suite B Profile (RFC 6460)
				* no RSA or classic DH, fully based on ECC
				* optimized for low RAM usage
				*
				Possible improvements:
				* if 128-bit security is enough, disable secp384r1 and SHA-512
				* use embedded certs in DER format and disable PEM_PARSE_C and BASE64_C

thread         - Minimal configuration for using TLS a part of Thread ( http://threadgroup.org ) 
				* no RSA or classic DH, fully based on ECC
				* no X.509
				* support for experimental EC J-PAKE key exchange
