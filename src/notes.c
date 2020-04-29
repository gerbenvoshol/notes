#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "helpers.c"
#include "md_renderer.h"

#define TAG_LIMIT 16
#define TAG_STR_BUFFER 128
#define STR_BUFFER 512
#define NOTE_BUFFER 8192

#define REMOTE_PATH "../remote/"
#define LOCAL_PATH "../local/"

const char *note_head = "<head><meta charset='UTF-8'><title>%s</title>"
                        "<link rel=stylesheet href='../css/main.css'>"
                        "<link rel=stylesheet href='../css/note.css'></head>";

const char *index_head =
    "<head><meta charset='UTF-8'><title>%s</title>"
    "<link rel=stylesheet href='../css/main.css'>"
    "<link rel=stylesheet href='../css/index.css'></head>";

const char *tag_index_head =
    "<head><meta charset='UTF-8'><title>%s</title>"
    "<link rel=stylesheet href='../../css/main.css'>"
    "<link rel=stylesheet href='../../css/index.css'></head>";

const char *wrapper_tag = "main";

typedef struct meta_note {
  char title[STR_BUFFER];
  char path[STR_BUFFER];
  char tags[TAG_LIMIT][TAG_STR_BUFFER];
  int tag_count;
  bool public;
} meta_note;

void
parse_metadata(FILE *in_f, meta_note *note) {
  char meta[STR_BUFFER], c;
  int tag_i = 0, i = 0, tag = 0;
  memset(meta, 0, STR_BUFFER);

  if (!fscanf(in_f, "title: %[^\n]\n", note->title)) {
    strcpy(note->title, note->path);
  }

  if (fscanf(in_f, "tags: %[^\n]\n", meta)) {
    do {
      c = meta[i];
      if (c == ',' || c == '\0') {
        note->tags[tag][tag_i] = '\0';
        tag_i = 0;
        tag++;
      } else {
        note->tags[tag][tag_i] = c;
        tag_i++;
      }
      i++;
    } while (c);
  }
  note->tag_count = tag;

  note->public = false;
  memset(meta, 0, STR_BUFFER);
  if (fscanf(in_f, "public: %[^\n]\n", meta)) {
    if (strcmp(meta, "yes") == 0) {
      note->public = true;
    }
  }
}

bool
unique_tag(char tag_dict[][TAG_STR_BUFFER], int tag_dict_size,
           const char *tag) {
  for (int i = 0; i < tag_dict_size; i++) {
    if (strcmp(tag, tag_dict[i]) == 0) {
      return false;
    }
  }
  return true;
}

bool
has_tag(const meta_note *note, const char *tag) {
  for (int i = 0; i < note->tag_count; i++) {
    if (strcmp(note->tags[i], tag) == 0) {
      return true;
    }
  }
  return false;
}

int
process_tag_dict(char tag_dict[][TAG_STR_BUFFER], int tag_dict_size,
                 meta_note *note, bool public) {
  int new_tag_count = 0;
  for (int i = 0; i < note->tag_count; i++) {
    if ((!(public) || (public && note->public)) &&
        unique_tag(tag_dict, tag_dict_size, note->tags[i])) {
      strcpy(tag_dict[tag_dict_size + new_tag_count], note->tags[i]);
      new_tag_count++;
    }
  }

  return new_tag_count;
}

int
read_note_file(const char *path, meta_note *note, char *note_contents,
               size_t *size) {
  char base_name[STR_BUFFER];
  memset(base_name, 0, STR_BUFFER);
  basename(base_name, path);
  FILE *f = fopen(path, "r");
  if (!f) {
    fprintf(stderr, "could not open file '%s'.\n", path);
    return 1;
  }

  sprintf(note->path, "%s.html", base_name);
  parse_metadata(f, note);

  *size = read_content(f, note_contents, NOTE_BUFFER);

  fclose(f);

  return 0;
}

int
write_note(const char *dir, const char *note_contents, size_t size,
           meta_note *note) {
  char out_path[STR_BUFFER];
  memset(out_path, 0, STR_BUFFER);
  sprintf(out_path, "%s%s", dir, note->path);
  FILE *f = fopen(out_path, "w");
  if (!f) {
    fprintf(stderr, "could not open file '%s'.", out_path);
    return 1;
  }

  fputs("<!DOCTYPE html><html>", f);

  fprintf(f, note_head, note->title);

  fputs("<body><header>", f);
  fputs("<a href='index.html'><span>index</span></a>", f);

  fprintf(f, "<h1>%s</h1>", note->title);

  if (note->tag_count > 0) {
    fputs("<nav>Tags:<ul>", f);
    for (int i = 0; i < note->tag_count; i++) {
      fprintf(f, "<li><a href='tags/%s.html'>%s</a></li>", note->tags[i],
              note->tags[i]);
    }
    fputs("</ul></nav>", f);
  }

  fputs("</header>", f);

  fprintf(f, "<%s>", wrapper_tag);

  md_render(note_contents, size, f);

  fprintf(f, "</%s>", wrapper_tag);

  fputs("</body></html>", f);

  fclose(f);

  return 0;
}

