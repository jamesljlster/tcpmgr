using System;
using System.Net;
using System.Net.Sockets;

namespace netlib
{
    public partial class tcpmgr
    {
        // Constructor
        public tcpmgr(String hostIP, int hostPort, int maxClient)
        {
            IPEndPoint hostAddrInfo = new IPEndPoint(IPAddress.Parse(hostIP), hostPort);
            this.listenSock.Bind(hostAddrInfo);
            this.listenSock.Listen(maxClient);
        }

        // Destructor
        ~tcpmgr()
        {

        }

        // Private objects
        Socket listenSock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
    }
}
