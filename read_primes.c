#include <stdio.h>
#define FILENAME "primes.io" // current directory

int main(void) {
	FILE *fp;
	int out;
	if (! (fp = fopen(FILENAME, "rb")))
		return -1;
	while (! feof(fp)) {
		if (fread(&out, sizeof out, 1, fp) != 1)
			return -1;
		printf("%d, ", out);
	}
	fclose(fp);
	printf("\n");
	return 0;
}
