#include "md_renderer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "helpers.h"

#define MD4C_FLAG_OPTIONS                                                      \
  MD_FLAG_WIKILINKS | MD_FLAG_LATEXMATHSPANS | MD_FLAG_STRIKETHROUGH |         \
      MD_FLAG_TABLES

static void
render_str(const MD_CHAR *str, const MD_SIZE size, FILE *f) {
  fwrite(str, 1, size, f);
  if ((errno = ferror(f))) {
    perror("render_str");
  }
}

static void
render_enter_h(MD_BLOCK_H_DETAIL *detail, FILE *f) {
  fprintf(f, "<h%d>", detail->level);
}

static void
render_leave_h(MD_BLOCK_H_DETAIL *detail, FILE *f) {
  fprintf(f, "</h%d>", detail->level);
}

static void
render_a(MD_SPAN_A_DETAIL *detail, FILE *f) {
  fputs("<a href='", f);
  render_str(detail->href.text, detail->href.size, f);
  if (is_external(detail->href.text, detail->href.size)) {
    fputs("' class='external", f);
  }
  fputs("'>", f);
}

static void
render_code(MD_BLOCK_CODE_DETAIL *detail, FILE *f) {
  fputs("<pre class='", f);
  render_str(detail->lang.text, detail->lang.size, f);
  fputs("'>", f);
}

static void
render_wikilink(MD_SPAN_WIKILINK_DETAIL *detail, FILE *f) {
  fputs("<a href='", f);
  render_str(detail->target.text, detail->target.size, f);
  if (is_external(detail->target.text, detail->target.size)) {
    fputs("' class='external", f);
  }
  fputs("'>", f);
}

static int
enter_block(MD_BLOCKTYPE type, void *detail, void *userdata) {
  FILE *f = userdata;
  switch (type) {
    case MD_BLOCK_DOC: break; // ignore
    case MD_BLOCK_QUOTE: fputs("<q>", f); break;
    case MD_BLOCK_UL: fputs("<ul>", f); break;
    case MD_BLOCK_OL: fputs("<ol>", f); break;
    case MD_BLOCK_LI: fputs("<li>", f); break;
    case MD_BLOCK_HR: fputs("<hr>", f); break;
    case MD_BLOCK_H: render_enter_h(detail, f); break;
    case MD_BLOCK_CODE: render_code(detail, f); break;
    case MD_BLOCK_HTML: fputs("<html>", f); break; // 'html' for verbosity
    case MD_BLOCK_P: fputs("<p>", f); break;
    case MD_BLOCK_TABLE: fputs("<table>", f); break;
    case MD_BLOCK_THEAD: fputs("<thead>", f); break;
    case MD_BLOCK_TBODY: fputs("<tbody>", f); break;
    case MD_BLOCK_TR: fputs("<tr>", f); break;
    case MD_BLOCK_TH: fputs("<th>", f); break;
    case MD_BLOCK_TD: fputs("<td>", f); break;
  }
  return 0;
}

static int
leave_block(MD_BLOCKTYPE type, void *detail, void *userdata) {
  FILE *f = userdata;
  switch (type) {
    case MD_BLOCK_DOC: break; // ignore
    case MD_BLOCK_QUOTE: fputs("</q>", f); break;
    case MD_BLOCK_UL: fputs("</ul>", f); break;
    case MD_BLOCK_OL: fputs("</ol>", f); break;
    case MD_BLOCK_LI: fputs("</li>", f); break;
    case MD_BLOCK_HR: fputs("</hr>", f); break;
    case MD_BLOCK_H: render_leave_h(detail, f); break;
    case MD_BLOCK_CODE: fputs("</pre>", f); break;
    case MD_BLOCK_HTML: fputs("</html>", f); break; // 'html' for verbosity
    case MD_BLOCK_P: fputs("</p>", f); break;
    case MD_BLOCK_TABLE: fputs("</table>", f); break;
    case MD_BLOCK_THEAD: fputs("</thead>", f); break;
    case MD_BLOCK_TBODY: fputs("</tbody>", f); break;
    case MD_BLOCK_TR: fputs("</tr>", f); break;
    case MD_BLOCK_TH: fputs("</th>", f); break;
    case MD_BLOCK_TD: fputs("</td>", f); break;
  }
  return 0;
}

static int
enter_span(MD_SPANTYPE type, void *detail, void *userdata) {
  FILE *f = userdata;
  switch (type) {
    case MD_SPAN_EM: fputs("<em>", f); break;
    case MD_SPAN_STRONG: fputs("<strong>", f); break;
    case MD_SPAN_A: render_a(detail, f); break;
    case MD_SPAN_IMG: fputs("<img>", f); break;
    case MD_SPAN_CODE: fputs("<code>", f); break; // ignore
    case MD_SPAN_DEL: fputs("<del>", f); break;
    case MD_SPAN_LATEXMATH: fputs("<$>", f); break;
    case MD_SPAN_LATEXMATH_DISPLAY: fputs("<$$>", f); break;
    case MD_SPAN_WIKILINK: render_wikilink(detail, f); break;
    case MD_SPAN_U: fputs("<u>", f); break;
  }

  return 0;
}

static int
leave_span(MD_SPANTYPE type, void *detail, void *userdata) {
  FILE *f = userdata;
  switch (type) {
    case MD_SPAN_EM: fputs("</em>", f); break;
    case MD_SPAN_STRONG: fputs("</strong>", f); break;
    case MD_SPAN_A: fputs("</a>", f); break;
    case MD_SPAN_IMG: fputs("</img>", f); break;
    case MD_SPAN_CODE: fputs("</code>", f); break; // ignore
    case MD_SPAN_DEL: fputs("</del>", f); break;
    case MD_SPAN_LATEXMATH: fputs("</$>", f); break;
    case MD_SPAN_LATEXMATH_DISPLAY: fputs("</$$>", f); break;
    case MD_SPAN_WIKILINK: fputs("</a>", f); break;
    case MD_SPAN_U: fputs("</u>", f); break;
  }
  return 0;
}

static int
text(MD_TEXTTYPE type, const MD_CHAR *text, MD_SIZE size, void *userdata) {
  FILE *f = userdata;
  switch (type) {
    case MD_TEXT_NORMAL: render_str(text, size, f); break;
    case MD_TEXT_NULLCHAR: break; // ignore
    case MD_TEXT_BR: fputs("<br><br>", f); break;
    case MD_TEXT_SOFTBR: fputs("<br>", f); break;
    case MD_TEXT_ENTITY: fputs("[entity]", f); break;
    case MD_TEXT_CODE: render_str(text, size, f); break;
    case MD_TEXT_HTML: render_str(text, size, f); break;
    case MD_TEXT_LATEXMATH: fputs("[some latexmath]", f); break;
  }
  return 0;
}

static MD_PARSER md_parser = {0,           MD4C_FLAG_OPTIONS, enter_block,
                              leave_block, enter_span,        leave_span,
                              text};

int
md_render(const char *data, size_t size, FILE *f) {
  return md_parse(data, size, &md_parser, f);
}
