#! /bin/sh


while true; do
    inotifywait -q -e close_write *.h \
                   -e close_write *.c \
                   -e close_write Makefile 2>&1 1>/dev/null
    echo ""
    echo "======= inotify-run begin ======"
    make
    ./chatty
    echo "======= inorify-run end   ======"
    echo ""
done;
