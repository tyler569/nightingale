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
    ENV['DOWNLOADS_DIR'] = File.expand_path(@config['build']['downloads_dir'] || 'downloads')
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
    @limine_dir = File.join(ENV['DOWNLOADS_DIR'] || 'downloads', 'limine')
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

class CommandHandler
  def initialize
    # Load configuration and set environment first
    @config = YAML.load_file('nightingale.yml')

    # Set environment from config - this must happen before package manager
    ENV['SYSROOT'] = File.expand_path(@config['build']['sysroot'] || 'sysroot')
    ENV['BUILD_DIR'] = @config['build']['build_dir'] || 'build'
    ENV['DOWNLOADS_DIR'] = File.expand_path(@config['build']['downloads_dir'] || 'downloads')
    ENV['CMAKE_BUILD_TYPE'] = @config['environment']['CMAKE_BUILD_TYPE'] || 'Release'
    ENV['CMAKE_SYSTEM_PROCESSOR'] = @config['environment']['CMAKE_SYSTEM_PROCESSOR'] || 'X86_64'
    ENV['CMAKE_GENERATOR'] = @config['build']['cmake_generator'] || 'Ninja'

    @builder = NightingaleBuild.new
    @package_manager = @builder.instance_variable_get(:@package_manager)
  end

  def handle_command(args)
    command = args[0]
    target = args[1] || 'all'

    case command
    when 'build'
      handle_build_command(target)
    when 'clean'
      handle_clean_command(target)
    when 'install'
      handle_install_command(target)
    when 'status'
      handle_status_command(target)
    when 'info'
      handle_info_command(target)
    when 'download'
      handle_download_command(target)
    when 'patch'
      handle_patch_command(target)
    when 'test'
      handle_test_command(target)
    when 'iso'
      @builder.create_iso
    when 'help'
      show_help(target == 'all' ? nil : target)
    when 'list'
      show_package_list
    when 'core'
      @builder.build_core
    when 'userspace'
      @builder.build_userspace
    when 'optional'
      @builder.build_optional
    when 'all'
      @builder.build_all
    when nil
      # Default behavior: full build cycle (build all + create ISO)
      @builder.build_all
      @builder.create_iso
    else
      puts "Unknown command: #{command}"
      puts "Run '#{$0} help' for usage information."
      exit 1
    end
  end

  private

  def handle_build_command(target)
    case target
    when 'all'
      @builder.build_all
    when 'core'
      @builder.build_core
    when 'userspace'
      @builder.build_userspace
    when 'optional'
      @builder.build_optional
    else
      begin
        @package_manager.build_package(target)
      rescue => e
        puts "Error building #{target}: #{e.message}"
        exit 1
      end
    end
  end

  def handle_clean_command(target)
    case target
    when 'all'
      @builder.clean
    else
      begin
        @package_manager.clean_package(target)
      rescue => e
        puts "Error cleaning #{target}: #{e.message}"
        exit 1
      end
    end
  end

  def handle_install_command(target)
    case target
    when 'all'
      @package_manager.install_all_packages
    else
      begin
        @package_manager.install_package(target)
      rescue => e
        puts "Error installing #{target}: #{e.message}"
        exit 1
      end
    end
  end

  def handle_status_command(target)
    case target
    when 'all'
      @package_manager.status_all_packages
    else
      begin
        @package_manager.status_package(target)
      rescue => e
        puts "Error getting status for #{target}: #{e.message}"
        exit 1
      end
    end
  end

  def handle_info_command(target)
    if target == 'all'
      puts "Use 'info <package>' for specific package information"
      show_package_list
    else
      begin
        @package_manager.info_package(target)
      rescue => e
        puts "Error getting info for #{target}: #{e.message}"
        exit 1
      end
    end
  end

  def handle_download_command(target)
    case target
    when 'all'
      @package_manager.download_all_packages
    else
      begin
        @package_manager.download_package(target)
      rescue => e
        puts "Error downloading #{target}: #{e.message}"
        exit 1
      end
    end
  end

  def handle_patch_command(target)
    case target
    when 'all'
      @package_manager.patch_all_packages
    else
      begin
        @package_manager.patch_package(target)
      rescue => e
        puts "Error patching #{target}: #{e.message}"
        exit 1
      end
    end
  end

  def handle_test_command(target)
    case target
    when 'all'
      @package_manager.test_all_packages
    else
      begin
        @package_manager.test_package(target)
      rescue => e
        puts "Error testing #{target}: #{e.message}"
        exit 1
      end
    end
  end

  def show_package_list
    packages = @package_manager.list_packages
    puts "\nAvailable packages:"
    puts "=" * 40
    packages.each { |name| puts "  #{name}" }
    puts "\nAvailable groups:"
    puts "  core, userspace, optional, all"
  end

  def show_help(command = nil)
    if command
      show_command_help(command)
    else
      show_general_help
    end
  end

  def show_command_help(command)
    case command
    when 'build'
      puts <<~HELP
        Build packages:
          #{$0} build [target]

        Examples:
          #{$0} build all          # Build all packages (default)
          #{$0} build core         # Build core packages
          #{$0} build userspace    # Build userspace packages
          #{$0} build optional     # Build optional packages
          #{$0} build lua          # Build specific package
      HELP
    when 'clean'
      puts <<~HELP
        Clean build artifacts:
          #{$0} clean [target]

        Examples:
          #{$0} clean all          # Clean everything (build/, sysroot/, iso)
          #{$0} clean lua          # Clean specific package build directory
      HELP
    when 'install'
      puts <<~HELP
        Install packages:
          #{$0} install [target]

        Examples:
          #{$0} install all        # Install all packages
          #{$0} install lua        # Install specific package
      HELP
    when 'status'
      puts <<~HELP
        Show build status:
          #{$0} status [target]

        Examples:
          #{$0} status all         # Show status of all packages
          #{$0} status lua         # Show status of specific package
      HELP
    when 'info'
      puts <<~HELP
        Show package information:
          #{$0} info <package>

        Examples:
          #{$0} info lua           # Show detailed info for lua package
      HELP
    else
      puts "No help available for command: #{command}"
    end
  end

  def show_general_help
    puts <<~HELP
      Nightingale OS Build System

      Usage: #{$0} <command> [target]

      Commands:
        build [target]     Build packages (default: all)
        clean [target]     Clean build artifacts (default: all)
        install [target]   Install packages (default: all)
        status [target]    Show build status (default: all)
        info <package>     Show package information
        download [target]  Download package sources (default: all)
        patch [target]     Apply patches (default: all)
        test [target]      Run package tests (default: all)
        iso                Create bootable ISO
        list               List available packages
        help [command]     Show help information

      Targets:
        all                All packages (default)
        core               Core system packages
        userspace          Userspace packages
        optional           Optional packages
        <package>          Specific package name

      Examples:
        #{$0}                          # Full build cycle (build all + create ISO)
        #{$0} build                    # Build all packages only
        #{$0} build lua                # Build lua package
        #{$0} clean lua                # Clean lua build directory
        #{$0} status                   # Show status of all packages
        #{$0} info lua                 # Show lua package info
        #{$0} help build               # Show help for build command
    HELP
  end
end

if __FILE__ == $0
  handler = CommandHandler.new
  handler.handle_command(ARGV)
end