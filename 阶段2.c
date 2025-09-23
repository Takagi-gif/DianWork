#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <ctype.h>

#define MAX_TARGETS 64
#define MAX_DEPS 32
#define MAX_COMMANDS 16
#define MAX_NAME_LEN 33

// 结构体定义：存储目标、依赖、命令
typedef struct {
    char target[MAX_NAME_LEN];
    char deps[MAX_DEPS][MAX_NAME_LEN];
    int dep_count;
    char commands[MAX_COMMANDS][256];
    int cmd_count;
    int line_no; // 记录目标定义所在行号
} Rule;
Rule rules[MAX_TARGETS];

int rule_count = 0;
// 检查文件是否存在于当前目录
int file_exists(const char *filename) {
    DIR *dir = opendir(".");
    struct dirent *entry;
    if (!dir) return 0;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, filename) == 0) {
            closedir(dir);
            return 1;
        }
    }
    closedir(dir);
    return 0;
}

// 查找目标是否已定义
int find_target(const char *name) {
    for (int i = 0; i < rule_count; ++i) {
        if (strcmp(rules[i].target, name) == 0) return i;
    }
    return -1;
}

// 解析一行目标及依赖
int parse_target_line(char *line, Rule *rule, int lineno) {
    char *colon = strchr(line, ':');
    if (!colon) return 0;
    *colon = '\0';
    strncpy(rule->target, line, MAX_NAME_LEN-1);
    rule->target[MAX_NAME_LEN-1] = '\0';
    rule->dep_count = 0;
    rule->cmd_count = 0;
    rule->line_no = lineno;
    char *dep = colon + 1;
    char *token = strtok(dep, " \t\r\n");
    while (token && rule->dep_count < MAX_DEPS) {
        strncpy(rule->deps[rule->dep_count], token, MAX_NAME_LEN-1);
        rule->deps[rule->dep_count][MAX_NAME_LEN-1] = '\0';
        rule->dep_count++;
        token = strtok(NULL, " \t\r\n");
    }
    return 1;
}

// 解析Makefile
void parse_makefile(const char *filename) {
    FILE *fin = fopen(filename, "r");
    if (!fin) {
        fprintf(stderr, "Error: Makefile not found.\n");
        return;
    }
    char line[256];
    int lineno = 0;
    Rule *current = NULL;
    while (fgets(line, sizeof(line), fin)) {
        ++lineno;//行号+1
        int len = strlen(line);//计算当前长度
        while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r' || isspace(line[len-1]))) line[--len] = '\0';
        if (len == 0) continue;
        // 注释处理
        char *hash = strchr(line, '#');
        if (hash) *hash = '\0';
        // 目标行
        if (strchr(line, ':')) {
            Rule rule;
            if (!parse_target_line(line, &rule, lineno)) continue;
            // 检查重复目标
            if (find_target(rule.target) != -1) {
                printf("Line%d: Duplicate target definition '%s'\n", lineno, rule.target);
                continue;
            }
            rules[rule_count++] = rule;
            current = &rules[rule_count-1];
        } else if (line[0] == '\t' && current) {
            // 命令行
            if (current->cmd_count < MAX_COMMANDS) {
                strncpy(current->commands[current->cmd_count], line+1, 255);
                current->commands[current->cmd_count][255] = '\0';
                current->cmd_count++;
            }
        } else {
            current = NULL;
        }
    }
    fclose(fin);
}

// 检查依赖合法性
void check_dependencies() {
    for (int i = 0; i < rule_count; ++i) {
        Rule *rule = &rules[i];
        for (int j = 0; j < rule->dep_count; ++j) {
            const char *dep = rule->deps[j];
            // 依赖是目标
            if (find_target(dep) != -1) continue;
            // 依赖是文件
            if (file_exists(dep)) continue;
            // 都不是，报错
            printf("Line%d: Invalid dependency '%s'\n", rule->line_no, dep);
        }
    }
}

int main() {
    parse_makefile("Makefile");
    check_dependencies();
    return 0;
}
