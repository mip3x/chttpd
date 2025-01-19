#ifndef RESPONSE_H
#define RESPONSE_H

#define HTTP_404 "HTTP/2 404 Not Found\r\n"
#define HTTP_200 "HTTP/2 200 OK\r\n"

#define CONTENT_TYPE(type) "Content-Type: " type "; charset=UTF-8\r\n"

#define CONTENT_TYPE_TEXT CONTENT_TYPE("text/plain")
#define CONTENT_TYPE_HTML CONTENT_TYPE("text/html")
#define CONTENT_TYPE_CSS CONTENT_TYPE("text/css")
#define CONTENT_TYPE_JS CONTENT_TYPE("application/javascript")

typedef struct {
} response;

#endif
