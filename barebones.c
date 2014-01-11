// Bare Bones interpreter
// $Id: barebones.c 14 2008-02-19 19:34:41Z eric $
// Copyright 2008 Eric Smith <eric@brouhaha.com>

// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License version 3 as
// published by the Free Software Foundation.  Note that I am not
// granting permission to redistribute or modify this program under the
// terms of any other version of the General Public License.

// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program (in the file "COPYING"); if not, see
// <http://www.gnu.org/licenses/>.

// Modified by Michael KHAU
// with print statement


#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "barebones.h"
#include "parser.tab.h"

struct var_t
{
  struct var_t *next;
  char *name;
  bool init;
  stmt_t *fonc;
  env_t *env_l;
  uintmax_t val;
};

struct env_t
{
  struct env_t *parent;
  var_t *first;
  int level;
};

int stmt_line;  // line number of currently executing statement, for
		// error reporting purposes

bool init_to_zero = true;  // if false, reference to uninitialized variables
                           // (other than in a clear statement) will result
                           // in a run time error

bool verbose = false; // if true, show some variable informations

void usage (FILE *f)
{
  fprintf (f, "%s [options]  [initializers...] src-file:\n", progname);
  fprintf (f, "options:\n");
  fprintf (f, "  -u            report uninitialized variables\n");
  fprintf (f, "  -O            optimize\n");
  fprintf (f, "  -v            with variable informations\n");
  fprintf (f, "initializers:\n");
  fprintf (f, "  var=value     e.g. X=37\n");
}


void error (const char *fmt, ...)
{
  va_list ap;

  fprintf (stderr, "error on line %d: ", stmt_line);
  va_start (ap, fmt);
  vfprintf (stderr, fmt, ap);
  va_end (ap);
  fprintf (stderr, "\n");
  exit (2);
}

// linked list of macros
// unsorted because it is assumed that there will only be a small number
// of macros, so the cost of looking them up by a linear search of a
// list at "compile time" will be negligible.
stmt_t *proc_head = NULL;

stmt_t *find_proc (char *name){
  stmt_t *proc;
  for (proc = proc_head; proc; proc = proc->proc)
    {
      if (strcasecmp (name, proc->name) == 0)
	return proc;
    }
  return NULL;
}		

env_t *create_env (env_t *parent)
{
  env_t *env;
  env = alloc (sizeof (env_t));
  env->parent = parent;
  env->first = NULL;
  if (parent){
  env->level = parent->level + 1;
  } else {
	env->level = 0;
  }
  return env;
}

env_t *current_env = NULL;

var_t *copy_find_var (char *name)
{
  var_t *res = 	alloc (sizeof (var_t));
  res->val = 0;
  res->name = name;
  res->init = true;
  return res;
}

var_t *find_var (char *name)
{
  var_t *var;
  env_t *env;

  if (!current_env) {
    current_env = create_env(NULL);
  }

  // linear search for named variable
  for (env = current_env; env; env = env->parent)
    {
    for (var = env->first; var; var = var->next)
      {
        if (strcasecmp (name, var->name) == 0)
        {
//		  printf("%s found at level %d\n", name, env->level);
		  return var;
		}
      }
  }

  // if not found, create
  var = alloc (sizeof (var_t));
  var->name = strdup (name);
  if (init_to_zero)
    {
      var->val = 0;
      var->init = true;
    }
  else
    {
      var->init = false;
    }
  var->next = current_env->first;
  current_env->first = var;
  return var;
}

var_t *new_int_var (int num)
{
  var_t *var = alloc (sizeof (var_t));
  char *c = alloc (sizeof (char) * 16);
  sprintf(c, "%d", num);
	var->name = c;
	var->val = num;
//	printf("%s => %d\n",var->name, var->val);
	var->init = true;
  return var;
}

var_t *new_arg_var (char* s)
{
  var_t *var = alloc (sizeof (var_t));
  var->name = alloc (sizeof (char) * 32);
  strcpy(var->name, s);
//  printf("New var : %s\n", var->name);
  var->val = 0;
  var->init = false;
  var->next = NULL;
  return var;
}

void set_var (var_t *var, uintmax_t val)
{
  var->val = val;
  var->init = true;
}

static void define_var (char *s)
{
  var_t *var;
  char *p = strchr (s, '=');
  char *name = newstrn (s, p - s);
  intmax_t val;
  p++;

  val = a_to_intmax (p);
  if (val < 0)
    fatal (2, "negative values are not permitted");

  var = find_var (name);
  set_var (var, val);
}


void print_vars (bool show_uninitialized)
{
  var_t *var;
  env_t *env;
  for (env = current_env; env; env = env->parent){
  for (var = env->first; var; var = var->next)
    {
      if ((! var->init) && (! show_uninitialized))
	continue;
      printf ("%s at level %d: ", var->name, env->level);
      if (var->init)
	printf ("%" PRIuMAX, var->val);
      else
	printf ("uninitialized");
      printf ("\n");
    }
  }
}


