/**
 *
 * @author: Trachi Yassin
 * @date: 04-12-2020
 * @file: table.c
 * --
 * 09-12-2020: Aggiunti gli stili per le tabelle
 */
#ifdef __linux__
#define _GNU_SOURCE
#endif

#define MARGIN (1)
//#define DEBUG
#define FILE_TOO_BIG 1000000 /* Quando viene letto 1kb si usa un file esterno */
#define AUX_FILE ("__aux__.tbx")
#define UNICODE

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#ifdef _WIN32
/** 
 * Se non stiamo compilando su linux verra' usata la mia
 * implementazione della funzione getline (_GNU_SOURCE)
 */
ssize_t getline(char** lineptr, size_t* n, FILE* stream);
#endif
struct bufferstate
{
  int rows; /* Numero di righe lette dal file 'fptr' */
  int cols; /* Numero attuale di colonna */
  size_t bytes; /* Byte letti (caratteri) */
  FILE* fptr;	 /* File da cui leggere (valido anche stdin) */
};
typedef struct bufferstate buf_state;
#define BUFF_INIT {0, 0, 0, stdin}

struct comment
{
  const char* lang;
  const char* single_line;
  const char* first_line;
  const char* last_line;
};

struct comment comment_styles [] = 
{
#define COM(lang, single, ms, me) {lang, single, ms, me}
#define CSTYLE "c|cpp|cc|h|java|hpp|php|js|y"
  COM(CSTYLE, "//", "/*", "*/"),
  COM("d", "//", "/+", "+/"),
  COM("html|htm|xhtml|xml|", NULL, "<!--", "-->"),
  COM("py|", "#", NULL, NULL),
  COM("matlab|", "%", "%{", "%}"),
  COM("fortran|", "C", NULL, NULL),
  COM("basic|", "REM", NULL, NULL)
#define COMMENTNO ((int)(sizeof(comment_styles) / sizeof(comment_styles[0])))
};

struct field
{
  size_t chars;

#define ALIG_LEFT 00
#define ALIG_CENTER 01
#define ALIG_RIGHT 02

  unsigned alignment; /* 0 sx 1 dx 2 centro */
  int fieldno;	
  char* buff; /* Contenuto (testo da stampare) */
};

struct qnode
{
  struct field* field;
  struct qnode* next_field; /* Puntatore al nodo successivo */
};
#define GROUP_SEPARATOR 29
#define RECORD_SEPARATOR 30
#define UNIT_SEPARATOR 31

