# HTTP Packet Sniffer
## 1. Project Overview

This project is a simple HTTP packet sniffer implemented in C using the PCAP API.

The sniffer captures TCP packets from a network interface and prints the following information:

- Ethernet Header
  - Source MAC address
  - Destination MAC address
- IP Header
  - Source IP address
  - Destination IP address
- TCP Header
  - Source port
  - Destination port
- HTTP Message
  - TCP payload data when the packet uses HTTP port 80

This repository also includes a simple HTTP sender program for generating HTTP traffic during testing.

---

## 2. Directory Structure

```text
http_sniffer/
├── Makefile
├── README.md
├── include/
│   └── myheader.h
└── src/
    ├── sniff_http.c
    └── send_http.c
```

| File | Description |
|---|---|
| `include/myheader.h` | Defines Ethernet, IP, and TCP header structures |
| `src/sniff_http.c` | Captures packets using libpcap and prints Ethernet/IP/TCP/HTTP information |
| `src/send_http.c` | Sends a simple HTTP request to a target server |
| `Makefile` | Builds the sniffer and sender programs |

---

## 3. Test Environment

The experiment was performed using three Ubuntu virtual machines on VirtualBox.

```text
Client VM   → sends an HTTP request
Server VM   → runs an HTTP server
Sniffer VM  → captures HTTP packets
```

Example IP configuration:

```text
Client VM   : 10.0.2.5
Server VM   : 10.0.2.6
Sniffer VM  : same virtual network
```

The virtual machines were connected to the same VirtualBox network.

Recommended VirtualBox network setting:

```text
Adapter: NAT Network or Internal Network
Promiscuous Mode: Allow All
```

For the Sniffer VM, promiscuous mode should be enabled so that it can capture packets transmitted between the Client VM and the Server VM.

---

## 4. Requirements

Install required packages on the Sniffer VM and Client VM.

```bash
sudo apt update
sudo apt install gcc make libpcap-dev git
```

The Server VM needs Python 3 to run a simple HTTP server.

```bash
sudo apt install python3
```

---

## 5. Build

Clone this repository.

```bash
git clone https://github.com/ddoyon7/http_sniffer.git
cd http_sniffer
```

Build the programs.

```bash
make
```

After compilation, two executable files are created:

```text
sniff_http
send_http
```

To remove compiled files:

```bash
make clean
```

---

## 6. How to Run the Experiment

### Step 1. Run HTTP Server on Server VM

On the Server VM, start a simple HTTP server on port 80.

```bash
sudo python3 -m http.server 80 
```

### Step 2. Run Packet Sniffer on Sniffer VM

On the Sniffer VM, run the packet sniffer.

```bash
sudo ./sniff_http
```

The program captures packets using this BPF filter:

```text
tcp port 80
```

In the current implementation, the network interface name is set in `src/sniff_http.c`.

```c
pcap_open_live("enp0s3", BUFSIZ, 1, 1000, errbuf);
```

If the interface name is different, check it with:

```bash
ip link
```

Then modify the interface name in the source code.

Example:

```c
pcap_open_live("ens33", BUFSIZ, 1, 1000, errbuf);
```

After modifying the source code, rebuild the program.

```bash
make clean
make
```

### Step 3. Send HTTP Request from Client VM

On the Client VM, send an HTTP request to the Server VM.

```bash
./send_http 10.0.2.6 80
```

Here:

```text
10.0.2.6 → Server VM IP address
80       → HTTP server port
```

The sender program sends an HTTP request containing the message:

```text
Hello World!
```

---

## 7. Expected Output

When an HTTP packet is captured successfully, the Sniffer VM prints output similar to the following:

```text
=======Ethernet Header=======
src mac : 08:00:27:f0:87:45
dst mac : 08:00:27:34:e0:b8

=======IP Header=======
src ip : 10.0.2.5
dst ip : 10.0.2.6

=======TCP Header=======
src port : 48202
dst port : 80

=======HTTP=======
message :
POST / HTTP/1.1
Host: 10.0.2.6:80
Content-Type: text/plain
Content-Length: 12
Connection: close

Hello World!
```

Some captured TCP packets may show:

```text
no data
```

This is normal. TCP connection setup packets such as SYN, SYN-ACK, and ACK do not contain HTTP payload data.

---

## 8. Notes

### Promiscuous Mode

The sniffer uses promiscuous mode when opening the network interface.

```c
pcap_open_live("enp0s3", BUFSIZ, 1, 1000, errbuf);
```

The third argument `1` enables promiscuous mode in libpcap.

However, promiscuous mode only allows the NIC to accept frames delivered to it.  
If the virtual switch or physical switch does not forward the traffic to the Sniffer VM, the Sniffer VM cannot capture it.

For this reason, the Sniffer VM should be configured with promiscuous mode enabled in VirtualBox.

```text
Promiscuous Mode: Allow All
```

### Permission

Packet capture usually requires root privileges.

```bash
sudo ./sniff_http
```
### About POST Requests

The `send_http` program sends an HTTP `POST` request containing the message `Hello World!`.

When using Python's built-in HTTP server:

```bash
sudo python3 -m http.server 80
```

the server may respond with an error such as:

```text
Unsupported method ('POST')
```

This is because Python's default `http.server` is mainly designed to serve files using methods such as `GET` and `HEAD`, and it does not handle `POST` requests like a real web application server.

However, this does not affect the purpose of this experiment.

The goal of this project is not to test whether the server processes the HTTP request successfully. The goal is to capture the HTTP message at the packet level and print the TCP payload.

Therefore, even if the server returns an error for the `POST` request, the sniffer can still capture and display the HTTP request message:

```text
POST / HTTP/1.1
Host: 10.0.2.6:80
Content-Type: text/plain
Content-Length: 12
Connection: close

Hello World!
```

In other words, the experiment is successful as long as the Sniffer VM captures and prints the HTTP request payload.