#ifndef __HISTORY_H
#define __HISTORY_H

#define MAXLINE 4096

struct cmd {
        void *pid;
        char *cmd;
        int exitStatus;
        char **argv;
};

struct history {
        unsigned int capacity;
        unsigned int offset;
        struct cmd **entries;
};

struct history* init_history(int capacity);

void add_history(struct history *, char *cmd, int);

void print_history(struct history *);

void clear_history(struct history *);

#endif /* __HISTORY_H */
