#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


using namespace std;


// Program used to mimic terminal command:  'ps -A | grep | wc -l'
int main(int argc, char * argv[]) {
    pid_t pid; //process id

    int pipe1[2]; // 2 file descriptors created
    int pipe2[2]; // since the command has 2 pipes
    int read = 0; // reading data from a pipe index
    int write = 1; //writing data to a pipe index


    if (pipe(pipe1) < 0) { //Pipe fail
        perror("pipe error");
        return EXIT_FAILURE;
    }

    if (pipe(pipe2) < 0) { //Pipe fail
        perror("pipe error");
        return EXIT_FAILURE;
    }


    if ((pid = fork()) < 0) {
        perror("fork error");
        return EXIT_FAILURE;

    } else if (pid == 0) { //child

        if ((pid = fork()) < 0) {
            perror("fork error");
            return EXIT_FAILURE;

        } else if (pid == 0) { //grand child

            if ((pid = fork()) < 0 ) {
                perror("fork error");
                return EXIT_FAILURE;

            } else if (pid == 0) { //great grand child

                close(pipe2[write]); //does not touch 2nd pipe
                close(pipe2[read]); //does not touch 2nd pipe
                close(pipe1[read]); //does not read anything.

                dup2(pipe1[write], write);
                execlp("ps", "ps", "-A", NULL);

            } else { //grandchild else


                close(pipe1[write]); //does not write to grandchild. -> reads from granchild
                close(pipe2[read]); //writes to 2nd pipe, so does not read


                dup2(pipe1[read],read); //standard input is now piped
                dup2(pipe2[write], write);
                execlp("grep", "grep", argv[1], NULL); //change

            }

        } else { //child else

            close(pipe2[write]); //does not write to pipe
            close(pipe1[read]);  //does not directly touch 1st pipe
            close(pipe1[write]); //does not directly touch 1st pipe

            dup2(pipe2[read], read);
            execlp("wc", "wc", "-l", NULL);

        }
    } else { //parent else

        close(pipe1[read]);
        close(pipe1[write]);
        close(pipe2[read]);
        close(pipe2[write]);

        wait(NULL);

    }
    return 0;
}


