# Defuse the bomb
In this assignment you have to reverse engineer a software "bomb", which explodes if it doesn't receive specific input.

Each of you got a randomly generated binary program that expects "something" in input to print a congratulation message. Download your own executable from the folder "[Custom bombs](https://2020.aulaweb.unige.it/mod/folder/view.php?id=60529)" on AulaWeb. Each program is named after a student that participated in this course's Git classroom before (if you haven't submitted any prior assignment, then you don't have a custom bomb; it's not a bug, it's a feature).

The defusing process consists of a sequence of stages with increasing difficulty. Your job is to analyze your bomb, finding out what is needed to defuse it.

Later phases can be rather challenging: solve as many phases as you can, and do submit your solution even if it is a partial one.
To be clear: **your submission is NOT required to solve all phases to be considered sufficient** (of course, the more you solve, the better).

You should submit both:
* a **Python script** that runs and defuses your bomb, leveraging pwntools. Of course, you're not allowed to alter the executable in any way to make it print the success message; and your solution must work with the original executable in any environment. That is, no dynamic "hacks" are allowed: no DBI, PRELOAD tricks, ptrace/debuggers and so on. Obviously, you can use any of those tools during your analysis; in particular, a debugger can be quite handy.
* a **detailed PDF report** describing the result of your analysis, and your analysis process, including what tools you used and how.

Happy reversing ;)
