#ifndef __HTTPSERVER_NETCONN_H__
#define __HTTPSERVER_NETCONN_H__

extern uint8_t bufferForIndex[];
extern uint32_t bytesreadFromIndex;

extern uint8_t bufferForLog[];
extern uint32_t bytesreadFromLog;

extern uint8_t bufferForLogo[];
extern uint32_t bytesreadFromLogo;

void http_server_netconn_init(void);
void setWebLoginTimer(void);
void logOffPeer(void);

#endif /* __HTTPSERVER_NETCONN_H__ */
