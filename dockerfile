# Use a lightweight Ubuntu image with essential build tools
FROM ubuntu:22.04

# Prevent interactive prompts during installation
ENV DEBIAN_FRONTEND=noninteractive

# Install g++, make, cmake, and git
RUN apt-get update && apt-get install -y \
    g++ \
    make \
    cmake \
    git \
    libasio-dev \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

# Set working directory inside container
WORKDIR /app

# Copy all project files into the container
COPY . /app

# Compile your C++ Crow application
# (Replace main.cpp and user.cpp with all your actual .cpp files if you have more)
RUN g++ -std=c++17 main.cpp user.cpp -o arogya_app -lpthread

# Render requires web apps to listen on port 10000 (or use PORT env variable)
# Make sure your main.cpp port or a startup script listens on the PORT environment variable if needed, 
# or configure Render to map port 18080.
EXPOSE 18080

# Run the compiled application
CMD ["./arogya_app"]