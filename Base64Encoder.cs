// Base64Encoder.cs
using System;
using System.IO;
using System.Text;
using System.Threading;

class Base64Encoder
{
    private const int ChunkSize = 1024 * 1024;

    static void ProgressBar(long current, long total, int width = 40)
    {
        if (total == 0) return;
        double percent = (double)current / total;
        int filled = (int)(width * percent);
        string bar = new string('█', filled) + new string('░', width - filled);
        Console.Error.Write($"\rProgress: [{bar}] {percent * 100:F1}%");
        if (current >= total) Console.Error.WriteLine();
    }

    static void EncodeFile(string inputPath, string outputPath, bool urlSafe)
    {
        if (!File.Exists(inputPath))
        {
            Console.Error.WriteLine($"Error: input file '{inputPath}' not found.");
            Environment.Exit(1);
        }
        var fileInfo = new FileInfo(inputPath);
        long fileSize = fileInfo.Length;
        var outStream = outputPath != null ? File.Create(outputPath) : Console.OpenStandardOutput();
        using var inStream = File.OpenRead(inputPath);
        byte[] buffer = new byte[ChunkSize];
        int read;
        long processed = 0;
        while ((read = inStream.Read(buffer, 0, buffer.Length)) > 0)
        {
            string encoded = urlSafe ?
                Convert.ToBase64String(buffer, 0, read).Replace('+', '-').Replace('/', '_') :
                Convert.ToBase64String(buffer, 0, read);
            byte[] bytes = Encoding.ASCII.GetBytes(encoded);
            outStream.Write(bytes, 0, bytes.Length);
            processed += read;
            ProgressBar(processed, fileSize);
        }
        if (outputPath != null) outStream.Close();
        Console.WriteLine($"\n✅ Encoded '{inputPath}' to '{(outputPath ?? "stdout")}'");
    }

    static void DecodeFile(string inputPath, string outputPath, bool urlSafe)
    {
        if (!File.Exists(inputPath))
        {
            Console.Error.WriteLine($"Error: input file '{inputPath}' not found.");
            Environment.Exit(1);
        }
        string data = File.ReadAllText(inputPath).Replace(" ", "").Replace("\n", "").Replace("\r", "").Replace("\t", "");
        // Add padding
        int missing = data.Length % 4;
        if (missing > 0) data += new string('=', 4 - missing);
        byte[] decoded;
        try
        {
            if (urlSafe)
            {
                string standard = data.Replace('-', '+').Replace('_', '/');
                decoded = Convert.FromBase64String(standard);
            }
            else
            {
                decoded = Convert.FromBase64String(data);
            }
        }
        catch (Exception e)
        {
            Console.Error.WriteLine($"Error: invalid Base64 input - {e.Message}");
            Environment.Exit(1);
            return;
        }
        var outStream = outputPath != null ? File.Create(outputPath) : Console.OpenStandardOutput();
        outStream.Write(decoded, 0, decoded.Length);
        if (outputPath != null) outStream.Close();
        Console.WriteLine($"\n✅ Decoded '{inputPath}' to '{(outputPath ?? "stdout")}'");
    }

    static void Main(string[] args)
    {
        string encode = null, decode = null, output = null;
        bool urlSafe = false;
        for (int i = 0; i < args.Length; i++)
        {
            switch (args[i])
            {
                case "-e": encode = args[++i]; break;
                case "-d": decode = args[++i]; break;
                case "-o": output = args[++i]; break;
                case "-u": urlSafe = true; break;
                case "-h": case "--help":
                    Console.WriteLine("Usage: dotnet run -- -e <file> [-o <out>] [-u]\n       dotnet run -- -d <file> [-o <out>] [-u]");
                    return;
            }
        }
        if (encode == null && decode == null)
        {
            Console.Error.WriteLine("Error: either -e or -d required");
            Environment.Exit(1);
        }
        if (encode != null) EncodeFile(encode, output, urlSafe);
        else DecodeFile(decode, output, urlSafe);
    }
}
