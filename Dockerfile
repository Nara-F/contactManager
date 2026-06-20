FROM ubuntu:22.04 AS builder

RUN apt-get -o Acquire::Retries=5 update && apt-get -o Acquire::Retries=5 install -y --no-install-recommends \
    g++ \
    cmake \
    libsqlite3-dev \
    && rm -rf /var/lib/apt/lists/*

RUN apt-get -o Acquire::Retries=5 update && apt-get -o Acquire::Retries=5 install -y --no-install-recommends \
    make \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY CMakeLists.txt .
COPY include/ ./include/
COPY src/ ./src/

RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build

FROM ubuntu:22.04

WORKDIR /app

COPY --from=builder /app/build/ContactManager .
COPY --from=builder /app/build/contactList/ ./contactList/
COPY --from=builder /usr/lib/x86_64-linux-gnu/libsqlite3.so.0* /usr/lib/x86_64-linux-gnu/

VOLUME ["/data"]

ENV CONTACT_DB_PATH=/data/contacts.db
ENV CONTACT_DATA_DIR=/app/contactList

ENTRYPOINT ["./ContactManager"]
