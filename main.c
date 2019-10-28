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
    write(1, "a", 1);
    if (t) {
        for (int i = 0; i < depth; ++i) {
            //printf("\t");
            write(1, "\t", 1);
        }
        //write(1, &t->val, sizeof(int));
        //printf("a\n");
        //printf("%d\n",t -> val);
        write(1, "a", 1);
        write(1, "\n", 1);
        tree_print(t -> s, depth + 1);
        tree_print(t -> b, depth);
    }
}

int main() {
    //signal(SIGPIPE, SIG_IGN);

    setvbuf(stdout, (char*)NULL, _IONBF, 0);
    pnode test = NULL;
    queue *path = q_create();
    int depth = 0;
    char cmd[100] = {'\0'};
    char cm[4] = {'\0'};
    char tmp_path[31] = {'\0'};
    char numb[11] = {'\0'};
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
            //printf("%s\n", cmd);
            int j = 0;
            int i = 0;
            while (cmd[i] != ' ' && cmd[i] != '\n') {
                cm[j] = cmd[i];
                i++;
                j++;
            }
            j = 0;
            i++;
            while (cmd[i] != ' ' && cmd[i] != '\n') {
                tmp_path[j] = cmd[i];
                i++;
                j++;
            }
            tmp_path[j] = '\0';
            if (cmd[i] == ' ') {
                i++;
                j = 0;
                while (cmd[i] != '\n' && cmd[i] != '\0') {
                    numb[j] = cmd[i];
                    j++;
                    i++;
                }
            }
            if (strcmp(cm,"add") == 0) {
                for (int i = 0; i < 31; i++) {
                    if (tmp_path[i] == 's' || tmp_path[i] == 'b') {
                        push(path, tmp_path[i]);
                    } else if (tmp_path[i] == '\0') {
                        break;
                    } else {
                        write(1, "wrong path\n", 12);
                        exit(1);
                    }
                }
            } else if (strcmp(cm, "rmv") == 0) {
                for (int i = 0; i < 31; i++) {
                    if (tmp_path[i] == 's' || tmp_path[i] == 'b') {
                        push(path, tmp_path[i]);
                    } else if (tmp_path[i] == '\0') {
                        break;
                    } else {
                        write(1, "wrong path\n", 12);
                        exit(1);
                    }
                }
            } else if (strcmp(cm, "prt") == 0){

            } else {
                write(1, "wrong command\n", 50);
                exit(1);
            }
            int red = 0;
            red  = write(fd1[1], cm, 4);
            red = write(fd1[1], path->body, 32);
            red = write(fd1[1], numb, sizeof(numb));
        }
    } else {
        close(fd1[1]);
        close(fd2[0]);
        while (1) {
            //write(1, "shit\n", 5);
            int red = -1;
            red = read(fd1[0], cm, 4);
            printf("%s\n", cm);
            red = read(fd1[0], path->body, 32);
            printf("%s\n", path->body);
            red = read(fd1[0], numb, sizeof(numb));
            printf("%s\n", numb);

            if (strcmp(cm, "add") == 0) {
                int value = atoi(numb);
                if (test == NULL) {
                    test = node_create(value);
                    pop(path);
                } else {
                    add(&test, value, path);
                    depth++;
                }
                write(fd2[1], "\0", sizeof("\0"));
            } else if (strcmp(cm, "rmv") == 0) {
                if (test == NULL) {
                    write(fd2[1], "empty tree\n", 11);
                } else {
                    rmv(search(&test, path));
                }
            } else if (strcmp(cm, "prt") == 0) {
                write(fd2[1], "fine?\n", 11);
                tree_print(test, 99);
            } else {
                write(fd2[1], "bad command\n", 12);
            }
        }

    }
    return 0;
}