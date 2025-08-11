#!/usr/bin/env ruby
# Nightingale OS Unified Build Script
# Builds the complete operating system and optionally creates bootable ISO

require 'yaml'
require 'fileutils'
require_relative 'pkgmgr/lib/package_manager'

class NightingaleBuild
  def initialize
    @config = YAML.load_file('nightingale.yml')
    @package_manager = PackageManager.new('packages')
    
    # Set environment from config
    ENV['SYSROOT'] = File.expand_path(@config['build']['sysroot'] || 'sysroot')
    ENV['BUILD_DIR'] = @config['build']['build_dir'] || 'build'
    ENV['CMAKE_BUILD_TYPE'] = @config['environment']['CMAKE_BUILD_TYPE'] || 'Release'
    ENV['CMAKE_SYSTEM_PROCESSOR'] = @config['environment']['CMAKE_SYSTEM_PROCESSOR'] || 'X86_64'
    ENV['CMAKE_GENERATOR'] = @config['build']['cmake_generator'] || 'Ninja'
    
    # ISO configuration
    @source_dir = '.'
    @build_dir = ENV['BUILD_DIR']
    @sysroot_dir = ENV['SYSROOT']
    @iso_dir = 'isoroot'
    @iso_boot_dir = File.join(@iso_dir, 'boot')
    @iso_file = File.join(@source_dir, 'ngos.iso')
    
    @limine_cfg_file = File.join(@source_dir, 'kernel', 'limine.cfg')
    @kernel_binary = File.join(@build_dir, 'kernel', 'nightingale_kernel')
    @limine_dir = File.join(@build_dir, 'limine')
    @initrd_file = File.join(@iso_boot_dir, 'initrd.tar')
  end
  
  def build_core
    puts "Building Nightingale OS core components..."
    @config['core_packages'].each do |package|
      puts "Building #{package}..."
      @package_manager.build_package(package)
    end
  end
  
  def build_userspace
    puts "Building userspace packages..."
    @config['userspace_packages'].each do |package|
      puts "Building #{package}..."
      @package_manager.build_package(package)
    end
  end
  
  def build_optional
    puts "Building optional packages..."
    @config['optional_packages'].each do |package|
      puts "Building #{package}..."
      @package_manager.build_package(package)
    end
  end
  
  def build_all
    build_core
    build_userspace
    build_optional
    puts "Nightingale OS build complete! Sysroot: #{ENV['SYSROOT']}"
  end
  
  def create_iso
    puts "Creating bootable ISO..."
    
    # Ensure we have a built system
    unless File.exist?(@kernel_binary)
      puts "Kernel not found. Building OS first..."
      build_all
    end
    
    unless File.exist?(@sysroot_dir)
      puts "Sysroot not found. Building OS first..."
      build_all
    end
    
    # Setup ISO root
    puts "Setting up ISO root: #{@iso_boot_dir}"
    FileUtils.mkdir_p(@iso_boot_dir)
    
    # Create initrd
    puts "Creating initrd tar from #{@sysroot_dir} → #{@initrd_file}"
    system('tar', '-cf', @initrd_file, '-C', @sysroot_dir, '.') or abort('Failed to create initrd.tar')
    
    # Copy files into ISO root
    puts "Copying limine.cfg to ISO root"
    FileUtils.cp(@limine_cfg_file, @iso_boot_dir)
    
    puts "Copying kernel binary to ISO root"
    FileUtils.cp(@kernel_binary, @iso_boot_dir)
    
    # Fetch & build Limine
    setup_limine
    
    # Create ISO
    puts "Generating ISO: #{@iso_file}"
    system(
      'xorriso',
      '-as', 'mkisofs',
      '-b', 'boot/limine/limine-bios-cd.bin',
      '-no-emul-boot',
      '-boot-load-size', '4',
      '--boot-info-table',
      '--efi-boot', 'boot/limine/limine-uefi-cd.bin',
      '-efi-boot-part',
      '--efi-boot-image',
      '--protective-msdos-label',
      @iso_dir,
      '-o', @iso_file
    ) or abort('Failed to create ISO')
    
    # Install Limine
    puts "Installing Limine to ISO"
    system(File.join(@limine_dir, 'limine'), 'bios-install', @iso_file) or abort('Limine install failed')
    
    puts "Done! ISO ready at: #{@iso_file}"
  end
  
  def clean
    build_dir = ENV['BUILD_DIR'] || 'build'
    sysroot = ENV['SYSROOT'] || 'sysroot'
    
    puts "Cleaning build artifacts..."
    FileUtils.rm_rf(build_dir) if File.exist?(build_dir)
    FileUtils.rm_rf(sysroot) if File.exist?(sysroot)
    FileUtils.rm_rf(@iso_dir) if File.exist?(@iso_dir)
    FileUtils.rm_f(@iso_file) if File.exist?(@iso_file)
    puts "Clean complete"
  end
  
  private
  
  def setup_limine
    unless Dir.exist?(@limine_dir)
      puts "Cloning Limine..."
      system('git', 'clone', '--depth', '1', '--branch', 'v7.x-binary',
             'https://github.com/limine-bootloader/limine.git', @limine_dir) or abort('Failed to clone Limine')
    end
    
    puts "Building Limine..."
    Dir.chdir(@limine_dir) do
      system('make') or abort('Failed to build Limine')
    end
    
    # Ensure limine subdirectory exists
    limine_boot_dir = File.join(@iso_boot_dir, 'limine')
    FileUtils.mkdir_p(limine_boot_dir)
    
    # Copy Limine files
    puts "Copying Limine bootloader files to ISO root"
    %w[limine-bios.sys limine-bios-cd.bin limine-uefi-cd.bin].each do |filename|
      source = File.join(@limine_dir, filename)
      dest = File.join(limine_boot_dir, filename)
      unless File.exist?(source)
        abort("Missing Limine file: #{source}")
      end
      FileUtils.cp(source, dest)
    end
  end
end

if __FILE__ == $0
  builder = NightingaleBuild.new
  
  case ARGV[0]
  when 'core'
    builder.build_core
  when 'userspace'
    builder.build_userspace  
  when 'optional'
    builder.build_optional
  when 'iso'
    builder.create_iso
  when 'clean'
    builder.clean
  when 'all', nil
    builder.build_all
  else
    puts "Usage: #{$0} [core|userspace|optional|all|iso|clean]"
    puts "  core      - Build core system packages"
    puts "  userspace - Build userspace packages"
    puts "  optional  - Build optional packages"
    puts "  all       - Build all packages (default)"
    puts "  iso       - Create bootable ISO (builds OS if needed)"
    puts "  clean     - Clean all build artifacts"
    exit 1
  end
end