#ifdef UNICODE
#  define LIGHT_HR "\u2500"
#  define HEAVY_HR "\u2501"
#  define LIGHT_VT "\u2502"
#  define HEAVY_VT "\u2503"
#  define LIGHT_TRIPLE_DASH_HR "\u2504"
#  define HEAVY_TRIPLE_DASH_HR "\u2505"
#  define LIGHT_TRIPLE_DASH_VT "\u2506"
#  define HEAVY_TRIPLE_DASH_VT "\u2507"
#  define LIGHT_QUADRUPLE_DASH_HR "\u2508"
#  define HEAVY_QUADRUPLE_DASH_HR "\u2509"
#  define LIGHT_QUADRUPLE_DASH_VT "\u250A"
#  define HEAVY_QUADRUPLE_DASH_VT "\u250B"
#  define LIGHT_DOWN_AND_RIGHT "\u250C"
#  define DOWN_LIGHT_AND_RIGHT_HEAVY "\u250D"
#  define DOWN_HEAVY_AND_RIGHT_LIGHT "\u250E"
#  define HEAVY_DOWN_AND_RIGHT "\u250F"
#  define LIGHT_DOWN_AND_LEFT "\u2510"
#  define DOWN_LIGHT_AND_LEFT_HEAVY "\u2511"
#  define DOWN_HEAVY_AND_LEFT_LIGHT "\u2512"
#  define HEAVY_DOWN_AND_LEFT "\u2513"
#  define LIGHT_UP_AND_RIGHT "\u2514"
#  define UP_LIGHT_AND_RIGHT_HEAVY "\u2515"
#  define UP_HEAVY_AND_RIGHT_LIGHT "\u2516"
#  define HEAVY_UP_AND_RIGHT "\u2517"
#  define LIGHT_UP_AND_LEFT "\u2518"
#  define UP_LIGHT_AND_LEFT_HEAVY "\u2519"
#  define UP_HEAVY_AND_LEFT_LIGHT "\u251A"
#  define HEAVY_UP_AND_LEFT "\u251B"
#  define LIGHT_VT_AND_RIGHT "\u251C"
#  define VT_LIGHT_AND_RIGHT_HEAVY "\u251D"
#  define UP_HEAVY_AND_RIGHT_DOWN_LIGHT "\u251E"
#  define DOWN_HEAVY_AND_RIGHT_UP_LIGHT "\u251F"
#  define VT_HEAVY_AND_RIGHT_LIGHT "\u2520"
#  define DOWN_LIGHT_AND_RIGHT_UP_HEAVY "\u2521"
#  define UP_LIGHT_AND_RIGHT_DOWN_HEAVY "\u2522"
#  define HEAVY_VT_AND_RIGHT "\u2523"
#  define LIGHT_VT_AND_LEFT "\u2524"
#  define VT_LIGHT_AND_LEFT_HEAVY "\u2525"
#  define UP_HEAVY_AND_LEFT_DOWN_LIGHT "\u2526"
#  define DOWN_HEAVY_AND_LEFT_UP_LIGHT "\u2527"
#  define VT_HEAVY_AND_LEFT_LIGHT "\u2528"
#  define DOWN_LIGHT_AND_LEFT_UP_HEAVY "\u2529"
#  define UP_LIGHT_AND_LEFT_DOWN_HEAVY "\u252A"
#  define HEAVY_VT_AND_LEFT "\u252B"
#  define LIGHT_DOWN_AND_HR "\u252C"
#  define LEFT_HEAVY_AND_RIGHT_DOWN_LIGHT "\u252D"
#  define RIGHT_HEAVY_AND_LEFT_DOWN_LIGHT "\u252E"
#  define DOWN_LIGHT_AND_HR_HEAVY "\u252F"
#  define DOWN_HEAVY_AND_HR_LIGHT "\u2530"
#  define RIGHT_LIGHT_AND_LEFT_DOWN_HEAVY "\u2531"
#  define LEFT_LIGHT_AND_RIGHT_DOWN_HEAVY "\u2532"
#  define HEAVY_DOWN_AND_HR "\u2533"
#  define LIGHT_UP_AND_HR "\u2534"
#  define LEFT_HEAVY_AND_RIGHT_UP_LIGHT "\u2535"
#  define RIGHT_HEAVY_AND_LEFT_UP_LIGHT "\u2536"
#  define UP_LIGHT_AND_HR_HEAVY "\u2537"
#  define UP_HEAVY_AND_HR_LIGHT "\u2538"
#  define RIGHT_LIGHT_AND_LEFT_UP_HEAVY "\u2539"
#  define LEFT_LIGHT_AND_RIGHT_UP_HEAVY "\u253A"
#  define HEAVY_UP_AND_HR "\u253B"
#  define LIGHT_VT_AND_HR "\u253C"
#  define LEFT_HEAVY_AND_RIGHT_VT_LIGHT "\u253D"
#  define RIGHT_HEAVY_AND_LEFT_VT_LIGHT "\u253E"
#  define VT_LIGHT_AND_HR_HEAVY "\u253F"
#  define UP_HEAVY_AND_DOWN_HR_LIGHT "\u2540"
#  define DOWN_HEAVY_AND_UP_HR_LIGHT "\u2541"
#  define VT_HEAVY_AND_HR_LIGHT "\u2542"
#  define LEFT_UP_HEAVY_AND_RIGHT_DOWN_LIGHT "\u2543"
#  define RIGHT_UP_HEAVY_AND_LEFT_DOWN_LIGHT "\u2544"
#  define LEFT_DOWN_HEAVY_AND_RIGHT_UP_LIGHT "\u2545"
#  define RIGHT_DOWN_HEAVY_AND_LEFT_UP_LIGHT "\u2546"
#  define DOWN_LIGHT_AND_UP_HR_HEAVY "\u2547"
#  define UP_LIGHT_AND_DOWN_HR_HEAVY "\u2548"
#  define RIGHT_LIGHT_AND_LEFT_VT_HEAVY "\u2549"
#  define LEFT_LIGHT_AND_RIGHT_VT_HEAVY "\u254A"
#  define HEAVY_VT_AND_HR "\u254B"
#  define LIGHT_DBL_DASH_HR "\u254C"
#  define HEAVY_DBL_DASH_HR "\u254D"
#  define LIGHT_DBL_DASH_VT "\u254E"
#  define HEAVY_DBL_DASH_VT "\u254F"
#  define DBL_HR "\u2550"
#  define DBL_VT "\u2551"
#  define DOWN_SINGLE_AND_RIGHT_DBL "\u2552"
#  define DOWN_DBL_AND_RIGHT_SINGLE "\u2553"
#  define DBL_DOWN_AND_RIGHT "\u2554"
#  define DOWN_SINGLE_AND_LEFT_DBL "\u2555"
#  define DOWN_DBL_AND_LEFT_SINGLE "\u2556"
#  define DBL_DOWN_AND_LEFT "\u2557"
#  define UP_SINGLE_AND_RIGHT_DBL "\u2558"
#  define UP_DBL_AND_RIGHT_SINGLE "\u2559"
#  define DBL_UP_AND_RIGHT "\u255A"
#  define UP_SINGLE_AND_LEFT_DBL "\u255B"
#  define UP_DBL_AND_LEFT_SINGLE "\u255C"
#  define DBL_UP_AND_LEFT "\u255D"
#  define VT_SINGLE_AND_RIGHT_DBL "\u255E"
#  define VT_DBL_AND_RIGHT_SINGLE "\u255F"
#  define DBL_VT_AND_RIGHT "\u2560"
#  define VT_SINGLE_AND_LEFT_DBL "\u2561"
#  define VT_DBL_AND_LEFT_SINGLE "\u2562"
#  define DBL_VT_AND_LEFT "\u2563"
#  define DOWN_SINGLE_AND_HR_DBL "\u2564"
#  define DOWN_DBL_AND_HR_SINGLE "\u2565"
#  define DBL_DOWN_AND_HR "\u2566"
#  define UP_SINGLE_AND_HR_DBL "\u2567"
#  define UP_DBL_AND_HR_SINGLE "\u2568"
#  define DBL_UP_AND_HR "\u2569"
#  define VT_SINGLE_AND_HR_DBL "\u256A"
#  define VT_DBL_AND_HR_SINGLE "\u256B"
#  define DBL_VT_AND_HR "\u256C"
#endif

