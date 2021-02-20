#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <linux/random.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static unsigned int stack_canary;
static const int win_points = 10;
static const unsigned int CHEAT = 770283071;

static void init_random()
{
	unsigned int r;
	int fd = open("/dev/urandom", O_RDONLY);
	if (fd < 0) {
		perror("open(/dev/urandom)");
		exit(EXIT_FAILURE);
	}
	if (read(fd, &r, sizeof(r)) != sizeof(r)) {
		fprintf(stderr, "Cannot initialize the random generator.\n");
		exit(EXIT_FAILURE);
	}
	close(fd);
	srand(r);
	stack_canary = time(0);
}

static void print_name(char *s)
{
	struct {
		char name[16];
		unsigned int canary;
	} x = {
		"", stack_canary
	};
	strcpy(x.name, s);
	if (x.canary!=stack_canary) {
		printf("*** stack smashed ***\n");
		exit(EXIT_FAILURE);
	}
	x.name[15] = 0;
	printf("Hi %s!\nLet's play a game; in each round, the first that reaches %d points wins.\n\n", x.name, win_points);
}

unsigned int my_hash(char *x)
{
	int i = 1;
	int result = 0;
	for(;*x>='A' && *x<='Z'; ++x) {
		result += (*x-('A'-1)) * i;
		i *= 23;
	}
	return result;
}

static int rand42()
{
	extern char **environ;
	for(char **e = environ; *e; ++e)
		if (my_hash(*e) == CHEAT)
			return 42;
	return 1 + (rand() % 42);
}

static void play_the_game()
{
	int my_score=0, your_score=0;
	int number, guess;
	for(;;) {
		printf("My score: %d, your score: %d\n", my_score, your_score);
		printf("Thinking...");
		fflush(stdout);
		sleep(1);
		number = rand42();
		printf("Ok, I'm thinking a number between 1 and 42. Your guess?\n");
		for(;;) {
			char buf[12];
			memset(buf, 0, sizeof(buf));
			if (!fgets(buf, sizeof(buf), stdin))
				exit(EXIT_FAILURE);
			size_t len = strlen(buf);
			if (len && buf[--len]=='\n')
				buf[len]=0;
			if (sscanf(buf, "%d", &guess) == 1)
				break;
			if (*buf=='%' || buf[1]=='%' || buf[2]=='%' || strchr(buf, 'd') || strchr(buf, 'x') || strchr(buf, 'p')) {
				printf("*** boom ***\n");
				exit(EXIT_FAILURE);
			}
			printf("Hey! \"");
			printf(buf);
			printf("\" is not a number! Enter a number.\n");
		}
		if (guess==number) {
			printf("You got it! Well done\n");
			if (++your_score == win_points) {
				printf("You have won, congratulations!\n");
				exit(EXIT_SUCCESS);
			}
		} else {
			printf("Nope, sorry. I thought %d\n", number);
			if (++my_score == win_points) {
				printf("I have won!\n");
				return;
			}
		}
	}
}

void game_loop()
{
	for(int round=0;;++round) {
		char choice[128];
		printf("--- Round %d ---\n", round);
		play_the_game();
		printf("\n\nWould you play again? Y/n?\n");
		gets(choice);
		if (*choice=='n' || *choice=='N')
			return;
	}
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		printf("Usage: %s <your-name>\n", *argv);
		return EXIT_FAILURE;
	}
	init_random();
	print_name(argv[1]);
	game_loop();
}
