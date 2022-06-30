FROM golang:1.18-alpine
WORKDIR /app
COPY go.mod /app
COPY go.sum /app
COPY main.go /app
RUN set -ex && CGO_ENABLED=0 GOOS=linux go build

from alpine:latest
WORKDIR /app

COPY --from=0 /app/sensors-esp32 /app

CMD ["/app/sensors-esp32"]
