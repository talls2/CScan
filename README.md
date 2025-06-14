# CScan
A Port Scanner made in C

## Features

- TCP Port Scanning
- UDP Port Scanning
- Multithreaded (Fast Scanning)
- Exports Open Ports to `results.txt`
- Simple and lightweight

## Getting Started

Clone the repository:

``` bash 
git clone https://github.com/talls2/CScan
```

Compile the project:

``` bash
gcc main.c -o cscan -lpthread
```

Run a Scan:

``` bash
./cscan <tcp|udp> <target IP> <start port> <end port>
```

## Examples

#### TCP Scan example:
``` bash
./cscan tcp 127.0.0.1 20 100
```
#### UDP Scan example:
``` bash
./cscan udp 127.0.0.1 50 60
```

## Output

Open ports are printed in the console.

Open ports are saved to a file named results.txt.

Example of results.txt:

``` sql
TCP Port 22 OPEN
UDP Port 53 OPEN|FILTERED
```

## Resources
Made for educational purposes and learning C programming with networking.

