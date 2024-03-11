FROM ubuntu:24.04
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
      build-essential cmake ninja-build git pkg-config \
      qt6-base-dev qt6-base-dev-tools libgl1-mesa-dev \
      libavformat-dev libavcodec-dev libavutil-dev libswscale-dev \
      xvfb ca-certificates \
 && rm -rf /var/lib/apt/lists/*
WORKDIR /work
