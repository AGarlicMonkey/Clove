using System.IO;
using Membrane;

namespace Bulb {
    public class FileViewModel : DirectoryItemViewModel {
        public override ObjectType Type => ObjectType.File;

        /// <summary>
        /// Guid used by the internal asset manager to track files
        /// </summary>
        public ulong AssetGuid { get; }

        /// <summary>
        /// The type of asset this file is (i.e. Mesh, Texture etc.)
        /// </summary>
        public FileType AssetType { get; }

        public FileViewModel(FileInfo file, FolderViewModel parent)
            : base(file.Name, file.FullName, parent) {
            AssetGuid = FileSystem.GetAssetFileGuid(FullPath);
            AssetType = FileSystem.GetAssetFileType(FullPath);
        }

        public override void Rename(string newName) {
            string newFileName = $"{newName}.clvasset";
            string newPath = $"{Parent.FullPath}{Path.DirectorySeparatorChar}{newFileName}";

            File.Move(FullPath, newPath);

            FileSystem.MoveAssetFile(FullPath, newPath);

            Name = newFileName;
            FullPath = newPath;
        }

        public override void Reconstruct() {
            string newPath = $"{Parent.FullPath}{Path.DirectorySeparatorChar}{Name}";

            if (newPath != FullPath) {
                FileSystem.MoveAssetFile(FullPath, newPath);

                FullPath = newPath;
            }
        }

        public override bool CanDropFile(string file) => false;

        public override void OnFileDropped(string file) => throw new System.InvalidOperationException("Attempting to drop a file onto another file. Operation is not supported.");

        public override void OnFileDropped(DirectoryItemViewModel file) => throw new System.InvalidOperationException("Attempting to drop a file onto another file. Operation is not supported.");
    }
}