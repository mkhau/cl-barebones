// Bare Bones interpreter
// $Id: barebones.h 9 2008-02-13 00:58:47Z eric $
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


typedef struct var_t var_t;

typedef struct env_t env_t;

typedef enum 
{
  CLEAR_STMT,
  INCR_STMT,
  DECR_STMT,
  WHILE_STMT,
  COPY_STMT,
  PRINT_STMT,
  DEFPROC_STMT,
  RUNPROC_STMT,
  EXIT_STMT,
  LAMBDA_STMT,
  ADD_CLEAR_STMT  // optimizer may transform a while statement into this
} stmt_type_t;

typedef struct stmt_t
{
  struct stmt_t *next;	// used to chain a statement list
  struct stmt_t *tail;  // only used for head of a statement list
  int line;
  stmt_type_t type;
  var_t *var;
  var_t *dest;  // only used for COPY_STMT
  struct stmt_t *stmt_list;  // only used for WHILE_STMT
  char *name;	// only used for procedure-related stmt.
  struct stmt_t *proc; // only used for proc-related stmt.
} stmt_t;

void error (const char *fmt, ...);

var_t *find_var (char *name);
stmt_t *find_proc (char *name);
void set_var (var_t *var, uintmax_t value);

stmt_t *new_stmt (stmt_type_t type, var_t *var);

void add_stmt_to_list (stmt_t *head, stmt_t *stmt);
void add_val_to_list (var_t *head, var_t *var);
var_t *new_int_var (int num);
var_t *new_arg_var (char* s);
var_t *copy_find_var (char *name);
void merge_env (env_t *base, env_t *add);

int execute_stmt (stmt_t *stmt);
int execute_stmt_list (stmt_t *list);

extern stmt_t *main_prog;
