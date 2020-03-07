#!/bin/sh

C=./mk/.cache

touch $C/mk/*
find  $C/mybuild/files | xargs touch
touch $C/mybuild/myfiles-model.mk
touch $C/mybuild/myfiles-list.mk

