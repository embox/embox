Here are some examples from the examples folder of the libiec61850 library source code for testing purposes.
The "basic_io" server is an implementation of the server_example_basic_io.c,
and the "tls_server" is based on tls_server_example.c along with certificates.
To test the functionality, clients are necessary to build separately:

Client for basic_io   - examples/iec61850_client_example1.
Client for tsl_server - examples/tls_client_example.

Both clients are launched with the server's IP address specified (default is 10.0.2.16).
It is not permissible to start both servers simultaneously.


