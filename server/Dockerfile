FROM        node:5.7

ENV         APP_HOME /vidyard/foosbot/server

RUN         mkdir -p $APP_HOME

WORKDIR     $APP_HOME

COPY        package.json npm-shrinkwrap.json ./

RUN         npm install

COPY        . $APP_HOME
