#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
using namespace std;
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
        remove(&((*t) -> s));
    }
    pnode tmp = *t;
    *t = (*t) -> b;
    free(tmp);
}

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
pnode test = NULL;
queue <char> *path = new queue <char>;
int main() {
    int depth = 0;
    char cmd[100];
    char cm[4];
    char tmp_path[31];
    char numb[11] = {'0'};
    cmd[99] = '\0';
    cm[3] = '\0';
    //int value = -1;
    int fd1[2], fd2[2];
    pid_t pr;
    if (pipe(fd1) == -1 || pipe(fd2) == -1) {
        perror("pipe\n");
        exit(0);
    }
    pr = fork();
    if (pr < 0) {
        write(1, "Can't create process\n", 22);
    } else if (pr > 0) {
        while(read(0, cmd, 100)) {
            close(fd1[0]);
            close(fd2[1]);
            int j = 0;
            int i = 0;
            while (cmd[i] != ' ') {
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
                        path->push(tmp_path[i]);
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
                        path->push(tmp_path[i]);
                    } else if (tmp_path[i] == '\0') {
                        break;
                    } else {
                        write(1, "wrong path\n", 12);
                        exit(1);
                    }
                }
            } else {
                write(1, "wrong command\n", 50);
                exit(1);
            }

        }
    } else {
        while ((read(fd1[0], cm, 4) > 0) && (read(fd1[0], path, path->size()) > 0) && (read(fd1[0], numb, strlen(numb))) > 0) {
            close(fd1[1]);
            close(fd2[0]);
            if (strcmp(cm, "add") == 0) {
                int value = stoi(numb);
                if (test == NULL) {
                    test = node_create(value);
                    path->pop();
                } else {
                    add(&test, value, path);
                    depth++;
                }
            } else if (strcmp(cm, "rmv") == 0) {
                if (test == NULL) {
                    write(fd2[1], "empty tree\n", 11);
                } else {
                    remove(search(&test, path));
                }
            } else if (strcmp(cm, "prt") == 0) {
                write(fd2[1], "fine?\n", 11);
                tree_print(test, 99);
            }
        }

    }
    return 0;
}