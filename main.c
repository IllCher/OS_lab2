#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include<pthread.h>
#include "d_queue.h"
#include <math.h>
typedef struct ans ans;
struct ans {
    char cm[4];
    char path[32] ;
    int val;
};
typedef struct node node,*pnode;
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
pnode* search(pnode* t, queue *path) {
    if (!(*t) && !q_is_empty(path)) {
        return NULL;
    }
    if (!q_is_empty(path)) {
        char c = q_front(path);
        pop(path);
        if (c == 's') {
            return search(&(*t) -> s, path);
        } else if (c == 'b') {
            return search(&(*t) -> b, path);
        }
        return NULL;
    }
    return t;
}
bool add(pnode* t, int val, queue *path) {
    if (!(*t) && q_is_empty(path)) {
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
void rmv(pnode* t) {
    while((*t) -> s != NULL){
        rmv(&((*t) -> s));
    }
    pnode tmp = *t;
    *t = (*t) -> b;
    free(tmp);
}

void tree_print(pnode t, int depth) {
    setvbuf(stdout, (char*)NULL, _IONBF, 0);
    if (t) {
        for (int i = 0; i < depth; ++i) {
            printf("\t");
        }
        printf("%d\n",t -> val);
        tree_print(t -> s, depth + 1);
        tree_print(t -> b, depth);
    }
}
ans* parser(char cmd[100]) {
    ans* parsed = (ans*)malloc(sizeof(ans));
    char numb[11] = {'\0'};
    int j = 0;
    int i = 0;
    while (cmd[i] != ' ' && cmd[i] != '\n') {
        parsed->cm[j] = cmd[i];
        i++;
        j++;
    }
    j = 0;
    i++;
    while (cmd[i] != ' ' && cmd[i] != '\n') {
        parsed->path[j] = cmd[i];
        i++;
        j++;
    }
    parsed->path[j] = '\0';
    if (cmd[i] == ' ') {
        i++;
        j = 0;
        while (cmd[i] != '\n' && cmd[i] != '\0') {
            numb[j] = cmd[i];
            j++;
            i++;
        }
    }
    parsed->val = atoi(numb);
}
bool check_path(char path[32]) {
    for (int i = 0; i < 31; i++) {
        if (path[i] == '\0') {
            break;
        } else if (path[i] != 's' && path[i] != 'b') {
            return false;
        }
    }
    return true;
}
int main() {
    setvbuf(stdout, (char*)NULL, _IONBF, 0);
    pnode test = NULL;
    queue *path = q_create();
    int depth = 0;
    char cmd[100] = {'\0'};
    ans* parsed = (ans*)malloc(sizeof(ans));
    int fd1[2];
    int fd2[2];
    pid_t pr = -1;
    if (pipe(fd1) == -1 || pipe(fd2) == -1) {
        perror("pipe\n");
        exit(1);
    }
    pr = fork();
    if (pr < 0) {
        write(1, "Can't create process\n", 22);
    } else if (pr > 0) {
        close(fd1[0]);
        close(fd2[1]);
        while(read(0, cmd, 100)) {
        parsed = parser(cmd);
            if (strcmp(parsed->cm,"add") == 0) {
                if (!check_path(parsed->path)) {
                    write(1, "wrong path\n", 12);
                    exit(1);
                }
            } else if (strcmp(parsed->cm, "rmv") == 0) {
                if (!check_path(parsed->path)) {
                    write(1, "wrong path\n", 12);
                    exit(1);
                }
            } else if (strcmp(parsed->cm, "prt") == 0){}
            else {
                write(1, "wrong command\n", 50);
                exit(1);
            }
            int red = 0;
            write(fd1[1], parsed->cm, 4);
            write(fd1[1], &parsed->val, 4);
            write(fd1[1], parsed->path, 32);
        }
    } else {
        close(fd1[1]);
        close(fd2[0]);
        while (1) {
            read(fd1[0], parsed->cm, 4);
            read(fd1[0], &parsed->val, 4);
            read(fd1[0], parsed->path, 32);
            if (strcmp(parsed->cm, "add") == 0) {
                int value = parsed->val;
                if (test == NULL) {
                    test = node_create(value);
                    pop(path);
                } else {
                    add(&test, value, path);
                    depth++;
                }
            } else if (strcmp(parsed->cm, "rmv") == 0) {
                if (test == NULL) {
                    write(fd2[1], "empty tree\n", 11);
                } else {
                    rmv(search(&test, path));
                }
            } else if (strcmp(parsed->cm, "prt") == 0) {
                if (test == NULL) {
                    write(fd2[1], "empty tree\n", 11);
                } else {tree_print(test, 1);}
            } else {
                exit(1);
                //write(fd2[1], "bad command\n", 12);
            }
        }

    }
    return 0;
}