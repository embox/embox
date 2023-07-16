#!/bin/sh

C=./mk/.cache

touch $C/mk/*
find  $C/mybuild/files -exec touch ';'
touch $C/mybuild/myfiles-model.mk
touch $C/mybuild/myfiles-list.mk
exit 0
