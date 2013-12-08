# Test : macros

print N;

defmac ajouter;
  while arg1 not 0 do;
    incr arg0;
    decr arg1;
  end;
  copy arg0 to res;
endmac;

print N;
copy N to arg0;
copy N to arg1;
run ajouter;
run bidon;
print res;

