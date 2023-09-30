# HTTP web server

## Включение в сборку
В файл **mods.conf** необходимо добавить:

```
include embox.cmd.net.httpd
```

## Опции конфигурирования
- **use_ip_ver**
  - **\* 4** - использовать IPv4
  - **6** - использовать IPv6
- **use_index_as_404**
  - **true** - отдавать файл index.html в случае, если запрощенный файл не нашелся  (полезно в случае разработки Single Page Application на JavaScript).
  - **\* false** - отдавать код 404
- **log_level** - уровень журналирования
  - **0** - LOG_NONE
  - **\* 1** - LOG_ERROR
  - **2** - LOG_WARNING
  - **3** - LOG_INFO
  - **4** - LOG_DEBUG

## Запуск
Сервис запускается командой:

```
service httpd /website/
```

где /website/ это имя каталога с файлами сайта (по умолчанию "/")

## Обработчики
Сервис требует наличия одного модуля, реализующего **embox.cmd.net.httpd_processor_interface**

В комплекте поставки идут две реазиции:
- **httpd_no_cgi** - ничего не делающий обработчик (подключается по умолчанию)
- **httpd_cgi** - реализует интерфейс [CGI (Common Gateway Interface)](https://ru.wikipedia.org/wiki/CGI)


# Модуль httpd_cgi
## Включение в сборку
В файл **mods.conf** необходимо добавить:

```
include embox.cmd.net.httpd_cgi
````

## Опции конфигурирования
- **log_level** - уровень журналирования
  - **0** - LOG_NONE
  - **\* 1** - LOG_ERROR
  - **2** - LOG_WARNING
  - **3** - LOG_INFO
  - **4** - LOG_DEBUG
- **cgi_prefix** - префикс запроса, по которому модуль определят что запрос к нему, по умолчанию - "/cgi-bin/"
- **use_real_cmd**
  - **\* false** - искать скрипты только в каталоге /cgi-bin/
  - **tru** - пытаться отбросить из запроса строку /cgi-bin/ и попробовать найти реальную команду.
- **use_parallel_cgi**
  - **\* true** - разрешать выполнение нескольких параллельных команд
  - **false** - разрешать выполнение только одной одновременной команды


# Пример собственного модуля обработчика
Обработчик будет обрабатывать **GET** запрс **/ping** и отвечать текстом **pong**

**httpd_embedded_cgi.my**
```
package embox.cmd.net

module httpd_example_processor extends embox.cmd.net.httpd_processor_interface {
	source "httpd_example_processor.c"
}
```
**mods.conf**
```
include embox.cmd.net.httpd
include embox.cmd.net.httpd_example_processor
```
**httpd_example_processor.c**

```C
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>

struct client_info {
	struct sockaddr ci_addr;
	socklen_t ci_addrlen;
	int ci_sock;
	int ci_index;
	const char *ci_basedir;
};

struct http_req_uri {
	char *target;
	char *query;
};

struct http_req {
	struct http_req_uri uri;
	char *method;
	char *content_len;
	char *content_type;
};


pid_t httpd_try_process(const struct client_info *cinfo, const struct http_req *hreq) {
	int cbyte;
	#define BUFF_SZ     1024
	static char outbuf[BUFF_SZ];

	printf("recieved %s req for %s, query=%s\n", hreq->method, hreq->uri.target, hreq->uri.query);

	if (!strcmp(hreq->uri.target, "/ping")){
		if (!strcmp(hreq->method, "GET")){
			cbyte = snprintf(outbuf, BUFF_SZ,
					"HTTP/1.1 %d %s\r\n"
					"Content-Type: %s\r\n"
					"Connection: close\r\n"
					"\r\n"
					"%s",
					200, "", "text/html", "pong");

			if (write(cinfo->ci_sock, outbuf, cbyte) == cbyte) {
				return 1;
			}else{
				return 0;
			}
		}
	}
	return 0;
}
```