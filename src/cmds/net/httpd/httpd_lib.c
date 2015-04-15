static char *httpd_parse_uri(char *str, struct http_req_uri *huri) {
	char *pb;
	pb = str;

	huri->target = pb;

	pb = strchr(pb, '?');
	if (pb) {
		*(pb++) = '\0';
		huri->query = pb;
	} else {
		pb = huri->target;
		huri->query = NULL;
	}

	pb = strchr(pb, ' ');
	if (!pb) {
		HTTPD_ERROR("%s: can't find URI-Version separator\n", __func__);
		return NULL;
	}

	*(pb++) = '\0';
	return pb;
}

static char *httpd_parse_request_line(char *str, struct http_req *hreq) {
	char *pb;
	pb = str;

	hreq->method = pb;
	pb = strchr(pb, ' ');
	if (!pb) {
		return NULL;
	}
	*(pb++) = '\0';

	pb = httpd_parse_uri(pb, &hreq->uri);
	if (!pb) {
		HTTPD_ERROR("%s: can't parse uri\n", __func__);
		return NULL;
	}

	pb = strstr(pb, "\r\n");
	if (!pb) {
		HTTPD_ERROR("%s: can't find sentinel\n", __func__);
		return NULL;
	}

	return pb + strlen("\r\n");
}


