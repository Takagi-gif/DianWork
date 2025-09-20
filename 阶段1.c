#include <stdio.h>
#include <string.h>

void print_usage() {
	printf("Usage: Minimake [--help]\n");
	printf("  --help        Show this help message\n");
}//对读取参数的用法回复

int main(int argc, char *argv[]) {
	if (argc == 1) {
		print_usage();
		return 0;
	}// 检查参数数量，若无参数则输出用法
	
	// 遍历所有参数
	for (int i = 1; i < argc; ++i) {
		if (strcmp(argv[i], "--help") == 0) {
			print_usage();
			return 0;
		}// 检查--help参数，输出帮助信息
		else {
			printf("Unknown argument: %s\n", argv[i]);
			print_usage();
			return 1;
		}// 识别不正确的参数，输出错误提示和用法
	}
	return 0;
}