struct table_style
{

#define HAS_HEADER_TOP 0x01
#define HAS_HEADER_BOTTOM 0x02
#define HAS_LAST_LINE 0x04
#define HAS_ROW_SEP 0x10 
#define IS_UNICODE 0x20

  int style;
  unsigned opts;
  const char* rowsep_fill;
  const char* normalcolsep;
  /**
   *
   * Riga di intestazione superiore 
   * Nota: HT sta per header top */
  const char* htlc; /* Left Corner */
  const char* htfill; /* Separatore */
  const char* htrc; /* Right Corner */
  const char* htcs; /* Column separator */

  /**
   *
   * Riga di intestazione inferiore 
   * Nota: HB sta per header bottom */
  const char* hblc; /* Left Corner */
  const char* hbfill; /* Separatore */
  const char* hbrc; /* Right corner */
  const char* hbcs; /* Column separator */

  /**
   * 
   * Ultima riga della tabella 
   * Nota: ET sta per end of table */
  const char* etlc;
  const char* etfill;
  const char* etrc;
  const char* etcs;

#define HT(p1, p2, p3, p4) .htlc = p1, .htfill = p2, .htcs = p3, .htrc = p4

#define HB(p1, p2, p3, p4) .hblc = p1, .hbfill = p2, .hbcs = p3, .hbrc = p4

#define LAST(p1, p2, p3, p4) .etlc = p1, .etrc = p4, .etfill = p2, .etcs = p3 

};

#define STYLE_MYSQL 0
#define STYLE_COMPACT 1
#define STYLE_SEPARATED 2 
#define STYLE_DOTS 3
#define STYLE_UNICODE 4
#define STYLE_UNICODE_DOUBLE 5
#define STYLE_REDDIT_MARKDOWN 6
#define STYLE_RST_SIMPLE 7

#define STRTOSTYLE(string) \
(!strcmp(string, "mysql") ? STYLE_MYSQL : \
 !strcmp(string, "compact") ? STYLE_COMPACT : \
 !strcmp(string, "separated") ? STYLE_SEPARATED : \
 !strcmp(string, "dots") ? STYLE_DOTS : \
 !strcmp(string, "u8") ? STYLE_UNICODE : \
 !strcmp(string, "u8-double") ? STYLE_UNICODE_DOUBLE : \
 !strcmp(string, "reddit-markdown") ? STYLE_REDDIT_MARKDOWN : \
 !strcmp(string, "rst-simple") ? STYLE_RST_SIMPLE : STYLE_MYSQL)

