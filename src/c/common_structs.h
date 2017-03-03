#ifndef COMMON_STRUCTS_H
#define COMMON_STRUCTS_H

// Container for unsigned strings which contain raw data.  Relies on a length variable instead of null termination
// since numerical values are stored directly in these strings instead of being translated through characters.  This
// means that the null character (AKA: "\0", #0, end of string) is frequently used to represent the actual number 0 and
// can't be utilized in this structure.  Without any kind of terminator it's up to the function which is handling one
// of these data strings to never read beyond the provided length of the string.  Likewise, any function which writes
// to one of these strings must accurately update the length variable after doing so.
typedef struct data_string
{
  unsigned short length;
  unsigned char* data;
} Data_string;

// Linked list structure for playing field positions.  Copy_nodes list the filtered contents of playing field
// positions.  Information regarding unrevealed adjacency and mine presence will be removed from the position data when
// creating these nodes if the game has not yet been finished.  A separate ref_node structure is used internally for
// reading and writing unfiltered data directly to/from the playing field.
typedef struct copy_node
{
  unsigned char x;
  unsigned char y;
  unsigned char position;
  struct copy_node* next;
} Copy_node;

#endif // COMMON_STRUCTS_H

