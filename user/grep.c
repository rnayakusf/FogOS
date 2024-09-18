// Simple grep.  Only supports ^ . * $ operators.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

char buf[1024];
int match(char*, char*);

/* -F, -R, -v flags */
/* No bool type, so use uint8 instead */
uint8 Fflag = 0;
uint8 Rflag = 0;
uint8 vflag = 0;

void
grep(char *pattern, int fd)
{
  int n, m;
  char *p, *q;

  m = 0;
  /* Write continuously to the buffer. If runs out of space, move current
   * position to beginning of buffer and read again.*/
  while((n = read(fd, buf+m, sizeof(buf)-m-1)) > 0){
    m += n;
    buf[m] = '\0';
    p = buf;
    while((q = strchr(p, '\n')) != 0){
      *q = 0;
      /* Print line if match and no -v flag or if no match and -v flag
       * Simple xor operation */
      if (match(pattern, p) != vflag) {
        *q = '\n';
        write(1, p, q+1 - p);
      }
      p = q+1;
    }
    if(m > 0){
      m -= p - buf;
      memmove(buf, p, m);
    }
  }
}

int
main(int argc, char *argv[])
{
  int fd, i;
  char *pattern;

  if(argc <= 1){
    fprintf(2, "usage: grep [-F] [-R] [-v] pattern [file ...]\n");
    exit(1);
  }

  /* check for flags */
  i = 1;
  while(1) {
    if (argv[i][0] == '-') {
      switch (argv[i][1]) {
        case 'F':
	  Fflag = 1;
	  break;
        case 'R':
	  Rflag = 1;
	  break;
        case 'v':
	  vflag = 1;
	  break;
	default:
	  fprintf(2, "Unrecognized flag: -%c\n", argv[i][1]);
	  exit(1);
      }
      i++;
    } else {
      break;
    }
  }
  /* Mostly so I don't get unused variable errors*/
  if (Rflag){
	  fprintf(1, "Flag found\n");
  }

  pattern = argv[i++];

  if(argc <= i){
    grep(pattern, 0);
    exit(0);
  }

  while(i < argc){
    if((fd = open(argv[i], 0)) < 0){
      printf("grep: cannot open %s\n", argv[i]);
      exit(1);
    }
    grep(pattern, fd);
    close(fd);
    i++;
  }
  exit(0);
}

// Regexp matcher from Kernighan & Pike,
// The Practice of Programming, Chapter 9, or
// https://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html

int matchhere(char*, char*);
int matchstar(int, char*, char*);

int
match(char *re, char *text)
{
  // Ignore regex stuff when -F is passed
  if(re[0] == '^' && !Fflag)
    return matchhere(re+1, text);
  do{  // must look at empty string
    if(matchhere(re, text))
      return 1;
  }while(*text++ != '\0');
  return 0;
}

// matchhere: search for re at beginning of text
int matchhere(char *re, char *text)
{
  if(re[0] == '\0')
    return 1;
  // Ignore regex stuff when -F is passed
  if (!Fflag){
    if(re[1] == '*')
      return matchstar(re[0], re+2, text);
    if(re[0] == '$' && re[1] == '\0')
      return *text == '\0';
  }
  if(*text!='\0' && ((!Fflag && re[0]=='.') || re[0]==*text))
    return matchhere(re+1, text+1);
  return 0;
}

// matchstar: search for c*re at beginning of text
int matchstar(int c, char *re, char *text)
{
  do{  // a * matches zero or more instances
    if(matchhere(re, text))
      return 1;
  }while(*text!='\0' && (*text++==c || c=='.'));
  return 0;
}

