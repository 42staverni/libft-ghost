FROM debian:bookworm-slim

RUN apt-get update && apt-get install -y --no-install-recommends \
    gcc \
    libc6-dev \
    libbsd-dev \
    valgrind \
    make \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY server /app/server
COPY testcases/ /app/testcases/

ENV PORT=8080
ENV TEST_CASES_DIR=/app/testcases

EXPOSE 8080

CMD ["/app/server"]
