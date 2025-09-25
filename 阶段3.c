#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <ctype.h>

#define MAX_TARGETS 64
#define MAX_DEPS 32
#define MAX_NAME_LEN 33

// 顶点类型：目标或依赖
typedef struct Vertex {
    char name[MAX_NAME_LEN];
    int indegree; // 入度
} Vertex;

// 邻接表边
typedef struct Edge {
    int to; // 依赖顶点下标
    struct Edge *next;
} Edge;

Vertex vertices[MAX_TARGETS + MAX_DEPS];
int vertex_count = 0;
Edge *adj[MAX_TARGETS + MAX_DEPS]; // 邻接表

// 查找顶点下标
int find_vertex(const char *name) {
    for (int i = 0; i < vertex_count; ++i) {
        if (strcmp(vertices[i].name, name) == 0) return i;
    }
    return -1;
}

// 添加顶点，返回下标
int add_vertex(const char *name) {
    int idx = find_vertex(name);
    if (idx != -1) return idx;
    strncpy(vertices[vertex_count].name, name, MAX_NAME_LEN-1);
    vertices[vertex_count].name[MAX_NAME_LEN-1] = '\0';
    vertices[vertex_count].indegree = 0;
    adj[vertex_count] = NULL;
    return vertex_count++;
}

// 添加边 target->dep
void add_edge(int from, int to) {
    Edge *e = malloc(sizeof(Edge));
    e->to = to;
    e->next = adj[from];
    adj[from] = e;
    vertices[to].indegree++;
}

// 解析Makefile并构建依赖图
void build_graph(const char *filename) {
    FILE *fin = fopen(filename, "r");
    if (!fin) {
        fprintf(stderr, "Error: Makefile not found.\n");
        return;
    }
    char line[256];
    int lineno = 0;
    while (fgets(line, sizeof(line), fin)) {
        ++lineno;
        int len = strlen(line);
        while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r' || isspace(line[len-1]))) line[--len] = '\0';
        if (len == 0) continue;
        char *hash = strchr(line, '#');
        if (hash) *hash = '\0';
        if (strchr(line, ':')) {
            // 目标行
            char *colon = strchr(line, ':');
            *colon = '\0';
            int target_idx = add_vertex(line);
            char *dep = colon + 1;
            char *token = strtok(dep, " \t\r\n");
            while (token) {
                int dep_idx = add_vertex(token);
                add_edge(target_idx, dep_idx);
                token = strtok(NULL, " \t\r\n");
            }
        }
    }
    fclose(fin);
}

// 输出依赖关系图
void print_graph() {
    printf("依赖关系图：\n");
    for (int i = 0; i < vertex_count; ++i) {
        printf("%s (入度:%d): ", vertices[i].name, vertices[i].indegree);
        Edge *e = adj[i];
        if (!e) {
            printf("无依赖");
        } else {
            while (e) {
                printf("%s ", vertices[e->to].name);
                e = e->next;
            }
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    const char *filename = (argc > 1) ? argv[1] : "Makefile";
    build_graph(filename);
    print_graph();
    return 0;
}
