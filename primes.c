#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <math.h>

//#define BENCHMARK // define to get the number of divisions
//#define WATCH_LOOP // define to see done_primes growing (much slower)
#define FILENAME "primes.io" // current directory
#define DEFAULT_LIST_SIZE 100000

/* compile with:
$ gcc primes.c -o primes -Ofast -march=native -lm
longest prime number for the first 100.000.000 primes: 2.038.074.743 */

#define HELP	"usage: %s [int]\n" \
				"where [int] >= 0\n" \
				"if there is no [int] %d primes are produced\n"

static int ultimate_primes(int *primes, int n) {
	int root, loop, i, cnt = 0, done_primes = 0, candidate = 2;

	primes[0] = 2;
	while (++done_primes < n) {
#ifdef WATCH_LOOP
		printf("%d\r", done_primes);
#endif
		loop = 1;
		while (loop) {
			root = (int) sqrt((double) ++candidate);
			i = 0;
			while (loop) {
				if (primes[i] > root) {
					primes[done_primes] = candidate;
					loop = 0;
				}
#ifdef BENCHMARK
				cnt++;
#endif
				if (!(candidate % primes[i++]))
					break;
			}
		}
	}
	return cnt;
}

static int print_primes(int *primes, int n) {
	int i;

	fprintf(stderr, "printing to stdout...\n");
	n--;
	for (i = 0; i < n; i++)
		printf("%d, ", primes[i]);
	printf("%d\n", primes[i]);
	return ++i;
}

static char *from_ch_to_end(char *str, char ch) { /*
	skips str to the last ch */
	char *start;
	start = str;

	if (! *str) return str;
	while(*(++str)); // skip str until it points to '\0'
	while(*(--str) != ch && str != start);
	if (*str == ch)
		return ++str;
	return str;
}

static char *skip_path(char *exe) {
	char *start;
	start = exe;

	exe = from_ch_to_end(exe, '/');
	if (start == exe)
		return from_ch_to_end(exe, '\\');
	return exe;
}

int main(int argc, char **argv) {
	int *primes, i, help_len, n = DEFAULT_LIST_SIZE;
#ifdef BENCHMARK
	int cnt;
#endif
	clock_t start_t, end_t;
	// clock_t: returned by clock, usually equivalent to long
	FILE *fp;
	char *help;

	*argv = skip_path(*argv);
	help_len = strlen(HELP) - 4		// 4 = %s + %d
		+ strlen(*argv)				// %s
		+ 6							// %d (DEFAULT_LIST_SIZE)
		+ 1;						// '\0'

	if (! (help = (char *) malloc(help_len))) {
		fprintf(stderr, "not enough memory\n");
		return -1;
	}
	snprintf(help, help_len, HELP, *argv, DEFAULT_LIST_SIZE);

	if (argc >= 2) {
		argv++;
		for (i = 0; (*argv)[i]; i++) {
			if (! isdigit((*argv)[i])) {
				fprintf(stderr, "%s", help);
				free(help);
				return -1;
			}
		}
		n = atoi(*argv);
	}
	if (! (primes = malloc(n * sizeof *primes))) {
		fprintf(stderr, "\tnot enough memory for %d primes\n%s", n, help);
		free(help);
		return -1;
	}
#ifdef WATCH_LOOP
	printf("number of primes:\n");
#endif
	start_t = clock();

#ifdef BENCHMARK
	cnt = ultimate_primes(primes, n);
#else
	ultimate_primes(primes, n);
#endif

	end_t = clock();
#ifdef WATCH_LOOP
	printf("%d\n", n);
#endif
#ifdef _MSC_VER
	if (fopen_s(&fp, FILENAME, "wb")) {
#else
	if (! (fp = fopen(FILENAME, "wb"))) {
#endif
		print_primes(primes, n);
	} else {
		if ((signed) fwrite(primes, sizeof *primes, n, fp) < n)
			print_primes(primes, n);
		fclose(fp);
	}
#ifdef BENCHMARK
	printf("number of divisions: %d\n", cnt);
#endif
	printf("processor time: %f seconds\n",
	(float) (end_t - start_t) / (float) CLOCKS_PER_SEC);

	free(primes);
	free(help);
	return 0;
}
