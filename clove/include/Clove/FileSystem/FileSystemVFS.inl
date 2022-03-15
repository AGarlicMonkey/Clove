namespace clove {
    FileSystemVFS::FileSystemVFS() = default;

    FileSystemVFS::FileSystemVFS(FileSystemVFS &&other) noexcept = default;

    FileSystemVFS &FileSystemVFS::operator=(FileSystemVFS &&other) noexcept = default;

    FileSystemVFS::~FileSystemVFS() = default;
}