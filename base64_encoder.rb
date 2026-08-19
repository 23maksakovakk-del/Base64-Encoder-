# base64_encoder.rb
#!/usr/bin/env ruby
require 'optparse'
require 'base64'
require 'stringio'

def progress_bar(current, total, width = 40)
  return if total == 0
  percent = current.to_f / total
  filled = (width * percent).to_i
  bar = '█' * filled + '░' * (width - filled)
  $stderr.print("\rProgress: [#{bar}] #{'%.1f' % (percent * 100)}%")
  $stderr.puts if current >= total
end

def encode_file(input_path, output_path, url_safe)
  unless File.exist?(input_path)
    $stderr.puts "Error: input file '#{input_path}' not found."
    exit 1
  end
  file_size = File.size(input_path)
  chunk_size = 1024 * 1024
  out = output_path ? File.open(output_path, 'w') : $stdout
  processed = 0
  File.open(input_path, 'rb') do |f|
    while chunk = f.read(chunk_size)
      encoded = url_safe ? Base64.urlsafe_encode64(chunk) : Base64.strict_encode64(chunk)
      out.write(encoded)
      processed += chunk.bytesize
      progress_bar(processed, file_size)
    end
  end
  out.close if output_path
  puts "\n✅ Encoded '#{input_path}' to '#{output_path || 'stdout'}'"
end

def decode_file(input_path, output_path, url_safe)
  unless File.exist?(input_path)
    $stderr.puts "Error: input file '#{input_path}' not found."
    exit 1
  end
  data = File.read(input_path).gsub(/\s/, '')
  # Add padding
  missing = data.length % 4
  data += '=' * (4 - missing) if missing > 0
  begin
    decoded = url_safe ? Base64.urlsafe_decode64(data) : Base64.decode64(data)
  rescue => e
    $stderr.puts "Error: invalid Base64 input - #{e.message}"
    exit 1
  end
  out = output_path ? File.open(output_path, 'wb') : $stdout
  out.write(decoded)
  out.close if output_path
  puts "\n✅ Decoded '#{input_path}' to '#{output_path || 'stdout'}'"
end

options = {}
OptionParser.new do |opts|
  opts.banner = "Usage: ruby base64_encoder.rb [options]"
  opts.on('-e FILE', '--encode FILE', 'Encode file') { |v| options[:encode] = v }
  opts.on('-d FILE', '--decode FILE', 'Decode file') { |v| options[:decode] = v }
  opts.on('-o FILE', '--output FILE', 'Output file') { |v| options[:output] = v }
  opts.on('-u', '--url-safe', 'Use URL-safe alphabet') { options[:url_safe] = true }
  opts.on('-h', '--help', 'Show help') { puts opts; exit }
end.parse!

if options[:encode].nil? && options[:decode].nil?
  $stderr.puts "Error: either -e or -d required"
  exit 1
end

if options[:encode]
  encode_file(options[:encode], options[:output], options[:url_safe])
else
  decode_file(options[:decode], options[:output], options[:url_safe])
end
