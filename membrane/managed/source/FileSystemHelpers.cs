using System.Runtime.InteropServices;

namespace Membrane {
    public enum FileType {
        Unknown,
        Mesh,
        Texture,
    }

    public class FileSystemHelpers {
        [DllImport("MembraneNative.dll", EntryPoint = "getContentPath")]
        [return: MarshalAs(UnmanagedType.BStr)]
        public static extern string GetContentPath();
        public static string GetAssetExtension() => ".clvasset";

        [DllImport("MembraneNative.dll", EntryPoint = "isFileSupported", CharSet = CharSet.Unicode)]
        public static extern bool IsFileSupported(string file);

        [DllImport("MembraneNative.dll", EntryPoint = "isAssetFile", CharSet = CharSet.Unicode)]
        public static extern bool IsAssetFile(string file);

        public static FileType GetFileType(string fullFilePath) => convertIntToFileType(getFileType(fullFilePath));

        [DllImport("MembraneNative.dll", EntryPoint = "createAssetFile", CharSet = CharSet.Unicode)]
        public static extern void CreateAssetFile(string assetLocation, string fileToCreateFrom, string relPathOfCreateFrom, string assetVfsPath);
        [DllImport("MembraneNative.dll", EntryPoint = "moveAssetFile", CharSet = CharSet.Unicode)]
        public static extern void MoveAssetFile(string sourceFileName, string destFileName);
        public static void RemoveAssetFile(ulong assetGuid, FileType assetFileType) => removeAssetFile(assetGuid, convertFileTypeToInt(assetFileType));

        [DllImport("MembraneNative.dll", EntryPoint = "getAssetFileType", CharSet = CharSet.Unicode)]
        public static extern FileType GetAssetFileType(string fullFilePath);
        [DllImport("MembraneNative.dll", EntryPoint = "getAssetFileGuid", CharSet = CharSet.Unicode)]
        public static extern uint GetAssetFileGuid(string fullFilePath);

        [DllImport("MembraneNative.dll", CharSet = CharSet.Unicode)]
        private static extern int getFileType(string file);

        [DllImport("MembraneNative.dll")]
        private static extern void removeAssetFile(ulong assetGuid, int assetFileType);

        private static int convertFileTypeToInt(FileType type) {
            switch (type) {
                case FileType.Mesh:
                    return 0;
                case FileType.Texture:
                    return 1;
                default:
                    return -1;
            }
        }

        private static FileType convertIntToFileType(int type) {
            switch (type) {
                case 0:
                    return FileType.Mesh;
                case 1:
                    return FileType.Texture;
                default:
                    return FileType.Unknown;
            }
        }
    }
}