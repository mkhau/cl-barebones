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

# Just a little program to test the defmac and run statements.
# "ajouter" is french for "add", and will put arg0 + arg1 into res.
# note that this is not optimized, and the initial values of arg0 and arg1 will be forgotten.
# "ajoute5" will just add 5 to arg0 and put it into res.

defproc ajouter;
  while arg1 not 0 do;
    incr arg0;
    decr arg1;
  end;
  copy arg0 to res;
endproc;

defproc ajoute5;
  clear arg1;
  incr arg1;
  incr arg1;
  incr arg1;
  incr arg1;
  incr arg1;
  run ajouter;
endproc;

print N;
print M;
copy N to arg0;
run ajoute5;
print res;

defproc gauche;
  while arg0 not 0 do;
    decr arg0;
    run droite;
  end;
endproc;

defproc droite;
  while arg0 not 0 do;
    decr arg0;
    run gauche;
  end;
endproc;

run gauche;

