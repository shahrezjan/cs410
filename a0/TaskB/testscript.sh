#!/bin/bash

echo "getsyms_dl, RTLD_LAZY"
for i in `seq 1 50`;
do
        "./getsyms_dl" objsect.o 0
done
echo ""

echo "getsyms_dl, RTLD_NOW"
for i in `seq 1 50`;
do
        "./getsyms_dl" objsect.o 1
done
echo ""

echo "getsections_dl, RTLD_LAZY"
for i in `seq 1 50`;
do
        "./getsections_dl" objsect.o 0
done
echo ""

echo "getsections_dl, RTLD_NOW"
for i in `seq 1 50`;
do
        "./getsections_dl" objsect.o 1
done
