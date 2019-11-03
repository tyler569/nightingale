
require 'pathname'

def get_guard(filename, content)
  content.each_line.take(5).each do |line|
    match = line.match /#ifndef (.*)/
    return match[1] if match
  end
  nil
end

class String
  def to_guard
    self.gsub("/", "_").upcase
  end
end

def correct_guard(filename)
  case filename
  when /ng\/(.*)\.h/
    "NG_#{$1.to_guard}_H"
  when /nc\/(.*)\.h/
    "_#{$1.to_guard}_H_"
  when /nx\/(.*).hh/
    "_NX_#{$1.to_guard}_HH_"
  when /kernel\/(.*)\.h/
    "NG_K_#{$1.to_guard}_H"
  else
    raise "Unknown file type: #{filename}"
  end
end

def strip_guard(f)
  new_content = []

  found_guard_ifndef = false
  found_guard_define = false
  line_number = 0
  f.each_line.each do |line|
    line_number += 1
    if line.match /pragma once/
      next
    end
    if !found_guard_ifndef and line_number < 5 and line.match /ifndef/
      found_guard_ifndef = true
      next
    end
    if found_guard_ifndef and !found_guard_define and line_number < 5 and line.match /define/
      found_guard_define = true
      next
    end

    new_content << line
  end

  if found_guard_ifndef
    new_content = new_content.reverse.drop_while{|l|l.match /^\s*$/}
    new_content.shift if new_content[0].match /#endif/
    new_content = new_content.drop_while{|l|l.match /^\s*$/}
    new_content = new_content.reverse
  end

  new_content.drop_while{|l|l.match /^\s*$/}.join("")
end

def add_guard(content, guard)
<<EOF

#pragma once
#ifndef #{guard}
#define #{guard}

#{content}
#endif // #{guard}

EOF
  end

headers = Dir.glob('**/*.h') + Dir.glob('**/*.hh')

headers.each do |filename|
  if filename.match /lua|libm/
    next
  end

  # f = File.open(filename)
  # now = get_guard(filename, f)
  # good = correct_guard(filename)

  # puts "#{filename}: #{now or "<>"} -> #{good}"
  # f.close

  f = File.open(filename)
  stripped = strip_guard(f)
  fixed = add_guard(stripped, correct_guard(filename))
  f.close

  puts "fixing #{filename}"

  f = File.open(filename, "w")
  f.puts(fixed)
  f.close
end
