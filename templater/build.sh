#!/bin/bash

cc -Wall -Wextra -std=gnu11 -pedantic -ggdb -o templater \
   template.c \
   input.c \
   main.c