void check_var_init (var_t *var)
{
  if (! var->init)
    error ("unitialized variable %s", var->name);
}

void print_stmt (stmt_t *stmt){
	stmt_t *tmp;
	var_t *var;
switch (stmt->type)
    {
		case CLEAR_STMT:
		printf("clear %s\n", stmt->var->name);
		break;
		case INCR_STMT:
		printf("incr %s\n", stmt->var->name);
		break;
		case DECR_STMT:
		printf("decr %s\n", stmt->var->name);
		break;
		case WHILE_STMT:
		printf("while %s not 0 do\n", stmt->var->name);
		tmp = stmt->stmt_list;
		while (tmp){
			print_stmt(tmp);
			tmp = tmp->next;
		}
		break;
		case COPY_STMT:
		printf("copy %s to %s\n", stmt->var->name, stmt->dest->name);
		break;
		case PRINT_STMT:
		printf("print %s\n", stmt->var->name);
		break;
		case DEFPROC_STMT:
		printf("defproc %s\n", stmt->name);
		break;
		case RUNPROC_STMT:
		printf("runproc %s (", stmt->name);
		var = stmt->var;
		while (var){
			printf(" %s",var->name);
			var = var->next;
		}
		printf(")\n"); 
		break;
		default:
		printf("other\n");
		break;
				
	}
}
/*
 * Changed the return type of execute_stmt and execute_stmt_list
 * to enable exit statements.
 * If execute_stmt returns 1, it means that there is an exit statement.
 */
int execute_stmt (stmt_t *stmt)
{
	
  int tmp;
  env_t *next_env;
  stmt_t *proc;
  var_t *var1;
  var_t *var2;
  var_t *modifvar;
  stmt_line = stmt->line;
  if (stmt->type != RUNPROC_STMT && stmt->type != DEFPROC_STMT && stmt->var){
	  stmt->var = find_var(stmt->var->name);
  }
  switch (stmt->type)
    {
    case CLEAR_STMT:
      stmt->var->val = 0;
      stmt->var->init = true;
      break;
    case INCR_STMT:
      check_var_init (stmt->var);
      if (stmt->var->val == UINTMAX_MAX)
	fatal (2, "overflow");
      stmt->var->val++;
      break;
    case DECR_STMT:
      check_var_init (stmt->var);
      if (stmt->var->val)
	stmt->var->val--;
      break;
    case WHILE_STMT:
      check_var_init (stmt->var);
      while (stmt->var->val){
		tmp = execute_stmt_list (stmt->stmt_list);
		if (tmp == 1){ // exited out of a procedure inside a loop
			return 1;
		}
	  }
      break;
    case COPY_STMT:
      check_var_init (stmt->var);
      stmt->dest->val = stmt->var->val;
      stmt->dest->fonc = stmt->var->fonc;
      stmt->dest->env_l = stmt->var->env_l;
      stmt->dest->init = true;
      break;
    case ADD_CLEAR_STMT:
      check_var_init (stmt->var);
      check_var_init (stmt->dest);
      stmt->dest->val += stmt->var->val;
      stmt->var->val = 0;
      break;
    case PRINT_STMT: // Adding print for easier reading
      check_var_init (stmt->var);
      
      if (stmt->var->fonc){
		  printf("%s : ", stmt->var->name);
		  proc = stmt->var->fonc->stmt_list;
		  while (proc){
		  print_stmt(proc);
		  proc = proc->next;
		  }
		  break;
      }
      printf("%s : %d ", stmt->var->name, (int)stmt->var->val);
      if (verbose){
		printf("at level %d\n", current_env->level);
	  } else {
		  printf("\n");
	  }
      break;
    case DEFPROC_STMT:
      stmt->proc = proc_head;
      proc_head = stmt;
      printf("New Subroutine : %s\n", proc_head->name);
      var1 = stmt->var;
      if (verbose)
		while (var1){
			printf("\tArgument needed : %s\n", var1->name);
			var1 = var1->next;
		}
      break;
    case LAMBDA_STMT:
        var1 = stmt->var;
        var1->env_l = current_env;
        proc = new_stmt(DEFPROC_STMT, stmt->dest);
        var1->fonc = proc;
        proc->var = stmt->dest;
        proc->stmt_list = stmt->stmt_list;
        break;
    case RUNPROC_STMT:
      proc = find_proc(stmt->name);
      next_env = create_env(current_env);
      if (!proc){
		  var1 = find_var(stmt->name);
		  proc = var1->fonc;
		  if (!proc){
			  	fatal(2, "subroutine doesn't exist");
		  }
		  merge_env(next_env, var1->env_l);		  
	  }
      printf("Running subroutine : %s\n", stmt->name);
      if (proc) { // There is a macro with this name
		var1 = proc->var;
		var2 = stmt->var;
		while (var1 != NULL){
			if (verbose)
				printf("\targ %s",var1->name);
			if (var2 != NULL) {
			modifvar = copy_find_var(var1->name);
			if (var2->val == 0){ // variable argument
				modifvar->val = find_var(var2->name)->val;
			} else { // integer argument
				modifvar->val = var2->val;
			}
			if (verbose)
				printf(" => %d \n", (int) modifvar->val);
			modifvar->next = next_env->first;
			next_env->first = modifvar;
			var1 = var1->next;
			var2 = var2->next;
		} else {
		  fatal(2, "not enough arguments");
		}
	}
	current_env = next_env;
	execute_stmt_list (proc->stmt_list);
  	current_env = current_env->parent;
      } else {
	fatal(2, "subroutine doesn't exist");
      }
      break;
    case EXIT_STMT:
      return 1;
      break;
    }

    return 0;
}

