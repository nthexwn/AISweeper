#include "sweeper.h"

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

// Bit flags for position scheme
const unsigned char ADJACENT_BITS = 0x0fu;
const unsigned char CHECKED_BIT = 0x10u;
const unsigned char MINED_BIT = 0x20u;
const unsigned char FLAGGED_BIT = 0x40u;
const unsigned char REVEALED_BIT = 0x80u;

// Getters and setters for position data
unsigned char getAdjacent(unsigned char *position)
{
   return *position & ADJACENT_BITS;
}
void setAdjacent(unsigned char* position, unsigned char adjacent_mines)
{
   *position |= adjacent_mines;
}
bool isChecked(unsigned char* position)
{
   return (*position & CHECKED_BIT) == CHECKED_BIT;
}
void setChecked(unsigned char* position, bool value)
{
   if(value) *position |= CHECKED_BIT;
   else *position &= ~(CHECKED_BIT);
}
bool isMined(unsigned char* position)
{
   return (*position & MINED_BIT) == MINED_BIT;
}
void setMined(unsigned char* position, bool value)
{
   if(value) *position |= MINED_BIT;
   else *position &= ~(MINED_BIT);
}
bool isFlagged(unsigned char* position)
{
   return (*position & FLAGGED_BIT) == FLAGGED_BIT;
}
void setFlagged(unsigned char* position, bool value)
{
   if(value) *position |= FLAGGED_BIT;
   else *position &= ~(FLAGGED_BIT);
}
bool isRevealed(unsigned char* position)
{
   return (*position & REVEALED_BIT) == REVEALED_BIT;
}
void setRevealed(unsigned char* position, bool value)
{
   if(value) *position |= REVEALED_BIT;
   else *position &= ~(REVEALED_BIT);
}

// TODO: Break this out into its own encapsulated file+function.  Main will then be used as an API layer between the
// user and this encapsulated game instance.
int start_game()
{
   // Variables to set once
   unsigned char* field_begin; // Pointer to first position char in field.
   unsigned char* field_end; // Pointer to char after last field position.
   unsigned char height; // How many positions tall the field is.
   unsigned short mines; // Number of mines on the field.
   unsigned short size; // Total number of positions on the field.
   unsigned short urand; // dev/urandom number used to seed srand.
   unsigned char width; // How many positions wide the field is.

   // Variables which change 
   unsigned short mines_placed; // Numbe of mines randomly placed on field.
   unsigned char* position; // Pointer into current field position.

   // Initialize field
   width = 30;
   height = 24;
   size = width * height;
   field_begin = (unsigned char*)calloc(size, sizeof(unsigned char));
   field_end = field_begin + size;

   // Initialize mines
   mines = 200;
   mines_placed = 0;
   syscall(SYS_getrandom, &urand, sizeof(unsigned short), 0);
   srand(urand);
   while(mines_placed < mines)
   {
      // At this point we view the field as a linked list of unmined positions.  This means that there are only (size -
      // mines_placed) positions in the list at any given time.  We choose a random number which corresponds to one of
      // the remaining positions.
      unsigned short positions_until_random = rand() % (size - mines_placed);
      position = field_begin;

      // We will now traverse this "list" of unmined positions.  For the sake of performance we'll just traverse the
      // existing positions and not count the mined ones against the positions_until_random variable.  This is
      // computationally less expensive than maintaining an actual linked list and updating references between the
      // nodes/positions.
      while(positions_until_random > 0)
      {
         if(!isMined(position))
         {
            // Position is unmined, so it counts as a traverse towards the final randomly chosen position.  Mined
            // positions do not.
            positions_until_random--;
         }

         // Move to the next position, even if the current one did not count as one of the traversed locations in the
         // list because it was mined.
         position++;
      }

      // We have now performed all of the traverses of unmined locations which we needed to, but we must manually check
      // the remaining locations to see if they are mined positions which would not have been "caught" by the traverse,
      // since they don't count against it.  In other words: these are the nodes that would have been removed from the
      // end of our conceptual linked list after being mined.
      while(isMined(position))
      {
         position++;
      }
      setMined(position, true);
      mines_placed++; 
   }

   // Calculate adjacency info for all unmined positions
   for(unsigned short y = 0; y < height; y++)
   {
      for(unsigned short x = 0; x < width; x++)
      {
         position = field_begin + width * y + x;
         if(!isMined(position))
         {
            unsigned char adjacent_mines = 0;
            if(x > 0 && y > 0)
            {
               unsigned char* above_left = field_begin + width * (y - 1) + x - 1;
               if(isMined(above_left)) adjacent_mines++;
            }
            if(y > 0)
            {
               unsigned char* above = field_begin + width * (y - 1) + x;
               if(isMined(above)) adjacent_mines++;
            }
            if(x < width - 1 && y > 0)
            {
               unsigned char* above_right = field_begin + width * (y - 1) + x + 1;
               if(isMined(above_right)) adjacent_mines++;
            }
            if(x > 0)
            {
               unsigned char* left = field_begin + width * y + x - 1;
               if(isMined(left)) adjacent_mines++;
            }
            if(x < width - 1)
            {
               unsigned char* right = field_begin + width * y + x + 1;
               if(isMined(right)) adjacent_mines++;
            }
            if(x > 0 && y < height - 1)
            {
               unsigned char* below_left = field_begin + width * (y + 1) + x - 1;
               if(isMined(below_left)) adjacent_mines++;
            }
            if(y < height - 1)
            {
               unsigned char* below = field_begin + width * (y + 1) + x;
               if(isMined(below)) adjacent_mines++;
            }
            if(x < width - 1 && y < height - 1)
            {
               unsigned char* below_right = field_begin + width * (y + 1) + x + 1;
               if(isMined(below_right)) adjacent_mines++;
            }
            setAdjacent(position, adjacent_mines);
         }
      }
   }

   // Bolting on rudimentary output for testing purposes.  Will replace later.
   for(unsigned short y = 0; y < height; y++)
   {
      for(unsigned short x = 0; x < width; x++)
      {
         position = field_begin + y * width + x;
         if(isMined(position))
         {
            printf("X");
         }
         else
         {
            printf("%d", getAdjacent(position));
         }
      }
      printf("\n");
   }

   // Clear the playing field
   free(field_begin);
   return 0;
}

