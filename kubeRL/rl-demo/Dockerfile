FROM ubuntu:18.04
RUN apt-get update && apt-get install -y curl
COPY test_host.sh /
RUN chmod +x /test_host.sh
ENTRYPOINT ["/test_host.sh"]
