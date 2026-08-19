// Base64Encoder.java
import java.io.*;
import java.nio.file.*;
import java.util.Base64;

public class Base64Encoder {
    private static final int CHUNK_SIZE = 1024 * 1024;

    private static void progressBar(long current, long total, int width) {
        if (total == 0) return;
        double percent = (double) current / total;
        int filled = (int) (width * percent);
        StringBuilder bar = new StringBuilder();
        for (int i = 0; i < filled; i++) bar.append('█');
        for (int i = filled; i < width; i++) bar.append('░');
        System.err.printf("\rProgress: [%s] %.1f%%", bar, percent * 100);
        if (current >= total) System.err.println();
    }

    private static void encodeFile(String inputPath, String outputPath, boolean urlSafe) throws IOException {
        Path input = Paths.get(inputPath);
        if (!Files.exists(input)) {
            System.err.println("Error: input file '" + inputPath + "' not found.");
            System.exit(1);
        }
        long fileSize = Files.size(input);
        OutputStream out = outputPath != null ? Files.newOutputStream(Paths.get(outputPath)) : System.out;
        Base64.Encoder encoder = urlSafe ? Base64.getUrlEncoder() : Base64.getEncoder();
        try (InputStream in = Files.newInputStream(input)) {
            byte[] buffer = new byte[CHUNK_SIZE];
            int read;
            long processed = 0;
            while ((read = in.read(buffer)) != -1) {
                byte[] encoded = encoder.encode(buffer, 0, read);
                out.write(encoded);
                processed += read;
                progressBar(processed, fileSize, 40);
            }
        }
        if (outputPath != null) out.close();
        System.out.printf("\n✅ Encoded '%s' to '%s'\n", inputPath, outputPath != null ? outputPath : "stdout");
    }

    private static void decodeFile(String inputPath, String outputPath, boolean urlSafe) throws IOException {
        Path input = Paths.get(inputPath);
        if (!Files.exists(input)) {
            System.err.println("Error: input file '" + inputPath + "' not found.");
            System.exit(1);
        }
        String data = new String(Files.readAllBytes(input)).replaceAll("\\s", "");
        // Add padding
        int missing = data.length() % 4;
        if (missing > 0) data += "=".repeat(4 - missing);
        byte[] decoded;
        try {
            Base64.Decoder decoder = urlSafe ? Base64.getUrlDecoder() : Base64.getDecoder();
            decoded = decoder.decode(data);
        } catch (IllegalArgumentException e) {
            System.err.println("Error: invalid Base64 input - " + e.getMessage());
            System.exit(1);
            return;
        }
        OutputStream out = outputPath != null ? Files.newOutputStream(Paths.get(outputPath)) : System.out;
        out.write(decoded);
        if (outputPath != null) out.close();
        System.out.printf("\n✅ Decoded '%s' to '%s'\n", inputPath, outputPath != null ? outputPath : "stdout");
    }

    public static void main(String[] args) throws IOException {
        String encode = null, decode = null, output = null;
        boolean urlSafe = false;
        for (int i = 0; i < args.length; i++) {
            switch (args[i]) {
                case "-e": encode = args[++i]; break;
                case "-d": decode = args[++i]; break;
                case "-o": output = args[++i]; break;
                case "-u": urlSafe = true; break;
                case "-h":
                case "--help":
                    System.out.println("Usage: java Base64Encoder -e <file> [-o <out>] [-u]\n       java Base64Encoder -d <file> [-o <out>] [-u]");
                    System.exit(0);
            }
        }
        if (encode == null && decode == null) {
            System.err.println("Error: either -e or -d required");
            System.exit(1);
        }
        if (encode != null) encodeFile(encode, output, urlSafe);
        else decodeFile(decode, output, urlSafe);
    }
}
