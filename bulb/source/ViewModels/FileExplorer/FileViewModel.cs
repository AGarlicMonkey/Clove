using System.IO;

namespace Bulb {
    public class FileViewModel : DirectoryItemViewModel {
        public override ObjectType Type => ObjectType.File;

        public FileViewModel(FileInfo file, DirectoryItemViewModel parent)
            : base(file.Name, file.FullName, parent) {
        }

        public override bool CanDropFile(string file) => false;

        public override void OnFileDropped(string file) => throw new System.InvalidOperationException("Attempting to drop a file onto another file. Operation is not supported.");
    }
}