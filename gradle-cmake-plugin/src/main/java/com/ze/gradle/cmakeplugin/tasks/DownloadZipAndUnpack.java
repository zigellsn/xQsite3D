package com.ze.gradle.cmakeplugin.tasks;

import org.gradle.api.DefaultTask;
import org.gradle.api.file.DirectoryProperty;
import org.gradle.api.provider.Property;
import org.gradle.api.tasks.Input;
import org.gradle.api.tasks.OutputDirectory;
import org.gradle.api.tasks.TaskAction;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.nio.file.Files;
import java.nio.file.Paths;

public class DownloadZipAndUnpack extends DefaultTask {
    private final Property<String> url = getProject().getObjects().property(String.class);
    private final Property<Boolean> deleteAfterDownload = getProject().getObjects().property(Boolean.class).convention(false);
    private final DirectoryProperty dest = getProject().getObjects().directoryProperty().convention(getProject().getLayout().getBuildDirectory().dir(getProject().getName()));

    public DownloadZipAndUnpack() {
        onlyIf(it -> getUrl().isPresent());
    }

    @TaskAction
    public void doDownloadZipAndUnpack() throws IOException {
        URL downloadUrl = new URL(url.get());
        getLogger().warn("Downloading " + downloadUrl);

        final String fileName = Paths.get(downloadUrl.getPath()).getFileName().toString();
        final File zipDestination = new File(getTemporaryDir(), fileName);
        zipDestination.delete();
        try (InputStream inStream = downloadUrl.openStream()) {
            Files.copy(inStream, zipDestination.toPath());
        }
        getLogger().warn("Downloaded to " + zipDestination.getAbsolutePath());

        final File unzipDestination = dest.get().getAsFile();
        getProject().copy(copySpec -> {
            copySpec.from(getProject().zipTree(zipDestination));
            copySpec.into(unzipDestination);
        });
        if (deleteAfterDownload.get()) {
            zipDestination.delete();
            getTemporaryDir().delete();
        }
    }

    @Input
    public Property<String> getUrl() {
        return url;
    }

    @Input
    public Property<Boolean> getDeleteAfterDownload() {
        return deleteAfterDownload;
    }

    @OutputDirectory
    public DirectoryProperty getDest() {
        return dest;
    }
}
