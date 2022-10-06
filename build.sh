#!/bin/bash

if [[ $(arch) == 'i386' ]]; then
  	echo Intel Mac
	IDIR="/usr/local/include"
	LDIR="/usr/local/lib"
	NCL="-lncurses"
elif [[ $(arch) == 'arm64' ]]; then
  	echo M1 Mac
	IDIR="/opt/homebrew/include"
	LDIR="/opt/homebrew/lib"
	NCL="-lncurses"
else
	echo Win PC
	IDIR="/mingw64/include"
	LDIR="/mingw64/lib"
	NCL=
fi
c++ -Wno-deprecated -o synth main.cpp synth.cpp user_io.cpp \
	key_tables.c paUtils.c \
	-I/usr/local/include \
	-L/usr/local/lib -lsndfile -lportaudio $NCL
