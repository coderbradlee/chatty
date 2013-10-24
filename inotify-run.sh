#! /bin/sh


while true; do
    inotifywait -q -e close_write *.h \
                   -e close_write *.c \
                   -e close_write Makefile 2>&1 1>/dev/null
    echo ""
    date
    echo "======= inotify-run begin ======"
    #echo "stop chatty"
    #pkill phantomjs 2>&1 1>/dev/null
    #pkill chatty 2>&1 1>/dev/null
    make
    #./chatty &
    #sleep 0.5
    #phantomjs ws.js &
    echo "======= inotify-run end   ======"
done;
