#include <linux/random.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>

/*
Position Scheme:
Bit1 = Adjacent 1's column
Bit2 = Adjacent 2's column
Bit3 = Adjacent 4's column
Bit4 = Adjacent 8's column
Bit5 = Unchecked/Checked
Bit6 = Unmined/Mined
Bit7 = Unflagged/Flagged
Bit8 = Unrevealed/Revealed
*/

// bit flags for position scheme
const unsigned char CHECKED_BIT = 0x10u;
const unsigned char MINED_BIT = 0x20u;
const unsigned char FLAGGED_BIT = 0x40u;
const unsigned char REVEALED_BIT = 0x80u;

int main()
{
   // vars to set once
   unsigned char* field_begin; // Pointer to first position char in field.
   unsigned char* field_end; // Pointer to char after last field position.
   unsigned char height; // How many positions tall the field is.
   unsigned short mines; // Number of mines on the field.
   unsigned short size; // Total number of positions on the field.
   unsigned const short urand; // dev/urandom number used to seed srand.
   unsigned char width; // How many positions wide the field is.

   // vars which change 
   unsigned short mines_placed; // Numbe of mines randomly placed on field.
   unsigned char* position; // Pointer into current field position.
   unsigned short positions_until_random; // Number of unmined positions left
                                          // to traverse before arriving at
                                          // position where the next mine was
                                          // randomly chosen to be placed.

   // initialize field
   width = 30;
   height = 24;
   size = width * height;
   field_begin = (unsigned char*)calloc(size, sizeof(unsigned char));
   field_end = field_begin + size;

   // initialize mines
   mines = 200;
   mines_placed = 0;
   syscall(SYS_getrandom, &urand, sizeof(unsigned short), 0);
   srand(urand);
   while(mines_placed < mines)
   {
      // At this point we view the field as a linked list of unmined positions.
      // This means that there are only (size - mines_placed) positions in the
      // list at any given time.  We choose a random number which corresponds
      // to one of the remaining positions.
      positions_until_random = rand() % (size - mines_placed);
      position = field_begin;

      // We will now traverse this "list" of unmined positions.  For the sake
      // of performance we'll just traverse the existing positions and not
      // count the mined ones against the positions_until_random variable.
      // This is computationally less expensive than maintaining an actual
      // linked list and updating references between the nodes/positions.
      while(positions_until_random > 0)
      {
         if((*position & MINED_BIT) != MINED_BIT)
         {
            // Position is unmined, so it counts as a traverse towards the
            // final randomly chosen position.  Mined positions do not.
            positions_until_random--;
         }

         // Move to the next position, even if the current one did not count
         // as one of the traversed locations in the list because it was mined.
         position++;
      }

      // We have now performed all of the traverses of unmined locations which
      // we needed to, but we must manually check the remaining locations to
      // see if they are mined positions which would not have been "caught" by
      // the traverse, since they don't count against it.  In other words:
      // these are the nodes that would have been removed from the end of our
      // conceptual linked list after being mined.
      while((*position & MINED_BIT) == MINED_BIT)
      {
         position++;
      }
      *position |= MINED_BIT;
      mines_placed++;
   }

   // Bolting on rudimentary output for testing purposes.  Will replace later.
   for(unsigned short y = 0; y < height; y++)
   {
      for(unsigned short x = 0; x < width; x++)
      {
         position = field_begin + y * width + x;
         if((*position & MINED_BIT) == MINED_BIT)
         {
            printf("X");
         }
         else
         {
            printf("_");
         }
      }
      printf("\n");
   }

   // clear the playing field
   free(field_begin);
   return 0;
}

