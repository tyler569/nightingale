require 'yaml'
require 'digest'
require 'fileutils'

class Package
  attr_reader :name, :version, :source, :patches, :build, :dependencies, :manifest_path

  def initialize(manifest_path)
    @manifest_path = manifest_path
    @manifest = YAML.load_file(manifest_path)

    @name = @manifest['name']
    @version = @manifest['version']
    @source = @manifest['source']
    @patches = @manifest['patches'] || []
    @build = @manifest['build'] || {}
    @dependencies = @manifest['dependencies'] || []

    validate!
  end

  def package_dir
    @package_dir ||= File.join(ENV['BUILD_DIR'] || 'build', 'packages', name)
  end

  def source_dir
    @source_dir ||= File.join(package_dir, 'src')
  end

  def build_dir
    @build_dir ||= File.join(package_dir, 'build')
  end

  def sysroot
    @sysroot ||= ENV['SYSROOT'] || File.expand_path('sysroot')
  end

  def patches_dir
    @patches_dir ||= File.join(File.dirname(manifest_path), 'patches')
  end

  def build_working_dir
    # For external packages, run build commands in the source directory
    # For local packages, run from project root (existing behavior)
    source ? source_dir : Dir.pwd
  end

  def downloaded?
    source.nil? || File.exist?(source_dir)
  end

  def configured?
    return true unless build['configure']
    File.exist?(File.join(build_dir, '.configured'))
  end

  def built?
    return true unless build['build']
    File.exist?(File.join(build_dir, '.built'))
  end

  def installed?
    File.exist?(File.join(build_dir, '.installed'))
  end

  private

  def validate!
    raise "Package name is required" unless name
    raise "Package version is required" unless version
  end
end