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
    if package.source_dir != package.build_dir && !File.exist?(File.join(package.build_dir, '.source_copied'))
      FileUtils.cp_r(Dir.glob("#{package.source_dir}/*"), package.build_dir)
      FileUtils.touch(File.join(package.build_dir, '.source_copied'))
    end
  end

  def self.configure_package(package)
    return if package.configured?

    puts "Configuring #{package.name}..."

    configure_cmd = expand_variables(package.build['configure'], package)

    success = Dir.chdir(package.build_working_dir) do
      system(configure_cmd)
    end

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

    success = Dir.chdir(package.build_working_dir) do
      system(build_cmd)
    end

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

    success = Dir.chdir(package.build_working_dir) do
      system(install_cmd)
    end

    unless success
      raise "Installation failed for #{package.name}"
    end

    # Verify installation actually worked before marking as installed
    verify_installation(package)

    # Mark as installed
    FileUtils.touch(File.join(package.build_dir, '.installed'))
    puts "Installation completed"
  end

  def self.verify_installation(package)
    # Basic verification - check if sysroot has expected directories
    sysroot_usr = File.join(package.sysroot, 'usr')

    # For packages with install commands, we expect sysroot/usr to exist
    if package.build['install'] && package.build['install'].include?('$SYSROOT')
      unless File.exist?(sysroot_usr)
        raise "Installation verification failed for #{package.name}: sysroot/usr directory not created"
      end

      # Additional check for header packages
      if package.name.include?('headers')
        sysroot_include = File.join(sysroot_usr, 'include')
        unless File.exist?(sysroot_include) && !Dir.empty?(sysroot_include)
          raise "Installation verification failed for #{package.name}: headers not installed"
        end
      end

      # Additional check for library packages
      if package.name.include?('libc') || package.name.include?('lib')
        sysroot_lib = File.join(sysroot_usr, 'lib')
        if package.build['install'].include?('lib')
          unless File.exist?(sysroot_lib)
            puts "Warning: #{package.name} expected to install libraries but sysroot/usr/lib not found"
          end
        end
      end
    end
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