static struct table_style styles [] =
{{
    .style = STYLE_MYSQL,
    .opts = HAS_HEADER_TOP | HAS_HEADER_BOTTOM | HAS_LAST_LINE,
    .normalcolsep = "|",
    .rowsep_fill = NULL,
    HT("+", "-", "+", "+"), HB("+", "-", "+", "+"),
    LAST("+", "-", "+", "+")
  },{
    .style = STYLE_COMPACT,
    .opts = HAS_HEADER_BOTTOM,
    .normalcolsep = " ",
    .rowsep_fill = NULL,
    HT(NULL, NULL, NULL, NULL), HB(" ", "-", " ", " "),
    LAST(NULL, NULL, NULL, NULL)
  },{
    .style = STYLE_SEPARATED,
    .opts = HAS_HEADER_TOP | HAS_HEADER_BOTTOM | HAS_LAST_LINE | HAS_ROW_SEP,
    .normalcolsep = "|",
    .rowsep_fill = "-",
    HT("+", "=", "+", "+"), HB("+", "-", "+", "+"),
    LAST("+", "-", "+", "+")
  },{
    .style = STYLE_DOTS,
    .opts = HAS_HEADER_TOP | HAS_HEADER_BOTTOM | HAS_LAST_LINE,
    .normalcolsep = ":",
    .rowsep_fill = NULL,
    HT(".", ".", ".", "."), HB(":", ".", ":", ":"),
    LAST(":", ".", "'", ":")
  },{
    .style = STYLE_UNICODE,
    .opts = HAS_HEADER_TOP | HAS_HEADER_BOTTOM | HAS_LAST_LINE,
    .normalcolsep = LIGHT_VT,
    .rowsep_fill = NULL,
    HT(LIGHT_DOWN_AND_RIGHT, LIGHT_HR, LIGHT_DOWN_AND_HR, LIGHT_DOWN_AND_LEFT ),
    HB(LIGHT_VT_AND_RIGHT, LIGHT_HR, LIGHT_VT_AND_HR, LIGHT_VT_AND_LEFT),
    LAST(LIGHT_UP_AND_RIGHT, LIGHT_HR, LIGHT_UP_AND_HR, LIGHT_UP_AND_LEFT)
  },{
    .style = STYLE_UNICODE_DOUBLE,
    .opts = HAS_HEADER_TOP | HAS_HEADER_BOTTOM | HAS_LAST_LINE,
    .normalcolsep = DBL_VT,
    .rowsep_fill = NULL,
    HT(DBL_DOWN_AND_RIGHT, DBL_HR, DBL_DOWN_AND_HR, DBL_DOWN_AND_LEFT ),
    HB(DBL_VT_AND_RIGHT, DBL_HR, DBL_VT_AND_HR, DBL_VT_AND_LEFT),
    LAST(DBL_UP_AND_RIGHT, DBL_HR, DBL_UP_AND_HR, DBL_UP_AND_LEFT)
  },{
    .style = STYLE_REDDIT_MARKDOWN,
    .opts = HAS_HEADER_BOTTOM,
    .normalcolsep = "|",
    .rowsep_fill = NULL,
    HT(NULL, NULL, NULL, NULL), HB("|", "-", "|", "|"),
    LAST(NULL, NULL, NULL, NULL)
  },{
    .style = STYLE_RST_SIMPLE,
    .opts = HAS_HEADER_TOP | HAS_HEADER_BOTTOM | HAS_LAST_LINE,
    .normalcolsep = " ",
    .rowsep_fill = NULL,
    HT("=", "=", " ", "="), HB("=", "=", " ", "="),
    LAST("=", "=", " ", "="),
  }};

struct buffer
{
  struct table_style* table_style; 
  struct comment* comment;
  /* 1:  Se l'input e' diviso tra due file, 0: No */
#define BUFF_FRAGMENTED 0
#define BUFF_NO_HEADER 1
  int options;
  size_t bufsize;
  /* Quante colonne? */
  int cols; 
  /* Dimensione del campo piu largo x colonna */
  size_t* field_max_size; 
  /* Puntatore al prossimo campo da stampare */
  struct qnode* head; 
  /* Puntatore all'ultimo campo usato per appendere un nuovo campo*/
  struct qnode* tail; 
};
  int
fputsn(const char* str, int times)
{
  int i = 0;
  for(; i < times; ++i) { fputs(str, stdout); }
  return 0;
}

#define HEADER_TOP_ROW 1
#define HEADER_BOTTOM_ROW 2
#define ROW_SEPARATOR 3
#define LAST_ROW 4

  void
