namespace membrane {
    EditorVFS::EditorVFS(std::filesystem::path contentDirectory)
        : contentDirectory{ std::move(contentDirectory) } {
    }

    EditorVFS::EditorVFS(EditorVFS &&other) noexcept = default;

    EditorVFS &EditorVFS::operator=(EditorVFS &&other) noexcept = default;

    EditorVFS::~EditorVFS() = default;
}