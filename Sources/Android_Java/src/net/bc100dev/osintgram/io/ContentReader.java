package net.bc100dev.osintgram.io;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;

public class ContentReader {

    private final File file;

    private FileInputStream fd;

    public ContentReader(File file) {
        this.file = file;
    }

    public ContentReader(File file, boolean autoOpen) throws IOException {
        this.file = file;

        if (autoOpen)
            open();
    }

    public void open() throws IOException {
        File parent = file.getParentFile();
        if (parent != null) {
            if (!parent.exists()) {
                if (!parent.mkdirs())
                    throw new IOException("Could not create parent directory");
            }
        }

        fd = new FileInputStream(file);
    }

    public boolean isOpen() {
        return fd != null;
    }

    private void throwIfNotOpen() throws IOException {
        if (!isOpen())
            throw new IOException("File not opened");
    }

    public void close() throws IOException {
        fd.close();
        fd = null;
    }

    public byte[] read() throws IOException {
        throwIfNotOpen();
        byte[] data = new byte[(int) file.length()];
        fd.read(data);
        return data;
    }

    public byte[] read(int len) throws IOException {
        throwIfNotOpen();
        byte[] data = new byte[len];
        int bytesRead = fd.read(data, 0, len);
        if (bytesRead < len) {
            byte[] trimmed = new byte[bytesRead];
            System.arraycopy(data, 0, trimmed, 0, bytesRead);
            return trimmed;
        }

        return data;
    }

    public byte[] read(int off, int len) throws IOException {
        throwIfNotOpen();

        if (fd.skip(off) < off)
            throw new IOException("Could not skip to offset " + off);

        return read(len);
    }

    public String sread() throws IOException {
        return new String(read());
    }

    public String sread(int len) throws IOException {
        return new String(read(len));
    }

    public String sread(int off, int len) throws IOException {
        return new String(read(off, len));
    }

    public int avail() throws IOException {
        throwIfNotOpen();
        return fd.available();
    }

    public long skip(long n) throws IOException {
        throwIfNotOpen();
        return fd.skip(n);
    }

    public FileInputStream getDescriptor() {
        return fd;
    }

}
