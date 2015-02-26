using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ObjConverter
{
	/// <summary>
	///  Loads models from disk and returns in a format agnostic object.
	/// </summary>
	public interface IModelImporter
	{
		StaticModelData Import(Stream input);
	}
}
