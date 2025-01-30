#include "../include/types.h"

const char *get_enum_name(enum token_type type)
{
  switch(type)
  {
    case NONE:         return "NONE";
    case UNKNOWN:      return "UNKNOWN";

    case STR_LITERAL:  return "STR_LITERAL";
    case INT_LITERAL:  return "INT_LITERAL";
    case IDENTIFIER:   return "IDENTIFIER";
    case END:          return "END";

    case VERTICAL_BAR: return "VERTICAL_BAR";
    case SEMICOLON:    return "SEMICOLON";
    case DOLLAR:       return "DOLLAR";
    case OPEN_PAR:     return "OPEN_PAR";
    case CLOSE_PAR:    return "CLOSE_PAR";
    default:           return "NULL";
  }
}
