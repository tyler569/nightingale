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

      # Apply patch with --forward to ignore reversed/already applied patches and --silent to avoid prompts
      system("patch", "-p1", "--forward", "--silent", "-i", File.absolute_path(patch_path),
             chdir: package.source_dir)
      exit_code = $?.exitstatus

      case exit_code
      when 0
        # Success - patch applied
      when 1
        # Patch skipped (already applied or reversed) - this is OK with --forward
        puts "    Patch #{patch_file} already applied, skipping..."
      else
        # Actual error (exit code 2 or higher typically indicates real failure)
        raise "Failed to apply patch: #{patch_file} (exit code: #{exit_code})"
      end
    end

    puts "All patches applied successfully"
  end
end