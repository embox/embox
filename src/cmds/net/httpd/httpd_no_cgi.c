
int httpd_send_response_cgi(const struct client_info *cinfo, const struct http_req *hreq) {
	FILE *skf;

	skf = fdopen(cinfo->ci_sock, "rw");
	if (!skf) {
		HTTPD_ERROR("can't allocate FILE for socket\n");
		return -ENOMEM;
	}

	fprintf(skf,
		"HTTP/1.1 %d %s\r\n"
		"Content-Type: %s\r\n"
		"Connection: close\r\n"
		"\r\n"
		"%s", 200, "OK", "text/plain",
		"Sorry, CGI support is disabled");

	fclose(skf);

	return 0;
}

