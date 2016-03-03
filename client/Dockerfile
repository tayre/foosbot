FROM        alpine:3.3

RUN         apk update && \
            apk add libc-dev gcc make libwebsockets

ENV         APP_HOME /vidyard/foosbot

RUN         mkdir -p $APP_HOME

WORKDIR     $APP_HOME

COPY        . $APP_HOME

RUN         make clean && make