print_special_row(const struct buffer* b, int rowtype)
{
  int i = 0, l = b->cols;
  switch(rowtype)
  {
    case HEADER_TOP_ROW:
      if(b->table_style->opts & HAS_HEADER_TOP)
        {
          printf("%c%s",RECORD_SEPARATOR, b->table_style->htlc);
          for(;i < l;++i)
            {
              fputsn(b->table_style->htfill, b->field_max_size[i]);
              if(l-1 == i)
              {
                break;
              }
              else
              {
                fputs(b->table_style->htcs, stdout);
              }
            }
            printf("%s", b->table_style->htrc);
        }
      break;
    case HEADER_BOTTOM_ROW:
      if(b->table_style->opts & HAS_HEADER_BOTTOM)
        {
          printf("%c%s",RECORD_SEPARATOR, b->table_style->hblc);
          for(;i < l;++i)
            {
              fputsn(b->table_style->hbfill, b->field_max_size[i]);
              if(l-1 == i)
              {
                break;
              }
              else
              {
                fputs(b->table_style->hbcs, stdout);
              }
            }
            printf("%s", b->table_style->hbrc);
        }
      break;
    case LAST_ROW:
      if(b->table_style->opts & HAS_LAST_LINE)
        {
          printf("%c%s",RECORD_SEPARATOR, b->table_style->etlc);
          for(;i < l;++i)
            {
              fputsn(b->table_style->etfill, b->field_max_size[i]);
              if(l-1 == i)
              {
                break;
              }
              else
              {
                fputs(b->table_style->etcs, stdout);
              }
            }
            printf("%s", b->table_style->etrc);
        }
      break;
    case ROW_SEPARATOR:
      if(b->table_style->opts & HAS_ROW_SEP)
        {
          printf("%s", b->table_style->normalcolsep);
          for(;i < l;++i)
            {
              fputsn(b->table_style->rowsep_fill, b->field_max_size[i]);
              if(l-1 == i)
                {
                  break;
                }
              else
                {
                  fputs(b->table_style->normalcolsep, stdout);
                }
            }
            printf("%s", b->table_style->normalcolsep);
        }
      break;
  }
  putchar('\n');
  return;
}

#define FIND_CASE 1
#define ISALPHA(c) ((c > 0x60 && c < 0x7b) || (c > 0x40 && c < 0x5b) ? 1 : 0)
#define TOL(ch) (ISALPHA(ch) ? 0x20 | ch : ch)
  int 
strlist(const char* string, const char* substr, char del, int sensitive)
{
  int matching = 0;
  int offset = 0;
  int found = 0;
  int sl = strlen(substr);
  while(*string)
    {
      if(sensitive ? 
         TOL(*string) == TOL(*(substr + offset)) : 
             *string  ==     *(substr + offset))
        {
          ++matching;
          ++offset;
        }
      else 
        {
          if(del == *(string) || !(*string)) 
            {
              if(matching == sl)
                {
                  found = 1;
                  break; 
                }
            }
          while(*(++string) != del && *string);
          matching = offset = 0;
        }
      ++string;
    }
  return found;
}
#undef ISALPHA
#undef TOL

  struct buffer*
buffer_constructor()
{
  struct buffer* ret = (struct buffer*) malloc(sizeof(*ret));
  if(NULL == ret)
    {
      fprintf(stderr, "buffer_constructor(): impossibile allocare memoria "
          "per il buffer\n");
      exit(EXIT_FAILURE);
    }
  ret->comment = NULL;
  ret->bufsize = 0;
  ret->options = 0;
  ret->cols  = 0;
  ret->field_max_size = NULL;
  ret->head = NULL;
  ret->tail = NULL;
  return ret;
}

  int
field_destructor(struct field* info)
{
  info->chars = 0;
  info->alignment = ALIG_CENTER;
  info->fieldno = 0;
  if(NULL != info->buff) { free(info->buff); }
  return 0;
}

  int
buffer_destroy(struct buffer** buff)
{
  free((*buff)->field_max_size);
  free((*buff)->comment);
  free((*buff)->table_style);
  struct qnode* it = NULL;
  struct qnode* tmp = NULL;
  it = (*buff)->head;
  while(NULL != it)
    {
      field_destructor(it->field);
      free(it->field);
      tmp = it;
      it = it->next_field;
      free(tmp);
    }
  free(*buff);
  return 0;
}

  struct field* 
field_constructor(const char* string)
{
  struct field* info = (struct field*) malloc(sizeof(*info));
  info->chars = strlen(string);
  info->fieldno = -1;
  info->buff = (char*) malloc(sizeof(*(info->buff)) * (info->chars + 1));
  strcpy(info->buff, string);
  return info;
}

  struct qnode*
node(struct field* field)
{
  struct qnode* ret = (struct qnode*) malloc(sizeof(*ret));
  ret->field = field;
  ret->next_field = NULL;
  return ret;
}

#if defined(DEBUG)
  void
buff_dump(struct buffer* buff)
{
  for(struct qnode* q = buff->head; q != NULL; q = q->next_field)
    {
      printf("{\n");
      printf(" $buff = %s\n", q->field->buff);
      printf(" $fieldno = %i\n", q->field->fieldno);
      printf(" $chars = %lu\n", q->field->chars);
      printf("}\n");
    }
}
#endif

  int
print_comment(const struct buffer* buff)
{
  int printed = 0;
  if(NULL != buff->comment)
    {
      printed = printf("%s ", buff->comment->single_line);
    }
  return printed;
}

  void
