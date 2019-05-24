FROM ubuntu

RUN mkdir -p /home/ball-game

RUN apt-get update

RUN apt-get install -y make gcc

WORKDIR /home/ball-game

COPY ./ball-game .

RUN make all

RUN chmod +x server

ENTRYPOINT ["./server"]