void merge_env(env_t *base, env_t *add){
		var_t *var;
		var_t *cpy;
		for (var = add->first; var; var = var->next){
				cpy = copy_find_var(var->name);
				cpy->val = var->val;
				cpy->env_l = var->env_l;
				cpy->fonc = var->fonc;
				cpy->next = base->first;
				base->first = cpy;
//				printf("base now has %s = %d\n", cpy->name, cpy->val);
		}
}

int execute_stmt_list (stmt_t *list)
{
  int tmp;
  while (list)
    {
      tmp = execute_stmt (list);
      if (tmp == 1){ // encountered an exit statement
		  return 1; // to be sure it isn't an exit statement inside of a while loop
	  }
      list = list->next;
    }
  return 0;
}


stmt_t *new_stmt (stmt_type_t type, var_t *var)
{
  stmt_t *stmt = alloc (sizeof (stmt_t));
  stmt->type = type;
  stmt->var = var;
  return stmt;
}


void add_stmt_to_list (stmt_t *head, stmt_t *stmt)
{
  if (head->tail)
    head->tail->next = stmt;
  else
    head->next = stmt;
  head->tail = stmt;
}

void add_val_to_list (var_t *head, var_t *var)
{
  head->next = var;
}

extern FILE *yyin;
extern int yydebug;
int yyparse (void);

bool parse_prog (char *fn)
{
  int status;
  yyin = fopen (fn, "r");
  if (! yyin)
    fatal (1, "can't read program\n");
#if 0
  yydebug = 1;
#endif
  status = yyparse ();
  fclose (yyin);
  return status == 0;
}


void optimize_stmt_list (stmt_t *list);


void optimize_while_stmt (stmt_t *stmt)
{
  bool match = false;
  var_t *loop_var = stmt->var;
  stmt_t *first_stmt = stmt->stmt_list;
  stmt_t *second_stmt = first_stmt->next;
  var_t *src;
  var_t *dest;
  if (first_stmt && second_stmt && ! second_stmt->next)
    {
      if ((first_stmt->type == INCR_STMT) &&
	  (first_stmt->var != loop_var) &&
	  (second_stmt->type == DECR_STMT) &&
	  (second_stmt->var == loop_var))
	{
	  match = true;
	  src = second_stmt->var;
	  dest = first_stmt->var;
	}
      else if ((second_stmt->type == INCR_STMT) &&
	       (second_stmt->var != loop_var) &&
	       (first_stmt->type == DECR_STMT) &&
	       (first_stmt->var == loop_var))
	{
	  match = true;
	  src = first_stmt->var;
	  dest = second_stmt->var;
	}
    }

  if (match)
    {
      stmt->type = ADD_CLEAR_STMT;
      stmt->var = src;
      stmt->dest = dest;
    }
  else
    optimize_stmt_list (stmt->stmt_list);
}


void optimize_stmt_list (stmt_t *list)
{
  while (list)
    {
      if (list->type == WHILE_STMT)
	optimize_while_stmt (list);
      list = list->next;
    }
}


stmt_t *main_prog = NULL;


int main (int argc, char *argv [])
{
  bool opt_flag = false;
  progname = argv [0];

  while (--argc)
    {
      argv++;
      if (argv [0][0] == '-')
	{
	  if (strcmp (argv [0], "-u") == 0)
	    init_to_zero = false;
	  else if (strcmp (argv [0], "-O") == 0)
	    opt_flag = true;
	  else if (strcmp (argv [0], "-v") == 0)
	    verbose = true;
	  else
	    fatal (1, "unrecognized option '%s'", argv [0]);
	}
      else if (strchr (argv [0], '='))
	define_var (argv [0]);
      else if (main_prog)
	fatal (1, "only one program may be specified");
      else if (! parse_prog (argv [0]))
	fatal (2, "parse failed");
      else
	{
	  ;  // everything OK!
	}
    }

  if (! main_prog)
    fatal (1, "no program found");

  if (opt_flag)
    optimize_stmt_list (main_prog);

  if (verbose){
  printf ("initial values of variables:\n");
  print_vars (false);
  }
  printf ("\nbegin main_prog:\n\n");

  execute_stmt_list (main_prog);

  printf ("\nend of main_prog:\n\n");
  if (verbose){
  printf ("final values of variables:\n");
  print_vars (true);
  }

  exit (0);
}