table_size(struct buffer* buff)
{
  if(NULL == buff->field_max_size)
    {
      buff->field_max_size = (size_t*) malloc(sizeof(size_t) * (1 + buff->cols));
      memset(buff->field_max_size, 0, sizeof(size_t) * (1 + buff->cols));
    }
  for(struct qnode* q = buff->head; q != NULL; q = q->next_field)
    {
      if(q->field->chars + (MARGIN << 1) > buff->field_max_size[q->field->fieldno])
        {
          buff->field_max_size[q->field->fieldno] = (MARGIN << 1) + q->field->chars;
        }
    }
}

  int
serialize(struct buffer* buff, const char* string)
{
  int retval = 0;
  FILE* fp = fopen(string, "a");
  if(NULL == fp) 
    {
      fprintf(stderr, "serialize(): impossibile aprire il file");
      retval = 1; 
      buffer_destroy(&buff);
    }
  else
    {
      struct qnode* q = NULL;
      for(q = buff->head; NULL != q;q = q->next_field)
        {
          fprintf(fp, "%lu|%i|%i|%s\n", q->field->chars, q->field->alignment,
              q->field->fieldno, q->field->buff); 
          buff->cols = (q->field->fieldno > buff->cols) ? q->field->fieldno : buff->cols;
        }
      /* Cerca il numero massimo per ogni colonna prima di liberare la memoria */
      table_size(buff);
      struct qnode* tmp = NULL;
      for(q = buff->head; NULL != q; )
        {
          field_destructor(q->field);
          free(q->field);
          tmp = q;
          q = q->next_field;
          free(tmp);
        }
      buff->head = NULL;
      buff->tail = NULL;
      buff->bufsize = 0;
      buff->options = 1;
    }
  fclose(fp);
  return retval;
}

  int
append_field(struct buffer* field_list,int field_no, int alig,
    size_t field_size, const char* string)
{
  struct field* finf = field_constructor(string);
  finf->fieldno = field_no;
  finf->alignment = alig;
  field_list->bufsize += field_size;
  struct qnode* to_append = node(finf);
  if(NULL == to_append)
    {
      fprintf(stderr, "Impossibille allocare memoria per il seguente campo");
      buffer_destroy(&field_list);
      exit(EXIT_FAILURE);
    }
  /* La lista e vuota quindi l'elemento da inserire e' in testa; */
  if(NULL == field_list->head)
    {
      field_list->head = to_append;
      field_list->tail = field_list->head;
    }
  else
    {
      field_list->tail->next_field = to_append;
      field_list->tail = to_append;
    }
  if(field_list->bufsize >= FILE_TOO_BIG) 
    { 
      field_list->options = 1;
      field_list->bufsize = 0;
      serialize(field_list, AUX_FILE); 
#if 1 && defined(DEBUG)
      fprintf(stderr, "File troppo grande, usato il file ausiliario");
#endif

    }
  return field_list->cols;
}

  static inline void
printn(char c, int n)
{
  int i = 0;
  for(;i < n;++i) { putchar(c); }
}

  int
remove_trailing_withespaces(char* string, int strlen)
{
  int removed = 0, spaces = 0;
  int i = strlen;
  for(;i>-1;--i)
    {
      if(' ' == *(string + i))
        {
          ++spaces; 
          ++removed;
        }
      else 
        {
          break;
        }
    }
  if(0 != spaces) { free(string - spaces); }
  return spaces; 
}

  static inline void
text_center(const char* buff, int bufsize, int max)
{
  int start = (int)(.5 * (max - bufsize - MARGIN - 1));
  int recupero = max - ((start << 1) + bufsize + (MARGIN << 1));
  printn(' ', start);
  fputs(buff, stdout);
  printn(' ', start + recupero);
}

#define TEXT_RIGHT(_buffer, _buffer_size, limit) \
  printn(' ', limit - MARGIN * 2 - _buffer_size); \
  fputs(_buffer, stdout);

#define TEXT_LEFT(_buffer, _buffer_size, limit) \
  fputs(_buffer, stdout); \
  printn(' ', limit - _buffer_size - MARGIN - 1);

  static inline void
text_align(const char* buffer, int buffer_size, int limit, int alignment)
{
  switch(alignment)
    {
      case ALIG_RIGHT:
        TEXT_RIGHT(buffer, buffer_size, limit);
        break;
      case ALIG_CENTER:
        text_center(buffer, buffer_size, limit);
        break;
      default:
        TEXT_LEFT(buffer, buffer_size, limit);
    }
}

  int
is_rowsep(const char* str)
{
  int i = 0;
  int l = strlen(str);
  int r = -1;
  for(;i < l; ++i)
    {
      if(' ' == *(str + i))
        {
          r = 0;
          break;
        }
    }
  return (!r ? 0 : 1);
}

struct colpos
{
  size_t column_position;
  struct colpos* next_column;
};
struct columns
{
  int cols;
  struct colpos* first_col;
  struct colpos* last_col;
};
  void
