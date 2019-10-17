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

int main() {
   // int fd1[2], fd2[2], temp_fork, cnt = 0;
    pnode test = NULL;
    queue <char> *path = new queue <char>;
    /*if (pipe(fd1) == -1 || pipe(fd2) == -1) {
        printf("Can\'t create pipe\n");
        exit(0);
    }*/
    char cmd[40];
    while (scanf("%s", cmd)) {
        if (strcmp(cmd,"add") == 0) {
            char tmp_path[40];
            scanf("%s", tmp_path);
            if (tmp_path != nullptr) {
                for (int i = 0; i < 40; i++) {
                    if (tmp_path[i] == 's' || tmp_path[i] == 'b') {
                        path->push(tmp_path[i]);
                    } else if (tmp_path[i] == '\0') {
                        break;
                    } else {
                        std::cout << "wrong path\n" << std::endl;
                        exit;
                    }
                }
                int value;
                scanf("%d", &value);
                add(&test, value, path);
                tree_print(test, 3);
            }

        } else {
            printf("hi\n");
        }
    }
    tree_print(test, 3);

    return 0;
}