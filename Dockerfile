FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    g++ \
    make \
    cmake \
    git \
    libasio-dev \
    libssl-dev \
    libboost-all-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . /app

# Ensure txt-files directory and runtime data files exist
RUN mkdir -p txt-files && \
    touch users.txt beds.txt waitlist.txt appointments.txt billing.txt emergency.txt investigations.txt medical_records.txt patients.txt doctors.txt staff.txt \
          txt-files/users.txt txt-files/beds.txt txt-files/waitlist.txt txt-files/appointments.txt txt-files/billing.txt txt-files/emergency.txt txt-files/investigations.txt txt-files/medical_records.txt txt-files/patients.txt txt-files/doctors.txt txt-files/staff.txt && \
    chmod -R 666 *.txt txt-files/*.txt

# Compile main.cpp (which includes modules/*.cpp) with current directory in include path
RUN g++ -std=c++17 -O2 -I. main.cpp -o arogya_app -lpthread

EXPOSE 10000

CMD ["./arogya_app"]