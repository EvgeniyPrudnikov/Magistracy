#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <vector>
#include <cstring>
#include <iostream>

using namespace std;


int main(int argc, char *argv[])
{
    vector<vector<char *>> commands;

    const char *sep = "|";

    int param_pos = 1;
    int sep_pos = 0;
    int cmd_cnt = 0;
    while (param_pos < argc)
    {
        commands.push_back(vector<char *>());

        for (int i = param_pos; i < argc; ++i)
        {
            if (!strcmp(argv[i], sep))
            {
                commands[cmd_cnt].push_back(NULL);

                if (param_pos == 1)
                {
                    cerr << "Empty arguments before | " << endl;

                    return 2;

                } else if (param_pos == sep_pos + 1)
                {
                    cerr << "Empty arguments between | | " << endl;
                    return 2;

                } else if (param_pos + 1 == argc)
                {
                    cerr << "Empty arguments after | " << endl;

                    return 2;
                }

                sep_pos = param_pos;
                param_pos++;

                break;
            }

            commands[cmd_cnt].push_back(argv[i]);

            param_pos++;
        }
        cmd_cnt++;
    }

    if (cmd_cnt > 0)
    {
        commands[cmd_cnt - 1].push_back(NULL);
    }

    pid_t children[cmd_cnt];

    if (cmd_cnt == 0)
    {
        cerr << "Programm started without arguments. Exit." << endl;
        return 2;

    } else if (cmd_cnt == 1)
    {
        pid_t pid = fork();

        if (pid == -1)
        {
            perror("fork");
            return 1;

        } else if (pid == 0)
        {
            char **args = commands[0].data();

            if (execvp(args[0], args) == -1)
            {
                cerr << "Wrong command " << args[0] << endl;
                return 1;
            }

        } else
        {
            children[0] = pid;
        }

    } else
    {
        int pipefds[cmd_cnt - 1][2];

        for (int k = 0; k < cmd_cnt - 1; ++k)
        {
            if (pipe((int *) pipefds[k]) == -1)
            {
                perror("pipe");
                return 1;
            }
        }


        for (int i = 0; i < cmd_cnt; ++i)
        {
            int curr_pipe = i == 0 ? 0 : i - 1;
            int next_pipe = curr_pipe + 1;

            pid_t pid = fork();

            if (pid == -1)
            {
                perror("fork");
                return 1;

            } else if (pid == 0)
            {

                char **args = commands[i].data();

                if (i == 0)
                {
                    close(pipefds[curr_pipe][0]);
                    dup2(pipefds[curr_pipe][1], 1);

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

                if (execvp(args[0], args) == -1)
                {
                    cerr << "Wrong command " << args[0] << endl;
                    return 1;
                }

            } else
            {
                children[i] = pid;
            }
        }

        for (int l = 0; l < cmd_cnt - 1; ++l)
        {
            close(pipefds[l][0]);
            close(pipefds[l][1]);
        }
    }
    int exitCode = 0;
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
