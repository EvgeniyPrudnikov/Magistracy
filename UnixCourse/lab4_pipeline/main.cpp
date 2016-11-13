#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <vector>
#include <cstring>
#include <iostream>

using namespace std;


int main(int argc, char *argv[])
{
    int exitCode = 0;
    vector<vector<char *>> commands;

    char *sep = (char *) "|";

    int param_p = 1;
    int sep_p = 0;
    int cmd_cnt = 0;
    while (param_p < argc)
    {
        commands.push_back(vector<char *>());

        for (int i = param_p; i < argc; ++i)
        {
            if (!strcmp(argv[i], sep))
            {

                if (param_p == 1)
                {
                    cerr << "Wrong arguments before | " << endl;
                    exitCode = 2;
                    return exitCode;
                }

                if (param_p == sep_p + 1)
                {
                    cerr << "Wrong arguments between | " << endl;
                    exitCode = 2;
                    return exitCode;
                }

                sep_p = param_p;
                param_p++;

                if (param_p == argc)
                {
                    cerr << "Wrong arguments after | " << endl;
                    exitCode = 2;
                    return exitCode;
                }

                break;
            }

            commands[cmd_cnt].push_back(argv[i]);

            param_p++;
        }
        cmd_cnt++;
    }


    int pipefds[cmd_cnt - 1][2];

    for (int k = 0; k < cmd_cnt - 1; ++k)
    {
        if (pipe((int *) pipefds[k]) == -1)
        {
            perror("pipe");
            return 1;
        }
    }

    pid_t children[cmd_cnt];

    for (int i = 0; i < cmd_cnt; ++i)
    {
        int curr_pipe = i == 0 ? 0 : i - 1;
        int next_pipe = curr_pipe + 1;

        pid_t pid = fork();

        if (pid == 0)
        {

            char **args = commands[i].data();


            if (i == 0)
            {

                dup2(pipefds[curr_pipe][1], 1);
                close(pipefds[curr_pipe][0]);


            } else if (i == cmd_cnt - 1)
            {
                dup2(pipefds[curr_pipe][0], 0);

                for (int j = 0; j < next_pipe; ++j)
                {
                    close(pipefds[j][1]);
                }
            } else
            {
                dup2(pipefds[curr_pipe][0], 0);
                dup2(pipefds[next_pipe][1], 1);

                for (int j = 0; j <= next_pipe; ++j)
                {
                    if (j == next_pipe)
                    {
                        close(pipefds[j][0]);
                    } else
                    {
                        close(pipefds[j][1]);
                    }
                }
            }

            execvp(args[0], args);

        } else
        {
            children[i] = pid;
        }
    }

    for (int l = 0; l < cmd_cnt - 1; ++l)
    {
        for (int i = 0; i < 2; ++i)
        {
            close(pipefds[l][i]);
        }
    }


    int status;
    for (int i = 0; i < cmd_cnt; ++i)
    {
        waitpid(children[i], &status, 0);
        if (WIFEXITED(status))
        {
            if (WEXITSTATUS(status) != 0)
            {
                exitCode = 1;
            }
        }
    }

    return exitCode;
}