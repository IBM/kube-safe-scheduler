# Build image
# https://hub.docker.com/r/instrumentisto/dep/
FROM instrumentisto/dep:0.5 as builder

ENV GOBASE=$GOPATH/src/kubeRL/rl-monitor
ENV OUT=/out

RUN mkdir -p $GOBASE
COPY . $GOBASE/
RUN cd $GOBASE/ && dep ensure
RUN go build -o $OUT/rl-monitor $GOBASE

# Runtime image
FROM ubuntu:14.04

ENV OUT=/out
ENV TARGET /usr/local/bin
WORKDIR $TARGET

COPY --from=builder $OUT/rl-monitor rl-monitor

ENTRYPOINT ["rl-monitor"]