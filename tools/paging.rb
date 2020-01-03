
def field(i, off, len)
  return (i >> off) & ((1<<len)-1)
end

class Integer
  def lower_half
    self >= 0 && self < 0x800000000000
  end
  def higher_half
    self >= 0xFFFF800000000000 && self <= 0xFFFFFFFFFFFFFFFF
  end
  def canonical
    higher_half || lower_half
  end
  def page_offset
    field(self, 0, 12)
  end
  def p1
    field(self, 12, 9)
  end
  def p2
    field(self, 21, 9)
  end
  def p3
    field(self, 30, 9)
  end
  def p4
    field(self, 39, 9)
  end
end

p = ARGV[0].to_i(16)

puts "** non-canonical **" unless p.canonical

puts "offset: " + p.page_offset.to_s(16)
puts "p1    : " + p.p1.to_s(16)
puts "p2    : " + p.p2.to_s(16)
puts "p3    : " + p.p3.to_s(16)
puts "p4    : " + p.p4.to_s(16)

