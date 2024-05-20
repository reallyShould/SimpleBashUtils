#define _GNU_SOURCE
#include <getopt.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Flags {
  bool e;
  bool i;
  bool v;
  bool c;
  bool l;
  bool n;
  bool h;
  bool s;
} Flags;

int parse_arg(int argc, char *argv[], Flags *flags);
int pattern_index(int argc, char *argv[]);
int parse_files(int argc, char *argv[], int startFrom, char ***lst_files,
                int *len_files);
int read_file(char **filename, int files_len, char *pattern, Flags *flags);
int search_in_text(char *pattern, const char *text, Flags *flags);

int main(int argc, char *argv[]) {
  Flags flags = {false, false, false, false, false, false, false, false};

  int pattern_i = pattern_index(argc, argv);
  char *pattern = argv[pattern_i];
  char **files;
  int len_files;
  if (!parse_files(argc, argv, pattern_i + 1, &files, &len_files))
    return -1;
  if (!parse_arg(argc, argv, &flags))
    return -1;
  if (!read_file(files, len_files, pattern, &flags))
    return -1;
  free(files);
  return 0;
}

int parse_arg(int argc, char *argv[], Flags *flags) {
  int opt;

  int flags_len = 0;
  char *shortArgs = "eivclnhs";

  while ((opt = getopt(argc, argv, shortArgs)) != -1) {
    switch (opt) {
    case 'e':
      flags->e = true;
      flags_len++;
      break;
    case 'i':
      flags->i = true;
      flags_len++;
      break;
    case 'v':
      flags->v = true;
      flags_len++;
      break;
    case 'c':
      flags->c = true;
      flags_len++;
      break;
    case 'l':
      flags->l = true;
      flags_len++;
      break;
    case 'n':
      flags->n = true;
      flags_len++;
      break;
    case 'h':
      flags->h = true;
      flags_len++;
      break;
    case 's':
      flags->s = true;
      flags_len++;
      break;
    }
  }
  return flags_len;
}

int pattern_index(int argc, char *argv[]) {
  int flags = 0;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-')
      flags++;
  }
  return flags + 1;
}

int parse_files(int argc, char *argv[], int startFrom, char ***lst_files,
                int *len_files) {
  int len = argc - startFrom;
  if (!len) {
    return 0;
  }

  char **files = (char **)malloc((len) * sizeof(char *));
  int fileIndex = 0;
  for (int i = startFrom; i < argc; i++) {
    files[fileIndex] = argv[i];
    fileIndex++;
  }
  *lst_files = files;
  *len_files = len;
  return 1;
}

int read_file(char **filename, int files_len, char *pattern, Flags *flags) {
  int count = 0;
  int global_count = 0;
  int line_counter = 1;
  for (int i = 0; i < files_len; i++) {
    FILE *file = fopen(filename[i], "r");
    if (file == NULL) {
      if (!flags->s)
        printf("./s21_grep: %s: No such file or directory\n", filename[i]);
      return 0;
    }

    char *line = NULL;
    size_t line_len = 0;
    ssize_t read;

    while ((read = getline(&line, &line_len, file)) != -1) {
      if (search_in_text(pattern, line, flags)) {
        if (!flags->c && !flags->l) {
          if (files_len > 1 && !flags->h) {
            printf("%s:", filename[i]);
          }
          if (flags->n) {
            printf("%d:", line_counter);
          }
          if (line[strlen(line) - 1] == '\n') {
            printf("%s", line);
          } else {
            printf("%s\n", line);
          }
        }
        count++;
        global_count++;
      }
      line_counter++;
    }
    free(line);
    fclose(file);
    if (flags->l && count) {
      printf("%s\n", filename[i]);
    }
    if (flags->c) {
      if (files_len > 1 && !flags->h) {
        printf("%s:", filename[i]);
      }
      printf("%d\n", count);
    }
    count = 0;
    line_counter = 1;
  }
  if (!global_count && flags->c && !flags->v)
    return 0;
  return 1;
}

int search_in_text(char *pattern, const char *text, Flags *flags) {
  int code;
  regex_t regex;
  int ret;

  if (flags->e || flags->v || flags->c || flags->l || flags->n || flags->i ||
      flags->h || flags->s) {
    ret = regcomp(&regex, pattern, REG_EXTENDED);
    if (flags->i) {
      ret = regcomp(&regex, pattern, REG_EXTENDED | REG_ICASE);
    }
  } else {
    regfree(&regex);
    return 0;
  }

  if (ret)
    return 0;

  ret = regexec(&regex, text, 0, NULL, 0);
  if (!ret)
    code = 1;
  else if (ret == REG_NOMATCH)
    code = 0;
  else {
    char msgbuf[100];
    regerror(ret, &regex, msgbuf, sizeof(msgbuf));
    regfree(&regex);
    code = 0;
  }

  regfree(&regex);
  if (flags->v) {
    code--;
  }
  return code;
}
