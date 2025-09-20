#include <stdio.h>
#include <string.h>
#include <ctype.h>

// 判断一行是否仅包含空白字符（空行过滤）
int is_blank_line(const char *line) {
	for (int i = 0; line[i]; ++i) {
		if (!isspace((unsigned char)line[i])) return 0;
	}
	return 1;
}

// 去除字符串末尾的空白字符（行尾空格过滤）
void rtrim(char *line) {
	int len = strlen(line);
	while (len > 0 && isspace((unsigned char)line[len-1])) {
		line[--len] = '\0';
	}
}

// verbose=1时输出处理结果到Minimake_cleared.mk
// 预处理Makefile，过滤空行、去除注释和行尾空格
// verbose=1时输出处理结果到Minimake_cleared.mk
void preprocess_makefile(int verbose) {
	FILE *fin = fopen("./Makefile", "r");
	if (!fin) {
		fprintf(stderr, "Error: Makefile not found in current directory.\n");
		return;
	}
	FILE *fout = NULL;
	if (verbose) fout = fopen("Minimake_cleared.mk", "w");
	char line[1024];
	while (fgets(line, sizeof(line), fin)) {
		rtrim(line);
		char *hash = strchr(line, '#');
		if (hash) *hash = '\0';
		rtrim(line);
		if (is_blank_line(line)) continue;
		if (verbose && fout) fprintf(fout, "%s\n", line);
	}
	if (fout) fclose(fout);
	fclose(fin);
}
// 任务3
// 静态语法检查，检查目标行冒号和命令行Tab，输出带行号错误信息
void syntax_check() {
	FILE *fin = fopen("Minimake_cleared.mk", "r");
	if (!fin) return;
	char line[1024];
	int lineno = 0;
	int expect_target = 1;
	while (fgets(line, sizeof(line), fin)) {
		++lineno;
		rtrim(line);
		if (is_blank_line(line)) continue;
		if (expect_target) {
			if (!strchr(line, ':')) {
				printf("Line%d: Missing colon in target definition\n", lineno);
				expect_target = 1;
			} else {
				expect_target = 0;
			}
		} else {
			if (line[0] != '\t') {
				printf("Line%d: Command must start with Tab\n", lineno);
				expect_target = 1;
			} else {
				expect_target = 1;
			}
		}
	}
	fclose(fin);
}
//任务1
// 打印程序用法说明，包含--help参数
void print_usage() {
	printf("Usage: Minimake [--help] [-v|--verbose]\n");
	printf("  --help        Show this help message\n");
	printf("  -v, --verbose Output preprocessed Makefile to Minimake_cleared.mk\n");
}

int main(int argc, char *argv[]) {
	int verbose = 0;
	if (argc == 1) {
		print_usage();
		return 0;
	}
	for (int i = 1; i < argc; ++i) {
		if (strcmp(argv[i], "--help") == 0) {
			print_usage();
			return 0;
		} else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
			verbose = 1;
		} else {
			printf("Unknown argument: %s\n", argv[i]);
			print_usage();
			return 1;
		}
	}
	preprocess_makefile(verbose);
	syntax_check();
	return 0;
}