reverse(buf_state* buff)
{
  ssize_t bytes;
  char* line = NULL;
  size_t stn = 0;
  int tablesize = -1;
  int i = 0;
  struct columns* cols = malloc(sizeof(*cols));
  cols->first_col = cols->last_col = NULL;
  cols->cols = 0;
  while(-1 != (bytes = getline(&line, &stn, buff->fptr)))	
    {
      buff->bytes += bytes;
      buff->cols = 0;
      ++buff->rows;
      if(!is_rowsep(line))
        {
          for(i = 1; i < bytes; ++i)
            if(*(line + i) != ' ') break;
          for(; i < bytes; ++i)
            {
              if(*(line + i) == '|') 
                {
                  putchar(';');
                  ++i;
                  while(*(line + i + 1) == ' ') ++i;
                }
              else
                putchar(*(line + i));
            }
          putchar('\n');
        }
#ifdef DEBUG
      else { printf("Separatore, riga: %d\n", buff->rows); }
#endif
    }
}

#define WHITESPACE(n) printn(' ', n)
#define PUT_MARGIN() WHITESPACE(MARGIN) 

#define PUT_FIELD(buff, b, siz, limit, alignment, fieldno) \
    fputs(buff->table_style->normalcolsep, stdout); \
    PUT_MARGIN(); \
    text_align(b, siz, limit, alignment); \
    PUT_MARGIN(); \
    if(cols == fieldno + 1)  \
      {\
        fputs(buff->table_style->normalcolsep, stdout); \
        putchar('\n'); \
        if(buff->table_style->opts & HAS_ROW_SEP) \
          {\
            print_special_row(buff, ROW_SEPARATOR); \
          }\
      }

  void
print_external_file(const struct buffer* b, const char* string)
{
  FILE* fp = fopen(string, "r");
  char* line = NULL;
  ssize_t bytes = 0;
  size_t stn = 0;

  char* saveptr = NULL;
  char* token = NULL;
  char* buff = NULL;
  int fieldno = 0;
  int align = 0;
  int rows = 0;
  int siz = 0;
  int cols = b->cols;
  while(-1 != (bytes = getline(&line, &stn, fp)))	
    {
      /* '\n' = 1 byte */
      if(bytes > 1)
        {

          //sscanf()
          token = strtok_r(line, "|", &saveptr);
          siz = atoi(token);
          token = strtok_r(NULL, "|", &saveptr);
          align = atoi(token);
          token = strtok_r(NULL, "|", &saveptr);
          fieldno = atoi(token);
          token = strtok_r(NULL, "|", &saveptr);
          buff = token;
          buff[strlen(buff) - 1] = '\0';


          print_comment(b);
//          row_sep(b, 1);
          print_comment(b);
          PUT_FIELD(b, buff, siz, b->field_max_size[fieldno], align, fieldno)
          print_comment(b);
 //         row_sep(b, 1);
          print_comment(b);
//          row_sep(b, 1);
          if(fieldno + 1 == cols)
            ++rows;
        }
    }
  free(line);
  fclose(fp);
  remove(string);
}
  void
print_table(struct buffer* b)
{

  if(0 != b->bufsize)
    {
      int cols = b->cols;
      if(b->options)
        {
          print_external_file(b, AUX_FILE);
        }
      struct qnode* it = b->head;
      /* Stampa della prima riga */
      print_comment(b);
      print_special_row(b, HEADER_TOP_ROW);
      print_comment(b);
      struct field* f;
      do
        {
          f = it->field;
          PUT_FIELD(b, f->buff, f->chars, b->field_max_size[f->fieldno],
              f->alignment, f->fieldno)
          it = it->next_field;
        } while (NULL != it && (0 != it->field->fieldno));
      /* Fine se presente una sola riga */
      if(NULL != it)
        {
          print_comment(b);
          print_special_row(b, HEADER_BOTTOM_ROW);
          for(; NULL != it; it = it->next_field)
            {
              if(it->field->fieldno == 0) { print_comment(b); }
              f = it->field;
              PUT_FIELD(b, f->buff, f->chars, b->field_max_size[f->fieldno],
                  f->alignment, f->fieldno)
            }
        }
      print_comment(b);
      print_special_row(b, LAST_ROW);
    }
}

  int
