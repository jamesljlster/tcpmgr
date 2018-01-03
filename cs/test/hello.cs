using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using netlib;

namespace Stack_HW
{
    class Program
    {
        static void Main(string[] args)
        {
            tcpmgr tmgr = new tcpmgr("0.0.0.0", 27015, 5);
            Console.WriteLine("hello");
        }
    }
}
