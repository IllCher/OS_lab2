#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include<pthread.h>
#include "c_queue.h"
#include <math.h>
typedef struct ans ans;
struct ans {
    char cm[4];
    char path[32];
    char val[11];
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

ans* parser(char cmd[100]) {
    ans* parsed = (ans*)malloc(sizeof(ans));
    for (int i = 0; i < 11; i++) {
        parsed->val[i] = '\0';
    }
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
            parsed->val[j] = cmd[i];
            j++;
            i++;
        }
    }
    return  parsed;
}
bool valid_numb(char numb[11]) {
    bool flag = true;
    int i = 0;
    if (numb[i] != '-' && !(numb[i] >= '0' && numb[i] <= '9')) {
        write(1, "wrong value\n", 12);
        flag = false;
    }
    i++;
    while (i < 11) {
        if (numb[i] == '\0') {
            break;
        }
        if (!(numb[i] >= '0' && numb[i] <= '9')) {
            flag = false;
            break;
        }
        i++;
    }
    return flag;
}
bool valid_path(char path[32]) {
    for (int i = 0; i < 32; i++) {
        if (path[i] == '\0') {
            break;
        } else if (path[i] != 's' && path[i] != 'b') {
            return false;
        }
    }
    return true;
}
/*queue* int_to_queue(int val) {
    queue* number = q_create();
    bool negative = false;
    if (val < 0) {
        negative = true;
    } else if (val == 0) {
        push(number, '0');
    } else {
        while (val != 0) {
            int d = val % 10;
            push(number, d+'0');
            val /= 10;
        }
    }
    return number;
}*/
void tree_print(pnode t, int depth) {
    if (t) {
        for (int i = 0; i < depth; i++) {
            write(1, "\t", 1);
        }
        char numb[11] = {'\0'};
        sprintf(numb, "%d", t->val);
        int i = 0;
        while (numb[i] != '\0') {
            i++;
        }
        write(1, numb, i);
        write(1,"\n", 1);
        tree_print(t -> s, depth + 1);
        tree_print(t -> b, depth);
    }
}
int main() {
    setvbuf(stdout, (char*)NULL, _IONBF, 0);
    pnode test = NULL;
    char cmd[100] = {'\0'};
    ans* parsed = (ans*)malloc(sizeof(ans));
    int error_code = 0;
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
        //close (fd2[0]);
        while(read(0, cmd, 100)) {
        parsed = parser(cmd);
            if (strcmp(parsed->cm,"add") == 0) {
                if (!valid_path(parsed->path)) {
                    error_code = 1;
                    //write(fd2[1], &error_code, 4);
                    write(1, "wrong path\n", 12);
                }
                if (!valid_numb(parsed->val)) {
                    error_code = 1;
                    write(fd2[1], &error_code, 4);
                    write(1, "wrong value\n", 13);
                }
            } else if (strcmp(parsed->cm, "rmv") == 0) {
                if (!valid_path(parsed->path)) {
                    error_code = 1;
                    write(1, "wrong path\n", 12);
                    //write(fd2[1], &error_code, 4);
                }
            } else if (strcmp(parsed->cm, "prt") == 0){
                for (int i = 0; i < 11; i++) {
                    parsed->val[i] = '\0';
                }
                for (int i = 0; i < 32; i++) {
                    parsed->path[i] = '\0';
                }
            }
            else {
                write(1, "wrong command\n", 14);
                error_code = 1;
                //write(fd2[1], &error_code, 4);
                exit(1);
            }
            write(fd1[1], parsed->cm, 4);
            write(fd1[1], parsed->val, 11);
            write(fd1[1], parsed->path, 32);
            //write(fd2[1], &error_code, 4);
        }
    } else {
        while (1) {
            close(fd1[1]);
            read(fd1[0], parsed->cm, 4);
            read(fd1[0], parsed->val, 11);
            read(fd1[0], parsed->path, 32);
            queue *q = q_create();
            int k = 0;
            while (parsed->path[k] != '\0') {
                push(q, parsed->path[k]);
                k++;
            }
            if (strcmp(parsed->cm, "add") == 0) {
                int value = atoi(parsed->val);
                if (test == NULL) {
                    while (q_size(q) != 0) {
                        pop(q);
                    }
                    test = node_create(value);
                } else {
                    if(add(&test, value, q)) {
                    } else {
                        write(1, "can't add an element\n", 21);
                    }
                }
            } else if (strcmp(parsed->cm, "rmv") == 0) {
                if (test == NULL) {
                    write(1, "empty tree rmv\n", 14);
                } else {
                    rmv(search(&test, q));
                }
            } else if (strcmp(parsed->cm, "prt") == 0) {
                if (test == NULL) {
                    write(1, "empty tree prt\n", 15);
                } else {tree_print(test, 0);}
            } else {
                //write(1, "wtf?\n", 5);
                //break;
                exit(1);
            }
        }
    }
    return 0;
}