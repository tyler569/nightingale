require 'net/http'
require 'uri'
require 'fileutils'

class Downloader
  def self.download(url, dest_dir)
    uri = URI(url)
    filename = File.basename(uri.path)

    # Handle GitHub download URLs that don't have proper extensions
    if uri.host == 'codeload.github.com' && filename !~ /\.(tar\.gz|zip|tar\.bz2|tar\.xz)$/
      # Extract project name and version from the path
      path_parts = uri.path.split('/')
      if path_parts.length >= 4
        project = path_parts[2]
        format = path_parts[3] # usually 'tar.gz'
        version = path_parts[5] if path_parts[5] # version/tag
        filename = "#{project}-#{version}.#{format}"
      else
        filename += '.tar.gz'  # default assumption for GitHub
      end
    end

    dest_file = File.join(dest_dir, filename)

    return dest_file if File.exist?(dest_file)

    FileUtils.mkdir_p(dest_dir)

    puts "Downloading #{url}..."

    Net::HTTP.start(uri.host, uri.port, use_ssl: uri.scheme == 'https') do |http|
      request = Net::HTTP::Get.new(uri)

      http.request(request) do |response|
        case response.code
        when '200'
          File.open(dest_file, 'wb') do |file|
            response.read_body do |chunk|
              file.write(chunk)
            end
          end
        when '301', '302', '303', '307', '308'
          # Handle redirects
          redirect_url = response['location']
          puts "Following redirect to #{redirect_url}"
          return download(redirect_url, dest_dir)
        else
          raise "Failed to download #{url}: #{response.code} #{response.message}"
        end
      end
    end

    dest_file
  end

  def self.extract(archive_path, dest_dir)
    FileUtils.mkdir_p(dest_dir)

    # Check file type instead of just extension
    file_type = `file "#{archive_path}"`.strip

    if file_type.include?('gzip compressed')
      system("tar", "-xzf", archive_path, "-C", dest_dir, "--strip-components=1")
    elsif file_type.include?('bzip2 compressed')
      system("tar", "-xjf", archive_path, "-C", dest_dir, "--strip-components=1")
    elsif file_type.include?('XZ compressed')
      system("tar", "-xJf", archive_path, "-C", dest_dir, "--strip-components=1")
    elsif file_type.include?('Zip archive')
      system("unzip", "-q", archive_path, "-d", dest_dir)
    else
      # Fall back to extension-based detection
      case File.extname(archive_path).downcase
      when '.gz'
        if archive_path.end_with?('.tar.gz')
          system("tar", "-xzf", archive_path, "-C", dest_dir, "--strip-components=1")
        else
          raise "Unsupported archive format: #{archive_path}"
        end
      when '.bz2'
        if archive_path.end_with?('.tar.bz2')
          system("tar", "-xjf", archive_path, "-C", dest_dir, "--strip-components=1")
        else
          raise "Unsupported archive format: #{archive_path}"
        end
      when '.xz'
        if archive_path.end_with?('.tar.xz')
          system("tar", "-xJf", archive_path, "-C", dest_dir, "--strip-components=1")
        else
          raise "Unsupported archive format: #{archive_path}"
        end
      when '.zip'
        system("unzip", "-q", archive_path, "-d", dest_dir)
      else
        raise "Unsupported archive format: #{archive_path}"
      end
    end

    unless $?.success?
      raise "Failed to extract #{archive_path}"
    end
  end
end