int
write_index_tag(const char *dir, const char tag[TAG_STR_BUFFER],
                const meta_note *notes, int notes_size, bool public) {
  char out_path[STR_BUFFER];
  memset(out_path, 0, STR_BUFFER);
  sprintf(out_path, "%stags/%s.html", dir, tag);
  FILE *f = fopen(out_path, "w");
  if (!f) {
    fprintf(stderr, "could not open file '%s'.", out_path);
    return 1;
  }

  fputs("<!DOCTYPE html><html>", f);
  fprintf(f, tag_index_head, tag);
  fputs("<body><header>", f);
  fputs("<a href='../index.html'><span>index</span></a>", f);
  fprintf(f, "<h1>%s</h1>", tag);
  fputs("</header>", f);

  fprintf(f, "<%s><ul>", wrapper_tag);
  for (int i = 0; i < notes_size; i++) {
    if ((!(public) || (public && notes[i].public)) && has_tag(&notes[i], tag)) {
      fprintf(f, "<a href='../%s'><li>%s</li></a>", notes[i].path,
              notes[i].title);
    }
  }
  fprintf(f, "</ul></%s></body></html>", wrapper_tag);

  fclose(f);

  return 0;
}

int
write_indexes(const char *dir, const char tag_dict[][TAG_STR_BUFFER],
              int tag_dict_size, const meta_note *notes, int notes_size,
              bool public) {
  int i;
  char out_path[STR_BUFFER];
  memset(out_path, 0, STR_BUFFER);
  sprintf(out_path, "%sindex.html", dir);
  FILE *f = fopen(out_path, "w");
  if (!f) {
    fprintf(stderr, "could not open file '%s'.\n", out_path);
    return 1;
  }

  fputs("<!DOCTYPE html><html>", f);
  fprintf(f, index_head, "notes");
  fprintf(f, "<body><header><h1>%s</h1><nav><ul>", "notes");

  // build tags
  for (i = 0; i < tag_dict_size; i++) {
    write_index_tag(dir, tag_dict[i], notes, notes_size, public);

    fprintf(f, "<a href='tags/%s.html'><li>%s</li></a>", tag_dict[i],
            tag_dict[i]);
  }
  fputs("</ul></nav></header>", f);
  fprintf(f, "<%s>", wrapper_tag);

  // build note list
  fputs("<ul>", f);

  for (i = 0; i < notes_size; i++) {
    if (!(public) || (public && notes[i].public)) {
      fprintf(f, "<a href='%s'><li>%s</li></a>", notes[i].path, notes[i].title);
    }
  }

  fprintf(f, "</ul></%s></body></html>", wrapper_tag);
  fclose(f);

  return 0;
}

int
main(int argc, const char **argv) {
  if (argc < 2) {
    fprintf(stderr, "no arguments provided.\n");
    printf("Usage: notes my-note.md [my-second-note.md ...]\n");

    return 1;
  }

  int tag_dict_size = 0, public_tag_dict_size = 0;
  size_t size = 0;
  char note_contents[NOTE_BUFFER];
  char tag_dict[argc - 1][TAG_STR_BUFFER];
  char public_tag_dict[argc - 1][TAG_STR_BUFFER];
  meta_note notes[argc - 1];

  memset(note_contents, 0, NOTE_BUFFER);
  memset(notes, 0, sizeof(notes));
  memset(tag_dict, 0, sizeof(tag_dict));
  memset(public_tag_dict, 0, sizeof(public_tag_dict));
  printf("Note metadata is %llu kB\n", sizeof(notes) / 1024);
  printf("Tag dictionaries are %llu kB\n",
         (sizeof(public_tag_dict) + sizeof(tag_dict)) / 1024);

  // build all notes
  // ===========================================================================
  for (int i = 1; i < argc; i++) {
    if (read_note_file(argv[i], &notes[i - 1], note_contents, &size)) {
      fprintf(stderr, "error reading note file '%s'.\n", argv[i]);
      continue;
    }

    // add any unique tags to the dictionary
    tag_dict_size +=
        process_tag_dict(tag_dict, tag_dict_size, &notes[i - 1], false);

    if (write_note(LOCAL_PATH, note_contents, size, &notes[i - 1])) {
      fprintf(stderr, "error writing note file '%s%s'.\n", LOCAL_PATH,
              notes[i - 1].path);
      return 3;
    }

    if (notes[i - 1].public) {
      public_tag_dict_size += process_tag_dict(
          public_tag_dict, public_tag_dict_size, &notes[i - 1], true);

      if (write_note(REMOTE_PATH, note_contents, size, &notes[i - 1])) {
        fprintf(stderr, "error writing remote note file '%s%s'.\n", REMOTE_PATH,
                notes[i - 1].path);
        return 3;
      }
    }

    memset(note_contents, 0, NOTE_BUFFER);
  }

  // build indexes
  // ===========================================================================
  if (write_indexes(LOCAL_PATH, tag_dict, tag_dict_size, notes, argc - 1,
                    false)) {
    fprintf(stderr, "error writing local indexes.\n");
    return 4;
  }

  if (write_indexes(REMOTE_PATH, public_tag_dict, public_tag_dict_size, notes,
                    argc - 1, true)) {
    fprintf(stderr, "error writing remote indexes.\n");
    return 4;
  }

  return 0;
}