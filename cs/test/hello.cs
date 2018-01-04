using System;
using System.Net;
using System.Net.Sockets;

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

        void client_task(object usrData, Socket sock)
        {

        }
    }
}
