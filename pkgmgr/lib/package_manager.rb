require 'set'
require_relative 'package'
require_relative 'downloader'
require_relative 'patcher'
require_relative 'builder'
require_relative 'dependency_resolver'

class PackageManager
  def initialize(packages_dir = 'packages')
    @packages_dir = packages_dir
  end

  def load_packages
    packages = []

    Dir.glob(File.join(@packages_dir, '**/package.yml')).each do |manifest_path|
      packages << Package.new(manifest_path)
    end

    packages
  end

  def build_package(package_name)
    packages = load_packages
    package = packages.find { |p| p.name == package_name }

    unless package
      raise "Package not found: #{package_name}"
    end

    # Resolve dependencies
    dependencies = resolve_dependencies([package], packages)

    # Build in dependency order
    dependencies.each do |pkg|
      build_single_package(pkg)
    end
  end

  def build_all
    packages = load_packages
    dependencies = resolve_dependencies(packages, packages)

    dependencies.each do |package|
      build_single_package(package)
    end
  end

  def build_group(group_name)
    case group_name
    when 'core'
      build_packages_by_names(['nightingale-headers', 'nightingale-libc', 'nightingale-kernel', 'nightingale-modules'])
    when 'userspace'
      build_packages_by_names(['nightingale-coreutils', 'nightingale-sh'])
    when 'optional'
      build_packages_by_names(['libm', 'lua'])
    else
      raise "Unknown package group: #{group_name}"
    end
  end

  def clean_package(package_name)
    packages = load_packages
    package = packages.find { |p| p.name == package_name }

    unless package
      raise "Package not found: #{package_name}"
    end

    puts "Cleaning #{package_name}..."
    FileUtils.rm_rf(package.package_dir) if File.exist?(package.package_dir)
    puts "Cleaned #{package_name}"
  end

  def clean_all_packages
    build_dir = ENV['BUILD_DIR'] || 'build'
    packages_build_dir = File.join(build_dir, 'packages')

    puts "Cleaning all packages..."
    FileUtils.rm_rf(packages_build_dir) if File.exist?(packages_build_dir)
    puts "Cleaned all packages"
  end

  def install_package(package_name)
    packages = load_packages
    package = packages.find { |p| p.name == package_name }

    unless package
      raise "Package not found: #{package_name}"
    end

    # Build with dependencies if not built
    unless package.installed?
      build_package(package_name)
      return
    end

    # Just run install step
    puts "Installing #{package_name}..."
    Builder.install_package(package)
    puts "#{package_name} installed successfully"
  end

  def install_all_packages
    packages = load_packages
    dependencies = resolve_dependencies(packages, packages)

    dependencies.each do |package|
      unless package.installed?
        build_single_package(package)
      else
        puts "#{package.name} already installed"
      end
    end
  end

  def status_package(package_name)
    packages = load_packages
    package = packages.find { |p| p.name == package_name }

    unless package
      raise "Package not found: #{package_name}"
    end

    show_package_status(package)
  end

  def status_all_packages
    packages = load_packages

    puts "\nPackage Status:"
    puts "=" * 60
    packages.each { |pkg| show_package_status(pkg) }
  end

  def info_package(package_name)
    packages = load_packages
    package = packages.find { |p| p.name == package_name }

    unless package
      raise "Package not found: #{package_name}"
    end

    show_package_info(package)
  end

  def download_package(package_name)
    packages = load_packages
    package = packages.find { |p| p.name == package_name }

    unless package
      raise "Package not found: #{package_name}"
    end

    download_single_package(package)
  end

  def download_all_packages
    packages = load_packages
    packages.each { |pkg| download_single_package(pkg) }
  end

  def patch_package(package_name)
    packages = load_packages
    package = packages.find { |p| p.name == package_name }

    unless package
      raise "Package not found: #{package_name}"
    end

    puts "Applying patches for #{package_name}..."
    Patcher.apply_patches(package) if File.exist?(package.patches_dir)
    puts "Patches applied for #{package_name}"
  end

  def patch_all_packages
    packages = load_packages
    packages.each do |package|
      if File.exist?(package.patches_dir)
        puts "Applying patches for #{package.name}..."
        Patcher.apply_patches(package)
      end
    end
  end

  def test_package(package_name)
    puts "Testing #{package_name}... (not implemented yet)"
  end

  def test_all_packages
    puts "Testing all packages... (not implemented yet)"
  end

  def list_packages
    load_packages.map(&:name).sort
  end

  private

  def build_packages_by_names(package_names)
    packages = load_packages
    target_packages = package_names.map do |name|
      pkg = packages.find { |p| p.name == name }
      raise "Package not found: #{name}" unless pkg
      pkg
    end

    dependencies = resolve_dependencies(target_packages, packages)
    dependencies.each { |pkg| build_single_package(pkg) }
  end

  def show_package_status(package)
    status_parts = []
    status_parts << (package.downloaded? ? "✓ Downloaded" : "✗ Not Downloaded")
    status_parts << (package.configured? ? "✓ Configured" : "✗ Not Configured") if package.build['configure']
    status_parts << (package.built? ? "✓ Built" : "✗ Not Built") if package.build['build']
    status_parts << (package.installed? ? "✓ Installed" : "✗ Not Installed")

    printf "%-20s %s\n", package.name, status_parts.join(", ")
  end

  def show_package_info(package)
    puts "\nPackage: #{package.name}"
    puts "Version: #{package.version}"
    puts "Dependencies: #{package.dependencies.empty? ? 'None' : package.dependencies.join(', ')}"
    if package.source
      puts "Source: #{package.source['url']}"
      puts "Type: #{package.source['type']}"
    else
      puts "Source: Local package"
    end
    puts "Build Dir: #{package.build_dir}"
    puts "Patches: #{package.patches.empty? ? 'None' : package.patches.join(', ')}"
    puts "Status:"
    show_package_status(package)
  end

  def download_single_package(package)
    return if package.downloaded?

    if package.source
      puts "Downloading #{package.name}..."
      downloads_dir = ENV['DOWNLOADS_DIR'] || 'downloads'
      package_downloads_dir = File.join(downloads_dir, 'packages')
      archive_path = Downloader.download(package.source['url'], package_downloads_dir)
      Downloader.extract(archive_path, package.source_dir)
      puts "Downloaded #{package.name}"
    else
      # Local package - copy from source directory if exists
      local_src = File.join(File.dirname(package.manifest_path), 'src')
      if File.exist?(local_src)
        FileUtils.cp_r(local_src, package.source_dir)
        puts "Copied local source for #{package.name}"
      end
    end
  end

  def resolve_dependencies(target_packages, all_packages)
    DependencyResolver.resolve(target_packages.map { |pkg|
      resolve_package_dependencies(pkg, all_packages)
    }.flatten.uniq { |p| p.name })
  end

  def resolve_package_dependencies(package, all_packages, resolved = [])
    return resolved if resolved.any? { |p| p.name == package.name }

    # Add dependencies first
    package.dependencies.each do |dep_name|
      dep_package = all_packages.find { |p| p.name == dep_name }

      unless dep_package
        raise "Dependency not found: #{dep_name} (required by #{package.name})"
      end

      resolve_package_dependencies(dep_package, all_packages, resolved)
    end

    resolved << package
    resolved
  end

  def build_single_package(package)
    puts "Processing #{package.name} v#{package.version}..."

    # Verify dependencies are properly installed first
    verify_dependencies_installed(package)

    # Download and extract if needed
    unless package.downloaded?
      if package.source
        puts "Downloading #{package.name}..."
        downloads_dir = ENV['DOWNLOADS_DIR'] || 'downloads'
        package_downloads_dir = File.join(downloads_dir, 'packages')
        archive_path = Downloader.download(package.source['url'], package_downloads_dir)
        Downloader.extract(archive_path, package.source_dir)
      else
        # Local package - copy from source directory if exists
        local_src = File.join(File.dirname(package.manifest_path), 'src')
        if File.exist?(local_src)
          FileUtils.cp_r(local_src, package.source_dir)
        end
      end
    end

    # Apply patches
    Patcher.apply_patches(package) if File.exist?(package.patches_dir)

    # Build the package
    Builder.build_package(package)

    puts "#{package.name} completed successfully"
  end

  def verify_dependencies_installed(package)
    return if package.dependencies.empty?

    packages = load_packages
    sysroot = ENV['SYSROOT'] || 'sysroot'

    package.dependencies.each do |dep_name|
      dep_package = packages.find { |p| p.name == dep_name }
      next unless dep_package

      # Check if dependency claims to be installed
      unless dep_package.installed?
        puts "Warning: Dependency #{dep_name} not marked as installed, rebuilding..."
        build_package(dep_name)
        next
      end

      # Additional verification for critical dependencies
      case dep_name
      when 'nightingale-headers'
        headers_dir = File.join(sysroot, 'usr', 'include')
        unless File.exist?(headers_dir) && !Dir.empty?(headers_dir)
          puts "Warning: nightingale-headers claims to be installed but headers missing, reinstalling..."
          # Remove install marker and rebuild
          install_marker = File.join(dep_package.build_dir, '.installed')
          FileUtils.rm_f(install_marker) if File.exist?(install_marker)
          build_package(dep_name)
        end
      when 'nightingale-libc'
        libc_dir = File.join(sysroot, 'usr', 'lib')
        if dep_package.build['install']&.include?('lib') && !File.exist?(libc_dir)
          puts "Warning: nightingale-libc claims to be installed but libraries missing, reinstalling..."
          install_marker = File.join(dep_package.build_dir, '.installed')
          FileUtils.rm_f(install_marker) if File.exist?(install_marker)
          build_package(dep_name)
        end
      end
    end
  end
end