# Build image
# https://hub.docker.com/r/instrumentisto/dep/
FROM instrumentisto/dep:0.5 as builder

ENV GOBASE=$GOPATH/src/kubeRL/rl-scheduler-extender
ENV OUT=/out

RUN mkdir -p $GOBASE
COPY . $GOBASE/
RUN cd $GOBASE/ && dep ensure
RUN go build -o $OUT/rl-scheduler-extender $GOBASE

# Runtime image
FROM ubuntu:14.04

ENV OUT=/out
ENV TARGET /usr/local/bin
WORKDIR $TARGET

COPY --from=builder $OUT/rl-scheduler-extender rl-scheduler-extender
ENTRYPOINT ["rl-scheduler-extender"]