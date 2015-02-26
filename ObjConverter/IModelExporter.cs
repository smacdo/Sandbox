using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ObjConverter
{
	public interface IModelExporter
	{
		void Export(StaticModelData model, Stream output);
    }
}
