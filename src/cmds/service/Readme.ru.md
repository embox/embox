# service
Утилита для работы с сервисами
## Включение в сборку
В файл **mods.conf** необходимо добавить:

```
include embox.cmd.service
```
## Опции конфигурирования
- **service_name_len** - максимальная длина имени сервиса вместе с аргументами (по умолчанию 32)
- **services_count** - максимальное количество запущенных сервисов (по умолчанию 16)

## Запуск
Команда запуска:
```
service [[-s stack_size] service_name [stop|restart]]
```

Запуск без параметров выводит список запущенных сервисов:
```
embox>service
[ Task Id=4 ]   httpd /website/    [ exited ]
[ Task Id=3 ]   telnetd          
```

