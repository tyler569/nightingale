require 'fileutils'

class Patcher
  def self.apply_patches(package)
    return if package.patches.empty?
    
    puts "Applying patches for #{package.name}..."
    
    package.patches.each do |patch_file|
      patch_path = File.join(package.patches_dir, patch_file)
      
      unless File.exist?(patch_path)
        raise "Patch file not found: #{patch_path}"
      end
      
      puts "  Applying #{patch_file}..."
      
      success = system("patch", "-p1", "-i", patch_path, 
                      chdir: package.source_dir,
                      out: "/dev/null", 
                      err: "/dev/null")
      
      unless success
        raise "Failed to apply patch: #{patch_file}"
      end
    end
    
    puts "All patches applied successfully"
  end
end