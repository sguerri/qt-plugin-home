#!/bin/bash

~/Qt/5.15.2/gcc_64/bin/linguist ../i18n/$(find ../*.pro | cut -f 2 -d '/' | cut -f 1 -d '.')_fr_FR.ts

