FROM gcc

RUN mkdir -p /home/ball-game

RUN apt-get update

RUN apt-get install -y make

WORKDIR /home/ball-game

COPY ./ball-game .

RUN make all

RUN chmod 777 server

CMD ["./server"]
