require 'fileutils'

class Builder
  def self.build_package(package)
    prepare_build_directory(package)
    configure_package(package) if package.build['configure']
    build_package_impl(package) if package.build['build']
    install_package(package) if package.build['install']
  end

  private

  def self.prepare_build_directory(package)
    FileUtils.mkdir_p(package.build_dir)
    
    # Copy source to build directory if not already there
    if package.source_dir != package.build_dir
      FileUtils.cp_r(Dir.glob("#{package.source_dir}/*"), package.build_dir)
    end
  end

  def self.configure_package(package)
    return if package.configured?
    
    puts "Configuring #{package.name}..."
    
    configure_cmd = expand_variables(package.build['configure'], package)
    
    success = system(configure_cmd)
    
    unless success
      raise "Configuration failed for #{package.name}"
    end
    
    # Mark as configured
    FileUtils.touch(File.join(package.build_dir, '.configured'))
    puts "Configuration completed"
  end

  def self.build_package_impl(package)
    return if package.built?
    
    puts "Building #{package.name}..."
    
    build_cmd = expand_variables(package.build['build'], package)
    
    success = system(build_cmd)
    
    unless success
      raise "Build failed for #{package.name}"
    end
    
    # Mark as built
    FileUtils.touch(File.join(package.build_dir, '.built'))
    puts "Build completed"
  end

  def self.install_package(package)
    return if package.installed?
    
    puts "Installing #{package.name}..."
    
    # Ensure sysroot exists
    FileUtils.mkdir_p(package.sysroot)
    
    install_cmd = expand_variables(package.build['install'], package)
    
    success = system(install_cmd)
    
    unless success
      raise "Installation failed for #{package.name}"
    end
    
    # Mark as installed
    FileUtils.touch(File.join(package.build_dir, '.installed'))
    puts "Installation completed"
  end

  def self.expand_variables(command, package)
    expanded = command.gsub('$SYSROOT', package.sysroot)
                     .gsub('$(nproc)', `nproc`.strip)
    
    # Add Ninja generator to CMake configure commands if CMAKE_GENERATOR is set
    if ENV['CMAKE_GENERATOR'] && command.include?('cmake -B')
      expanded = expanded.gsub(/cmake -B/, "cmake -G #{ENV['CMAKE_GENERATOR']} -B")
    end
    
    expanded
  end
  
end