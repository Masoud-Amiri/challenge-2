#include<stdio.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/un.h>
#include <sys/socket.h>


#define BUF_SIZE 1024
#define SHM_KEY 0x1234
#define ID  (int)2
#define SocketName "/tmp/socket"

struct shmMsgBox {
   int SrcID;
   char Msgbuff[BUF_SIZE];
};

int main(int argc, char *argv[]) {
    int shmid;
    struct shmMsgBox *shmp;   
    pid_t pid, sid;
    FILE* log;


//fork
    pid = fork();
    if (pid < 0) {
        log = fopen("/tmp/daemon2_log.txt", "a+");
        fputs("fork error!!\n", log);
        fclose(log);
    }
    
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }
    
    sid = setsid();
    if (sid < 0) {
        log = fopen("/tmp/daemon2_log.txt", "a+");
        fputs("setsid() error!!\n", log);
        fclose(log);
        exit(EXIT_FAILURE);
    }
    
    if ((chdir("/")) < 0) {
        log = fopen("/tmp/daemon2_log.txt", "a+");
        fputs("change Dir to root error!!\n", log);
        fclose(log);
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

//UNIX socket
    int sock;
    struct sockaddr_un server;
    char buf[1024];
    char data[100];
    sprintf(data, "Hello from %d", ID);

    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        log = fopen("/tmp/daemon2_log.txt", "a+");
        fputs("opening stream socket error!!\n", log);
        fclose(log);
        exit(EXIT_FAILURE);
    }
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, SocketName);
    while (connect(sock, (struct sockaddr *) &server, sizeof(struct sockaddr_un)) < 0) {
        close(sock);
        log = fopen("/tmp/daemon2_log.txt", "a+");
        fputs("connecting stream socket error!!\n", log);
        fclose(log);
        exit(EXIT_FAILURE);
    }

//Shared Memory
    shmid = shmget(SHM_KEY, sizeof(struct shmMsgBox), 0644|IPC_CREAT);
    
    while(1)
    {
        write(sock, data, strlen(data)+1);
        log = fopen("/tmp/daemon2_log.txt", "a+");
        shmp = (struct shmMsgBox*) shmat(shmid, NULL, 0);
        if(shmp->SrcID != (int)ID){
            fputs(shmp->Msgbuff, log);
            fputc('\n', log);
            shmp->SrcID = ID;
            sprintf(shmp->Msgbuff, "Hello from %d", ID);
        }
        shmdt(shmp);
        fclose(log);
        sleep(1);
    }
    
    close(sock);
    unlink(SocketName);
    return 0;
}