# Guess the number

Guess-the-number is a simple interactive game.

You are given:
1. a (statically-linked, not stripped) binary, `guess_the_number_5c1549905c60d35ca812b2e666b7644b` and
2. its C source, `guess_the_number.c`

You can use any tools of your choice for analyzing/exploiting the game.

Can you win the game? Can you even spawn a shell? ;)

Your job is to write Python programs that win the game most of the time or, even better, that spawn a shell. These scripts must work with the given binary, not with a re-compiled one. In other words, we give you the C sources only to ease your analysis work in the search for weaknesses/vulnerabilities.

The game does not cheat, but the odds of winning are tiny, so to win, you'll need to exploit something.

**Note**: The game contains *multiple* weaknesses/vulnerabilities. It's not required to discover them all, obviously, the more you find, the better.

You must write a detailed report describing the results of your analysis, in particular:
- what tools did you use and how
- the weakness/vulnerabilities that you found, even if you did not manage to exploit them
- the exploits that you crafted and their requirements/constraints/limitations, if any

Happy pwning :-)
