#include <stdio.h>
#include <string.h>
#include <ctype.h>
//任务2
int is_blank_line(const char *line) {
	for (int i = 0; line[i]; ++i) {
		if (!isspace((unsigned char)line[i])) return 0;
	}
	return 1;
} // 判断一行是否仅包含空白字符（空行过滤）

void delete_blank(char *line) {
	int len = strlen(line);
	while (len > 0 && isspace((unsigned char)line[len-1])) {
		line[--len] = '\0';
	}
}// 去除字符串末尾的空白字符（行尾空格过滤）

// verbose=1时输出处理结果到Minimake_cleared.mk
void preprocess_makefile(int verbose) {
	FILE *fin = fopen("./Makefile", "r"); // 打开当前目录下Makefile
	if (!fin) {
		fprintf(stderr, "Error: Makefile not found in current directory.\n");
		return;
	}
	FILE *fout = NULL;
	if (verbose) fout = fopen("Minimake_cleared.mk", "w"); // 调试模式输出
	char line[1024];
	while (fgets(line, sizeof(line), fin)) {
		delete_blank(line); // 去除行尾空格
		// 去除注释（#及其后内容）
		char *hash = strchr(line, '#');
		if (hash) *hash = '\0';
		delete_blank(line); // 再次去除行尾空格
		if (is_blank_line(line)) continue; // 过滤空行
		if (verbose && fout) fprintf(fout, "%s\n", line); // 输出到文件
	}
	if (fout) fclose(fout);
	fclose(fin);
}
// 任务3
void grammar_check() {
	FILE *fin = fopen("Minimake_cleared.mk", "r"); // 读取预处理后的文件
	if (!fin) return;
	char line[1024];
	int lineno = 0;
	int expect_target = 1; // 1: 期待目标行, 0: 期待命令行
	while (fgets(line, sizeof(line), fin)) {
		++lineno;
		delete_blank(line);
		if (is_blank_line(line)) continue;
		if (expect_target) {
			// 检查目标行是否有冒号
			if (!strchr(line, ':')) {
				printf("Line%d: Missing colon in target definition\n", lineno);
				expect_target = 1; // 继续期待目标行
			}
			 else {
				expect_target = 0; // 下一个应为命令行
			}
		} 
		else {
			// 检查命令行是否以Tab开头
			if (line[0] != '\t') {
				printf("Line%d: Command must start with Tab\n", lineno);
				expect_target = 1; // 错误后继续期待目标行
			} 
			else {
				expect_target = 1; // 下一个应为目标行
			}
		}
	}
	fclose(fin);
}
//任务1
void print_usage() {
	printf("Usage: Minimake [--help]\n");
	printf("  --help        Show this help message\n");
}//对读取参数的用法回复

int main(int argc, char *argv[]) {
	int verbose = 0;
	if (argc == 1) {
		print_usage();
		return 0;
	} // 检查参数数量，若无参数则输出用法

	// 遍历所有参数
	for (int i = 1; i < argc; ++i) {
		if (strcmp(argv[i], "--help") == 0) {
			print_usage();
			return 0;
		} 
		else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
			verbose = 1; // 开启调试模式
		} 
		else {
			printf("Unknown argument: %s\n", argv[i]);
			print_usage();
			return 1;
		}
	}
	preprocess_makefile(verbose);
	grammar_check(); 
	return 0;
}
