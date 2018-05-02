#!/usr/bin/env bash

cd ..
rm -f submission.zip
cd src
zip -r submission.zip *.h *.hpp *.c *.cpp Makefile
mv submission.zip ../
