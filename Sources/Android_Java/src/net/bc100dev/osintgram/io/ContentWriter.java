package net.bc100dev.osintgram.io;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

public class ContentWriter {

    private final File file;

    private FileOutputStream fd;

    public ContentWriter(File file) {
        this.file = file;
    }

    public ContentWriter(File file, boolean autoOpen) throws IOException {
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

        fd = new FileOutputStream(file);
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

    public void write(byte[] data) throws IOException {
        if (!isOpen())
            throw new IOException("File not opened");

        fd.write(data);
    }

    public void write(String data) throws IOException {
        write(data.getBytes());
    }

    public FileOutputStream getDescriptor() {
        return fd;
    }

}
