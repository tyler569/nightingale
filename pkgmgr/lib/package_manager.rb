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

  private

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
    
    # Download and extract if needed
    unless package.downloaded?
      if package.source
        puts "Downloading #{package.name}..."
        download_dir = File.join(package.package_dir, 'downloads')
        archive_path = Downloader.download(package.source, download_dir)
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
end