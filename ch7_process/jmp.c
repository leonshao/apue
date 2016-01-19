/*
 * jmp.c
 *
 *  Created on: 2016年1月19日
 *      Author: leon
 */

#include "apue.h"
#include <setjmp.h>

#define		TOK_ADD 5

void 	do_line(char *);
int		get_token(void);
void	cmd_add(void);

jmp_buf		jmpbuffer;

int main(void) {
	char	line[MAXLINE];

	if(setjmp(jmpbuffer) != 0)
		printf("error");

	while(fgets(line, MAXLINE, stdin) != NULL)
		do_line(line);

	return 0;
}

char *tok_ptr = NULL;

void do_line(char *ptr) {
	int		cmd;

	tok_ptr = ptr;
	while((cmd = get_token()) > 0) {
		switch(cmd) {
		case TOK_ADD:
			cmd_add();
			break;
		}
	}
}

void cmd_add(void) {
	int		token;

	token = get_token();
	if(token < 0)
		longjmp(jmpbuffer, 1);
	/* rest of processing for this command */
}

int get_token(void) {
	/* fetch next token from line pointed to by tok_ptr */
	if(tok_ptr != NULL) {
		tok_ptr = NULL;
		return TOK_ADD;
	}

	return -1;
}
