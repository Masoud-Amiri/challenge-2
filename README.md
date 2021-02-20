# Shared Memory vs UNIX domain socket.
daemon1: ID =1 --> send `Hello from 1.` in `shared mem` and then log the recived messages in `/tmp/daemon1_log.txt`

daemon2: ID =2 --> send `Hello from 2.` in `shared mem` & `UNIX domain socket` and log the recived messages in `/tmp/daemon1_log.txt`

# Compile and Run
for compile please run `make all`
To test please exec `./daemon1` first then run `./daemon2`.

Check log files in `/tmp/daemon1_log.txt` , `/tmp/daemon2_log.txt`
