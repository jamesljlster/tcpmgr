using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace netlib
{
    partial class tcpmgr
    {
        private void tcpmgr_client_thread(ref object obj)
        {
            tcpmgr_list listRef = (tcpmgr_list)obj;

            // Run client task
            listRef.client_task(ref listRef.usrData, ref listRef.clientSock);

            // Close socket
            listRef.clientSock.Close();
            listRef.sockStatus = 0;

            // Cleanup
            Monitor.Pulse(this.cond);
            listRef.closeJoin = 1;
        }

        private void tcpmgr_accept_thread(ref object obj)
        {
            tcpmgr mgrRef = (tcpmgr)obj;

            while(mgrRef.stop == 0)
            {
                Socket clientSocket = this.listenSock.Accept();
            }
        }
    }
}