#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


using namespace std;


// Program used to mimic terminal command:  'ps -A | grep | wc -l'
int main(int argc, char * argv[]) {
    pid_t pid; //process id

    int filedescriptor1[2]; // 2 file descriptors created
    int filedescriptor2[2]; // since the command has 2 pipes
    int status;
    int read = 0; // reading data from a pipe index
    int write = 1; //writing data to a pipe index


    if (pipe(filedescriptor1) < 0) { //Pipe fail
        perror("pipe error");
        return EXIT_FAILURE;
    }

    if (pipe(filedescriptor2) < 0) { //Pipe fail
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
                close(filedescriptor2[write]); //does not touch 2nd pipe
                close(filedescriptor2[read]); //does not touch 2nd pipe
                close(filedescriptor1[read]); //does not read anything.
                execlp("ps", "ps", "-A", NULL);

            } else { //grandchild else

                close(filedescriptor1[write]); //does not write to grandchild. -> reads from granchild

                close(filedescriptor2[read]); //writes to 2nd pipe, so does not read

                execlp("grep", "grep", argv[1], NULL);

            }

        } else { //child else
            cout << "child";
            close(filedescriptor2[write]); //does not write to pipe
            close(filedescriptor1[read]);  //does not directly touch 1st pipe
            close(filedescriptor1[write]); //does not directly touch 1st pipe
            execlp("wc", "wc", "-l", NULL);
        }
    } else { //parent else

        wait(NULL);
        cout << "parent";
        exit(0);


    }
    return 0;
}


