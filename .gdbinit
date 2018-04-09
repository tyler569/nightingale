
target remote localhost:1234
symbol-file ./ngk
set architecture i386:x86-64
break start_higher_half
continue

set logging on

define traverse
  while(1)
      bt
      step
  end
end

