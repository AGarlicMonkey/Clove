using System.IO;

using Membrane = membrane;

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
        public Membrane.FileType AssetType { get; }

        public FileViewModel(FileInfo file, FolderViewModel parent)
            : base(file.Name, file.FullName, parent) {
            AssetGuid = Membrane.FileSystemHelpers.getAssetFileGuid(FullPath);
            AssetType = Membrane.FileSystemHelpers.getAssetFileType(FullPath);
        }

        public override bool CanDropFile(string file) => false;

        public override void OnFileDropped(string file) => throw new System.InvalidOperationException("Attempting to drop a file onto another file. Operation is not supported.");
    }
}