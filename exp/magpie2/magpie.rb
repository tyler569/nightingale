#!/usr/bin/env ruby

require 'pathname'
require_relative "./default_compilers"

class Target
  attr_accessor :build, :name, :buildmode

  def initialize(build, name, buildmode, inputs, link: [], install: nil)
    @build = build
    @name = name
    @buildmode = buildmode
    @link = link
    if install
      @install = Pathname.new(install)
      if @install.directory?
        @install /= @name
      end
    else
      @install = nil
    end
    @inputs = inputs.flat_map { |i| Pathname.glob(i) }
  end

  def object(input)
    build.build_dir / name / input.sub_ext(".o")
  end

  def dep(input)
    build.dep_dir / name / input.sub_ext(".o")
  end

  def objects
    @inputs.map { |i| object(i) }
  end

  def target
    build.build_dir / name
  end

  def render_input(input)
    output = object input
    dep = dep input

    build_line = case input.extname
                 when ".c"
                   buildmode[:cc].call(input: input, output: output)
                 when ".cc", ".cpp", ".cxx"
                   buildmode[:cxx].call(input: input, output: output)
                 when ".asm", ".S"
                   buildmode[:as].call(input: input, output: output)
                 end

    <<~EOF
    #{output}: #{input}
    \tmkdir -p #{output.dirname}
    \tmkdir -p #{dep.dirname}
    \t#{build_line}
    EOF
  end

  def render
    topline = <<~EOF
    #{target}: #{objects.map(&:to_s).join " "}
    \t#{buildmode[:ld].call(input: objects, output: target, libs: @link)}
    EOF

    install = <<~EOF if @install
    #{install}: #{target}
    \tcp #{target} #{install}
    EOF
    install ||= ""

    @inputs.map { |i| render_input i }.join + topline + install
  end
end

class Build
  attr_accessor :name, :build_dir, :dep_dir

  def initialize(name)
    @name = name
    @targets = []
    @build_dir = Pathname.new "build"
    @dep_dir = Pathname.new "dep"
  end

  def define_target(name, mode, inputs, link: [], install: nil)
    target = Target.new(self, name, mode, inputs, link: link, install: install)
    @targets << target
    target
  end

  def render
    @targets.map(&:render).join "\n"
  end
end


# build = Build.new "nightingale"
# 
# mode_kernel = build.define_mode "kernel" do |m|
#   m.cc = "x86_64-nightingale-gcc"
#   m.link = :static
# end
# 
# build.define_target "hello" do |t|
#   t.mode = Mode.define do |m|
#     m.cc = "gcc"
#     m.link = false
#   end
# end
# 
# build.define_target "world" do |t|
#   t.mode = mode_kernel
# end
