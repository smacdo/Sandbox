using System;
using System.Collections.Generic;
using System.Linq;

namespace ObjConverter
{
    public class TextureInfo
    {
        public Vector3 Color { get; set;}
        public string Map { get; set; }

        public TextureInfo()
        {
            Color = new Vector3 { X = 1.0f, Y = 1.0f, Z = 1.0f };
        }
    }

    /// <summary>
    ///  A model material
    /// </summary>
    public class Material
    {
        public string Name { get; set; }
        public TextureInfo Ambient { get; set; }
        public TextureInfo Diffuse { get; set; }
        public TextureInfo Specular { get; set; }
        public float SpecularPower { get; set; }
        public bool Wireframe { get; set; }
        public float Opacity { get; set; }

        public Material(string materialName)
        {
            if (string.IsNullOrEmpty(materialName))
            {
                throw new ArgumentNullException("materialName");
            }

            Name = materialName;
            Wireframe = false;
            Opacity = 1.0f;
        }
    }
}
