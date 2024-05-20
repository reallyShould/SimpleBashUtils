#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  bool b;
  bool e;
  bool n;
  bool s;
  bool t;
  bool v;
} Flags;

void parse_arg(int argc, char* argv[], Flags* flags);
void return_help();
void read_files(char** files, int filesLen, Flags* flags);
int count_files(char* argv[], int argc);
char** parse_files(char* argv[], int argc, int c);
void putwarg(char act, char prev, char next, int* lineNum, Flags* flags);
void nonblank(char act, char prev, bool first, int* lineNum);
void number(char prev, bool first, int* lineNum);
void tabs(char* act);
int findZero(int num);
void squeeze(char act, char prev, char next, bool* isEmpty);
void ends(char act);
void non_printing(char act, bool* isE);

int main(int argc, char* argv[]) {
  Flags flags = {false, false, false, false, false, false};
  parse_arg(argc, argv, &flags);

  int files_count = count_files(argv, argc);
  char** files = parse_files(argv, argc, files_count);
  if (argc == 1) {
    return_help();
  } else {
    read_files(files, files_count, &flags);
  }
  free(files);
  return 0;
}

void parse_arg(int argc, char* argv[], Flags* flags) {
  int opt;
  int option_index;

  char* shortArgs = "beEnstTv";
  const struct option long_options[] = {
      {"number-nonblank", no_argument, NULL, 'b'},
      {"ends", no_argument, NULL, 'e'},
      {"show-ends", no_argument, NULL, 'E'},
      {"number", no_argument, NULL, 'n'},
      {"squeeze-blank", no_argument, NULL, 's'},
      {"tabs", no_argument, NULL, 't'},
      {"show-tabs", no_argument, NULL, 'T'},
      {"show-nonprinting", no_argument, NULL, 'v'},
      {NULL, 0, NULL, 0}};

  while ((opt = getopt_long(argc, argv, shortArgs, long_options,
                            &option_index)) != -1) {
    switch (opt) {
      case 'b':
        flags->b = true;
        break;
      case 'e':
        flags->e = true;
        flags->v = true;
        break;
      case 'E':
        flags->e = true;
        break;
      case 'n':
        flags->n = true;
        break;
      case 's':
        flags->s = true;
        break;
      case 't':
        flags->t = true;
        flags->v = true;
        break;
      case 'T':
        flags->t = true;
        break;
      case 'v':
        flags->v = true;
        break;
    }
  }
}

void return_help() {
  printf(
      "Options | Description \n\
-b (GNU: --number-nonblank) | numbers only non-empty lines\n\
-e implies -v (GNU only: -E the same, but without implying -v) | but also display end-of-line characters as $\n\
-n (GNU: --number) | number all output lines \n\
-s (GNU: --squeeze-blank) | squeeze multiple adjacent blank lines\n\
-t implies -v (GNU: -T the same, but without implying -v) | but also display tabs as ^I\n\
-v (GNU: --show-nonprinting) | use ^ and M- notation, except for LFD and TAB\n");
}

void read_files(char** files, int filesLen, Flags* flags) {
  int lineNum = 1;
  for (int i = 0; i < filesLen; i++) {
    FILE* fp = fopen(files[i], "r");
    char prev = EOF;
    if (fp != NULL) {
      int c = fgetc(fp);
      while (c != EOF) {
        int act = c;
        c = fgetc(fp);
        int next = c;
        putwarg(act, prev, next, &lineNum, flags);
        prev = act;
      }
      fclose(fp);
    } else {
      printf("cat: %s: No such file or directory\n", files[i]);
    }
  }
}

int count_files(char* argv[], int argc) {
  int len = 0;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-') {
      len++;
    }
  }
  return len;
}

char** parse_files(char* argv[], int argc, int c) {
  char** files = (char**)malloc(c * sizeof(char*));
  int fileIndex = 0;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-') {
      files[fileIndex] = argv[i];
      fileIndex++;
    }
  }
  return files;
}

void putwarg(char act, char prev, char next, int* lineNum, Flags* flags) {
  bool isE = false;
  if (!(flags->b || flags->e || flags->n || flags->s || flags->t || flags->v)) {
    printf("%c", act);
  } else {
    bool first = false;
    if (*lineNum == 1) first = true;
    if (flags->n) number(prev, first, lineNum);
    if (flags->b) nonblank(act, prev, first, lineNum);
    if (flags->t) tabs(&act);
    if (flags->s) squeeze(act, prev, next, &isE);
    if (flags->v) non_printing(act, &isE);
    if (flags->e) ends(act);
    if (!isE) {
      printf("%c", act);
      isE = false;
    }
  }
}

void non_printing(char act, bool* isE) {
  *isE = false;
  if (act >= 32 && act <= 126) {
  } else if (act == '\t') {
  } else if (act == '\n') {
  } else if (act < 32) {
    *isE = true;
    putchar('^');
    putchar(act + 64);
  } else if (act == 127) {
    *isE = true;
    putchar('^');
    putchar('?');
  } else {
    *isE = true;
    printf("M-");
    non_printing(act - 128, &*isE);
  }
}

void ends(char act) {
  if (act == '\n') {
    printf("$");
  }
}

void nonblank(char act, char prev, bool first, int* lineNum) {
  if ((prev == '\n' && act != '\n') || (first && act != '\n')) {
    printf("%6d\t", *lineNum);
    (*lineNum)++;
  }
}

void number(char prev, bool first, int* lineNum) {
  if (prev == '\n' || first) {
    printf("%6d\t", *lineNum);
    (*lineNum)++;
  }
}

void tabs(char* act) {
  if (*act == '\t') {
    printf("^");
    *act = 'I';
  }
}

void squeeze(char act, char prev, char next, bool* isEmpty) {
  if (prev == EOF) {
    prev = '\n';
  }
  if (act == '\n' && prev == '\n' && next == '\n') {
    *isEmpty = true;
  }
}

int findZero(int num) {
  int count = 0;
  while (num >= 10) {
    count++;
    num /= 10;
  }
  return count;
}