table(buf_state* buff, const char* separator, int comment_style, int tstyle)
{
  char* line = NULL;
  char* token = NULL;
  ssize_t bytes = 0;
  size_t stn = 0;
  struct buffer* b = buffer_constructor();
  if(comment_style != -1 && comment_style >= 0 && comment_style < COMMENTNO)
    {
      b->comment = (struct comment*) malloc(sizeof(struct comment));
      memcpy(b->comment, &comment_styles[comment_style], sizeof(struct comment));
    }
  else { b->comment = NULL; }

  b->table_style = (struct table_style*) malloc(sizeof(struct table_style));
  memcpy(b->table_style, &styles[tstyle], sizeof(struct table_style));

  int idx, virtual_fields = 0,cols = 0, alig = ALIG_CENTER;
  while(-1 != (bytes = getline(&line, &stn, buff->fptr)))	
    {
      buff->bytes += bytes;
      buff->cols = idx = 0;
      if(0 != buff->rows)
        {
          alig = ALIG_RIGHT; 
        }
      ++buff->rows;
      /* '\n' = 1 byte */
      if(bytes > 1)
        {
          token = strtok(line, separator);
          token[bytes - 1] = '\0';
          switch(*token)
            {
              case '>':
                alig = ALIG_RIGHT;
                append_field(b,cols, alig, strlen(token)-1,1+token);
                break;
              case '<':
                alig = ALIG_LEFT;
                append_field(b,cols, alig, strlen(token)-1,1+token);
                break;
              case '|':
                alig = ALIG_CENTER;
                append_field(b,cols, alig, strlen(token)-1,1+token);
                break;
              default:
                append_field(b,cols, alig, strlen(token), token);
            }
          while(NULL != token)
            {
              token = strtok(NULL, separator);
              ++cols;
              if(NULL != token)
                { 
                  switch(*token)
                    {
                      case '>':
                        alig = ALIG_RIGHT;
                        append_field(b,cols, alig, strlen(token)-1,1+token);
                        break;
                      case '<':
                        alig = ALIG_LEFT;
                        append_field(b,cols, alig, strlen(token)-1,1+token);
                        break;
                      case '|':
                        alig = ALIG_CENTER;
                        append_field(b,cols, alig, strlen(token)-1,1+token);
                        break;
                      default:
                        append_field(b,cols, alig, strlen(token), token);
                    }
                }
            }
        }
      if(cols > virtual_fields) { virtual_fields = cols; }
      cols = 0;
    }
  free(line);
  if(b->options == 1 && b->cols < virtual_fields)
  {
    b->cols = virtual_fields;
    b->field_max_size = realloc(b->field_max_size, sizeof(size_t) * virtual_fields);
  }
  b->cols = virtual_fields;
  table_size(b);

#if 0 && defined(DEBUG) 
  buff_dump(b);
#endif

  print_table(b);
  buffer_destroy(&b);
  return 0;
}

  int 
main(int argc, char* const argv[])
{
  int options;
  buf_state buff = BUFF_INIT;
  int rev = 0;
  int tstyle = 0;
  char* st = NULL;
  remove(AUX_FILE);
  int cstyle = -1;
  struct option long_options[] =
  {
    {"no-header", no_argument,       0, 'a'},
    {"style",     required_argument, 0, 's'},
    {"file",      required_argument, 0, 'f'},
    {"reverse",   required_argument, 0, 'r'},
    {"author",    no_argument,       0, 'a'}
  };
  int optind = 0;
  while(-1 != (options = getopt_long(argc, argv, "ars:f:S:P:c:",
          long_options, &optind)))
  {
    switch(options)
      {
        case 'c':
            {
              int i, n = COMMENTNO;
              for(i = 0; i < n; ++i) 
                {
                  if(strlist(comment_styles[i].lang, optarg, '|', 1))
                    {
                      cstyle = i; 
                      break;
                    }
                }
              break;
            }
        case 'r':
          rev = 1;
          break;
        case 'a':
          printf("Autore: Trachi Yassin 2020 (v.1.0)\n");
          exit(EXIT_SUCCESS);
          break;
        case 'P':
          printf("PREFISSO RIGA"); 
          break;
        case 'S':
          printf("SUFFISSO RIGA"); 
          break;
        case 'f':
          buff.fptr = fopen(optarg, "r");
          if(!buff.fptr)
            {
              fprintf(stderr, "Impossibile aprire il file");
              exit(EXIT_FAILURE);
            }
          break;
        case 's':
          tstyle = STRTOSTYLE(optarg);
          break;
        default:
          fprintf(stderr, "Opzione non riconosciuta");
          exit(EXIT_FAILURE);
      }	
  }	
  if(rev == 0)
    {
      if(NULL == st) { table(&buff, ";", cstyle, tstyle); }
      else           { table(&buff, st, cstyle, tstyle);  }
    }
  else 
    {
      reverse(&buff); 
    }
  if(stdin != buff.fptr) { fclose(buff.fptr); }
  return EXIT_SUCCESS;	
}

#ifdef _WIN32
  ssize_t
getline(char** lineptr, size_t* n, FILE* stream)
{
  char c;
  while((c = fgetc()))
}
#endif
