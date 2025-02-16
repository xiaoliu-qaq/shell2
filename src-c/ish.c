#include <stdlib.h>
#include <stdio.h>

#include "command.h"
#include "parser.h"

void
issuePrompt(FILE *pf)
{
    fprintf(pf, "> ");
    fflush(pf);
}

void processCommands(FILE *pf, int interactive)
{
    int eof;
    command *first = NULL;
    resetParser(pf);
    do {
        command *cmd;
        if (interactive) issuePrompt(pf);
        first = cmd = nextCommand(&eof);
        while (cmd) {
            printCommand(cmd, stdout); 
	    cmd = cmd->pcNext;
        }
        if (first) destroyCommand(first);
    } while (!eof);
}

int 
main(int argc, char **argv)
{
#if 0
	ishrc = fopen(.....)
	processCommands(ishrc, 0);
        fclose(ishrc);
#endif
	processCommands(stdin, 1);
	return 0;
}
