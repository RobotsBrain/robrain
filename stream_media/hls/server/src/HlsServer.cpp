#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <time.h>

#include "HlsServer.h"

using namespace std;



namespace Hls {

static const struct table_entry {
	const char *extension;
	const char *content_type;
} content_type_table[] = {
	{"txt", "text/plain"}, 
	{"c", "text/plain"}, 
	{"h", "text/plain"}, 
	{"html", "text/html"}, 
	{"htm", "text/htm"}, 
	{"css", "text/css"}, 
	{"gif", "image/gif"}, 
	{"jpg", "image/jpeg"}, 
	{"jpeg", "image/jpeg"}, 
	{"png", "image/png"}, 
	{"pdf", "application/pdf"}, 
	{"ps", "application/postsript"}, 
	{NULL, NULL},};

/* Try to guess a good content-type for 'path' */
static const char *guess_content_type(const char *path)
{
	const char *last_period, *extension;
	const struct table_entry *ent;

	last_period = strrchr(path, '.');
	if (!last_period || strchr(last_period, '/')) {
		goto not_found;			/* no exension */
	}

	extension = last_period + 1;
	for (ent = &content_type_table[0]; ent->extension; ++ent) {
		if (!evutil_ascii_strcasecmp(ent->extension, extension)) {
			return ent->content_type;
		}
	}

not_found:
	return "application/misc";
}

CHlsServer::CHlsServer(string host, int port, string rootdir)
: m_host(host)
, m_port(port)
, m_rootdir(rootdir)
, m_uriroot("")
{
}

CHlsServer::~CHlsServer()
{
}

bool CHlsServer::ExtractDisplayInfo(struct evhttp_bound_socket *handle)
{
	/* Extract and display the address we're listening on. */
	struct sockaddr_storage ss;
	evutil_socket_t fd;
	ev_socklen_t socklen = sizeof(ss);
	char addrbuf[128];
	void *inaddr;
	const char *addr;
	int got_port = -1;

	fd = evhttp_bound_socket_get_fd(handle);

	memset(&ss, 0, sizeof(ss));

	if (getsockname(fd, (struct sockaddr *)&ss, &socklen)) {
		perror("getsockname() failed");
		return 1;
	}

	if (ss.ss_family == AF_INET) {
		got_port = ntohs(((struct sockaddr_in *)&ss)->sin_port);
		inaddr = &((struct sockaddr_in *)&ss)->sin_addr;
	} else if (ss.ss_family == AF_INET6) {
		got_port = ntohs(((struct sockaddr_in6 *)&ss)->sin6_port);
		inaddr = &((struct sockaddr_in6 *)&ss)->sin6_addr;
	} else {
		fprintf(stderr, "Weird address family %d\n", ss.ss_family);
		return 1;
	}
	
	addr = evutil_inet_ntop(ss.ss_family, inaddr, addrbuf, sizeof(addrbuf));
	if (addr) {
		char uri_root[128];
		evutil_snprintf(uri_root, sizeof(uri_root), "http://%s:%d", addr, got_port);
		m_uriroot = uri_root;
		printf("Listening on %s:%d, uri root: %s\n", addr, got_port, uri_root);
	} else {
		fprintf(stderr, "evutil_inet_ntop failed\n");
		return 1;
	}

	return true;
}

/* Callback used for the /dump URI, and for every non-GET request:
 * dumps all information to stdout and gives back a trivial 200 ok */
void dump_request_cb(struct evhttp_request *req, void *arg)
{
	const char *cmdtype;
	struct evkeyvalq *headers;
	struct evkeyval *header;
	struct evbuffer *buf;

	switch (evhttp_request_get_command(req)) {
	case EVHTTP_REQ_GET:
		cmdtype = "GET";
		break;
	case EVHTTP_REQ_POST:
		cmdtype = "POST";
		break;
	case EVHTTP_REQ_HEAD:
		cmdtype = "HEAD";
		break;
	case EVHTTP_REQ_PUT:
		cmdtype = "PUT";
		break;
	case EVHTTP_REQ_DELETE:
		cmdtype = "DELETE";
		break;
	case EVHTTP_REQ_OPTIONS:
		cmdtype = "OPTIONS";
		break;
	case EVHTTP_REQ_TRACE:
		cmdtype = "TRACE";
		break;
	case EVHTTP_REQ_CONNECT:
		cmdtype = "CONNECT";
		break;
	case EVHTTP_REQ_PATCH:
		cmdtype = "PATCH";
		break;
	default:
		cmdtype = "unknown";
		break;
	}

	printf("Received a %s request for %s\nHeaders:\n", cmdtype, evhttp_request_get_uri(req));

	headers = evhttp_request_get_input_headers(req);
	for (header = headers->tqh_first; header; header = header->next.tqe_next) {
		printf("  %s: %s\n", header->key, header->value);
	}

	buf = evhttp_request_get_input_buffer(req);
	puts("Input data: <<<");
	while (evbuffer_get_length(buf)) {
		int n;
		char cbuf[128];
		n = evbuffer_remove(buf, cbuf, sizeof(buf) - 1);
		if (n > 0) {
			(void)fwrite(cbuf, 1, n, stdout);
		}
	}
	puts(">>>");

	evhttp_send_reply(req, 200, "OK", NULL);
}

/* This callback gets invoked when we get any http request that doesn't match
 * any other callback.  Like any evhttp server callback, it has a simple job:
 * it must eventually call evhttp_send_error() or evhttp_send_reply().
 */
void send_document_cb(struct evhttp_request *req, void *arg)
{
	CHlsServer *thiz = (CHlsServer *)arg;
	if(thiz != NULL) {
		thiz->SendDocument(req);
	}
}

bool CHlsServer::Start()
{
	struct event_base *base = NULL;
	struct evhttp *http = NULL;
	struct evhttp_bound_socket *handle = NULL;

	base = event_base_new();
	if (!base) {
		fprintf(stderr, "Couldn't create an event_base: exiting\n");
		return -1;
	}

	/* Create a new evhttp object to handle requests. */
	http = evhttp_new(base);
	if (!http) {
		fprintf(stderr, "couldn't create evhttp. Exiting.\n");
		return -1;
	}

	/* The /dump URI will dump all requests to stdout and say 200 ok. */
	evhttp_set_cb(http, "/dump", dump_request_cb, NULL);

	/* We want to accept arbitrary requests, so we need to set a "generic"
	 * cb.  We can also add callbacks for specific paths. */
	evhttp_set_gencb(http, send_document_cb, this);

	/* Now we tell the evhttp what port to listen on */
	handle = evhttp_bind_socket_with_handle(http, m_host.c_str(), m_port);
	if (!handle) {
		fprintf(stderr, "couldn't bind to port. Exiting.\n");
		return -1;
	}

	ExtractDisplayInfo(handle);

	event_base_dispatch(base);

	return true;
}

bool CHlsServer::Stop()
{
	return true;
}

void CHlsServer::SendDocument(struct evhttp_request *req)
{
	int fd = -1;
	size_t len;
	char *decoded_path;
	char *whole_path = NULL;
	const char *path;
	const char *uri = evhttp_request_get_uri(req);
	struct stat st;
	struct evbuffer *evb = NULL;
	struct evhttp_uri *decoded = NULL;

	if (evhttp_request_get_command(req) != EVHTTP_REQ_GET) {
		dump_request_cb(req, NULL);
		return;
	}

	printf("Got a GET request for <%s>\n", uri);

	/* Decode the URI */
	decoded = evhttp_uri_parse(uri);
	if (!decoded) {
		printf("It's not a good URI. Sending BADREQUEST\n");
		evhttp_send_error(req, HTTP_BADREQUEST, 0);
		return;
	}

	/* Let's see what path the user asked for. */
	path = evhttp_uri_get_path(decoded);
	if (!path) {
		path = "/";
	}

	/* We need to decode it, to see what path the user really wanted. */
	decoded_path = evhttp_uridecode(path, 0, NULL);
	if (decoded_path == NULL) {
		printf("the decode path error!\n");
		goto err;
	}

	/* Don't allow any ".."s in the path, to avoid exposing stuff outside
	 * of the docroot.  This test is both overzealous and underzealous:
	 * it forbids aceptable paths like "/this/one..here", but it doesn't
	 * do anything to prevent symlink following." */
	if (strstr(decoded_path, "..")) {
		printf("the decode path have error paramter!\n");
		goto err;
	}

	len = strlen(decoded_path) + m_rootdir.size() + 2;
	if (!(whole_path = (char *)malloc(len))) {
		perror("malloc");
		goto err;
	}

	evutil_snprintf(whole_path, len, "%s/%s", m_rootdir.c_str(), decoded_path);

	if (stat(whole_path, &st) < 0) {
		printf("can not stat the path, whole path is:%s!\n", whole_path);
		goto err;
	}

	/* This holds the content we're sending. */
	evb = evbuffer_new();

	if (S_ISDIR(st.st_mode)) {
		/* If it's a directory, read the comments and make a little
		 * index page */
		DIR *d;
		struct dirent *ent;
		const char *trailing_slash = "";

		if (!strlen(path) || path[strlen(path) - 1] != '/')
			trailing_slash = "/";

		if (!(d = opendir(whole_path))) {
			printf("opendir error, the whole path is: %s\n", whole_path);
			goto err;
		}

		evbuffer_add_printf(evb, "<html>\n <head>\n" "  <title>%s</title>\n" "  <base href='%s%s%s'>\n" " </head>\n" " <body>\n" "  <h1>%s</h1>\n" "  ", decoded_path,	/* XXX html-escape this. */
							m_uriroot.c_str(), path,	/* XXX html-escape this? */
							trailing_slash,
							decoded_path /* XXX html-escape this */ );
		evbuffer_add_printf(evb,
							"<table border cols=3 width=\"100%%\">\n <tr><td>Name</td><td>Type</td><td>Size</td><td>Modification time</td></tr>\n");

		while ((ent = readdir(d))) {
			struct stat info;
			char realname[128];
			const char *name = ent->d_name;

			evbuffer_add_printf(evb, "    <tr><td><a href=\"%s\">%s</a></td>\n", name, name);	/* XXX escape this */

			evutil_snprintf(realname, 128, "%s/%s", whole_path, name);
			if (stat(realname, &info) == 0) {

				if (S_ISDIR(info.st_mode)) {
					evbuffer_add_printf(evb, "<td>Directory</td><td>%ld</td>", info.st_size);
				} else if (S_ISREG(info.st_mode)) {
					evbuffer_add_printf(evb, "<td>Regular File</td><td>%ld</td>", info.st_size);
				} else if (S_ISLNK(info.st_mode)) {
					evbuffer_add_printf(evb, "<td>Link</td><td>%ld</td>", info.st_size);
				} else if (S_ISCHR(info.st_mode)) {
					evbuffer_add_printf(evb, "<td>Character Device</td><td>%ld</td>", info.st_size);
				} else if (S_ISBLK(info.st_mode)) {
					evbuffer_add_printf(evb, "<td>Block Device</td><td>%ld</td>", info.st_size);
				} else if (S_ISFIFO(info.st_mode)) {
					evbuffer_add_printf(evb, "<td>FIFO</td><td>%ld</td>", info.st_size);
				} else if (S_ISSOCK(info.st_mode)) {
					evbuffer_add_printf(evb, "<td>Socket</td><td>%ld</td>", info.st_size);
				} else {
					evbuffer_add_printf(evb, "<td>Unknown</td><td>Unknown</td>");
				}
			}

			evbuffer_add_printf(evb, "<td>%s</td></tr>\n", ctime(&info.st_ctime));
		}

		evbuffer_add_printf(evb, "</table></body></html>\n");

		closedir(d);

		evhttp_add_header(evhttp_request_get_output_headers(req), "Content-Type", "text/html");
	} else {
		/* Otherwise it's a file; add it to the buffer to get
		 * sent via sendfile */
		const char *type = guess_content_type(decoded_path);
		if ((fd = open(whole_path, O_RDONLY)) < 0) {
			perror("open");
			goto err;
		}

		if (fstat(fd, &st) < 0) {
			/* Make sure the length still matches, now that we
			 * opened the file :/ */
			perror("fstat");
			goto err;
		}

		evhttp_add_header(evhttp_request_get_output_headers(req), "Content-Type", type);
		evbuffer_add_file(evb, fd, 0, st.st_size);
	}

	evhttp_send_reply(req, HTTP_OK, "OK", evb);

	goto done;

err:
	evhttp_send_error(req, HTTP_NOTFOUND, "Document was not found");

	if (fd >= 0) {
		close(fd);
	}

done:
	if (decoded) {
		evhttp_uri_free(decoded);
	}

	if (decoded_path) {
		free(decoded_path);
	}

	if (whole_path) {
		free(whole_path);
	}

	if (evb) {
		evbuffer_free(evb);
	}
}

} // end namespace