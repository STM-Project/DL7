#ifndef __HTTP_REQUESTS_H__
#define __HTTP_REQUESTS_H__

void archive_download(struct netconn *conn, const char* filepath);
void archive_website(struct netconn *conn, const char* filepath, const char* host);
void sendSimpleHTTPResponse(int type, struct netconn *conn);
void http_server_serve_logged(void *pvParameters);
u32_t http_server_serve_not_logged(struct netconn *conn);
void archive_GetHost(char *request,char *hostIP);
#endif /* __HTTP_REQUESTS_H__ */
