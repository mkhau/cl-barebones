// Parser for Bare Bones programming language
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

%{
#include <stdbool.h>
#include <stdint.h>

#include "barebones.h"

%}

%union {
  int integer;
  char *string;
  var_t *var;
  stmt_t *stmt;
}

%token CLEAR COPY DECR DO END INCR INIT NOT TO WHILE 
%token PRINT DEFPROC ENDPROC RUN EXIT LAMBDA ENDLAM

%token <string> IDENT
%token <integer> INTEGER

%type <var> var val 
%type <var> val_list arg_list
%type <stmt> stmt clear_stmt incr_stmt decr_stmt while_stmt copy_stmt print_stmt defproc_stmt runproc_stmt exit_stmt
%type <stmt> stmt_list lambda_stmt
%%

program:	stmt_list { main_prog = $1; }
		| init_list stmt_list { main_prog = $2; };

init_list:	init
		| init_list init;

init:		INIT var '=' INTEGER ';'
		{
		  set_var ($2, $4);
		};

stmt_list:	stmt { $$ = $1; }
		| stmt_list stmt
			{
			  add_stmt_to_list ($1, $2);
			  $$ = $1;
			};


stmt:		clear_stmt 
		| incr_stmt 
		| decr_stmt 
		| while_stmt 
		| copy_stmt 
		| print_stmt
		| defproc_stmt
		| runproc_stmt
		| lambda_stmt
		| exit_stmt;

var:		IDENT
		{
		  $$ = find_var ($1);
		};

val:		INTEGER
		{
		  $$ = new_int_var($1); 
		}
		| IDENT 
		{
		  $$ = copy_find_var ($1); 
		};

val_list:	val
		{ 
		  $$ = $1;
		}
		| val_list ',' val
			{
			  add_val_to_list ($3, $1);
			  $$ = $3;
			};
			
arg_list:	IDENT
		{
		  $$ = new_arg_var($1);
		}
		| arg_list ',' IDENT
		{
		  $$ = new_arg_var($3);
		  add_val_to_list($$, $1);
		};

clear_stmt:	CLEAR var ';'
		{
		  $$ = new_stmt (CLEAR_STMT, $2);
		};

incr_stmt:	INCR var ';'
		{
		  $$ = new_stmt (INCR_STMT, $2);
		};

decr_stmt:	DECR var ';'
		{
		  $$ = new_stmt (DECR_STMT, $2);
		};

while_stmt:	WHILE var NOT INTEGER DO ';' stmt_list END ';'
		{
		  if ($4 != 0)
		    error ("literal in while statement must be zero");
		  $$ = new_stmt (WHILE_STMT, $2);
		  $$->stmt_list = $7;
		};

copy_stmt:	COPY var TO var ';'
		{
		  $$ = new_stmt (COPY_STMT, $2);
		  $$->dest = $4;
		};

print_stmt:	PRINT var ';'
		{
		  $$ = new_stmt (PRINT_STMT, $2);
		};

defproc_stmt:	DEFPROC IDENT ';' stmt_list ENDPROC ';'
		{
		  $$ = new_stmt (DEFPROC_STMT, NULL);
		  $$->name = $2;
		  $$->stmt_list = $4;
		}
		| DEFPROC IDENT '(' arg_list ')' ';' stmt_list ENDPROC ';'
		{
		  $$ = new_stmt (DEFPROC_STMT, $4);
		  $$->name = $2;
		  $$->stmt_list = $7;
		};

runproc_stmt:	RUN IDENT ';'
		{
		  $$ = new_stmt (RUNPROC_STMT, NULL);
		  $$->name = $2;
		}
		| RUN IDENT '(' val_list ')' ';'
		{
		  $$ = new_stmt (RUNPROC_STMT, $4);
		  $$->name = $2;
		};

exit_stmt:	EXIT ';'
		{ 
		  $$ = new_stmt (EXIT_STMT, NULL);
		};

lambda_stmt:	LAMBDA var '(' arg_list ')' ';' stmt_list ENDLAM ';'
		{
			$$ = new_stmt (LAMBDA_STMT, $2);
			$$->dest = $4;
			$$->stmt_list = $7;
		};
