# Use a lightweight Ubuntu image with essential build tools
FROM ubuntu:22.04

# Prevent interactive prompts during installation
ENV DEBIAN_FRONTEND=noninteractive

# Install g++, make, cmake, git, and boost dependencies
RUN apt-get update && apt-get install -y \
    g++ \
    make \
    cmake \
    git \
    libasio-dev \
    libssl-dev \
    libboost-all-dev \
    && rm -rf /var/lib/apt/lists/*

# Set working directory inside container
WORKDIR /app

# Copy all project files into the container
COPY . /app

# Pre-create text database files to prevent file-opening crashes on first boot
RUN touch users.txt beds.txt waitlist.txt && chmod 666 users.txt beds.txt waitlist.txt

# Compile your C++ Crow application with proper optimization and link flags
RUN g++ -std=c++17 -O2 main.cpp user.cpp -o arogya_app -lpthread -lboost_system

# Render expects the web service to listen on port 10000
EXPOSE 10000

# Run the compiled application
CMD ["./arogya_app"]