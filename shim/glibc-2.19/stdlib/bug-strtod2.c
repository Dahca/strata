#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static const char *tests[] =
  {
    "inf", "Inf", "iNf", "inF", "INf", "iNF", "INF", "InF",
    "infinity", "Infinity", "InfInity", "INFINITY"
  };
#define ntests (sizeof (tests) / sizeof (tests[0]))

static int
do_test (void)
{
  /* The Turkish locale is notorious because tolower() maps 'I' to the
     dotless lowercase 'i' and toupper() maps 'i' to an 'I' with a dot
     above.  */
  if (setlocale (LC_ALL, "tr_TR.UTF-8") == NULL)
    {
      puts ("cannot set locale");
      return 0;
    }

  int res = 0;
  for (int i = 0; i < ntests; ++i)
    {
      char *endp;
      double d = strtod (tests[i], &endp);
      if (*endp != '\0')
	{
	  printf ("did not consume all of '%s'\n", tests[i]);
	  res = 1;
	}
      if (!isinf (d))
	{
	  printf ("'%s' does not pass isinf\n", tests[i]);
	  res = 1;
	}
    }

  return res;
}

#define TEST_FUNCTION do_test ()
#include "../test-skeleton.c"
