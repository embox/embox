There are some examples from the folder of the lib60870/examples source code for testing purposes.
The "server104" server is an implementation of the simple_server.c,
and the "tls_server104" is based on tls_server.c along with certificates.
To test the functionality, clients are necessary to build separately:

Client for server104     - examples/simple_client.
Client for tsl_server104 - examples/tls_client.

Both clients are launched with the server's IP address specified (default is 10.0.2.16).
It is not permissible to start both servers simultaneously.


