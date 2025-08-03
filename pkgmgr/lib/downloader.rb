require 'net/http'
require 'uri'
require 'fileutils'

class Downloader
  def self.download(url, dest_dir)
    uri = URI(url)
    filename = File.basename(uri.path)
    dest_file = File.join(dest_dir, filename)

    return dest_file if File.exist?(dest_file)

    FileUtils.mkdir_p(dest_dir)
    
    puts "Downloading #{url}..."
    
    Net::HTTP.start(uri.host, uri.port, use_ssl: uri.scheme == 'https') do |http|
      request = Net::HTTP::Get.new(uri)
      
      http.request(request) do |response|
        if response.code == '200'
          File.open(dest_file, 'wb') do |file|
            response.read_body do |chunk|
              file.write(chunk)
            end
          end
        else
          raise "Failed to download #{url}: #{response.code} #{response.message}"
        end
      end
    end
    
    dest_file
  end

  def self.extract(archive_path, dest_dir)
    FileUtils.mkdir_p(dest_dir)
    
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
    
    unless $?.success?
      raise "Failed to extract #{archive_path}"
    end
  end
end