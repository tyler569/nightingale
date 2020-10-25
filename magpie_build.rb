require 'delegate'
require 'pathname'

class MagpieBuild
  attr_accessor :targets, :modes

  def initialize
    @targets = {}
    @modes = {}
    @build_dir = Pathname.new("obj")
    @dep_dir = Pathname.new("dep")
  end

  def build_dir(dir_name = nil)
    @build_dir = Pathname.new(dir_name) if dir_name
    @build_dir
  end

  def dep_dir(dir_name = nil)
    @dep_dir = Pathname.new(dir_name) if dir_name
    @dep_dir
  end

  def target(name, &block)
    t = Magpie::TargetBuilder.new(name, self)
    t.instance_eval(&block)
    @targets[name] = t.finalize
  end

  def mode(name, &block)
    m = Magpie::ModeBuilder.new(self)
    m.instance_eval(&block)
    @modes[name] = m.finalize
  end

  def self.define(&block)
    m = MagpieBuild.new
    m.define(&block)
  end

  def define(&block)
    instance_eval(&block)
    self
  end

  def all_targets
    @targets.map { |name, target| target.target }.join(" ")
  end

  def all_outputs
    @targets.map { |name, target| [target.target, *target.objects] }.flatten
  end

  def all_install
    @targets
      .map { |name, target| target.install_target }.compact
      .join("\n")
  end

  def all_install_targets
    @targets
      .map { |name, target| target.installed_path || target.target }
      .join(" ")
  end

  def all_only_install_targets
    @targets
      .map { |name, target| target.installed_path }.compact
      .join(" ")
  end

  def clean_block
    <<~END
      mp_clean:
      \t@echo "Clean magpie objects"
      \t@rm -f #{all_outputs.join(" ")}
    END
  end

  def render
    <<~END
      .PHONY: mp_all mp_clean mp_install

      mp_all: #{all_targets}

      #{clean_block}

      #{@targets.map { |name, target| target.render }.join("\n")}

      #{all_install}

      mp_install: #{all_install_targets}

      MP_ALL_INSTALL_TARGETS := #{all_only_install_targets}

      include $(shell find #{dep_dir} -name '*.d')
    END
  end
end

module Magpie
  class Builder < Delegator
    attr_accessor :build
    alias_method :__getobj__, :build

    def self.attr_value(*names)
      names.each do |name|
        define_method(name) do |value|
          @values[name] = value
        end
      end
    end

    def self.attr_array(*names)
      names.each do |name|
        define_method(name) do |*values|
          @values[name] = values.flatten
        end
      end
    end
  end

  class ModeBuilder < Builder
    attr_value :cc, :as, :ld
    attr_array :cflags, :ldflags, :asflags

    def initialize(build)
      @build = build
      @values = {
        cc: "gcc",
        as: "gcc",
        ld: "gcc",
        cflags: [],
        ldflags: [],
        asflags: [],
      }
    end

    def finalize
      Mode.new(@values)
    end
  end

  class Mode
    attr_reader :cc, :as, :ld

    def initialize(values)
      @cc = values[:cc]
      @as = values[:as]
      @ld = values[:ld]
      @cflags = values[:cflags]
      @ldflags = values[:ldflags]
      @asflags = values[:asflags]
    end

    def cflags
      @cflags.join(" ")
    end

    def ldflags
      @ldflags.join(" ")
    end

    def asflags
      @asflags.join(" ")
    end
  end

  class TargetBuilder < Builder
    attr_array :language, :link, :depends
    attr_value :alt_dir

    def initialize(name, build)
      @build = build
      @values = {
        name: name,
        language: "c",
        sources: [],
        link: [],
        depends: [],
      }
    end

    def mode(name)
      @values[:mode] = @build.modes[name]
    end

    def install(dir)
      @values[:install] = Pathname.new(dir)
    end

    def depends(*names)
      @values[:depends] = names.flatten.map do |name|
        @build.targets[name]
      end
    end

    def sources(*srcs)
      @values[:sources] = srcs.map do |src|
        Pathname.glob(src)
      end.flatten
    end

    def finalize
      Target.new(@values, @build)
    end
  end

  class TranslationUnit
    attr_accessor :source

    def initialize(source, target)
      @source = source
      @target = target
    end

    def obj
      s = source.sub_ext(".o")
      s = s.sub(s.descend.first.to_s, @target.alt_dir) if @target.alt_dir
      Pathname.new(@target.build.build_dir) + s
    end

    def dep
      s = source.sub_ext(".d")
      s = s.sub(s.descend.first.to_s, @target.alt_dir) if @target.alt_dir
      Pathname.new(@target.build.dep_dir) + s
    end

    def obj_dir
      obj.dirname
    end

    def dep_dir
      dep.dirname
    end

    def dep_opt
      "-MD -MF #{dep}"
    end
  end

  class Target
    attr_reader :name, :language, :mode, :libs, :deps, :tus, :build, :alt_dir, :install

    def initialize(values, build)
      @name = values[:name]
      @language = values[:language]
      @mode = values[:mode]
      @deps = values[:depends]
      @libs = values[:link]
      @tus = values[:sources].map { |s| TranslationUnit.new(s, self) }
      @alt_dir = values[:alt_dir]
      @install = values[:install]
      @build = build
    end

    def safe_name
      @name.to_s.gsub(".", "_").gsub("/", "_")
    end

    # in the build dir specifically, does not account for installation
    def target
      Pathname.new(build.build_dir) + @name
    end

    # nil if not installed, use #real_target if you just want final output
    def installed_path
      @install + @name if @install
    end

    def real_target
      installed_path || target
    end

    def sources
      @tus.map(&:source).join(" ")
    end

    def objects
      @tus.map(&:obj).join(" ")
    end

    def libraries
      @libs.map { |l| "-l#{l}" }.join(" ")
    end

    def dependancies
      @deps.map(&:real_target).join(" ")
    end

    def install_target_name
      "install_#{safe_name}" if @install
    end

    def install_target
      <<~END if @install
        #{installed_path}: #{target}
        \t@echo "install #{@name}"
        \t@cp #{target} #{installed_path}
      END
    end

    def linker
      <<~END
        #{target}: #{objects} #{dependancies}
        \t#{mode.cc} #{mode.ldflags} -o #{target} #{objects} #{libraries}
      END
    end

    def archive
      <<~END
        #{target}: #{objects} #{dependancies}
        \t#{mode.ld} rcs -o #{target} #{objects}
      END
    end

    def null_link
      raise "can't null link multiple objects" if @tus.length > 1
      <<~END
        #{target}: #{objects}
        \t@cp --preserve=timestamps #{objects} #{target}
      END
    end

    def compile_c(tu)
      <<~END
        #{tu.obj}: #{tu.source}
        \t@mkdir -p #{tu.obj_dir}
        \t@mkdir -p #{tu.dep_dir}
        \t#{mode.cc} #{mode.cflags} #{tu.dep_opt} -c #{tu.source} -o #{tu.obj}
      END
    end

    def compile_s(tu)
      <<~END
        #{tu.obj}: #{tu.source}
        \t@mkdir -p #{tu.obj_dir}
        \t@mkdir -p #{tu.dep_dir}
        \t#{mode.as} #{mode.asflags} #{tu.dep_opt} -c #{tu.source} -o #{tu.obj}
      END
    end

    def compile_asm(tu)
      <<~END
        #{tu.obj}: #{tu.source}
        \t@mkdir -p #{tu.obj_dir}
        \t#{mode.as} #{mode.asflags} -o #{tu.obj} #{tu.source}
      END
    end

    def link
      case mode.ld
      when "ar"
        archive
      when nil
        null_link
      else 
        linker
      end
    end

    def rules
      tus.map do |tu|
        case tu.source.extname
        when ".c"
          compile_c(tu)
        when ".asm"
          compile_asm(tu)
        when ".S"
          compile_s(tu)
        else
          raise "extension invalid"
        end
      end.join("\n")
    end

    def render
      link + rules
    end
  end
end
