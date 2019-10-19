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
    int fd1[2], fd2[2];
    int pr;
    pr = fork();
    if (pipe(fd1) == -1 || pipe(fd2) == -1) {
        printf("Can\'t create pipe\n");
        exit(0);
    }
    if (pr < 0) {
        printf("can't create process\n");
    } else if (pr > 0) {
        close(fd1[0]);
        close(fd2[1]);
        int depth = 0;
        char cmd[41];
        while(read(fd1[1], cmd, 1)) {
            char cm[4];
            for (int i = 0; i < 3; i++) {
                printf("%c", cmd[0] );
                cm[i] = cmd[i];
            }
            int i = 4;
            int j = 0;
            char tmp_path[31];
            while(cmd[i] != ' ') {
                tmp_path[j] = cmd[i];
                i++;
                j++;
            }
            int k = i + 1;
            j = 0;
            char numb[5];
            while (cmd[k] != '\0') {
                numb[j] = cmd[k];
            }
            if (strcmp(cm,"add") == 0) {
                //read(fd1[0], tmp_path, 31*sizeof(char));
                //scanf("%s", tmp_path);
                for (int i = 0; i < 31; i++) {
                    if (tmp_path[i] == 's' || tmp_path[i] == 'b') {
                        path->push(tmp_path[i]);
                    } else if (tmp_path[i] == '\0') {
                        break;
                    } else {
                        printf("wrong path\n");
                        exit(0);
                    }
                    int value = stoi(numb);
                    //write(fd1[1], &value, sizeof(int));
                    /*if (test == NULL) {
                        test = node_create(value);
                        path->pop();
                    } else {
                        add(&test, value, path);
                        depth++;
                    }*/
                }
            } else if (strcmp(cmd, "rmv") == 0) {
                for (int i = 0; i < 31; i++) {
                    if (tmp_path[i] == 's' || tmp_path[i] == 'b') {
                        path->push(tmp_path[i]);
                    } else if (tmp_path[i] == '\0') {
                        break;
                    } else {
                        printf("wrong path\n");
                        exit(0);
                    }
                }
            }

        }
        close(fd1[0]);
        close(fd2[1]);
    } else {
        char cm[4];
        int value;
        char tmp_path[31];
        int depth = 0;
        //char cmd[41];
        close(fd1[0]);
        close(fd2[1]);
        while (read(fd2[0], cm, 4)) {
            if (strcmp(cm, "add")) {
                close(fd1[1]);
                close(fd2[0]);
                write(fd2[1], test, sizeof(pnode));
                write(fd2[1], path, path->size());
                write(fd2[1], &value, sizeof(int));
                write(fd2[1], tmp_path, 31);
                close(fd1[0]);
                close(fd2[1]);
                add(&test, value, path);
                depth++;
            }
        }
        //while (read(fd1[0], cmd, 41*sizeof(char))) {

        //}

        close(fd1[0]);
        close(fd1[1]);
    }
    return 0;
}