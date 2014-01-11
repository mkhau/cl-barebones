# Test : procedures
## Created by Michael KHAU

# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3 as
# published by the Free Software Foundation.  Note that I am not
# granting permission to redistribute or modify this program under the
# terms of any other version of the General Public License.

# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program (in the file "COPYING"); if not, see
# <http://www.gnu.org/licenses/>.
######################################

# Just a little program to test the defproc and run statements.
# "add" will put a + b into res.
# note that this is not optimized, and the initial values of arg0 and arg1 will be forgotten.
# "add5" will just add 5 to a and put it into res.

defproc add ( a, b );
  print a;
  print b;
  while a not 0 do;
    incr b;
    decr a;
  end;
  copy b to res;
endproc;

incr c;
incr c;

run add ( 12 , c );
print res;

defproc add5 (e);
  print e;
  run add ( 5 , e );
endproc;

run add5 ( 70 );
print res;

run add5 (32);
print res;

incr c;
print c;

run add ( 12 , c );
print res;

defproc add10 (d);
  print d;
  run add5(d);
  run add5(res);
endproc;

run add10(777);
print res;


defproc gauche (g);
  print g;
  while g not 0 do;
    decr g;
    run droite(g);
    exit;
  end;
endproc;

defproc droite (d);
  print d;
  while d not 0 do;
    decr d;
    run gauche(d);
    exit;
  end;
endproc;

run gauche(5);
