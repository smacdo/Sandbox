using System;
using System.Collections.Generic;
using System.Linq;

namespace ObjConverter
{
	/// <summary>
	///  TODO: Better name for class?
	/// </summary>
	public class TextureInfo
    {
		private Vector3? mColor;
		private string mMap;

        public Vector3 Color
		{
			get
			{
				if (!mColor.HasValue)
				{
					throw new InvalidOperationException("Material texture is not a solid color");
				}

				return mColor.Value;
			}
			set
			{
				mColor = value;
				Map = null;
			}
		}

        public string Map
		{
			get
			{
				if (mMap == null)
				{
					throw new InvalidOperationException("Material texture is not a texture map");
				}

				return mMap;
			}
			set
			{
				mColor = null;
				mMap = value;
			}
		}

		public bool HasColor { get { return mColor.HasValue; } }

		public bool HasMap { get { return mMap != null; } }

        public TextureInfo()
        {
            mColor = new Vector3 { X = 1.0f, Y = 1.0f, Z = 1.0f };
        }

		public TextureInfo(Vector3 color)
		{
			mColor = color;
		}

		public TextureInfo(string mapName)
		{
			if (string.IsNullOrWhiteSpace(mapName))
			{
				throw new ArgumentNullException("mapName");
			}

			mMap = mapName;
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

        public Material(string materialName)
        {
            if (string.IsNullOrEmpty(materialName))
            {
                throw new ArgumentNullException("materialName");
            }

            Name = materialName;
            Wireframe = false;
        }
    }
}
