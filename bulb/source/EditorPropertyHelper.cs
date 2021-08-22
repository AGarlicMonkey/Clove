namespace Bulb{
    class EditorPropertyHelper {
        public static float InputStringToFloat(string value) => float.TryParse(value, out float result) ? result : 0;
    }
}