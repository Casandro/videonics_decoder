/*this program filters the output of compactor.c and only passes packets with clean bits*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct line_s{
	char bits[64];
	int line;
	struct line_s *prev;
	struct line_s *next;
}line_t;


line_t *append_line(line_t *p, const int lineno, const char bits[])
{
	line_t *l=malloc(sizeof(line_t));
	if (l==NULL) return p;
	memset(l, 0, sizeof(line_t));
	strcpy(l->bits, bits);
	l->line=lineno;
	l->prev=p;
	if (p!=NULL) {
		p->next=l;
	}
	return l;
}

void print_lines(line_t *l, int print)
{
	if (l==NULL) return;
	line_t *n=l->next;
	if (print!=0) printf("%03d %s\n", l->line, l->bits);
	memset(l, 0, sizeof(line_t));
	free(l);
	return print_lines(n, print);
}

line_t *find_first(line_t *l)
{
	if (l==NULL) return NULL;
	if (l->prev==NULL) return l;
	return find_first(l->prev);
}

int count_errors(const line_t *l)
{
	if (l==NULL) return 0;
	if (strchr(l->bits, 'X')!=NULL) return 1;
	return count_errors(l->next);
}


int main(int argc, char *argv[])
{
	int lastline=0;
	int lineno=0;
	char bits[64]; //24 would be enough
	line_t *l=NULL;
	while (scanf("%d%s", &lineno, bits)==2) {
		if (lastline>lineno) { //New field
			l=find_first(l);
			if (count_errors(l)==0) print_lines(l, 1);
			else print_lines(l, 0);
			l=NULL;
		}
		//only append lines in the relevant region Change this if image is not at this position
		if ( (lineno>=45) && (lineno<=225) ) l=append_line(l, lineno, bits);
		lastline=lineno;
	}
	return 0;
}
