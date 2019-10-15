#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <processthreadsapi.h>
#include <windows.h>

using namespace std;
typedef struct node node,* pnode;
struct node {
    pnode s;
    pnode b;
    int val;
};
pnode node_create(int val) {
    pnode new_node = (pnode)malloc(sizeof(node));
    if (new_node) {
        new_node -> val = val;
        new_node -> s = NULL;
        new_node -> b = NULL;
    }
    return new_node;
}
pnode* search(pnode* t, queue <char> *path) {
    if (!(*t) && !path->empty()) {
        return NULL;
    }
    if (!path->empty()) {
        char c = path->front();
        path->pop();
        if (c == 's') {
            return search(&(*t) -> s, path);
        } else if (c == 'b') {
            return search(&(*t) -> b, path);
        }
        return NULL;
    }
    return t;
}
bool add(pnode* t, int val, queue <char> *path) {
    if (!(*t) && path->empty()) {
        (*t) = node_create(val);
        return true;
    }
    pnode* pr = search(t, path);
    if (!pr) {
        return false;
    }
    pnode new_node = node_create(val);
    if (!new_node) {
        return false;
    }
    new_node -> b = (*pr);
    (*pr) = new_node;
    return true;
}
void remove(pnode* t) {
    while((*t) -> s != NULL){
        delete(&((*t) -> s));
    }
    pnode tmp = *t;
    *t = (*t) -> b;
    free(tmp);
}
/*int cnt(pnode t) {
    if (!t) {
        return 0;
    }
    return cnt(t -> b) + cnt(t -> s) + 1;
}*/
void tree_print(pnode t, int depth) {
    if (t) {
        for (int i = 0; i < depth; ++i) {
            printf("\t");
        }
        printf("%d",t -> val);
        printf("\n");
        tree_print(t -> s, depth + 1);
        tree_print(t -> b, depth);
    }
}
/*
int my_fork(int argc, char *argv[]) {

    int i, ret = 0;
    size_t sz = 0;
    char *cmd = NULL;
    char *p;

    for (i = 0; i < argc; i++)
    {
        size_t ssz;

        if (!argv[i])
            break;

        ssz = strlen(argv[i]);

        if (
            (
                (ssz == 2U) &&
                (!memcmp(argv[i], (void*)&"-k", 2U))
            )
            ||
            (
                (ssz == 9U) &&
                (!memcmp(argv[i], (void*)&"--nonloop", 9U))
            )
        )
            continue;

        if (!(p = realloc(cmd, (size_t)(sz + ssz + 2U))))
        {
            if (cmd)
                free(cmd);
            return -1;
        }

        cmd = p;
        memcpy((void*)(p + sz), argv[i], ssz);
        sz += ssz;
        p[sz] = 0x20;
        p[++sz] = 0x0;
    }

    sz = ((sz) ? (sz - 1) : 0);

    if ((!cmd) || (!sz))
    {
        errno = EINVAL;
        return -1;
    }

    cmd[sz] = 0x0;

    do
    {
        STARTUPINFO si;
        PROCESS_INFORMATION pi;

        memset(&si, 0, sizeof(si));
        memset(&pi, 0, sizeof(pi));
        si.cb = sizeof(si);
        si.dwFlags |= DETACHED_PROCESS;

        if (!CreateProcess(
                    NULL,
                    cmd,
                    NULL,
                    NULL,
                    FALSE,
                    CREATE_DEFAULT_ERROR_MODE | CREATE_UNICODE_ENVIRONMENT | DETACHED_PROCESS,
                    NULL,
                    NULL,
                    &si,
                    &pi
                )
           )
        {
            ret = -((int)GetLastError());
        }
    }
    while (0);

    free(cmd);
    return ret;
    (void) argc;
    (void) argv;

    switch(fork())
    {
        case 0:
        {
            int cfd;
            (void) signal(SIGHUP, SIG_IGN);

            if ((cfd = open("/dev/null", O_RDWR, 0)) >= 0)
            {
                dup2(cfd, STDIN_FILENO);
                dup2(cfd, STDOUT_FILENO);
                dup2(cfd, STDERR_FILENO);

                if (cfd > 2)
                {
                    close(cfd);
                }
            }
            if ((getppid() != 1) && (setsid() < 0))
            {
                return -1;
            }
            (void) sleep(12);
            return (int)getpid();
        }
        case -1:
        {
            return -1;
        }
        default:
        {
            (void) signal(SIGCHLD, SIG_IGN);
            errno = 0;
            return 0;
        }
    }
    return 100;
}*/
int main() {
    STARTUPINFO info={sizeof(info)};
    PROCESS_INFORMATION processInfo;
    CreateProcess("D:\\Study\\Labs2course\\OS_lab2\\cmake-build-debug\\lab2.exe", NULL, NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo);
    //int tmp_fork = fork();
    for(;;){
        Sleep(1000);
        std::cout<<"1"<<std::endl;
    }
    pnode test = NULL;
    queue <char> *path = new queue <char>;
    //path->push('s');
    test = node_create(72);
    path->push('b');
    add(&test, 42, path);
    tree_print(test, 2);
    cout<<endl<<endl;
    path->push('b');
    add(&test, 32, path);
    tree_print(test, 2);
    cout<<endl<<endl;
    path->push('b');
    add(&test, 22, path);
    tree_print(test, 2);
    /*char cmd[40];
    while (scanf("%s", cmd)) {
        if (strcmp(cmd,"add") == 0) {
            scanf("%s", cmd);
            if (cmd != -1) {

            } else {
                printf("wrong value type help for info\n");
                while (getchar() != '\n') {}
            }
        } else if (strcmp(cmd,"delete") == 0) {
            scanf("%s", cmd);
            cardsuit carded = string_to_card(cmd);
            if (carded != DEFAULT) {
                delete_node(&test, carded);
            } else {
                printf("wrong value type help for info\n");
                while (getchar() != '\n') {
                }
            }
        } else if (strcmp(cmd, "help") == 0) {
            printf("<command> <value or missing value>\n commands:\n trm - prints a value of non terminal tree node on max depth\n add <value> - creates a node \n help - type for manual \n exit - type to stop the program \n delete <value> - deletes node \n print - print a tree \n range of values and their comparison to integer numbers:\n ONE = 1\n TWO = 2\n THREE = 3\n FOUR = 4\n FIVE = 5\n SIX = 6\n CLUBS = 7\n DIAMONDS = 8\n HEARTS = 9\n SPADES = 10\n");
        } else if (strcmp(cmd, "exit") == 0) {
            break;
        } else if (strcmp(cmd, "trm") == 0) {
            if (test.root && (test.root -> l != NULL || test.root -> r != NULL)) {
                printf("#%s#\n", card_to_string(non_trm_with_md(test.root,0).t->val));
            } else {
                printf("no such value!\n");
            }
        } else if (strcmp(cmd,"print") == 0) {
            tree_print(test.root, 0);
        } else {
            printf("invalid command, type help for info\n <command> <value> \n");
            while (getchar() != '\n') {

            }
        }
    }*/
    return 0;
}